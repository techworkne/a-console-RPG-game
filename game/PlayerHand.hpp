#ifndef PLAYER_HAND_HPP
#define PLAYER_HAND_HPP

#include "Spell.hpp"
#include <vector>
#include <memory>

class PlayerHand {
private:
    std::vector<std::unique_ptr<Spell>> spells_;
    size_t max_size_;
    
public:
    PlayerHand(size_t max_size);
    
    
    bool addSpell(std::unique_ptr<Spell> spell);
    
    size_t getSpellCount() const {return spells_.size();};
    size_t getMaxSize() const {return max_size_;};
    Spell* getSpell(size_t index) {
        return (index < spells_.size()) ? spells_[index].get() : nullptr;
    };
    const Spell* getSpell(size_t index) const {
        return (index < spells_.size()) ? spells_[index].get() : nullptr;
    }
    
    bool canAddSpell() const{ return spells_.size() < max_size_; };
};

#endif