#ifndef BRAGI_DEEZER_CLIENT_H
#define BRAGI_DEEZER_CLIENT_H

#include "player/track/deezer_track.h"

#include <string>

class DeezerClient final {
public:
	static void Init();

	static DeezerTrack* Search(const std::string &request, unsigned int start = 0);

private:
	static inline std::string _arl_token;
	static inline std::string _headers;

	static inline std::string _session_id;
	static inline std::string _license_token;

	static inline unsigned long _session_timestamp;
	static constexpr unsigned int DELTA_TIME = 600;

	static constexpr char BASIC_HEADERS[] = "Accept: application/json, text/plain, */*\n"
	                                        "Content-Type: text/plain;charset=UTF-8\n"
	                                        "User-Agent: Deezer/7.17.0.2 CFNetwork/1098.6 Darwin/19.0.0\n"
	                                        "Cache-Control: max-age=0\n"
	                                        "Accept-Language: en-US,en;q=0.9,en-US;q=0.8,en;q=0.7\n"
	                                        "Accept-Charset: utf-8,ISO-8859-1;q=0.8,*;q=0.7\n"
	                                        "Cookie: arl=";
	static constexpr char URL_UPDATE_SESSION[] = "www.deezer.com/ajax/gw-light.php?version=8.32.0"
	                                             "&api_key=ZAIVAHCEISOHWAICUQUEXAEPICENGUAFAEZAIPHAELEEVAHPHUCUFONGUAPASUAY&output=3&input=3"
	                                             "&buildId=ios12_universal&screenHeight=480&screenWidth=320&lang=en&method=deezer.getUserData"
	                                             "&api_version=1.0&api_token";
	static constexpr char URL_TEMPLATE_SEARCH_TRACK[] = "http://api.deezer.com/1.0/gateway.php?"
	                                                    "api_key=ZAIVAHCEISOHWAICUQUEXAEPICENGUAFAEZAIPHAELEEVAHPHUCUFONGUAPASUAY&output=3&input=3"
	                                                    "&method=search.music&sid=";

	static inline std::string _url_search_track;

	/** Update the deezer session
	 * @param verbose true if we need to log the user data; false else
	 */
	static void UpdateSession(bool verbose = false);
};

#endif