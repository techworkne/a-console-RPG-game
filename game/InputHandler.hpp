#ifndef GAME_INPUTHANDLER_HPP_
#define GAME_INPUTHANDLER_HPP_

#include "Direction.hpp"
#include "Command.hpp"
#include <string>
#include <functional>


class InputHandler {
public:
    
    InputHandler() = default;
    ~InputHandler() = default;
    
    Command GetCommand() const;
    Direction GetAttackDirection() const;
    
private:
    char GetSingleCharInput() const;
};

#endif