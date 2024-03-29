#include "guild.h"
#include "util/logger.h"

Guild::Guild(dpp::snowflake* guild_id) : guild_id(*guild_id) {
	
}

Guild* Guild::Get(dpp::snowflake guild_id) {
	/* Try to get the guild in the array */
	for (unsigned int i = 0; i < _guild_count; i++)
		if (_guilds[i]->guild_id == guild_id) return _guilds[i];
	
	/* If there is not a such guild we need to add it to the array */
	return Add(guild_id);
}

Guild* Guild::Add(dpp::snowflake guild_id) {
	/* Increase the number of guilds */
	_guild_count++;
	
	/* Try to get the empty place for the pointer */
	for (unsigned int i = _guild_max_count - guilds_delta; i < _guild_max_count; i++) {
		if (_guilds[i] != nullptr) continue;
		_guilds[i] = new Guild(&guild_id);
		return _guilds[i];
	}
	
	/* If the array with pointer is full we need to increase it */
	auto** new_guilds = new Guild*[_guild_max_count + guilds_delta];
	for (unsigned int i = 0; i < _guild_max_count; i++) new_guilds[i] = _guilds[i];
	delete[] _guilds;
	_guilds = new_guilds;
	
	/* Add the new guild to the array */
	_guilds[_guild_max_count] = new Guild(&guild_id);
	return _guilds[_guild_max_count += guilds_delta];
}