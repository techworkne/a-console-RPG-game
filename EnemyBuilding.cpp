#include "EnemyBuilding.hpp"

EnemyBuilding::EnemyBuilding(int initial_health, size_t x, size_t y, int spawn_interval)
    : EnemyArchitecture(initial_health, x, y, spawn_interval) {}

EntityType EnemyBuilding::GetType() const { 
    return EntityType::kBuilding; 
}

void EnemyBuilding::TakeDamage(int damage) {
    EnemyArchitecture::TakeDamage(damage);
    std::cout << "Enemy building took " << damage << " damage.\n";
}

bool EnemyBuilding::Update() {
    return EnemyArchitecture::Update();
}