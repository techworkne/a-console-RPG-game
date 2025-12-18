#ifndef GAME_ENTITY_HPP_
#define GAME_ENTITY_HPP_

#include "EntityType.hpp"
#include <cstddef>
#include <string>



class Entity {
 public:
 
  Entity(int initial_health, int damage, size_t x, size_t y);

  
  virtual ~Entity() = default;

  
  int GetHealth() const;
  int GetDamage() const;
  std::string getEntityTypeName(EntityType type) const;

  
  size_t GetX() const;
  size_t GetY() const;


  void SetX(size_t x);
  void SetY(size_t y);


  virtual EntityType GetType() const = 0;
  virtual bool IsAlive() const;

  
  virtual void TakeDamage(int damage);

 protected:
  int health_;
  int damage_;
  size_t x_;
  size_t y_;
};

#endif
