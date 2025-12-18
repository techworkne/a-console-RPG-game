#ifndef GAME_INPUTCONFIG_HPP_
#define GAME_INPUTCONFIG_HPP_

#include "Command.hpp"
#include <unordered_map>
#include <string>
#include <memory>

class InputConfig {
public:
    InputConfig();
    
    bool LoadFromFile(const std::string& filename);
    bool SaveToFile(const std::string& filename) const;
    
    bool Validate() const;
    
    const std::unordered_map<Command, char>& GetBindings() const { return key_bindings_; }
    
    void SetBindings(const std::unordered_map<Command, char>& bindings);
    
    char GetKeyForCommand(Command cmd) const;
    
    void LoadDefaults();
    
private:
    std::unordered_map<Command, char> key_bindings_;
    
    bool HasKeyConflicts() const;
    bool IsComplete() const;
};
#endif