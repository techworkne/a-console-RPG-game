#include "EnemyTower.hpp"
#include "GameField.hpp"
#include "Player.hpp"
#include <cmath>
#define ATTACK_INTERVAL 3

EnemyTower::EnemyTower(int initial_health, size_t x, size_t y, int attack_radius)
    : EnemyArchitecture(initial_health, x, y, 0), attack_radius_(attack_radius), attack_spell_(std::make_unique<DirectDamageSpell>(14, 0)) {}

EntityType EnemyTower::GetType() const { 
    return EntityType::kTower; 
}

void EnemyTower::TakeDamage(int damage) {
    EnemyArchitecture::TakeDamage(damage);
    std::cout << "Enemy tower took " << damage << " damage.\n";
}

bool EnemyTower::Update() {
    if (attack_cooldown_ > 0) {
        attack_cooldown_--;
        std::cout << "Tower cooldown: " << attack_cooldown_ << " turns remaining.\n";
    }
    return false;
}

bool EnemyTower::AttackPlayerInRange(GameField& field, Player* player) {
    if (!player ||  player->IsDead() || attack_cooldown_ > 0) {
        return false;
    }
    
    int dx = std::abs(static_cast<int>(GetX()) - static_cast<int>(player->GetX()));
    int dy = std::abs(static_cast<int>(GetY()) - static_cast<int>(player->GetY()));
    double distance = dx + dy;
    
    if (distance <= attack_radius_) {
        
        if (attack_spell_->cast(field, this, player->GetX(), player->GetY())) {
            attack_cooldown_ = ATTACK_INTERVAL;
            std::cout << "Enemy tower attacks player!\n";
            return true;
        }
    }
    
    return false;
}