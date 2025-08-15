#include "track.h"

#include "locale/locale.h"
#include "types/string.hpp"
#include "util/logger.hpp"
#include "util/time.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
}

Track::~Track() {
    /* Destroy the opus encoder */
    opus_encoder_destroy(_encoder);
}

void Track::Play(Bragi::Player& player) {
    /* Log */
    TRACE_LOG("Start playing the track");

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

    /* Allocate resampled data buffer */
    unsigned char** resampled_data = nullptr;
    int resampled_samples_number = 0;
    av_samples_alloc_array_and_samples(
        &resampled_data,
        nullptr,
        CHANNELS,
        FRAME_SIZE,
        out_fmt,
        0
    );

    /* Allocate packer and frame */
    AVPacket* packet = av_packet_alloc();
    AVFrame* frame = av_frame_alloc();

    /* Send OPUS data to the discord */
    auto send_data = [&] {
        /* Resample audio */
        const int out_samples = swr_convert(
            swr,
            resampled_data,
            FRAME_SIZE - resampled_samples_number,
            frame->data,
            frame->nb_samples
        );

        /* Check for out samples number */
        if (out_samples == 0) {
            /* Reset the offset */
            resampled_data[0] -=
                resampled_samples_number * sizeof(short) * CHANNELS;
            resampled_samples_number = 0;

            /* Exit the lambda */
            return false;
        }

        /* Increase the offset */
        resampled_data[0] += out_samples * sizeof(short) * CHANNELS;
        resampled_samples_number += out_samples;

        /* If we have not enough pcm data, exit the lambda */
        if (resampled_samples_number < FRAME_SIZE) return true;

        /* Reset the offset */
        resampled_data[0] -= FRAME_SIZE * sizeof(short) * CHANNELS;
        resampled_samples_number = 0;

        /* Convert PCM to OPUS */
        unsigned char opus_buffer[OPUS_CHUNK_SIZE];
        const int opus_len = opus_encode(_encoder,
                                         (short*)resampled_data[0],
                                         FRAME_SIZE,
                                         opus_buffer,
                                         OPUS_CHUNK_SIZE);

        /* Send data to the discord and exit */
        player.voice_client->send_audio_opus(opus_buffer, opus_len, 60);
        return true;
    };

    /* Read data and send to the discord */
    while (av_read_frame(format_ctx, packet) >= 0) {
        avcodec_send_packet(codec_ctx, packet);
        while (avcodec_receive_frame(codec_ctx, frame) == 0) {
            if (_is_aborted) goto end;
            send_data();
        }
        av_packet_unref(packet);
    }

    /* Codec drain */
    avcodec_send_packet(codec_ctx, nullptr);
    while (avcodec_receive_frame(codec_ctx, frame) == 0) {
        if (_is_aborted) goto end;
        send_data();
    }

    /* Resampler drain */
    while (send_data() && !_is_aborted) { }

    /* Send EOF marker */
    player.voice_client->insert_marker();
    TRACE_LOG("Track has been fully sent to the voice client");

end:
    /* Packet and frame */
    av_packet_free(&packet);
    av_frame_free(&frame);

    /* Resampler buffer */
    av_freep(&resampled_data[0]);
    av_freep(&resampled_data);

    /* Codec context */
    avcodec_free_context(&codec_ctx);

    /* Resampler context */
    swr_free(&swr);

    /* AVIO context */
    av_free(format_ctx->pb->buffer);
    avio_context_free(&format_ctx->pb);

    /* Format context */
    avformat_close_input(&format_ctx);
    avformat_free_context(format_ctx);
}

void Track::Abort() { _is_aborted = true; }

dpp::message Track::GetMessage(const bool is_currently_playing,
                               const char* const track_title,
                               const unsigned int duration) {
    return {
        String::Format(is_currently_playing
                           ? _("**:notes: Currently playing: `%s`\n"
                               ":watch: Duration: `%s`**")
                           : _("**:notes: Added to playlist: `%s`\n"
                               ":watch: Duration: `%s`**"),
                       track_title,
                       (const char*)Time::Format(duration))
    };
}