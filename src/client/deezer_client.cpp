#include "deezer_client.h"
#include "http/http_client.h"
#include "util/settings.h"
#include "util/json.h"
#include "util/logger.h"
#include "player/track/deezer_track.h"
#include "http/https_client.h"

#include <iostream>

void DeezerClient::Init() {
	/* Init the arl otken and headers for the deezer requests */
	_arl_token = Settings::GetArlToken();
	_headers = BASIC_HEADERS + _arl_token;
	UpdateSession(true);
}

DeezerTrack* DeezerClient::Search(const std::string &request, const unsigned int start) {
	/* If the session has been expired update it */
	const unsigned long c_time = time(nullptr);
	if (c_time - _session_timestamp > DELTA_TIME) UpdateSession();
	else _session_timestamp = c_time;

	/* Send the request and init the json objects */
	const char* json_string = HttpClient(_url_search_track, _headers, std::format(BODY_TEMPLATE_SEARCH_TRACK, request, start), "POST").ReadAll();
	const Json json_results = Json(json_string).Get("results");

	/* Check for search results */
	if (!json_results.Has("data")) return nullptr;
	const unsigned short total = (unsigned short)json_results.Get("total");
	if (total == 0) return nullptr;
	const Json json_track = json_results.Get("data").Get(0);

	/* Create the track instance */
	const std::string track_url = GetEncodedTrackUrl((std::string)json_track.Get("TRACK_TOKEN"));
	DeezerTrack* result =
			new DeezerTrack((std::string)json_track.Get("SNG_ID"), (std::string)json_track.Get("ALB_ID"), (std::string)json_track.Get("ART_ID"),
			                (std::string)json_track.Get("SNG_TITLE"), (std::string)json_track.Get("ALB_TITLE"), (std::string)json_track.Get("ART_NAME"),
			                (std::string)json_track.Get("ALB_PICTURE"), (std::string)json_track.Get("ART_PICTURE"),
			                (std::string)json_track.Get("DURATION"), "NOT IMPLEMENTED YET",
			                (unsigned short)json_results.Get("total"), (unsigned short)json_results.Get("next"));

	/* Free the memory and return a result */
	delete[] json_string;
	json_string = nullptr;
	return result;
}


void DeezerClient::UpdateSession(const bool verbose) {
	/* Send the request and init the json objects */
	const char* json_string = HttpClient(URL_UPDATE_SESSION, _headers).ReadAll();
	const Json json_results = Json(std::strchr(json_string, '{')).Get("results");
	const Json json_user = json_results.Get("USER");

	/* Check the user for existing */
	if ((unsigned int)json_user.Get("USER_ID") == 0) {
		Logger::Fatal("Invalid Deezer ARL token");
		exit(100);
	}

	/* Assign values to fields */
	_session_id = (std::string)json_results.Get("SESSION_ID");
	_license_token = (std::string)json_user.Get("OPTIONS").Get("license_token");
	_session_timestamp = (unsigned long)json_results.Get("SERVER_TIMESTAMP");

	/* Update the dependent urls */
	_url_search_track = URL_TEMPLATE_SEARCH_TRACK + _session_id;
	_url_get_decoded_track_url = URL_TEMPLATE_GET_DECODED_TRACK_URL + _session_id;

	/* Get user data for logging */
	if (verbose) {
		const std::string user_name = (std::string)json_user.Get("BLOG_NAME");
		const std::string user_email = (std::string)json_user.Get("EMAIL");
		const std::string user_offer = (std::string)json_results.Get("OFFER_NAME");

		/* Log the user data */
		Logger::Info(std::format("Log in Deezer as \"{}\" <{}>", user_name, user_email));
		Logger::Info(std::format("Current Deezer offer - {}", user_offer));
	}

	/* Free the memory */
	delete[] json_string;
	json_string = nullptr;
}

std::string DeezerClient::GetEncodedTrackUrl(const std::string &token, Quality quality) {
	do {
		/* Send the request */
		const char* json_string =
				HttpsClient(_url_get_decoded_track_url, _headers, std::format(BODY_TEMPLATE_GET_DECODED_TRACK_URL, _license_token, "FLAC", token),
				            "POST").ReadAll();

		Json json_media = Json(json_string).Get("data").Get(0).Get("media");

		/* If there is no track in such quality */
		if (json_media.IsEmpty()) continue;

		/* If the track in such quality exists */
		delete[] json_string;
		json_string = nullptr;
		return (std::string)json_media.Get(0).Get("sources").Get(0).Get("url");
	} while ((quality = (Quality)(quality - 1)) >= 0);

	throw std::logic_error("Cannot find the encoded track url");
}