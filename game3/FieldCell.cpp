#include "FieldCell.hpp"
#include <utility>

FieldCell::FieldCell(FieldCellType type)
    : type_(type), entity_(nullptr) {}

FieldCell::FieldCell(const FieldCell& other)
    : type_(other.type_), entity_(nullptr) {}

FieldCell::FieldCell(FieldCell&& other) noexcept
    : type_(other.type_), entity_(std::move(other.entity_)) {} 

FieldCell& FieldCell::operator=(const FieldCell& other) {
  if (this != &other) {
    type_ = other.type_;
    entity_.reset(); 
  }
  return *this;
}

FieldCell& FieldCell::operator=(FieldCell&& other) noexcept {
  if (this != &other) {
    type_ = other.type_;
    entity_ = std::move(other.entity_);
  }
  return *this;
}

FieldCellType FieldCell::GetType() const { return type_; }

void FieldCell::SetType(FieldCellType type) { type_ = type; }

Entity* FieldCell::GetEntity() const { return entity_.get(); }

std::unique_ptr<Entity> FieldCell::SetEntity(std::unique_ptr<Entity> entity) {
  return std::exchange(entity_, std::move(entity));
}

bool FieldCell::IsEmpty() const { return entity_ == nullptr; }

bool FieldCell::IsPassable() const { return type_ != FieldCellType::kImpassable; }

std::unique_ptr<Entity> FieldCell::ExtractEntity() {
  return std::exchange(entity_, nullptr);
}
