#ifndef GAME_ENEMYTOWER_HPP_
#define GAME_ENEMYTOWER_HPP_

#include "EnemyArchitecture.hpp"
#include "DirectDamageSpell.hpp"

class Player;

class EnemyTower : public EnemyArchitecture {
public:
    EnemyTower(int initial_health, size_t x, size_t y, int attack_radius);

    EntityType GetType() const override;
    void TakeDamage(int damage) override;
    bool Update() override;
    
    bool AttackPlayerInRange(GameField& field, Player* player);
    
    int GetAttackRadius() const { return attack_radius_; }
    int GetAttackCooldown() const { return attack_cooldown_; }

    void SetAttackCooldown(int attack_cooldown){ attack_cooldown_ = attack_cooldown; }

private:
    int attack_radius_;
    int attack_cooldown_ = 0;
    std::unique_ptr<DirectDamageSpell> attack_spell_;
};

#endif