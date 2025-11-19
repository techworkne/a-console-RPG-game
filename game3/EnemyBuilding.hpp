#ifndef GAME_ENEMYBUILDING_HPP_
#define GAME_ENEMYBUILDING_HPP_

#include "EnemyArchitecture.hpp"

class EnemyBuilding : public EnemyArchitecture {
public:
    EnemyBuilding(int initial_health, size_t x, size_t y, int spawn_interval);

    EntityType GetType() const override;
    void TakeDamage(int damage) override;
    bool Update() override;
};

#endif