#ifndef GAME_PLAYER_HPP_
#define GAME_PLAYER_HPP_

#include "Entity.hpp"
#include "CombatMode.hpp"
#include "PlayerHand.hpp"
#include <cstddef> 
#include <iostream> 


class Player final : public Entity {
  public:

  Player(int initial_health, size_t x, size_t y);

  void TakeDamage(int damage) override;

  bool IsDead() const { return is_dead_ || health_ <= 0; }
  void SetDead(bool dead) { is_dead_ = dead; }


  CombatMode GetCombatMode() const;
  void SwitchCombatMode();
  void SetCombatMode(CombatMode mode) { combat_mode_ = mode;}
  void AddMeleeDamage(int damage) { melee_damage_ += damage; }
  void AddRangeDamage(int damage) { ranged_damage_ += damage; }

  int GetScore() const;
  void AddScore(int points);
  void SetScore(int score) { score_ = score; }

  int GetLives() const;
  void LoseLife();

  bool IsStunned() const;
  void Stun();
  void UnStun();

  EntityType GetType() const override;

  bool castSpell(size_t spell_index);
  void addSpellToHand(std::unique_ptr<Spell> spell);

  const PlayerHand& getHand() const {return hand_;};
  PlayerHand& getHand() {return hand_;};

  int getMana() const {return mana_;};
  void addMana(int mana);
  void setMana(int mana);

  int GetCurrentDamage() const;
  int GetMeleeRange();
  int GetRangedRange();
  int GetMeleeDamage();
  int GetRangedDamage();

  void AddLives() { if (lives_ < 3) lives_++; }
  void SetLives(int lives) { lives_ = lives; }
  void ResetHealth(int health) { health_ = health; }


  bool IsAlive() const override { return !is_dead_ && health_ > 0; }

  private:
    CombatMode combat_mode_;
    int melee_damage_ = 52;    
    int ranged_damage_ = 22;
    int melee_range_ = 1;
    int ranged_range_ = 3;
    int score_;
    int lives_;
    bool is_stunned_;
    bool is_dead_ = false;
    PlayerHand hand_;
    int mana_;
};

#endif
