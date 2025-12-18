#ifndef GAME_ENEMYARCHITECTURE_HPP_
#define GAME_ENEMYARCHITECTURE_HPP_

#include "Entity.hpp"
#include <cstddef>
#include <iostream>

class EnemyArchitecture : public Entity {
public:
    EnemyArchitecture(int initial_health, size_t x, size_t y, int spawn_interval);
    virtual ~EnemyArchitecture() = default;
    
    virtual bool Update();
    virtual void TakeDamage(int damage) override;
    
    int GetSpawnInterval() const { return spawn_interval_; }
    bool CanSpawn() const { return turns_since_spawn_ >= spawn_interval_; }
    void ResetSpawnTimer() { turns_since_spawn_ = 0; }

protected:
    int spawn_interval_;
    int turns_since_spawn_ = 0;
};

#endif