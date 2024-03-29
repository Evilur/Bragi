#ifndef BRAGI_GUILD_H
#define BRAGI_GUILD_H

#include <dpp/cluster.h>

class Guild {
public:
	const dpp::snowflake guild_id;
	
	explicit Guild(dpp::snowflake* guild_id);
	
	/* Get a guild from the array
	 * return: a founded guild from the array */
	static Guild* Get(dpp::snowflake guild_id);
private:
	static inline const unsigned int guilds_delta = 8;
	static inline unsigned int _guild_max_count = guilds_delta;
	static inline unsigned int _guild_count = 0;
	static inline Guild** _guilds = new Guild*[guilds_delta];
	
	/* Add a new guild to the array
	 * return: a new guild object */
	static Guild* Add(dpp::snowflake guild_id);
};

#endif
