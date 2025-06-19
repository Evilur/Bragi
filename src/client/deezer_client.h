#ifndef BRAGI_DEEZER_CLIENT_H
#define BRAGI_DEEZER_CLIENT_H

#include "track/deezer_track.h"
#include "util/properties.h"
#include "types/base.h"

#include <string>

class DeezerClient final {
public:
	static DeezerTrack* Search(const std::string &query,
                               uint start = 0);

	static std::string GetTrackUrl(const std::string &token,
                                   DeezerTrack::Quality quality);

private:
    static constexpr char TRACK_QUALITY_STR[][8] = {
        "MP3_128", "MP3_320", "FLAC"
    };

    static constexpr char HEADERS_TEMPLATE[] =
    "Accept: application/json, text/plain, */*\n"\
    "Content-Type: text/plain;charset=UTF-8\n"\
    "User-Agent: Deezer/7.17.0.2 CFNetwork/1098.6 Darwin/19.0.0\n"\
    "Cache-Control: max-age=0\n"\
    "Accept-Language: en-US,en;q=0.9,en-US;q=0.8,en;q=0.7\n"\
    "Accept-Charset: utf-8,ISO-8859-1;q=0.8,*;q=0.7\n"\
    "Cookie: arl=";

    static constexpr char UPDATE_SESSION_URL[] =
    "www.deezer.com/ajax/gw-light.php?version=8.32.0&api_key="\
    "ZAIVAHCEISOHWAICUQUEXAEPICENGUAFAEZAIPHAELEEVAHPHUCUFONGUAPASUAY&output="\
    "3&input=3&buildId=ios12_universal&screenHeight=480&screenWidth=320&lang="\
    "en&method=deezer.getUserData&api_version=1.0&api_token";

    static constexpr char SEARCH_TRACK_URL_TEMPLATE[] =
    "api.deezer.com/1.0/gateway.php?api_key="\
    "ZAIVAHCEISOHWAICUQUEXAEPICENGUAFAEZAIPHAELEEVAHPHUCUFONGUAPASUAY&output="\
    "3&input=3&method=search.music&sid=";

    static constexpr char SEARCH_TRACK_BODY_TEMPLATE[] =
    "{{\"query\":\"{}\",\"nb\":1,\"output\":\"TRACK\","\
    "\"filter\":\"TRACK\",\"start\":{}}}";

    static constexpr char GET_URL_URL_TEMPLATE[] =
    "media.deezer.com/v1/get_url?version=8.32.0&api_key="\
    "ZAIVAHCEISOHWAICUQUEXAEPICENGUAFAEZAIPHAELEEVAHPHUCUFONGUAPASUAY&output="\
    "3&input=3&buildId=ios12_universal&screenHeight=480&screenWidth=320&lang="\
    "en&sid=";

    static constexpr char GET_URL_BODY_TEMPLATE[] =
    "{{\"license_token\":\"{}\",\"media\":[{{\"type\":\"FULL\",\"formats\":"\
    "[{{\"format\":\"{}\",\"cipher\":\"BF_CBC_STRIPE\"}}]}}],"\
    "\"track_tokens\":[\"{}\"]}}";

	static inline std::string _headers;
	static inline std::string _session_id;
	static inline std::string _license_token;

	static inline std::string _search_track_url;
	static inline std::string _get_track_url_url;

	static inline unsigned long _session_timestamp = 0;
	static constexpr unsigned int DELTA_TIME = 600;

	/** Update the deezer session
	 * @param verbose true if we need to log the user data; false else
	 */
	static void UpdateSession();
};

#endif
