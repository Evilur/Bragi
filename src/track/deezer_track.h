#pragma once

#include "track.h"
#include "locale/locale.h"
#include "web/http_client.h"
#include "master.h"

#include <openssl/blowfish.h>

class DeezerTrack final : public Track {
public:
    DeezerTrack(unsigned short track_duration,
                unsigned int track_id, const std::string &track_title,
                const std::string &track_token,
                unsigned int album_id, const std::string &album_title,
                const std::string &album_picture_id,
                unsigned int artist_id, const std::string &artist_name,
                const std::string &artist_picture_id,
                unsigned short search_total,
                unsigned short search_next,
                const std::string &search_query);

    ~DeezerTrack() override;

    dpp::message GetMessage(bool is_currently_playing) const override;

    std::string GetTrackData() const override;

    Track *Next() const override;

    void Play(Bragi::Player& player) override;

private:
    const class {
    public:
        const unsigned int id;
        const unsigned int duration;
        const std::string title;
        const std::string token;
    } _track;

    const class {
    public:
        const unsigned int id;
        const std::string title;
        const std::string picture_id;
    } _album;

    const class {
    public:
        const unsigned int id;
        const std::string name;
        const std::string picture_id;
    } _artist;

    const class {
    public:
        const unsigned short total;
        const unsigned short next;
        const std::string next_query;
    } _search;

    BF_KEY _bf_key;
    std::string _data_url;
    std::thread _init_thread;
    HttpClient *_http = nullptr;

    static constexpr int DEEZER_AUDIO_CHUNK_SIZE = 2048;

    void GetKey(unsigned char *buffer);

    constexpr ffmpeg_read_callback GetReadAudioCallback() const override;

    constexpr int GetAudioBufferSize() const override;

    static int ReadDeezerAudio(void *opaque_context,
                          unsigned char *buffer, int buffer_size);
};