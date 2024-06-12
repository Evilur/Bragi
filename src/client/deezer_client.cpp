#include "deezer_client.h"
#include "http/http_client.h"
#include "util/settings.h"
#include "util/json.h"
#include "util/logger.h"
#include "player/track/deezer_track.h"

#include <iostream>

void DeezerClient::Init() {
	/* Init the arl otken and headers for the deezer requests */
	_arl_token = Settings::GetArlToken();
	_headers = BASIC_HEADERS + _arl_token;
	UpdateSession(true);
}

DeezerTrack* DeezerClient::Search(const std::string &request, const unsigned int start) {
	/* Send the request and init the json objects */
	const char* json_data = HttpClient(_url_search_track,
	                                   _headers,
	                                   std::format(R"({{"query":"{}","nb":1,"output":"TRACK","filter":"TRACK","start":{}}})", request, start),
	                                   "POST").ReadAll();
	const Json json(json_data);
	const Json errors = json.Get("errors");
	const Json results = json.Get("results");
	const Json track = json.Get("result").Get("data").Get(0);

	/* Create the track instance */
	DeezerTrack* result = new DeezerTrack((unsigned int)track.Get("SNG_ID"), (unsigned int)track.Get("ALB_ID"), (unsigned int)track.Get("ART_ID"),
	                                      (std::string)track.Get("SNG_TITLE"), (std::string)track.Get("ALB_TITLE"), (std::string)track.Get("ART_NAME"),
	                                      (std::string)track.Get("ALB_PICTURE"), (std::string)track.Get("ART_PICTURE"),
	                                      (std::string)track.Get("TRACK_TOKEN"), (unsigned short)track.Get("DURATION"));

	delete[] json_data;
	json_data = nullptr;

	return result;
}


void DeezerClient::UpdateSession(const bool verbose) {
	/* Send the request and init the json objects */
	const char* json_data = HttpClient(URL_UPDATE_SESSION, _headers).ReadAll();
	const Json results = Json(std::strchr(json_data, '{')).Get("results");
	const Json user = results.Get("USER");

	/* Check the user for existing */
	if ((unsigned int)user.Get("USER_ID") == 0) {
		Logger::Fatal("Invalid Deezer ARL token");
		exit(100);
	}

	/* Assign values to fields */
	_session_id = (std::string)results.Get("SESSION_ID");
	_license_token = (std::string)user.Get("OPTIONS").Get("license_token");

	/* Update the dependent urls */
	_url_search_track = URL_TEMPLATE_SEARCH_TRACK + _session_id;

	/* Get user data for logging */
	if (verbose) {
		const std::string user_name = (std::string)user.Get("BLOG_NAME");
		const std::string user_email = (std::string)user.Get("EMAIL");
		const std::string user_offer = (std::string)results.Get("OFFER_NAME");

		/* Log the user data */
		Logger::Info(std::format("Log in Deezer as \"{}\" <{}>", user_name, user_email));
		Logger::Info(std::format("Current Deezer offer - {}", user_offer));
	}

	/* Free the memory */
	delete[] json_data;
	json_data = nullptr;
}