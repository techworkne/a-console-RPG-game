#include "MovementEntity.hpp"
#include "RewardSystem.hpp"
#include <algorithm>
#include <queue>
#include <unordered_set>

MovementEntity::MovementEntity(GameField* field) : field_(field) {}

bool MovementEntity::MoveEntity(size_t from_x, size_t from_y, size_t to_x, size_t to_y, Player* player_for_rewards = nullptr) {
    if (!field_ || !IsValidMove(from_x, from_y, to_x, to_y)) return false;

    FieldCell* source_cell = field_->GetCell(from_x, from_y);
    FieldCell* target_cell = field_->GetCell(to_x, to_y);

    if (!source_cell || !target_cell || source_cell->IsEmpty()) return false;

    std::unique_ptr<Entity> entity_to_move = source_cell->ExtractEntity();
    Entity* raw_entity = entity_to_move.get();

    if (!raw_entity) return false;
    if (target_cell->IsEmpty()){
        target_cell->SetEntity(std::move(entity_to_move));
        raw_entity->SetX(to_x);
        raw_entity->SetY(to_y);
    }

    HandleCellEffects(target_cell, raw_entity, player_for_rewards);
    return true;
}

bool MovementEntity::MoveEntity(Entity* entity, size_t to_x, size_t to_y, Player* player = nullptr) {
    if (!entity || !field_) return false;
    return MoveEntity(entity->GetX(), entity->GetY(), to_x, to_y, player);
}

bool MovementEntity::MoveEntityInDirection(Entity* entity, Direction direction) {
    if (!entity || !field_) return false;

    size_t current_x = entity->GetX();
    size_t current_y = entity->GetY();
    size_t new_x = current_x;
    size_t new_y = current_y;

    switch (direction) {
        case Direction::kUp:    new_y--; break;
        case Direction::kDown:  new_y++; break;
        case Direction::kLeft:  new_x--; break;
        case Direction::kRight: new_x++; break;
    }

    return MoveEntity(entity, new_x, new_y);
}

bool MovementEntity::MovePlayer(Player* player, Direction direction) {
    if (!player || player->IsDead() || player->IsStunned()) {
        return false;
    }

    return MoveEntityInDirection(player, direction);
}

bool MovementEntity::MoveEnemyTowardsPlayer(Enemy* enemy, Player* player) {
    if (!enemy || !player || !enemy->IsAlive() || !player->IsAlive()) {
        return false;
    }
    size_t current_x = enemy->GetX();
    size_t current_y = enemy->GetY();
    size_t player_x = player->GetX();
    size_t player_y = player->GetY();

    if (CanAttackPlayer(enemy, player)) {
        AttackPlayer(enemy, player);
        return true;
    }

    size_t new_x = current_x;
    size_t new_y = current_y;

    int dx = player_x - current_x;
    int dy = player_y - current_y;

    if (std::abs(dx) > std::abs(dy)) {
        new_x += (dx > 0) ? 1 : -1;
    } else {
        new_y += (dy > 0) ? 1 : -1;
    }

    if (MoveEntity(enemy, new_x, new_y, player)) {
        return true;
    } 

    std::vector<std::pair<int, int>> move_attempts;

    move_attempts.emplace_back(1, 0);
    move_attempts.emplace_back(-1, 0);
    move_attempts.emplace_back(0, 1);
    move_attempts.emplace_back(0, -1);

    for (const auto& [dx, dy] : move_attempts) {
        size_t new_x = current_x + dx;
        size_t new_y = current_y + dy;

        FieldCell* target_cell = field_->GetCell(new_x, new_y);
        if (target_cell && !target_cell->IsEmpty()) {

            Entity* target_entity = target_cell->GetEntity();

            if (target_entity && target_entity->GetType() == EntityType::kPlayer) {
                std::cout << "Enemy attacks player!\n";
                player->TakeDamage(enemy->GetDamage());
                return true;
            }
        }else if (target_cell || target_cell->IsEmpty()){
            if (MoveEntity(enemy, new_x, new_y, player)) {
                if (CanAttackPlayer(enemy, player)) {
                    AttackPlayer(enemy, player);
                }
                return true;
            }
        }
        break;
        
    }

    return false;
}

bool MovementEntity::CanAttackPlayer(Enemy* enemy, Player* player) {
    if (!enemy || !player) return false;
    
    size_t enemy_x = enemy->GetX();
    size_t enemy_y = enemy->GetY();
    size_t player_x = player->GetX();
    size_t player_y = player->GetY();
    
    int dx = std::abs(static_cast<int>(enemy_x) - static_cast<int>(player_x));
    int dy = std::abs(static_cast<int>(enemy_y) - static_cast<int>(player_y));
    
    return (dx == 1 && dy == 0) || (dx == 0 && dy == 1);
}

void MovementEntity::AttackPlayer(Enemy* enemy, Player* player) {
    if (!enemy || !player) return;
    
    int damage = enemy->GetDamage();
    player->TakeDamage(damage);
    std::cout << "Enemy attacks player for " << damage << " damage!\n";
    
    if (!player->IsAlive()) {
        std::cout << "Player has been defeated!\n";
    }
}

bool MovementEntity::IsValidMove(size_t from_x, size_t from_y, size_t to_x, size_t to_y) const {
    if (!field_) return false;

    
    if (to_x >= field_->GetWidth() || to_y >= field_->GetHeight()) {
        return false;
    }

    FieldCell* source_cell = field_->GetCell(from_x, from_y);
    FieldCell* target_cell = field_->GetCell(to_x, to_y);

    if (!source_cell || !target_cell) return false;
    
    if (source_cell->IsEmpty()) {
        return false;
    }
    
    if (!target_cell->IsPassable()) {
        return false;
    }

    if (!target_cell->IsEmpty()) {
        return false;
    }

    return true;
}

void MovementEntity::HandleCellEffects(FieldCell* cell, Entity* entity, Player* player) {
    if (!cell || !entity) return;

    switch (cell->GetType()) {
        case FieldCellType::kSlowdown:
            if (Player* player = dynamic_cast<Player*>(entity)) {
                player->Stun();
                std::cout << "Slowed! Skipping next turn.\n";
            }
            break;
            
        case FieldCellType::kTrap:
            HandleTrapActivation(cell, entity, player);
            break;
            
        default:
            break;
    }
}

void MovementEntity::HandleTrapActivation(FieldCell* cell, Entity* victim, Player* player) {
    if (!cell || !victim || victim->GetType() == EntityType::kPlayer) return;
    
    int trap_damage = cell->GetTrapDamage();
    victim->TakeDamage(trap_damage);
    
    std::cout << "Trap activated! ";
    std::cout << victim->getEntityTypeName(victim->GetType()) << " takes " << trap_damage << " damage!\n";
    
    cell->RemoveTrap();
    std::cout << "Trap disappeared.\n";
    
    
    if (!victim->IsAlive()) {
        EntityType entity_type = victim->GetType();
        cell->ExtractEntity();
        RewardSystem::GiveDestructionReward(entity_type, player);
    }
}
