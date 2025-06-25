#include "master.h"
#include "base/bragi.h"
#include "exception/bragi_exception.h"
#include "locale/locale.h"
#include "util/logger.h"
#include "util/properties.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
}

#include <fstream>
#include <vector>
#include <iostream>

// Буфер для I/O
static const int IO_BUFFER_SIZE = 32768;

struct IOContext {
    std::ifstream *in;
    std::vector<uint8_t> buffer;
};

// callback для чтения
static int read_packet(void *opaque, uint8_t *buf, int buf_size) {
    IOContext *io = static_cast<IOContext *>(opaque);
    if (!io->in->good())
        return AVERROR_EOF;
    io->in->read(reinterpret_cast<char *>(buf), buf_size);
    return io->in->gcount();
}

int convert_to_pcm(std::ifstream &fin, std::ofstream &fout) {
    // 1) Настраиваем AVFormatContext с нашим AVIOContext
    IOContext io_ctx{&fin, std::vector<uint8_t>(IO_BUFFER_SIZE)};
    AVIOContext *avio_ctx = avio_alloc_context(
        io_ctx.buffer.data(), IO_BUFFER_SIZE,
        0, // флаг: 0 = только чтение
        &io_ctx, // opaque
        &read_packet, // read callback
        nullptr, // write callback
        nullptr // seek callback
        );
    if (!avio_ctx) {
        std::cerr << "Не удалось создать AVIOContext\n";
        return -1;
    }

    AVFormatContext *fmt_ctx = avformat_alloc_context();
    fmt_ctx->pb = avio_ctx;
    // подсказка формата (необязательно)
    // fmt_ctx->iformat = av_find_input_format("mp3");

    if (avformat_open_input(&fmt_ctx, nullptr, nullptr, nullptr) < 0) {
        std::cerr << "avformat_open_input failed\n";
        return -1;
    }
    if (avformat_find_stream_info(fmt_ctx, nullptr) < 0) {
        std::cerr << "avformat_find_stream_info failed\n";
        return -1;
    }

    // 2) Находим аудиопоток
    int audio_stream_index = av_find_best_stream(
        fmt_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
    if (audio_stream_index < 0) {
        std::cerr << "Аудиопоток не найден\n";
        return -1;
    }
    AVStream *audio_stream = fmt_ctx->streams[audio_stream_index];

    // 3) Открываем декодер
    const AVCodec *decoder = avcodec_find_decoder(
        audio_stream->codecpar->codec_id);
    if (!decoder) {
        std::cerr << "Декодер не найден\n";
        return -1;
    }
    AVCodecContext *dec_ctx = avcodec_alloc_context3(decoder);
    avcodec_parameters_to_context(dec_ctx, audio_stream->codecpar);
    if (avcodec_open2(dec_ctx, decoder, nullptr) < 0) {
        std::cerr << "avcodec_open2 failed\n";
        return -1;
    }

    // 4) Подготовка SwrContext, если нужно конвертировать формат/частоту
    AVChannelLayout in_ch_layout, out_ch_layout;

    // Инициализировать входящую раскладку из dec_ctx->channels
    av_channel_layout_default(&in_ch_layout, 2);

    // Инициализировать выходную раскладку для стерео (2 каналов)
    av_channel_layout_default(&out_ch_layout, 2);

    // 2) Аллокация SwrContext
    SwrContext* swr = nullptr;
    int ret = swr_alloc_set_opts2(
        &swr,
        &out_ch_layout,            // выходная раскладка (AVChannelLayout*)
        AV_SAMPLE_FMT_S16,         // выходной формат сэмплов
        44100,                     // выходная частота
        &in_ch_layout,             // входная раскладка (AVChannelLayout*)
        dec_ctx->sample_fmt,       // входной формат сэмплов
        dec_ctx->sample_rate,      // входная частота
        0, nullptr
    );
    if (ret < 0) {
        std::cerr << "swr_alloc_set_opts2 failed: " << ret << "\n";
        return -1;
    }
    if ((ret = swr_init(swr)) < 0) {
        std::cerr << "swr_init failed: " << ret << "\n";
        return -1;
    }
    swr_init(swr);

    // 5) Декодируем и пишем PCM
    AVPacket* packet = av_packet_alloc();
    AVFrame* frame   = av_frame_alloc();

    uint8_t** out_data    = nullptr;
    int      out_linesize = 0;
    int      max_out_samples = 0;

    while (av_read_frame(fmt_ctx, packet) >= 0) {
        if (packet->stream_index == audio_stream_index) {
            if (avcodec_send_packet(dec_ctx, packet) == 0) {
                while (avcodec_receive_frame(dec_ctx, frame) == 0) {
                    int out_samples = av_rescale_rnd(
                        swr_get_delay(swr, dec_ctx->sample_rate) + frame->nb_samples,
                        44100, dec_ctx->sample_rate, AV_ROUND_UP
                    );
                    if (out_samples > max_out_samples) {
                        max_out_samples = out_samples;
                        av_freep(&out_data);
                        ret = av_samples_alloc_array_and_samples(
                                  &out_data, &out_linesize,
                                  2, out_samples,
                                  AV_SAMPLE_FMT_S16, 0
                              );
                        if (ret < 0) {
                            std::cerr << "alloc_array_and_samples failed: " << ret << "\n";
                            return -1;
                        }
                    }
                    int converted = swr_convert(
                        swr, out_data, out_samples,
                        (const uint8_t**)frame->data, frame->nb_samples
                    );
                    int data_size = av_samples_get_buffer_size(
                        &out_linesize, 2, converted, AV_SAMPLE_FMT_S16, 1
                    );
                    fout.write(reinterpret_cast<char*>(out_data[0]), data_size);
                }
            }
        }
        av_packet_unref(packet);
    }

    // 6) Очистка
    av_freep(&out_data);
    av_frame_free(&frame);
    av_packet_free(&packet);
    swr_free(&swr);
    avcodec_free_context(&dec_ctx);
    avformat_close_input(&fmt_ctx);
    avio_context_free(&avio_ctx);

    return 0;
}

int main() {
    std::ifstream fin("/home/evilur/Downloads/fuck.flac", std::ios::binary);
    if (!fin) {
        std::cerr << "Не удалось открыть input.mp3\n";
        return -1;
    }
    std::ofstream fout("/tmp/out.pcm", std::ios::binary);
    if (!fout) {
        std::cerr << "Не удалось создать output.pcm\n";
        return -1;
    }
    return convert_to_pcm(fin, fout);
    /* Create a bot cluster */
    dpp::cluster bot = dpp::cluster(Properties::BotToken());

    /* Create event handlers */
    bot.on_slashcommand(on_slashcommand);
    bot.on_voice_state_update(on_voice_state_update);
    bot.on_voice_ready(on_voice_ready);
    bot.on_voice_track_marker(on_voice_track_marker);
    bot.on_ready(on_ready);

    /* Start the bot */
    INFO_LOG("Starting the bot");
    bot.start(dpp::st_wait);
}

void on_slashcommand(const dpp::slashcommand_t &event) {
    /* Get a command name */
    const std::string command_name = event.command.get_command_name();

    /* Get the bragi instance */
    Bragi *const bragi = Bragi::Get(event.command.guild_id);

    try {
        /* Run a command and get a result _message */
        const dpp::message message =
            command_name == "play"
                ? bragi->PlayCommand(event)
                : command_name == "skip"
                ? bragi->SkipCommand(event)
                : command_name == "list"
                ? bragi->ListCommand()
                : command_name == "next"
                ? bragi->NextCommand(event)
                : command_name == "loop"
                ? bragi->LoopCommand(event)
                : command_name == "join"
                ? bragi->JoinCommand(event)
                : command_name == "speed"
                ? bragi->SpeedCommand(event)
                : command_name == "leave"
                ? bragi->LeaveCommand(event)
                : command_name == "ping"
                ? Bragi::PingCommand(event)
                : throw BragiException(_("**Unexpected error**"),
                                       BragiException::HARD);
        event.reply(message);
    } catch (const BragiException &e) {
        /* Handle the exception and print the error message to the user */
        const dpp::message &message = e.GetMessage();
        event.reply(message);
    }
}

void on_voice_state_update(const dpp::voice_state_update_t &event) {
    /* If there isn't the bot, exit the function */
    if (event.state.user_id != event.owner->me.id)
        return;

    /* Handle the voice state update */
    Bragi::Get(event.state.guild_id)->OnVoiceStateUpdate(event);
}

void on_voice_ready(const dpp::voice_ready_t &event) {
    Bragi::Get(event.voice_client->server_id)->OnVoiceReady(event);
}

void on_voice_track_marker(const dpp::voice_track_marker_t &event) {
    Bragi::Get(event.voice_client->server_id)->OnMarker();
}

void on_ready(const dpp::ready_t &event) {
    dpp::cluster *bot = event.owner;

    /* Add slash commands */
    bot->global_command_create(
        dpp::slashcommand("ping", DIC_SLASH_PING, bot->me.id));

    bot->global_command_create(
        dpp::slashcommand("join", DIC_SLASH_JOIN, bot->me.id).add_option(
            dpp::command_option(dpp::co_user, "user", DIC_SLASH_JOIN_USER,
                                false)));

    bot->global_command_create(
        dpp::slashcommand("leave", DIC_SLASH_LEAVE, bot->me.id));

    bot->global_command_create(
        dpp::slashcommand("play", DIC_SLASH_PLAY, bot->me.id).add_option(
            dpp::command_option(dpp::co_string, "query", DIC_SLASH_PLAY_QUERY,
                                true)));

    bot->global_command_create(
        dpp::slashcommand("skip", DIC_SLASH_SKIP, bot->me.id).add_option(
            dpp::command_option(dpp::co_integer, "number",
                                DIC_SLASH_SKIP_NUMBER, false)));

    bot->global_command_create(
        dpp::slashcommand("speed", DIC_SLASH_SPEED, bot->me.id).add_option(
            dpp::command_option(dpp::co_integer, "percent",
                                DIC_SLASH_SPEED_PERCENT, false)));

    bot->global_command_create(
        dpp::slashcommand("next", DIC_SLASH_NEXT, bot->me.id).add_option(
            dpp::command_option(dpp::co_integer, "number",
                                DIC_SLASH_NEXT_ORDINAL, false)));

    bot->global_command_create(
        dpp::slashcommand("loop", DIC_SLASH_LOOP, bot->me.id).add_option(
            dpp::command_option(dpp::co_string, "type", DIC_SLASH_LOOP_TYPE,
                                false).add_choice(
                dpp::command_option_choice("Disabled", "d")).add_choice(
                dpp::command_option_choice("Track", "t")).add_choice(
                dpp::command_option_choice("Playlist", "p"))));

    bot->global_command_create(
        dpp::slashcommand("list", DIC_SLASH_LIST, bot->me.id));

    INFO_LOG("Bot is ready");
}