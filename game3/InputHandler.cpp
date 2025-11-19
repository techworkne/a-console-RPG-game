#include "InputHandler.hpp"
#include <iostream>
#include <cctype>

Command InputHandler::GetCommand() const {
    std::string input;
    std::getline(std::cin, input);
    
    if (input.empty()) {
        return Command::kSkipTurn;
    }
    
    if (input.length() != 1) {
        return Command::kInvalid;
    }
    
    char command = std::tolower(input[0]);
    switch (command) {
        case 'w': return Command::kMoveUp;
        case 's': return Command::kMoveDown;
        case 'a': return Command::kMoveLeft;
        case 'd': return Command::kMoveRight;
        case 'e': return Command::kCastSpell;
        case 'c': return Command::kSwitchMode;
        case 'f': return Command::kAttack;
        case 'q': return Command::kQuit;
        case 'h': return Command::kHelp;
        case 'r': return Command::kSave;
        case 'l': return Command::kLoad;
        default: return Command::kInvalid;
    }
}

Direction InputHandler::GetAttackDirection() const {
    std::string input;
    std::getline(std::cin, input);
    
    if (input.length() != 1) {
        return Direction::kUp;
    }
    
    char direction = std::tolower(input[0]);
    switch (direction) {
        case 'w': return Direction::kUp;
        case 's': return Direction::kDown;
        case 'a': return Direction::kLeft;
        case 'd': return Direction::kRight;
        default: return Direction::kUp;
    }
}