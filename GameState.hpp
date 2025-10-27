#ifndef GAME_GAMESTATE_HPP_
#define GAME_GAMESTATE_HPP_

#include "GameField.hpp"
#include "Player.hpp"
#include <memory>
#include <vector>

class GameState {
public:
    GameState(size_t width, size_t height);
    ~GameState() = default;
    
    
    GameField* GetField() { return game_field_.get(); }
    const GameField* GetField() const { return game_field_.get(); }
    Player* GetPlayer() { return player_; }
    const Player* GetPlayer() const { return player_; }
    
    
    void SetPlayer(Player* player) { player_ = player; }
    
    bool IsGameOver() const;
    int GetScore() const;
    
private:
    std::unique_ptr<GameField> game_field_;
    Player* player_;
};

#endif