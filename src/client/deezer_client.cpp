#include "deezer_client.h"
#include "http/http_client.h"
#include "util/settings.h"
#include "util/json.h"
#include "util/logger.h"
#include "player/track/deezer_track.h"
#include "http/https_client.h"

#include <iostream>

DeezerClient::TrackQuality operator++(DeezerClient::TrackQuality &quality, int) {
	quality = (DeezerClient::TrackQuality)(quality + 1);
	return quality;
}

DeezerClient::TrackQuality operator--(DeezerClient::TrackQuality &quality, int) {
	quality = (DeezerClient::TrackQuality)(quality - 1);
	return quality;
}

void DeezerClient::Init() {
	/* Init the arl otken and headers for the deezer requests */
	_arl_token = Settings::GetArlToken();
	_headers = BASIC_HEADERS + _arl_token;
	UpdateSession(true);
}

DeezerTrack* DeezerClient::Search(const std::string &query, const unsigned int start) {
	/* If the session has been expired update it */
	const unsigned long c_time = time(nullptr);
	if (c_time - _session_timestamp > DELTA_TIME) UpdateSession();
	else _session_timestamp = c_time;

	/* Send the request and init the json objects */
	const char* json_string = HttpClient(_url_search_track, _headers, std::format(BODY_TEMPLATE_SEARCH_TRACK, query, start), "POST").ReadAll();
	const Json json_results = Json(json_string).Get("results");

	/* Check for search results */
	if (!json_results.Has("data")) return nullptr;
	const unsigned short total = (unsigned short)json_results["total"];
	if (total == 0) return nullptr;
	const Json json_track = json_results["data"][0];

	/* Create the track instance */
	DeezerTrack* result =
			new DeezerTrack((std::string)json_track["SNG_ID"], (std::string)json_track["ALB_ID"], (std::string)json_track["ART_ID"],
			                (std::string)json_track["SNG_TITLE"], (std::string)json_track["ALB_TITLE"], (std::string)json_track["ART_NAME"],
			                (std::string)json_track["ALB_PICTURE"], (std::string)json_track["ART_PICTURE"],
			                (std::string)json_track["DURATION"], (std::string)json_track["TRACK_TOKEN"],
			                (unsigned short)json_results["total"], (unsigned short)json_results["next"]);

	/* Free the memory and return a result */
	delete[] json_string;
	json_string = nullptr;
	return result;
}

std::string DeezerClient::GetEncodedTrackUrl(const std::string &token, TrackQuality quality) {
	do {
		/* Send the request */
		const char* json_string =
				HttpsClient(_url_get_decoded_track_url, _headers, std::format(BODY_TEMPLATE_GET_DECODED_TRACK_URL, _license_token, "FLAC", token),
				            "POST").ReadAll();

		Json json_media = Json(json_string)["data"][0]["media"];

		/* If there is no track in such quality */
		if (json_media.IsEmpty()) continue;
		std::string result = (std::string)json_media[0]["sources"][0]["url"];

		/* If the track in such quality exists */
		delete[] json_string;
		json_string = nullptr;
		return result;
	} while (quality-- >= 0);  //If there is no such track quality decrese it

	throw std::logic_error("Cannot find the encoded track url");
}

void DeezerClient::UpdateSession(const bool verbose) {
	/* Send the request and init the json objects */
	const char* json_string = HttpClient(URL_UPDATE_SESSION, _headers).ReadAll();
	const Json json_results = Json(std::strchr(json_string, '{')).Get("results");
	const Json json_user = json_results["USER"];

	/* Check the user for existing */
	if ((unsigned int)json_user["USER_ID"] == 0) {
		Logger::Fatal("Invalid Deezer ARL token");
		exit(100);
	}

	/* Assign values to fields */
	_session_id = (std::string)json_results["SESSION_ID"];
	_license_token = (std::string)json_user["OPTIONS"]["license_token"];
	_session_timestamp = (unsigned long)json_results["SERVER_TIMESTAMP"];

	/* Update the dependent urls */
	_url_search_track = URL_TEMPLATE_SEARCH_TRACK + _session_id;
	_url_get_decoded_track_url = URL_TEMPLATE_GET_DECODED_TRACK_URL + _session_id;

	/* Get user data for logging */
	if (verbose) {
		const std::string user_name = (std::string)json_user["BLOG_NAME"];
		const std::string user_email = (std::string)json_user["EMAIL"];
		const std::string user_offer = (std::string)json_results["OFFER_NAME"];

		/* Log the user data */
		Logger::Info(std::format("Log in Deezer as \"{}\" <{}>", user_name, user_email));
		Logger::Info(std::format("Current Deezer subscription - {}", user_offer));
	}

	/* Free the memory */
	delete[] json_string;
	json_string = nullptr;
}