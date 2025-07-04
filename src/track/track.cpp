#include "track.h"

#include "util/logger.hpp"

Track::~Track() {
    /* Destroy the opus encoder */
    opus_encoder_destroy(_encoder);

    /* Abort the playing and join the play thread */
    Abort();
    if (_play_thread.joinable()) _play_thread.join();
}

void Track::Play(Bragi::Player& player) {
    /* Reset abort state */
    _is_aborted = false;

    /* Allocate the format context */
    AVFormatContext* format_ctx = avformat_alloc_context();

    /* Allocate and set avio context */
    format_ctx->pb = avio_alloc_context(
        (unsigned char*)av_malloc(GetAudioBufferSize()),
        GetAudioBufferSize(),
        0,
        this,
        GetReadAudioCallback(),
        nullptr,
        nullptr
    );

    /* Open audio stream */
    avformat_open_input(&format_ctx, nullptr, nullptr, nullptr);
    avformat_find_stream_info(format_ctx, nullptr);

    /* Init codec context */
    const AVCodecParameters* const codec_par = format_ctx->streams[0]->codecpar;
    AVCodecContext* codec_ctx =
        avcodec_alloc_context3(avcodec_find_decoder(codec_par->codec_id));
    avcodec_parameters_to_context(codec_ctx, codec_par);
    avcodec_open2(codec_ctx, codec_ctx->codec, nullptr);

    /* Set output audio format */
    constexpr AVChannelLayout out_ch_layout = AV_CHANNEL_LAYOUT_STEREO;
    constexpr AVSampleFormat out_fmt = AV_SAMPLE_FMT_S16;
    const int out_rate = 48000 * 100 / player.playback_rate;

    /* Init resampler */
    SwrContext* swr = nullptr;
    swr_alloc_set_opts2(
        &swr,
        &out_ch_layout, out_fmt, out_rate,
        &codec_ctx->ch_layout, codec_ctx->sample_fmt, codec_ctx->sample_rate,
        0, nullptr
    );
    swr_init(swr);

    // 3) Буфер для ресемплированных данных
    const int MAX_OUT_SAMPLES = FRAME_SIZE;
    uint8_t** resampled_data = nullptr;
    int resampled_linesize = 0;
    av_samples_alloc_array_and_samples(
        &resampled_data,
        &resampled_linesize,
        2,                // каналы
        MAX_OUT_SAMPLES,
        out_fmt,
        0
    );

    // 3) Бьем пакеты, декодируем, и сбрасываем raw PCM в out_pcm
    AVPacket* pkt = av_packet_alloc();
    AVFrame* frame = av_frame_alloc();

    while (av_read_frame(format_ctx, pkt) >= 0) {
        if (pkt->stream_index == 0) {
            if (avcodec_send_packet(codec_ctx, pkt) == 0) {
                while (avcodec_receive_frame(codec_ctx, frame) == 0) {
                    // 2) ресемплинг
                    int in_samples = frame->nb_samples;
                    // Преобразуем; возвращает число полученных сэмплов
                    int out_samples = swr_convert(
                        swr,
                        resampled_data,
                        MAX_OUT_SAMPLES,
                        frame->data,
                        in_samples
                    );
                    if (out_samples < 0) {
                        std::cerr << "Ошибка ресемплинга\n";
                        continue;
                    }
                    // Вычисляем размер в байтах
                    int data_size = av_samples_get_buffer_size(
                        nullptr, 2, out_samples, out_fmt, 1
                    );

                    if (_is_aborted) goto free_memory;

                    for (int i = 0; i < data_size; i++) {
                        *_pcm_buffer_ptr++ = resampled_data[0][i];
                        if (_pcm_buffer_ptr < _pcm_buffer_end)
                            continue;
                        _pcm_buffer_ptr = (char*)_pcm_buffer;

                        unsigned char opus_buffer[OPUS_CHUNK_SIZE];
                        const int len = opus_encode(
                            _encoder, _pcm_buffer, FRAME_SIZE,
                            opus_buffer, OPUS_CHUNK_SIZE
                        );
                        player.voice_client->send_audio_opus(
                            opus_buffer, len, 60
                        );
                    }
                }
            }
        }
        av_packet_unref(pkt);
    }

    // Завершаем декодирование (декодер может ещё вернуть фреймы)
    avcodec_send_packet(codec_ctx, nullptr);
    while (avcodec_receive_frame(codec_ctx, frame) == 0) {
        int out_samples = swr_convert(
            swr,
            resampled_data,
            MAX_OUT_SAMPLES,
            frame->data,
            frame->nb_samples
        );
        if (out_samples < 0) break;

        int data_size = av_samples_get_buffer_size(
            nullptr, 2, out_samples, out_fmt, 1
        );

        for (int i = 0; i < data_size; i++) {
            *_pcm_buffer_ptr++ = resampled_data[0][i];
            if (_pcm_buffer_ptr < _pcm_buffer_end)
                continue;
            _pcm_buffer_ptr = (char*)_pcm_buffer;

            unsigned char opus_buffer[OPUS_CHUNK_SIZE];
            const int len = opus_encode(
                _encoder, _pcm_buffer, FRAME_SIZE,
                opus_buffer, OPUS_CHUNK_SIZE
            );
            player.voice_client->send_audio_opus(
                opus_buffer, len, 60
            );
        }
    }

    // Дренаж SwrContext, чтобы забрать остатки
    int out_samples;
    do {
        out_samples = swr_convert(
            swr,
            resampled_data,
            MAX_OUT_SAMPLES,
            nullptr,
            0
        );

        if (out_samples > 0) {
            int data_size = av_samples_get_buffer_size(
                nullptr, 2, out_samples, out_fmt, 1
            );
            for (int i = 0; i < data_size; i++) {
                *_pcm_buffer_ptr++ = resampled_data[0][i];
                if (_pcm_buffer_ptr < _pcm_buffer_end)
                    continue;
                _pcm_buffer_ptr = (char*)_pcm_buffer;

                unsigned char opus_buffer[OPUS_CHUNK_SIZE];
                const int len = opus_encode(
                    _encoder, _pcm_buffer, FRAME_SIZE,
                    opus_buffer, OPUS_CHUNK_SIZE
                );
                player.voice_client->send_audio_opus(
                    opus_buffer, len, 60
                );
            }
        }
    } while (out_samples > 0);

    /* Send EOF marker */
    player.voice_client->insert_marker();

    /* Free the memory */
free_memory:

    /* AVIO context */
    av_free(format_ctx->pb->buffer);
    avio_context_free(&format_ctx->pb);

    /* Format context */
    avformat_close_input(&format_ctx);
    avformat_free_context(format_ctx);

    /* Codec context */
    avcodec_free_context(&codec_ctx);

    /* Resampler context */
    swr_free(&swr);
}

inline void Track::Abort() { _is_aborted = true; }