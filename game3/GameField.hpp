#ifndef GAMELAB1_GAMEFIELD_HPP_
#define GAMELAB1_GAMEFIELD_HPP_

#include "FieldCell.hpp"
#include <cstddef>
#include <vector>
#include <memory>


class GameField {
 public:
 
  GameField(size_t width, size_t height);

  
  ~GameField() = default;

  
  GameField(const GameField& other);

  
  GameField(GameField&& other) noexcept;

  
  GameField& operator=(const GameField& other);

  
  GameField& operator=(GameField&& other) noexcept;

  
  size_t GetWidth() const;
  size_t GetHeight() const;

  
  FieldCell* GetCell(size_t x, size_t y);
  const FieldCell* GetCell(size_t x, size_t y) const;

  bool MoveEntity(size_t from_x, size_t from_y, size_t to_x, size_t to_y);
  void PlaceEntity(size_t x, size_t y, std::unique_ptr<Entity> entity);
  std::unique_ptr<Entity> RemoveEntity(size_t x, size_t y);

  void GenerateLayout();

 private:
  size_t width_;
  size_t height_;
  std::vector<std::vector<FieldCell>> cells_;

  bool IsValidCoordinates(size_t x, size_t y) const {
    return x < width_ && y < height_;
  }
};

#endif
