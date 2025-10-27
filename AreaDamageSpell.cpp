#include "AreaDamageSpell.hpp"
#include "RewardSystem.hpp"
#include "GameField.hpp"


AreaDamageSpell::AreaDamageSpell(int damage, int mana_cost): damage_(damage), mana_cost_(mana_cost), enhanced_(false){};

bool AreaDamageSpell::cast(GameField& field, Entity* caster, size_t target_x, size_t target_y) {
    
    
    int area_size = enhanced_ ? 3 : 2;
    
    std::cout << "Casting area damage spell! Area: " << area_size << "x" << area_size << "\n";
    
    
    for (int offset_y = 0; offset_y < area_size; ++offset_y) {
        for (int offset_x = 0; offset_x < area_size; ++offset_x) {
            size_t current_x = target_x + offset_x;
            size_t current_y = target_y + offset_y;
            
            
            if (current_x >= field.GetWidth() || current_y >= field.GetHeight()) {
                continue;
            }
            
            FieldCell* cell = field.GetCell(current_x, current_y);
            if (!cell || cell->IsEmpty()) continue;
            
            Entity* entity = cell->GetEntity();
            if (entity && (entity->GetType() == EntityType::kEnemy || entity->GetType() == EntityType::kBuilding || entity->GetType() == EntityType::kTower)) {
                            
                entity->TakeDamage(damage_);
                std::cout << "Hit " << entity->getEntityTypeName(entity->GetType()) << " at (" << current_x << ", " << current_y  << ") for " << damage_ << " damage!\n";

                
                if (!entity->IsAlive()) {
                    Player* player = dynamic_cast<Player*>(caster);
                    RewardSystem::GiveDestructionReward(entity, player);
                    field.RemoveEntity(current_x, current_y);
                }
            }
        }
    }
    return true;
}