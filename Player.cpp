#include "Player.hpp"

Player::Player(int initial_health, int melee_damage, size_t x, size_t y)
  : Entity(initial_health, melee_damage, x, y),
    combat_mode_(CombatMode::kMelee),
    score_(0),
    lives_(3),
    is_stunned_(false),
    hand_(3),
    mana_(100)
{}

void Player::TakeDamage(int damage) {
  if (IsDead()) return;
  
  Entity::TakeDamage(damage);
  std::cout << "Player took " << damage << " damage. HP: " << health_ << "\n";
  
  if (health_ <= 0) {
    SetDead(true);
    health_ = 0;
    std::cout << "Player has been defeated!\n";
  }
}

CombatMode Player::GetCombatMode() const { return combat_mode_; }

void Player::SwitchCombatMode() {
  combat_mode_ = (combat_mode_ == CombatMode::kMelee) ? CombatMode::kRanged : CombatMode::kMelee;
}


int Player::GetScore() const { return score_; }
void Player::AddScore(int points) {

  if (points > 0) {
    score_ += points;
  }
}

int Player::GetLives() const { return lives_; }
void Player::LoseLife() {

  if (lives_ > 0) {
    lives_--;
  }
}


bool Player::IsStunned() const { return is_stunned_; }
void Player::Stun() { is_stunned_ = true; }
void Player::UnStun() { is_stunned_ = false; }


EntityType Player::GetType() const { return EntityType::kPlayer; }

int Player::GetCurrentDamage() const {
  return (combat_mode_ == CombatMode::kMelee) ? melee_damage_ : ranged_damage_;
}
int Player::GetMeleeRange(){
  return melee_range_;
}
int Player::GetRangedRange(){
  return ranged_range_;
}
int Player::GetMeleeDamage(){
  return melee_damage_;
}
int Player::GetRangedDamage(){
  return ranged_damage_;
}

void Player::setMana(int mana){
  if (mana < 0){ mana = 0; }
  mana_ = mana;
}

bool Player::castSpell(size_t spell_index) {
  if (spell_index >= hand_.getSpellCount()) {
    std::cout << "Invalid spell index!\n";
    return false;
  }

  Spell* spell = hand_.getSpell(spell_index);
  if (!spell) {
      std::cout << "Spell not found!\n";
      return false;
  }

  if (mana_ < spell->getManaCost()) {
    std::cout << "Not enough mana! Need " << spell->getManaCost() << ", but have " << mana_ << "\n";
    return false;
  }

  
  mana_ -= spell->getManaCost();
  return true;
  
}

void Player::addSpellToHand(std::unique_ptr<Spell> spell){
  if(hand_.addSpell(std::move(spell))){
    std::cout << "spell was added\n";
  }else{
    std::cout << "not enough place in the hand!\n";
  }
}

void Player::addMana(int mana){
  if (mana > 0) {
    mana_ += mana;
    if(mana_ > 100) mana_ = 100;
  }
}