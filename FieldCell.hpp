#ifndef GAMELAB1_FIELDCELL_HPP_
#define GAMELAB1_FIELDCELL_HPP_

#include "FieldCellType.hpp"
#include "Entity.hpp"
#include <memory> 

class FieldCell {
 public:
 
  FieldCell(FieldCellType type = FieldCellType::kNormal);
  
  ~FieldCell() = default;

  FieldCell(const FieldCell& other);
  
  FieldCell(FieldCell&& other) noexcept;
  
  FieldCell& operator=(const FieldCell& other);
  
  FieldCell& operator=(FieldCell&& other) noexcept;
  

  FieldCellType GetType() const;
  Entity* GetEntity() const;
  
  void SetType(FieldCellType type);
  std::unique_ptr<Entity> SetEntity(std::unique_ptr<Entity> entity);
  
  bool IsEmpty() const;
  bool IsPassable() const;
  
  std::unique_ptr<Entity> ExtractEntity();

  
  void SetTrapDamage(int damage) { trap_damage_ = damage; }
  int GetTrapDamage() const { return trap_damage_; }
  void RemoveTrap() { SetType(FieldCellType::kNormal); }

 private:
  int trap_damage_;
  FieldCellType type_;
  std::unique_ptr<Entity> entity_;
};

#endif