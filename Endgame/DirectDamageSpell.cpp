#include "DirectDamageSpell.hpp"
#include "RewardSystem.hpp"
#include "GameField.hpp"

DirectDamageSpell::DirectDamageSpell(int damage, int mana_cost) : damage_(damage), mana_cost_(mana_cost) {}

bool DirectDamageSpell::cast(GameField& field, Entity* caster, size_t target_x, size_t target_y) {
    if (!caster ) {
        return false;
    }
    
    FieldCell* cell = field.GetCell(target_x, target_y);
    if (!cell || cell->IsEmpty()) return false;
    
    Entity* target = cell->GetEntity();
    if (!target || !target->IsAlive()) {
        return false;
    }
    
    if (caster->GetType() == EntityType::kTower) {
        if (target->GetType() != EntityType::kPlayer) return false;
    }
    
    target->TakeDamage(damage_);
    std::cout << "Direct damage spell! Done " << (damage_) << " damage. " << target->getEntityTypeName(target->GetType()) << " HP: " << target->GetHealth() << "\n";

    if (target->GetType() != EntityType::kPlayer && !target->IsAlive()) {
        Player* player = dynamic_cast<Player*>(caster);
        RewardSystem::GiveDestructionReward(target->GetType(), player);
        field.RemoveEntity(target_x, target_y);
    }

    return true;
}