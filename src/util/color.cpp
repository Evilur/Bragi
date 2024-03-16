#include "color.h"
#include "util/logger.h"

int Color::Get(const char* c_code) {
	std::string parsed = c_code;
	parsed = parsed.substr(parsed.find('#') + 1);
	Logger::Debug(stoi(parsed, nullptr, 16));
	return stoi(parsed, nullptr, 16);
}
