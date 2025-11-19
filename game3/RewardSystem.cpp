#include "RewardSystem.hpp"
#define MANA_REWARD 10
#define SCORE_REWARD_ENEMY 10
#define SCORE_REWARD_ARCHITECTURE 20
void RewardSystem::GiveDestructionReward(EntityType entity_type, Player* player){
    if (!player) return;
    int score = (entity_type == EntityType::kEnemy) ? SCORE_REWARD_ENEMY : SCORE_REWARD_ARCHITECTURE;
    
    player->AddScore(score);
    player->addMana(MANA_REWARD);
    std::cout << GetEntityTypeName(entity_type) << " destroyed! +" << score << " points\n";
    std::cout << GetEntityTypeName(entity_type) << " destroyed! +10 mana\n";
    
    
    if (entity_type == EntityType::kBuilding && player->getHand().canAddSpell()) {
        GiveNewSpellReward(player);
    }
}

void RewardSystem::GiveNewSpellReward(Player* player) {

    static const std::vector<std::string> all_spells = {
        "Direct Damage", "Area Damage", "Trap"
    };
    
    
    std::vector<std::string> player_spells;
    PlayerHand& hand = player->getHand();
    
    for (size_t i = 0; i < hand.getSpellCount(); ++i) {
        if (const Spell* spell = hand.getSpell(i)) {
            player_spells.push_back(spell->getName());
        }
    }


    for (const auto& spell_name : all_spells) {
        if (std::find(player_spells.begin(), player_spells.end(), spell_name) == player_spells.end()) {
           
            
            if (auto new_spell = CreateSpell(spell_name)) {
                player->addSpellToHand(std::move(new_spell));
                std::cout << " Found new spell: " << spell_name << "!\n";
                return;
            }
        }
    }
    
    std::cout << " You already too strong!!!!!\n";
}

std::unique_ptr<Spell> RewardSystem::CreateSpell(const std::string& name) {
    if (name == "Direct Damage") return std::make_unique<DirectDamageSpell>(50, 30);
    if (name == "Area Damage") return std::make_unique<AreaDamageSpell>(30, 40);
    if (name == "Trap") return std::make_unique<TrapSpell>(25, 25);
    return nullptr;
}

const std::string RewardSystem::GetEntityTypeName(EntityType type){
    switch(type) {
        case EntityType::kEnemy: return "Enemy";
        case EntityType::kBuilding: return "Building";
        case EntityType::kTower: return "Tower";
        case EntityType::kPlayer: return "Player";
        default: return "Unknown";
    }
}