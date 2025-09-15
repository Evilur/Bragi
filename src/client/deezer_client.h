#ifndef BRAGI_DEEZER_CLIENT_H
#define BRAGI_DEEZER_CLIENT_H

#include "track/deezer_track.h"

#include <string>

class DeezerClient final {
public:
    static DeezerTrack* Search(const std::string &query,
                               uint start = 0);

    static std::string GetTrackUrl(const std::string &token);

private:
    enum TrackQuality { MP3_128, MP3_320, FLAC };

    static constexpr char TRACK_QUALITY_STR[][8] = {
        "MP3_128", "MP3_320", "FLAC"
    };

    static constexpr char HEADERS_TEMPLATE[] = "Cookie: arl=";

    static constexpr char UPDATE_SESSION_HOST[] = "www.deezer.com";
    static constexpr char UPDATE_SESSION_URL[] =
    "ajax/gw-light.php?method=deezer.getUserData&api_version=1.0&api_token";

    static constexpr char SEARCH_TRACK_HOST[] = "api.deezer.com";
    static constexpr char SEARCH_TRACK_URL_TEMPLATE[] =
    "1.0/gateway.php?method=search.music&input=3&output=3&sid=%s&"\
    "api_key=ZAIVAHCEISOHWAICUQUEXAEPICENGUAFAEZAIPHAELEEVAHPHUCUFONGUAPASUAY";
    static constexpr char SEARCH_TRACK_BODY_TEMPLATE[] =
    R"({"query":"%s","nb":1,"output":"TRACK","filter":"TRACK","start":%u})";

    static constexpr char GET_TRACK_URL_URL[] = "media.deezer.com/v1/get_url";
    static constexpr char GET_URL_BODY_TEMPLATE[] =
    R"({"license_token":"%s","media":[{"type":"FULL","formats":)"
    R"([{"format":"%s","cipher":"BF_CBC_STRIPE"}]}],"track_tokens":["%s"]})";

    static inline std::string _headers;
    static inline std::string _session_id;
    static inline std::string _license_token;

    static inline std::string _search_track_url;

    static inline unsigned long _session_timestamp = 0;
    static constexpr unsigned int DELTA_TIME = 600;

    static void UpdateSession();
};

#endif
