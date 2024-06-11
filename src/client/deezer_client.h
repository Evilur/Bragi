#ifndef BRAGI_DEEZER_CLIENT_H
#define BRAGI_DEEZER_CLIENT_H

#include <string>

class DeezerClient final {
public:
	static void Init();

private:
	static inline std::string _arl_token;
	static inline std::string _headers;

	static inline std::string _session_id;
	static inline std::string _license_token;

	static constexpr char BASIC_HEADERS[] = "Accept: application/json, text/plain, */*\n"
	                                        "Content-Type: text/plain;charset=UTF-8\n"
	                                        "User-Agent: Deezer/7.17.0.2 CFNetwork/1098.6 Darwin/19.0.0\n"
	                                        "Cache-Control: max-age=0\n"
	                                        "Accept-Language: en-US,en;q=0.9,en-US;q=0.8,en;q=0.7\n"
	                                        "Accept-Charset: utf-8,ISO-8859-1;q=0.8,*;q=0.7\n"
	                                        "Cookie: arl=";

	/** Update the deezer session
	 * @param verbose true if we need to log the user data; false else
	 */
	static void UpdateSession(bool verbose = false);
};

#endif