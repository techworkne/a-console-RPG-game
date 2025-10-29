#ifndef COMMAND_HPP_
#define COMMAND_HPP_

enum class Command {
    kMoveUp,
    kMoveDown,
    kMoveLeft,
    kMoveRight,
    kCastSpell,
    kSwitchMode,
    kAttack,
    kSkipTurn,
    kQuit,
    kInvalid,
    kHelp
};
#endif