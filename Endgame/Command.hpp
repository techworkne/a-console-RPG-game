#ifndef GAME_COMMAND_HPP
#define GAME_COMMAND_HPP

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
    kHelp,
    kSave,
    kLoad
};
#endif