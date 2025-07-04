#ifndef BRAGI_TRACK_H
#define BRAGI_TRACK_H

#include "base/bragi.h"

#include <dpp/dpp.h>
#include <string>
#include <opus/opus.h>

class Track {
public:
    virtual ~Track();

    virtual dpp::message GetMessage(const bool &is_playing_now,
                                    const dpp::snowflake &channel_id) const = 0;

    virtual std::string GetTrackData() const = 0;

    virtual Track *Next() const = 0;

    virtual void Play(Bragi::Player& player);

    inline void Abort();

protected:
    using ffmpeg_read_callback = int(*)(void*, unsigned char*, int);

    virtual constexpr ffmpeg_read_callback GetReadAudioCallback() const = 0;

    virtual constexpr int GetAudioBufferSize() const = 0;

private:
    static constexpr int FREQ = 48000;
    static constexpr int FRAME_SIZE = 2880;
    static constexpr int CHANNELS = 2;
    static constexpr int PCM_CHUNK_SIZE = FRAME_SIZE * CHANNELS * sizeof(short);
    static constexpr int OPUS_CHUNK_SIZE = 1024;

    OpusEncoder* _encoder = opus_encoder_create(FREQ,
                                                CHANNELS,
                                                OPUS_APPLICATION_AUDIO,
                                                nullptr);
    bool _is_aborted = false;
};

#endif