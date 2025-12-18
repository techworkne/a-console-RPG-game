#include "CommandUtils.hpp"
#include <unordered_map>

std::string CommandToString(Command cmd) {
    static const std::unordered_map<Command, std::string> commandMap = {
        {Command::kMoveUp, "MoveUp"},
        {Command::kMoveDown, "MoveDown"},
        {Command::kMoveLeft, "MoveLeft"},
        {Command::kMoveRight, "MoveRight"},
        {Command::kCastSpell, "CastSpell"},
        {Command::kSwitchMode, "SwitchMode"},
        {Command::kAttack, "Attack"},
        {Command::kQuit, "Quit"},
        {Command::kHelp, "Help"},
        {Command::kSave, "Save"},
        {Command::kLoad, "Load"},
        {Command::kSkipTurn, "SkipTurn"},
        {Command::kInvalid, "Invalid"}
    };
    
    auto it = commandMap.find(cmd);
    return (it != commandMap.end()) ? it->second : "Unknown";
}

Command CommandFromString(const std::string& str) {
    static const std::unordered_map<std::string, Command> stringMap = {
        {"MoveUp", Command::kMoveUp},
        {"MoveDown", Command::kMoveDown},
        {"MoveLeft", Command::kMoveLeft},
        {"MoveRight", Command::kMoveRight},
        {"CastSpell", Command::kCastSpell},
        {"SwitchMode", Command::kSwitchMode},
        {"Attack", Command::kAttack},
        {"Quit", Command::kQuit},
        {"Help", Command::kHelp},
        {"Save", Command::kSave},
        {"Load", Command::kLoad},
        {"SkipTurn", Command::kSkipTurn},
        {"Invalid", Command::kInvalid}
    };
    
    auto it = stringMap.find(str);
    return (it != stringMap.end()) ? it->second : Command::kInvalid;
}