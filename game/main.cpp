#include "GameLogic.hpp"
#include <iostream>

int main() {
  try {
    GameLogic game(20, 20);
    game.Run();
  } catch (const std::exception& e) {
    std::cerr << "An error occurred: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
