#include "PlayerHand.hpp"

PlayerHand::PlayerHand(size_t max_size) : max_size_(max_size) {}

bool PlayerHand::addSpell(std::unique_ptr<Spell> spell){
    if (spells_.size() < max_size_) {
        spells_.push_back(std::move(spell));
        return true;
    }
    return false;
};

void PlayerHand::removeSpell(size_t idx){
    if( idx < spells_.size() ){
        spells_.erase(spells_.begin() + idx);
    }
}