#include "AreaDamageSpell.hpp"
#include "RewardSystem.hpp"
#include "GameField.hpp"
#define AREA_SIZE 2


AreaDamageSpell::AreaDamageSpell(int damage, int mana_cost): damage_(damage), mana_cost_(mana_cost){};

bool AreaDamageSpell::cast(GameField& field, Entity* caster, size_t target_x, size_t target_y) {
    
    
    std::cout << "Casting area damage spell! Area: " << AREA_SIZE << "x" << AREA_SIZE << "\n";
    
    
    for (int offset_y = 0; offset_y < AREA_SIZE; ++offset_y) {
        for (int offset_x = 0; offset_x < AREA_SIZE; ++offset_x) {
            size_t current_x = target_x + offset_x;
            size_t current_y = target_y + offset_y;
            
            
            if (current_x >= field.GetWidth() || current_y >= field.GetHeight()) {
                continue;
            }
            
            FieldCell* cell = field.GetCell(current_x, current_y);
            if (!cell || cell->IsEmpty()) continue;
            
            Entity* entity = cell->GetEntity();
            if (entity && (entity->GetType() != EntityType::kPlayer || entity->GetType() != EntityType::kEmpty)) {
                            
                entity->TakeDamage(damage_);
                std::cout << "Hit " << entity->getEntityTypeName(entity->GetType()) << " at (" << current_x << ", " << current_y  << ") for " << damage_ << " damage!\n";

                
                if (!entity->IsAlive()) {
                    Player* player = dynamic_cast<Player*>(caster);
                    RewardSystem::GiveDestructionReward(entity->GetType(), player);
                    field.RemoveEntity(current_x, current_y);
                }
            }else std::cout << "There is no target o.o\n";
        }
    }
    return true;
}