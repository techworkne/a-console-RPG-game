#ifndef GAME_ENEMY_HPP_
#define GAME_ENEMY_HPP_

#include "Entity.hpp"
#include <cstddef>
#include <iostream>


class Enemy final : public Entity {
 public:
 
  Enemy(int initial_health, int damage, size_t x, size_t y);

  
  EntityType GetType() const override;
  void TakeDamage(int damage) override;
};

#endif