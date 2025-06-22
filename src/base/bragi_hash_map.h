#pragma once

#include "base/bragi.h"
#include "template/hash_map.hpp"
#include "types/base.h"

#include <dpp/dpp.h>

class BragiHashMap final : public HashMap<dpp::snowflake, Bragi*> {
public:
    explicit BragiHashMap(u_int capacity);
private:
    u_int GetHash(dpp::snowflake key) const override;
};
