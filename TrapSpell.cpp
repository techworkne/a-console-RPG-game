#include "TrapSpell.hpp"
#include "RewardSystem.hpp"
#include "GameField.hpp"

TrapSpell::TrapSpell(int damage, int mana_cost): trap_damage_(damage), mana_cost_(mana_cost), enhanced_(false) {}

bool TrapSpell::cast(GameField& field, Entity* caster, size_t target_x, size_t target_y) {
    if (!caster || caster->GetType() != EntityType::kPlayer) {
        return false;
    }

    FieldCell* target_cell = field.GetCell(target_x, target_y);
    if (!target_cell || !target_cell->IsPassable() || !target_cell->IsEmpty() || target_cell->GetType() == FieldCellType::kSlowdown) {
        return false;
    }

    target_cell->SetType(FieldCellType::kTrap);
    target_cell->SetTrapDamage(trap_damage_);
    
    std::cout << "Trap placed at (" << target_x << ", " << target_y << ")\n";
    return true;
}