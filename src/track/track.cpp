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
    format_ctx->pb =
        avio_alloc_context((unsigned char*)av_malloc(GetAudioBufferSize()),
                           GetAudioBufferSize(),
                           0,
                           this,
                           GetReadAudioCallback(),
                           nullptr,
                           nullptr);

    // 1.2) Открываем формат (FFmpeg сам «угадает» контейнер/кодек по заголовкам из потока)
    if (avformat_open_input(&format_ctx, nullptr, nullptr, nullptr) < 0) {
        std::cerr << "Не удалось открыть входной поток\n";
        throw -1;
    }
    if (avformat_find_stream_info(format_ctx, nullptr) < 0) {
        std::cerr << "Не удалось найти информацию о потоках\n";
        throw -1;
    }

    // 2) Ищем аудиопоток и инициализируем декодер
    const AVCodec* codec = nullptr;
    int stream_index = av_find_best_stream(format_ctx, AVMEDIA_TYPE_AUDIO,
                                           -1, -1,
                                           &codec, 0);
    if (stream_index < 0) {
        std::cerr << "Аудиопоток не найден\n";
        throw -1;
    }
    AVStream* audio_st = format_ctx->streams[stream_index];
    AVCodecContext* cctx = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(cctx, audio_st->codecpar);
    if (avcodec_open2(cctx, codec, nullptr) < 0) {
        std::cerr << "Не удалось открыть декодер\n";
        throw -1;
    }

    /* Set output audio format */
    constexpr AVChannelLayout out_ch_layout = AV_CHANNEL_LAYOUT_STEREO;
    constexpr AVSampleFormat out_fmt = AV_SAMPLE_FMT_S16;
    const int out_rate = 48000 * 100 / player.playback_rate;

    // 2) Аллоцируем и настраиваем SwrContext через swr_alloc_set_opts2
    SwrContext* swr = nullptr;
    int ret = swr_alloc_set_opts2(
        &swr,                                                   // Resampler
        &out_ch_layout, out_fmt, out_rate,                      // Output
        &cctx->ch_layout, cctx->sample_fmt, cctx->sample_rate,  // Input
        0, nullptr                                              // Logging
    );
    if (ret < 0 || !swr) {
        std::cerr << "Не удалось аллоцировать/настроить ресемплер ("
                  << av_err2str(ret) << ")\n";
        throw -1;
    }
    if ((ret = swr_init(swr)) < 0) {
        std::cerr << "swr_init failed: " << av_err2str(ret) << "\n";
        throw -1;
    }

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
        if (pkt->stream_index == stream_index) {
            if (avcodec_send_packet(cctx, pkt) == 0) {
                while (avcodec_receive_frame(cctx, frame) == 0) {
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
    avcodec_send_packet(cctx, nullptr);
    while (avcodec_receive_frame(cctx, frame) == 0) {
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

    // 5) cleanup
    av_frame_free(&frame);
    av_packet_free(&pkt);
    avcodec_free_context(&cctx);
}

inline void Track::Abort() { _is_aborted = true; }