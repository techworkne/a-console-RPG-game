#ifndef GAME_INPUTPROVIDER_HPP_
#define GAME_INPUTPROVIDER_HPP_

#include "Command.hpp"
#include "Direction.hpp"
#include <unordered_map>

class InputProvider {
public:
    virtual ~InputProvider() = default;
    
    virtual Command GetCommand() const = 0;
    virtual Direction GetAttackDirection() const = 0;
    
    virtual void UpdateKeyBindings(const std::unordered_map<Command, char>& newBindings) = 0;
    
    virtual const std::unordered_map<Command, char>& GetKeyBindings() const = 0;
};

#endif