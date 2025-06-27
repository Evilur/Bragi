#include "track.h"

Track::~Track() { opus_encoder_destroy(_encoder); }

void Track::Abort() {
}

void Track::Play(dpp::discord_voice_client *const voice_client,
                 const unsigned char playback_rate) {
    /* Allocate the format context */
    AVFormatContext *format_context = avformat_alloc_context();

    /* Allocate the avio context */
    AVIOContext *avio_context =
        avio_alloc_context((unsigned char *)av_malloc(GetAudioBufferSize()),
                           GetAudioBufferSize(),
                           0,
                           this,
                           GetReadAudioCallback(),
                           nullptr,
                           nullptr);

    /* Set avio context to the format context */
    format_context->pb = avio_context;

    // 1.2) Открываем формат (FFmpeg сам «угадает» контейнер/кодек по заголовкам из потока)
    if (avformat_open_input(&format_context, nullptr, nullptr, nullptr) < 0) {
        std::cerr << "Не удалось открыть входной поток\n";
        throw -1;
    }
    if (avformat_find_stream_info(format_context, nullptr) < 0) {
        std::cerr << "Не удалось найти информацию о потоках\n";
        throw -1;
    }

    // 2) Ищем аудиопоток и инициализируем декодер
    const AVCodec *codec = nullptr;
    int stream_index = av_find_best_stream(format_context, AVMEDIA_TYPE_AUDIO,
                                           -1, -1,
                                           &codec, 0);
    if (stream_index < 0) {
        std::cerr << "Аудиопоток не найден\n";
        throw -1;
    }
    AVStream *audio_st = format_context->streams[stream_index];
    AVCodecContext *cctx = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(cctx, audio_st->codecpar);
    if (avcodec_open2(cctx, codec, nullptr) < 0) {
        std::cerr << "Не удалось открыть декодер\n";
        throw -1;
    }

    // 3) Бьем пакеты, декодируем, и сбрасываем raw PCM в out_pcm
    AVPacket *pkt = av_packet_alloc();
    AVFrame *frame = av_frame_alloc();

    while (av_read_frame(format_context, pkt) >= 0) {
        if (pkt->stream_index == stream_index) {
            if (avcodec_send_packet(cctx, pkt) == 0) {
                while (avcodec_receive_frame(cctx, frame) == 0) {
                    // frame->data содержит декодированные PCM-сэмплы
                    int data_size = av_samples_get_buffer_size(
                        nullptr,
                        2,
                        frame->nb_samples,
                        cctx->sample_fmt,
                        1
                        );

                    for (int i = 0; i < data_size; i++) {
                        *_pcm_buffer_ptr++ = frame->data[0][i];

                        if (_pcm_buffer_ptr < _pcm_buffer_end)
                            continue;
                        _pcm_buffer_ptr = (char *)_pcm_buffer;

                        unsigned char opus_buffer[OPUS_CHUNK_SIZE];
                        const int len = opus_encode(
                            _encoder, _pcm_buffer, FRAME_SIZE, opus_buffer,
                            OPUS_CHUNK_SIZE);
                        voice_client->send_audio_opus(opus_buffer, len, 60);
                    }
                }
            }
        }
        av_packet_unref(pkt);
    }

    /* Send EOF marker */
    voice_client->insert_marker();

    /* Free the memory */
    av_free(avio_context->buffer);
    avio_context_free(&avio_context);
    avformat_close_input(&format_context);
    avformat_free_context(format_context);

    // 5) cleanup
    av_frame_free(&frame);
    av_packet_free(&pkt);
    avcodec_free_context(&cctx);
}