#include "Entity.hpp"

Entity::Entity(int initial_health, int damage, size_t x, size_t y)
    : health_(initial_health), damage_(damage), x_(x), y_(y) {}

int Entity::GetHealth() const {return (health_ < 0) ? 0 : health_; }

int Entity::GetDamage() const { return damage_; }

std::string Entity::getEntityTypeName(EntityType type) const {
  switch (type) {
    case EntityType::kEnemy: return "Enemy";
    case EntityType::kBuilding: return "Building";
    case EntityType::kTower: return "Tower";
    case EntityType::kPlayer: return "Player";
    case EntityType::kEmpty: return "Empty";
    default: return "Unknown";
  }
}


size_t Entity::GetX() const { return x_; }

size_t Entity::GetY() const { return y_; }

void Entity::SetX(size_t x) { x_ = x; }

void Entity::SetY(size_t y) { y_ = y; }

bool Entity::IsAlive() const { return health_ > 0; }

void Entity::TakeDamage(int damage) {
  if (damage > 0 && health_ > 0) {
    health_ = (damage >= health_) ? 0 : health_ - damage;
  }
}
