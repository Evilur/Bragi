#include "deezer_track.h"
#include "util/parser.h"
#include "util/color.h"
#include "util/logger.h"
#include "client/deezer_client.h"
#include "master.h"

#include <openssl/md5.h>
#include <openssl/blowfish.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

DeezerTrack::DeezerTrack(const unsigned short track_duration, Quality quality,
                         const unsigned int track_id,
                         const std::string &track_title,
                         const std::string &track_token,
                         const unsigned int album_id,
                         const std::string &album_title,
                         const std::string &album_picture_id,
                         const unsigned int artist_id,
                         const std::string &artist_name,
                         const std::string &artist_picture_id,
                         const unsigned short search_total,
                         const unsigned short search_next,
                         const std::string &search_query) :
    _quality(quality),
    _track(track_id, track_title, track_token),
    _album(album_id, album_title, album_picture_id),
    _artist(artist_id, artist_name, artist_picture_id),
    _search(search_total, search_next, search_query) {
    /* Get the encypted data url in the new thread */
    _init_thread = new std::thread([this]() {
        /* Set the url of the encrypted track data */
        _data_url = DeezerClient::GetTrackUrl(_track.token, _quality);

        /* Set the blowfish cipher key */
        unsigned char key_buffer[MD5_DIGEST_LENGTH];
        GetKey(key_buffer);
        BF_set_key(&_bf_key, MD5_DIGEST_LENGTH, key_buffer);
    });
}

DeezerTrack::~DeezerTrack() {
    if (_init_thread->joinable())
        _init_thread->join();
    delete _init_thread;
    _init_thread = nullptr;

    delete _http;
    _http = nullptr;
}

dpp::message DeezerTrack::GetMessage(const bool &is_playing_now,
                                     const dpp::snowflake &channel_id) const {
    std::string msg_body = '\n' + "14м88с";

    if (is_playing_now)
        msg_body.insert(0, std::format(DIC_TRACK_PLAYING_NOW, _track.title));
    else
        msg_body.
            insert(0, std::format(DIC_TRACK_ADD_TO_PLAYLIST, _track.title));

    dpp::message result = dpp::message(channel_id, msg_body);
    result.add_embed(dpp::embed()
                     .set_color(Color::RED)
                     .add_field(DIC_TRACK_ALBUM, _album.title)
                     .add_field(DIC_TRACK_ARTIST, _artist.name)
                     .set_image(
                         "https://e-cdns-images.dzcdn.net/images/cover/" +
                         _album.picture_id + "/1000x1000-000000-80-0-0.jpg")
                     .set_thumbnail(
                         "https://e-cdns-images.dzcdn.net/images/artist/" +
                         _artist.picture_id + "/1000x1000-000000-80-0-0.jpg"));
    return result;
}

std::string DeezerTrack::GetTrackData() const {
    return std::format(
        DIC_SLASH_LIST_FULL_TRACK_DATA, _track.title, _artist.name);
}

Track *DeezerTrack::Next() const {
    if (_search.next >= _search.total)
        return nullptr; //TODO: Check this
    return DeezerClient::Search(_search.next_query, _search.next);
}

void DeezerTrack::GetKey(unsigned char *buffer) {
    /* The salt for getting the key for track decrypting */
    constexpr unsigned char salt[] = "g4el58wc0zvf9na1";

    /* Get the md5 hash sum of the track id */
    const std::string id_str = std::to_string(_track.id);
    unsigned char md5_digest[MD5_DIGEST_LENGTH];
    MD5((unsigned char *)id_str.c_str(), id_str.size(), md5_digest);
    unsigned char md5_sum[MD5_DIGEST_LENGTH * 2];
    constexpr unsigned char alph[] = "0123456789abcdef";
    for (char i = 0; i < MD5_DIGEST_LENGTH; i++) {
        const char cur_i = i * 2;
        md5_sum[cur_i] = alph[md5_digest[i] >> 4];
        md5_sum[cur_i + 1] = alph[md5_digest[i] & 0xF];
    }

    /* Build the key */
    const unsigned char *md5_sum_fst_half = md5_sum;
    //Get the first half of the hash
    const unsigned char *md5_sum_sec_half = md5_sum + MD5_DIGEST_LENGTH;
    //Get the second half of the hash
    for (char i = 0; i < MD5_DIGEST_LENGTH; i++)
        buffer[i] = salt[i] ^ md5_sum_fst_half[i] ^ md5_sum_sec_half[i];
}


static int GetPCMData(void *opaque_context, unsigned char *buffer, int buffer_size) {
    static std::ifstream input("/home/evilur/Downloads/fuck.flac");
    const Track* ctx = (Track*)(opaque_context);
    input.read(reinterpret_cast<char*>(buffer), buffer_size);
    const int bytes_read = static_cast<int>(input.gcount());

    if (bytes_read == 0) return AVERROR_EOF;
    return bytes_read;
}
Track::ffmpeg_read_callback DeezerTrack::ReadPCMCallback() {
return GetPCMData;
}

#pragma GCC diagnostic pop