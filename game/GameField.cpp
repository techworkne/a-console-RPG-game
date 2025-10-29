#include "GameField.hpp"
#include <random>
#include <algorithm>
#include <stdexcept>
#include <iostream>

GameField::GameField(size_t width, size_t height)
    : width_(width), height_(height) {
  if (width < 10 || width > 25 || height < 10 || height > 25) {
    throw std::invalid_argument("Field dimensions must be between 10 and 25.");
  }
  cells_.resize(height_);
  for (auto& row : cells_) {
    row.resize(width_);
  }
  GenerateLayout();
}

GameField::GameField(const GameField& other)
    : width_(other.width_), height_(other.height_), cells_(other.height_) {
  for (size_t y = 0; y < height_; ++y) {
    for (size_t x = 0; x < width_; ++x) {
      cells_[y][x] = other.cells_[y][x];
    }
  }
}

GameField::GameField(GameField&& other) noexcept
    : width_(other.width_), height_(other.height_), cells_(std::move(other.cells_)) {}

GameField& GameField::operator=(const GameField& other) {
  if (this != &other) {
    width_ = other.width_;
    height_ = other.height_;
    cells_.resize(height_);
    for (size_t y = 0; y < height_; ++y) {
      cells_[y].resize(width_);
      for (size_t x = 0; x < width_; ++x) {
        cells_[y][x] = other.cells_[y][x];
      }
    }
  }
  return *this;
}

GameField& GameField::operator=(GameField&& other) noexcept {
  if (this != &other) {
    width_ = other.width_;
    height_ = other.height_;
    cells_ = std::move(other.cells_);
  }
  return *this;
}

size_t GameField::GetWidth() const { return width_; }

size_t GameField::GetHeight() const { return height_; }

FieldCell* GameField::GetCell(size_t x, size_t y) {
  if (IsValidCoordinates(x, y)) {
    return &cells_[y][x];
  }
  return nullptr;
}

const FieldCell* GameField::GetCell(size_t x, size_t y) const {
  if (IsValidCoordinates(x, y)) { 
    return &cells_[y][x];
  }
  return nullptr;
}

bool GameField::MoveEntity(size_t from_x, size_t from_y, size_t to_x, size_t to_y) {
  if (!IsValidCoordinates(from_x, from_y) || !IsValidCoordinates(to_x, to_y)) {
    return false;
  }

  FieldCell* source_cell = &cells_[from_y][from_x];
  FieldCell* target_cell = &cells_[to_y][to_x];

  if (source_cell->IsEmpty() || !target_cell->IsPassable()) {
    return false;
  }

  if (!target_cell->IsEmpty()) {
      return false;
  }

  std::unique_ptr<Entity> entity_to_move = source_cell->ExtractEntity();

  target_cell->SetEntity(std::move(entity_to_move));

  Entity* moved_entity = target_cell->GetEntity();
  if (moved_entity) {
      moved_entity->SetX(to_x);
      moved_entity->SetY(to_y);
  }

  return true;
}

void GameField::PlaceEntity(size_t x, size_t y, std::unique_ptr<Entity> entity) {
  if (IsValidCoordinates(x, y) && entity) { 
    FieldCell* cell = &cells_[y][x];
    if (cell->IsEmpty()) {
      entity->SetX(x);
      entity->SetY(y);
      cell->SetEntity(std::move(entity));
    }
  }
}

std::unique_ptr<Entity> GameField::RemoveEntity(size_t x, size_t y) {
  if (IsValidCoordinates(x, y)) {
    FieldCell* cell = &cells_[y][x];
    return cell->ExtractEntity();
  }
  return nullptr;
}

void GameField::GenerateLayout() {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, 100);

  for (size_t y = 0; y < height_; ++y) {
    for (size_t x = 0; x < width_; ++x) {
      cells_[y][x].SetType(FieldCellType::kNormal);
    }
  }


  for (size_t y = 1; y < height_ - 1; ++y) {
    for (size_t x = 1; x < width_ - 1; ++x) {
      
      if (x >= width_/2 - 2 && x <= width_/2 + 2 && 
        y >= height_/2 - 2 && y <= height_/2 + 2) {
        continue;
      }

      int chance = dis(gen);
      if (chance < 15) {
        cells_[y][x].SetType(FieldCellType::kImpassable);
      } else if (chance < 20) {
        cells_[y][x].SetType(FieldCellType::kSlowdown);
      }
    }
  }


  // рамка для поля
  for (size_t x = 0; x < width_; ++x) {
    cells_[0][x].SetType(FieldCellType::kImpassable);
    cells_[height_ - 1][x].SetType(FieldCellType::kImpassable);
  }
  for (size_t y = 0; y < height_; ++y) {
    cells_[y][0].SetType(FieldCellType::kImpassable);
    cells_[y][width_ - 1].SetType(FieldCellType::kImpassable);
  }


  for (size_t y = 1; y < height_ - 1; ++y) {
    for (size_t x = 1; x < width_ - 1; ++x) {
      if (cells_[y][x].GetType() != FieldCellType::kImpassable) continue;

      int passable_neighbors = 0;
      std::vector<std::pair<int, int>> directions = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}, {1, 1}, {-1, -1}, {1, -1}, {-1, 1}};
  
      for (auto [dx, dy] : directions) {
        size_t nx = x + dx;
        size_t ny = y + dy;
        if (cells_[ny][nx].IsPassable()) {
          passable_neighbors++;
        }
      }

      if (passable_neighbors < 3) {
        cells_[y][x].SetType(FieldCellType::kNormal);
      }

    }
  }
}