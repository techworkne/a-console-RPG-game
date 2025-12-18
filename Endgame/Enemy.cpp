#include "Enemy.hpp"

Enemy::Enemy(int initial_health, int damage, size_t x, size_t y)
    : Entity(initial_health, damage, x, y) {}

EntityType Enemy::GetType() const { return EntityType::kEnemy; }

void Enemy::TakeDamage(int damage){
    Entity::TakeDamage(damage);
    std::cout<< "Enemy took " << damage << " damage.\n";
}