#ifndef DIRECT_DAMAGE_SPELL_HPP
#define DIRECT_DAMAGE_SPELL_HPP

#include "Spell.hpp"
#include <iostream>

class DirectDamageSpell : public Spell {
private:
    int damage_;
    int mana_cost_;
    
public:
    DirectDamageSpell(int damage, int mana_cost);
    
    bool cast(GameField& field, Entity* caster, size_t target_x, size_t target_y) override;
    
    int getManaCost() const override { return mana_cost_; }
    std::string getName() const override { return "Direct Damage"; }
    SpellType getType() const override {return SpellType::DirectDamage;}
    int getDamage() const override {return damage_;}
};

#endif