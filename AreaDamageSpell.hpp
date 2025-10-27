#ifndef AREA_DAMAGE_SPELL_HPP
#define AREA_DAMAGE_SPELL_HPP

#include "Spell.hpp"

class AreaDamageSpell : public Spell {
private:
    int damage_;
    int range_;
    int mana_cost_;
    bool enhanced_;
    
public:
    AreaDamageSpell(int damage, int mana_cost);
    
    bool cast(GameField& field, Entity* caster, size_t target_x, size_t target_y) override;
    int getManaCost() const override { return mana_cost_; }
    std::string getName() const override { return "Area Damage"; }
    void enhance() override { enhanced_ = true; }
    bool isEnhanced() const override { return enhanced_; }
};

#endif