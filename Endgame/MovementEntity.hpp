#ifndef GAME_MOVEMENTENTITY_HPP_
#define GAME_MOVEMENTENTITY_HPP_

#include "GameField.hpp"
#include "Entity.hpp"
#include "Player.hpp"
#include "Enemy.hpp"
#include "Direction.hpp"
#include <vector>
#include <memory>

class MovementEntity{
public:
    MovementEntity(GameField* field);
    ~MovementEntity() = default;

    
    bool MoveEntity(size_t from_x, size_t from_y, size_t to_x, size_t to_y, Player* player_for_rewards);
    bool MoveEntity(Entity* entity, size_t to_x, size_t to_y, Player* player);
    bool MoveEntityInDirection(Entity* entity, Direction direction);
    
    
    bool MovePlayer(Player* player, Direction direction);
    bool MoveEnemyTowardsPlayer(Enemy* enemy, Player* player);
    bool CanAttackPlayer(Enemy* enemy, Player* player);
    void AttackPlayer(Enemy* enemy, Player* player);
    
    bool IsValidMove(size_t from_x, size_t from_y, size_t to_x, size_t to_y) const;
    

private:
    GameField* field_;
    
    void HandleCellEffects(FieldCell* cell, Entity* entity, Player* player);
    void HandleTrapActivation(FieldCell* cell, Entity* entity, Player* player);
    
};

#endif