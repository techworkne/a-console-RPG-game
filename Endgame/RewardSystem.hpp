#ifndef REWARD_SYSTEM_HPP
#define REWARD_SYSTEM_HPP

#include "Player.hpp"
#include "Entity.hpp"
#include "AreaDamageSpell.hpp"
#include "DirectDamageSpell.hpp"
#include "TrapSpell.hpp"
#include <algorithm>
#include <vector>

class RewardSystem {
public:
    static void GiveDestructionReward(EntityType entity_type, Player* player);
    static void GiveNewSpellReward(Player* player);
    static std::unique_ptr<Spell> CreateSpell(const std::string& name);
    static const std::string GetEntityTypeName(EntityType type);
};
#endif
