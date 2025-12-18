#include "InputHandler.hpp"
#include <iostream>
#include <cctype>
#include <fstream>
#include <sstream>
#include <limits>
InputHandler::InputHandler() {
    config_ = std::make_shared<InputConfig>();
    config_->LoadDefaults();
}

InputHandler::InputHandler(std::shared_ptr<InputConfig> config) 
    : config_(config) {
    if (!config_) {
        config_ = std::make_shared<InputConfig>();
        config_->LoadDefaults();
    }
}

Command InputHandler::GetCommand() const {
    std::cout << "Enter command: ";
    char input = GetSingleCharInput();
    
    if (input == '\n' || input == '\r') {
        return Command::kSkipTurn;
    }
    
    input = std::tolower(input);
    
    const auto& bindings = GetKeyBindings();
    for (const auto& [command, key] : bindings) {
        if (key == input) {
            return command;
        }
    }
    
    return Command::kInvalid;
}

Direction InputHandler::GetAttackDirection() const {
    std::cout << "Choose attack direction (w/a/s/d): ";
    char direction = GetSingleCharInput();
    direction = std::tolower(direction);
    
    switch (direction) {
        case 'w': return Direction::kUp;
        case 's': return Direction::kDown;
        case 'a': return Direction::kLeft;
        case 'd': return Direction::kRight;
        default: return Direction::kUp;
    }
}

void InputHandler::UpdateKeyBindings(const std::unordered_map<Command, char>& newBindings) {
    if (config_) {
        config_->SetBindings(newBindings);
    }
}

const std::unordered_map<Command, char>& InputHandler::GetKeyBindings() const {
    static std::unordered_map<Command, char> empty_map;
    return config_ ? config_->GetBindings() : empty_map;
}

void InputHandler::SetConfig(std::shared_ptr<InputConfig> config) {
    config_ = config;
    if (!config_) {
        config_ = std::make_shared<InputConfig>();
        config_->LoadDefaults();
    }
}

char InputHandler::GetSingleCharInput() const {
    char ch;
    std::cin.get(ch);
    if (ch != '\n') {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // Исправленный вызов
    }
    return ch;
}