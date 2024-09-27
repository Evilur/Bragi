#ifndef BRAGI_NEXT_H
#define BRAGI_NEXT_H

#include <dpp/dpp.h>

class Next final {
public:
	static void Exec(const dpp::slashcommand_t &event);

	static void Exec(const dpp::message_create_t &event, const std::string &argument);
};

#endif