#ifndef TRAP_SPELL_HPP
#define TRAP_SPELL_HPP

#include "Spell.hpp"

class TrapSpell : public Spell {
private:
    int trap_damage_;
    int mana_cost_;
    bool enhanced_;

public:
    TrapSpell(int damage, int mana_cost);
    
    bool cast(GameField& field, Entity* caster, size_t target_x, size_t target_y);
    int getManaCost() const override { return mana_cost_; }
    std::string getName() const override { return "Trap"; }
    void enhance() override { enhanced_ = true; }
    bool isEnhanced() const override { return enhanced_; }
};

#endif