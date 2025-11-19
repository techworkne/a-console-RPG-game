#ifndef GAME_GAMELOGIC_HPP_
#define GAME_GAMELOGIC_HPP_

#include "GameState.hpp"
#include "GameSerialize.hpp"
#include "GameField.hpp"
#include "Render.hpp"
#include "InputHandler.hpp"
#include "MovementEntity.hpp" 
#include "Player.hpp"
#include "Enemy.hpp"
#include "EnemyBuilding.hpp"
#include "EnemyTower.hpp"
#include <memory>
#include <fstream>

class GameLogic {
public:
    GameLogic(size_t field_width, size_t field_height);
    ~GameLogic() = default;

    void HandlePlayerInput(Direction direction);
    void HandlePlayerAttack();
    void HandlePlayerDeath();
    void HandleSpellSelection();

    bool IsGameOver() const { return game_state_.IsGameOver(); }
    
    int GetScore() const { return game_state_.GetScore(); }
    const GameField* GetField() const { return game_state_.GetField(); }
    GameField* GetField() { return game_state_.GetField(); }
    const Player* GetPlayer() const { return game_state_.GetPlayer(); }
    Player* GetPlayer() { return game_state_.GetPlayer(); }
    GameState* GetGameState() { return &game_state_; }
    const GameState* GetGameState() const { return &game_state_; }
    MovementEntity* GetMovementSystem() { return movement_system_.get(); }
    
    void InitializeGame(size_t level);
    void InitializeNewLevel(size_t width, size_t height, size_t level);

    bool SaveToFile(const std::string& filename) const;
    bool LoadFromFile(const std::string& filename);
private:
    GameState game_state_;
    Renderer renderer_;
    InputHandler input_handler_;
    std::unique_ptr<MovementEntity> movement_system_;

    void HandleCombat(Player* player, Enemy* enemy);
    void HandleCombat(Player* player, EnemyBuilding* building);

    bool CheckPathClear(size_t from_x, size_t from_y, size_t to_x, size_t to_y) const;
    bool FindValidSpawnPosition(size_t& x, size_t& y, size_t min_y, size_t max_y);
};

#endif
