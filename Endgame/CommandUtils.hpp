#ifndef GAME_COMMANDUTILS_HPP_
#define GAME_COMMANDUTILS_HPP_

#include "Command.hpp"
#include <string>
#include <unordered_map>

std::string CommandToString(Command cmd);
Command CommandFromString(const std::string& str);

#endif