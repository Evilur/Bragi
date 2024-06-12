#include <iostream>
#include "deezer_client.h"
#include "http/http_client.h"
#include "util/settings.h"
#include "util/json.h"
#include "util/logger.h"

void DeezerClient::Init() {
	/* Init the arl otken and headers for the deezer requests */
	_arl_token = Settings::GetArlToken();
	_headers = BASIC_HEADERS + _arl_token;
	UpdateSession(true);
}

void DeezerClient::Search(const std::string &request, const unsigned int) {

}


void DeezerClient::UpdateSession(const bool verbose) {
	/* Send the request to init the session */
	HttpClient cl("www.deezer.com/ajax/gw-light.php?version=8.32.0&api_key=ZAIVAHCEISOHWAICUQUEXAEPICENGUAFAEZAIPHAELEEVAHPHUCUFONGUAPASUAY"
	              "&output=3&input=3&buildId=ios12_universal&screenHeight=480&screenWidth=320&lang=en&method=deezer.getUserData"
	              "&api_version=1.0&api_token", _headers);

	/* Init the json objects */
	const char* json_data = cl.ReadAll();
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

	/* Get user data for logging */
	if (verbose) {
		const std::string user_name = (std::string)user.Get("BLOG_NAME");
		const std::string user_email = (std::string)user.Get("EMAIL");
		const std::string user_premium = (std::string)results.Get("OFFER_NAME");
		Logger::Info(std::format("Creating a Deezer session by \"{}\" <{}> ({})", user_name, user_email, user_premium));
	}

	/* Free the memory */
	delete[] json_data;
	json_data = nullptr;
}