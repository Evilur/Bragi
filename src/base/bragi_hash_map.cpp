#include "bragi_hash_map.h"

BragiHashMap::BragiHashMap(const u_int capacity) : HashMap(capacity) { }

u_int BragiHashMap::GetHash(const dpp::snowflake key) const {
    return key % _lists_size;
}
