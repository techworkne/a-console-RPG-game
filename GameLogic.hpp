#ifndef GAME_GAMELOGIC_HPP_
#define GAME_GAMELOGIC_HPP_

#include "GameState.hpp"
#include "Render.hpp"
#include "GameField.hpp"
#include "InputHandler.hpp"
#include "Player.hpp"
#include "Enemy.hpp"
#include "EnemyBuilding.hpp"
#include "EnemyTower.hpp"
#include "Direction.hpp"
#include <memory>

class GameLogic {
public:
    GameLogic(size_t field_width, size_t field_height);
    ~GameLogic() = default;

    void Run();
    void ProcessTurn();
    void HandlePlayerInput(Direction direction);
    void HandlePlayerAttack();
    bool IsGameOver() const { return game_state_.IsGameOver(); }
    int GetScore() const { return game_state_.GetScore(); }

private:
    GameState game_state_;
    Renderer renderer_;
    InputHandler input_handler_;

    void InitializeGame();
    void HandleCombat(Player* player, Enemy* enemy);
    void HandleCombat(Player* player, EnemyBuilding* building);
    void HandlePlayerDeath();
    void UpdateEnemies();
    void UpdateBuildings();
    void UpdateTowers();
    bool CheckPathClear(size_t from_x, size_t from_y, size_t to_x, size_t to_y) const;
    bool FindValidSpawnPosition(size_t& x, size_t& y, size_t min_y, size_t max_y);
    void HandleSpellSelection();
    void HandleTrapActivation(FieldCell* cell, Entity* victim);
};

#endif