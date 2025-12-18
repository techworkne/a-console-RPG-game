#include "InputConfig.hpp"
#include "CommandUtils.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include <limits> 

InputConfig::InputConfig() {
    LoadDefaults();
}

bool InputConfig::LoadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Cannot open configuration file: " << filename << std::endl;
        return false;
    }
    
    std::unordered_map<Command, char> new_bindings;
    std::string line;
    
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        std::istringstream iss(line);
        std::string command_str;
        char key;
        
        if (iss >> command_str >> key) {
            Command cmd = CommandFromString(command_str);
            if (cmd != Command::kInvalid) {
                new_bindings[cmd] = key;
            } else {
                std::cout << "Warning: Unknown command in config: " << command_str << std::endl;
            }
        }
    }
    
    file.close();
    
    if (new_bindings.empty()) {
        std::cout << "No valid bindings found in config file. Using defaults." << std::endl;
        return false;
    }
    
    SetBindings(new_bindings);
    if (!Validate()) {
        std::cout << "Invalid configuration detected. Using defaults." << std::endl;
        LoadDefaults();
        return false;
    }
    
    std::cout << "Key bindings loaded from " << filename << std::endl;
    return true;
}

bool InputConfig::SaveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cout << "Cannot create configuration file: " << filename << std::endl;
        return false;
    }
    
    file << "# Game Controls Configuration\n";
    file << "# Format: Command Key\n";
    file << "# Available commands: MoveUp, MoveDown, MoveLeft, MoveRight, CastSpell,\n";
    file << "# SwitchMode, Attack, Quit, Help, Save, Load, SkipTurn\n\n";
    
    for (const auto& [command, key] : key_bindings_) {
        if (command != Command::kInvalid) {
            file << CommandToString(command) << " " << key << "\n";
        }
    }
    
    file.close();
    std::cout << "Key bindings saved to " << filename << std::endl;
    return true;
}

bool InputConfig::Validate() const {
    return IsComplete() && !HasKeyConflicts();
}

void InputConfig::SetBindings(const std::unordered_map<Command, char>& bindings) {
    key_bindings_ = bindings;
}

char InputConfig::GetKeyForCommand(Command cmd) const {
    auto it = key_bindings_.find(cmd);
    return (it != key_bindings_.end()) ? it->second : '\0';
}

void InputConfig::LoadDefaults() {
    key_bindings_ = {
        {Command::kMoveUp, 'w'},
        {Command::kMoveDown, 's'},
        {Command::kMoveLeft, 'a'},
        {Command::kMoveRight, 'd'},
        {Command::kCastSpell, 'e'},
        {Command::kSwitchMode, 'c'},
        {Command::kAttack, 'f'},
        {Command::kQuit, 'q'},
        {Command::kHelp, 'h'},
        {Command::kSave, 'r'},
        {Command::kLoad, 'l'}
    };
}

bool InputConfig::HasKeyConflicts() const {
    std::unordered_map<char, int> key_counts;
    
    for (const auto& [command, key] : key_bindings_) {
        if (key != '\0') {
            key_counts[key]++;
            if (key_counts[key] > 1) {
                std::cout << "Key conflict detected: key '" << key 
                          << "' is assigned to multiple commands." << std::endl;
                return true;
            }
        }
    }
    
    return false;
}

bool InputConfig::IsComplete() const {
    const std::vector<Command> required_commands = {
        Command::kMoveUp, Command::kMoveDown, Command::kMoveLeft, Command::kMoveRight,
        Command::kCastSpell, Command::kSwitchMode, Command::kAttack,
        Command::kQuit, Command::kHelp, Command::kSave, Command::kLoad
    };
    
    for (Command cmd : required_commands) {
        if (key_bindings_.find(cmd) == key_bindings_.end() || 
            key_bindings_.at(cmd) == '\0') {
            std::cout << "Missing binding for command: " << CommandToString(cmd) << std::endl;
            return false;
        }
    }
    
    return true;
}