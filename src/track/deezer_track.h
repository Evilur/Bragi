#ifndef BRAGI_DEEZER_TRACK_H
#define BRAGI_DEEZER_TRACK_H

#include "track.h"
#include "locale/locale.h"
#include "web/http_client.h"
#include "master.h"

#include <openssl/blowfish.h>

class DeezerTrack final : public Track {
public:
    DeezerTrack(const unsigned short track_duration,
                const unsigned int track_id, const std::string &track_title,
                const std::string &track_token,
                const unsigned int album_id, const std::string &album_title,
                const std::string &album_picture_id,
                const unsigned int artist_id, const std::string &artist_name,
                const std::string &artist_picture_id,
                const unsigned short search_total,
                const unsigned short search_next,
                const std::string &search_query);

    ~DeezerTrack() override;

    void Play(Bragi::Player& player) override;

    dpp::message GetMessage(const bool &is_playing_now,
                            const dpp::snowflake &channel_id) const override;

    std::string GetTrackData() const override;

    Track *Next() const override;

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
    std::thread *_init_thread = nullptr;
    HttpClient *_http = nullptr;

    static constexpr int DEEZER_AUDIO_CHUNK_SIZE = 2048;

    void GetKey(unsigned char *buffer);

    constexpr ffmpeg_read_callback GetReadAudioCallback() override;

    int GetAudioBufferSize() override;

    static int ReadDeezerAudio(void *opaque_context,
                          unsigned char *buffer, int buffer_size);
};

#endif