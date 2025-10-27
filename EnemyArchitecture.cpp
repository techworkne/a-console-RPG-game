#include "EnemyArchitecture.hpp"

EnemyArchitecture::EnemyArchitecture(int initial_health, size_t x, size_t y, int spawn_interval)
    : Entity(initial_health, 0, x, y), 
      spawn_interval_(spawn_interval), 
      turns_since_spawn_(0) {}

bool EnemyArchitecture::Update() {
    turns_since_spawn_++;
    if (turns_since_spawn_ >= spawn_interval_) {
        turns_since_spawn_ = 0;
        return true;
    }
    return false;
}


void EnemyArchitecture::TakeDamage(int damage) {
    Entity::TakeDamage(damage);
}