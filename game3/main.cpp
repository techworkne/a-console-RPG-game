#include "GameLoop.hpp"
#include "GameLogic.hpp"
#include "InputHandler.hpp"
#include "Render.hpp"
#include <iostream>
#include <memory>

int main() {
  try {
    auto game_logic = std::make_unique<GameLogic>(15, 15);
    auto input_handler = std::make_unique<InputHandler>();
    auto renderer = std::make_unique<Renderer>();
      
    GameLoop game_loop(*game_logic, *input_handler, *renderer);
    game_loop.Run();
  } catch (const std::exception& e) {
    std::cerr << "An error occurred: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
