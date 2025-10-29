#include "GameLogic.hpp"
#include "Spell.hpp"
#include "DirectDamageSpell.hpp"
#include "PlayerHand.hpp"
#include "RewardSystem.hpp"
#include "AreaDamageSpell.hpp"
#include <iostream>
#include <random>
#include <algorithm>

GameLogic::GameLogic(size_t field_width, size_t field_height)
    : game_state_(field_width, field_height) {
    movement_system_ = std::make_unique<MovementEntity>(game_state_.GetField());
    InitializeGame();
}

void GameLogic::Run() {
    std::cout << "Welcome to the Console Game!\n";
    renderer_.RenderControls();

    while (!IsGameOver()) {
        renderer_.Render(*game_state_.GetField(), game_state_.GetPlayer());
        
        Player* player = game_state_.GetPlayer();
        
        
        if (player && player->GetHealth() <= 0 && !player->IsDead() && player->GetLives() > 0) {
            std::cout << "Player health reached 0! Handling death...\n";
            HandlePlayerDeath();
            continue; 
        }
        
        if (player && player->IsDead()) {
            std::cout << "Player is dead! Press Enter to continue...";
            std::string input;
            std::getline(std::cin, input);
            HandlePlayerDeath();
            continue;
        }

        
        if (player && player->IsAlive() && player->IsStunned()) {
            std::cout << "Player is stunned~\n";
            std::cout << "PRESS ENTER TO CONTINUE!\n";
            ProcessTurn();
            std::string input;
            std::getline(std::cin, input);
            player->UnStun();
            continue;
        }


        Command command = input_handler_.GetCommand();
        bool turnProcessed = false;

        if (player && player->IsDead()) {
            std::cout << "Player is dead! Cannot perform actions.\n";
            continue;
        }

        switch (command) {
            case Command::kMoveUp:
                if (!player->IsDead()) {
                    HandlePlayerInput(Direction::kUp);
                    turnProcessed = true;
                }
                break;
            case Command::kMoveDown:
                if (!player->IsDead()) {
                    HandlePlayerInput(Direction::kDown);
                    turnProcessed = true;
                }
                break;
            case Command::kMoveLeft:
                if (!player->IsDead()) {
                    HandlePlayerInput(Direction::kLeft);
                    turnProcessed = true;
                }
                break;
            case Command::kMoveRight:
                if (!player->IsDead()) {
                    HandlePlayerInput(Direction::kRight);
                    turnProcessed = true;
                }
                break;
            case Command::kSwitchMode:
                if (!player->IsDead() && !player->IsStunned()) {
                    player->SwitchCombatMode();
                    std::cout << "Combat mode: " << (player->GetCombatMode() == CombatMode::kMelee ? "Cursed dagger" : "Desert Eagle Mark XIX") << "\n";
                    turnProcessed = true;
                } else if (player->IsDead()) {
                    std::cout << "Player is dead! Cannot switch mode.\n";
                } else {
                    std::cout << "Stunned! Cannot switch mode.\n";
                }
                break;
            case Command::kAttack:
                HandlePlayerAttack();
                turnProcessed = true;
                break;
            case Command::kCastSpell:
                HandleSpellSelection();
                turnProcessed = true;
                break;
            case Command::kSkipTurn:
                std::cout << "Player skips a turn.\n";
                turnProcessed = true;
                break;
            case Command::kHelp:
                renderer_.RenderControls();
                break;
            case Command::kQuit:
                std::cout << "Quitting game...\n";
                return;
            case Command::kInvalid:
                std::cout << "Invalid command.\n";
                continue;
        }

        if (turnProcessed && !player->IsDead()) {
            ProcessTurn();
        }
    }

    renderer_.RenderGameOver(GetScore());
}

void GameLogic::ProcessTurn() {
    Player* player = game_state_.GetPlayer();
    if (!player || IsGameOver()) return;

    if (player->GetHealth() <= 0 && player->IsDead()){
        player->SetDead(true);
        HandlePlayerDeath();
        return;
    }

    if (player->IsDead()) {
        HandlePlayerDeath();
        return;
    }

    UpdateEnemies();
    UpdateBuildings();
    UpdateTowers(); 
}

void GameLogic::HandlePlayerInput(Direction direction) {
    Player* player = game_state_.GetPlayer();
    if (!player) return;

    if (player->IsDead()) {
        std::cout << "Player is dead! Cannot move.\n";
        return;
    }

    if (player->IsStunned()) {
        std::cout << "Stunned! Cannot move.\n";
        return;
    }

     if (movement_system_->MovePlayer(player, direction)) {
        std::cout << "Moved to (" << player->GetX() << ", " << player->GetY() << ")\n";
    } else {
        size_t current_x = player->GetX();
        size_t current_y = player->GetY();
        size_t target_x = current_x;
        size_t target_y = current_y;

        switch (direction) {
            case Direction::kUp:    target_y--; break;
            case Direction::kDown:  target_y++; break;
            case Direction::kLeft:  target_x--; break;
            case Direction::kRight: target_x++; break;
        }

         FieldCell* target_cell = game_state_.GetField()->GetCell(target_x, target_y);
        if (target_cell && !target_cell->IsEmpty()) {
            Entity* target_entity = target_cell->GetEntity();
            if (target_entity) {
                switch (target_entity->GetType()) {
                    case EntityType::kEnemy:
                        std::cout << "Attacking enemy!\n";
                        HandleCombat(player, dynamic_cast<Enemy*>(target_entity));
                        break;
                    case EntityType::kBuilding:
                        std::cout << "Attacking building!\n";
                        HandleCombat(player, dynamic_cast<EnemyBuilding*>(target_entity));
                        break;
                    default:
                        std::cout << "Cannot move there!\n";
                        break;
                }
            }
        } else {
            std::cout << "Cannot move there!\n";
        }
    }
}

void GameLogic::HandlePlayerAttack() {
    Player* player = game_state_.GetPlayer();
    if (!player) return;

    if (player->IsStunned()) {
        std::cout << "Stunned! Cannot attack.\n";
        return;
    }

    std::cout << "Choose attack direction (w/a/s/d): ";
    Direction direction = input_handler_.GetAttackDirection();
    
    CombatMode mode = player->GetCombatMode();
    int range = (mode == CombatMode::kMelee) ? player->GetMeleeRange() : player->GetRangedRange();
    int damage = (mode == CombatMode::kMelee) ? player->GetMeleeDamage() : player->GetRangedDamage();

    bool attacked = false;
    GameField* field = game_state_.GetField();
    
    for (int distance = 1; distance <= range; ++distance) {
        int target_x = player->GetX();
        int target_y = player->GetY();

        switch (direction) {
            case Direction::kUp: target_y -= distance; break;
            case Direction::kDown: target_y += distance; break;
            case Direction::kLeft: target_x -= distance; break;
            case Direction::kRight: target_x += distance; break;
        }

        if (target_x < 0 || target_x >= static_cast<int>(field->GetWidth()) ||
            target_y < 0 || target_y >= static_cast<int>(field->GetHeight())) {
            continue;
        }

        FieldCell* target_cell = field->GetCell(target_x, target_y);
        if (!target_cell) continue;

        if (mode == CombatMode::kRanged) {
            if (!CheckPathClear(player->GetX(), player->GetY(), target_x, target_y)) {
                break;
            }
        }

        if (!target_cell->IsEmpty()) {
            Entity* target_entity = target_cell->GetEntity();
            if (target_entity) {
                target_entity->TakeDamage(damage);
                std::cout << (mode == CombatMode::kMelee ? "Cursed dagger" : "Desert Eagle Mark XIX") << " attack on target at distance " << distance << "!\n";
                std::cout << "Hit for " << damage << " damage! ";
                std::cout << target_entity->getEntityTypeName(target_entity->GetType())<< " HP: " << target_entity->GetHealth() << "\n";

                if (!target_entity->IsAlive()) {
                    RewardSystem::GiveDestructionReward(target_entity->GetType(), player);
                    field->RemoveEntity(target_entity->GetX(), target_entity->GetY());
                }
                
                attacked = true;
                return;
            }
        }
    }

    if (!attacked) {
        std::cout << "No valid target in attack range!\n";
    }
}

void GameLogic::HandleSpellSelection() {
    Player* player = game_state_.GetPlayer();
    if (!player || player->IsStunned() || player->IsDead()) return;
    
    PlayerHand& hand = player->getHand();
    
    
    std::cout << "Choose spell:\n";
    for (size_t i = 0; i < hand.getSpellCount(); ++i) {
        const Spell* spell = hand.getSpell(i);
        std::cout << i + 1 << ". " << spell->getName() << " (mana: " << spell->getManaCost() <<") \n";
    }
    std::cout << "0. Revoke\n";
    
    
    std::string input;
    std::getline(std::cin, input);
    
    if (input.empty() || !std::isdigit(input[0])) {
        std::cout << "Invalid input! Enter a single digit.\n";
        return;
    }
    int choice = input[0] - '0';  
    if (choice == 0) return;
    
    size_t spell_index = choice - 1;
    if (spell_index >= hand.getSpellCount()) {
        std::cout << "Wrong choice!\n";
        return;
    }
    
    std::cout << "Choice direction (w/a/s/d): ";
    Direction direction = input_handler_.GetAttackDirection();
    
    
    size_t target_x = player->GetX();
    size_t target_y = player->GetY();
    
    switch (direction) {
        case Direction::kUp:    target_y-=3; break;
        case Direction::kDown:  target_y+=3; break;
        case Direction::kLeft:  target_x-=3; break;
        case Direction::kRight: target_x+=3; break;
    }
    
    
    Spell* spell = hand.getSpell(spell_index);
    if (player->getMana() >= spell->getManaCost()) {

       if (spell->cast(*game_state_.GetField(), player, target_x, target_y)) {
            std::cout << "Spell cast successfully!\n";
            player->setMana(player->getMana() - spell->getManaCost());
        } else {
            std::cout << "Spell failed!\n";
        }
    }
    else {
        std::cout << "Not enough mana!\n";
    }
}

void GameLogic::HandleCombat(Player* player, Enemy* enemy) {
    if (!player || !enemy || player->IsDead() || !enemy->IsAlive()) return;

    int player_damage = player->GetCurrentDamage();
    enemy->TakeDamage(player_damage);
    std::cout << "Enemy HP: " << enemy->GetHealth() << "\n";

    if (!enemy->IsAlive()) {
        std::cout << "Enemy defeated! +10 points! +10 mana!\n";
        player->AddScore(10);
        player->addMana(10);
        game_state_.GetField()->RemoveEntity(enemy->GetX(), enemy->GetY());
        return;
    }

    int enemy_damage = enemy->GetDamage();
    player->TakeDamage(enemy_damage);
    std::cout << "Enemy hits player for " << enemy_damage << " damage! ";
    std::cout << "Player HP: " << player->GetHealth() << "\n";

    if (player->GetHealth() <= 0) {
        player->SetDead(true);
    }
}

void GameLogic::HandleCombat(Player* player, EnemyBuilding* building) {
    if (!player || !building || player->IsDead() || !building->IsAlive()) return;

    int player_damage = player->GetDamage();
    building->TakeDamage(player_damage);
    std::cout << "Building HP: " << building->GetHealth() << "\n";

    if (!building->IsAlive()) {
        RewardSystem::GiveDestructionReward(building->GetType(), player);
        game_state_.GetField()->RemoveEntity(building->GetX(), building->GetY());
    }
}

void GameLogic::HandlePlayerDeath() {
    Player* player = game_state_.GetPlayer();
    if (!player) return;
    
    player->SetDead(true);
    int current_lives = player->GetLives() - 1;
    std::cout << "Player died! Lives remaining: " << current_lives << "\n";

    if (current_lives > 0) {
        size_t respawn_x, respawn_y;
        if (FindValidSpawnPosition(respawn_x, respawn_y, 1, 3)) {
            int current_score = player->GetScore();
            GameField* field = game_state_.GetField();
            
            field->RemoveEntity(player->GetX(), player->GetY());
            
            auto new_player = std::make_unique<Player>(100, respawn_x, respawn_y);
            new_player->SetScore(current_score);
            new_player->SetLives(current_lives);
            new_player->SetDead(false);
            
            auto starting_spell = std::make_unique<DirectDamageSpell>(50, 30);
            new_player->addSpellToHand(std::move(starting_spell));

            field->PlaceEntity(respawn_x, respawn_y, std::move(new_player));
            
            FieldCell* respawn_cell = field->GetCell(respawn_x, respawn_y);
            if (respawn_cell && respawn_cell->GetEntity()) {
                game_state_.SetPlayer(dynamic_cast<Player*>(respawn_cell->GetEntity()));
            }
            
            std::cout << "Player respawned at (" << respawn_x << ", " << respawn_y << ")\n";
        } else {
            std::cout << "Cannot find respawn position!\n";
        }
    } else {
        player->SetLives(0);
        std::cout<< "YOU ARE DEAD T-T";
    }
}

void GameLogic::UpdateEnemies() {
    GameField* field = game_state_.GetField();
    Player* player = game_state_.GetPlayer();
    if (!field || !player) return;

    std::vector<Enemy*> enemies;
    for (size_t y = 0; y < field->GetHeight(); ++y) {
        for (size_t x = 0; x < field->GetWidth(); ++x) {
            FieldCell* cell = field->GetCell(x, y);
            if (cell && !cell->IsEmpty()) {
                Entity* entity = cell->GetEntity();
                if (entity && entity->GetType() == EntityType::kEnemy && entity->IsAlive()) {
                    Enemy* enemy = dynamic_cast<Enemy*>(entity);
                    if (enemy) {
                        enemies.push_back(enemy);
                    }
                }
            }
        }
    }
    
    for (Enemy* enemy : enemies) {
        if (player->IsAlive()) {
            movement_system_->MoveEnemyTowardsPlayer(enemy, player);
        }
    }
}

void GameLogic::UpdateTowers() {
    GameField* field = game_state_.GetField();
    Player* player = game_state_.GetPlayer();
    if (!field || !player || player->IsDead()) return;

    for (size_t y = 0; y < field->GetHeight(); ++y) {
        for (size_t x = 0; x < field->GetWidth(); ++x) {
            FieldCell* cell = field->GetCell(x, y);
            if (cell && !cell->IsEmpty()) {
                Entity* entity = cell->GetEntity();
                if (entity && entity->GetType() == EntityType::kTower && entity->IsAlive()) {
                    EnemyTower* tower = dynamic_cast<EnemyTower*>(entity);
                    if (tower) {
                        tower->Update();
                        if (player && player->IsAlive()) {
                            tower->AttackPlayerInRange(*field, player);
                        }

                    }
                }
            }
        }
    }
}

void GameLogic::UpdateBuildings() {
    GameField* field = game_state_.GetField();
    if (!field) return;

    for (size_t y = 0; y < field->GetHeight(); ++y) {
        for (size_t x = 0; x < field->GetWidth(); ++x) {
            FieldCell* cell = field->GetCell(x, y);
            if (cell && !cell->IsEmpty()) {
                Entity* entity = cell->GetEntity();
                if (entity && entity->GetType() == EntityType::kBuilding && entity->IsAlive()) {
                    EnemyBuilding* building = dynamic_cast<EnemyBuilding*>(entity);
                    if (building && building->Update()) {
                        std::vector<std::pair<size_t, size_t>> adjacent_cells = {
                            {x-1, y}, {x+1, y}, {x, y-1}, {x, y+1}
                        };
                        
                        for (auto [adj_x, adj_y] : adjacent_cells) {
                            FieldCell* adj_cell = field->GetCell(adj_x, adj_y);
                            if (adj_cell && adj_cell->IsEmpty() && adj_cell->IsPassable()) {
                                auto new_enemy = std::make_unique<Enemy>(50, 15, adj_x, adj_y);
                                field->PlaceEntity(adj_x, adj_y, std::move(new_enemy));
                                std::cout << "Building spawned enemy at (" << adj_x << ", " << adj_y << ")\n";
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
}

bool GameLogic::CheckPathClear(size_t from_x, size_t from_y, size_t to_x, size_t to_y) const {
    const GameField* field = game_state_.GetField() ;
    if (!field) return false;

    if (from_x == to_x) {
        size_t start_y = std::min(from_y, to_y);
        size_t end_y = std::max(from_y, to_y);
        
        for (size_t y = start_y + 1; y < end_y; ++y) {
            const FieldCell* cell = field->GetCell(from_x, y);
            if (cell && !cell->IsPassable()) {
                return false;
            }
        }
    } else if (from_y == to_y) {
        size_t start_x = std::min(from_x, to_x);
        size_t end_x = std::max(from_x, to_x);
        
        for (size_t x = start_x + 1; x < end_x; ++x) {
            const FieldCell* cell = field->GetCell(x, from_y);
            if (cell && !cell->IsPassable()) {
                return false;
            }
        }
    } else {
        int dx = (to_x > from_x) ? 1 : -1;
        int dy = (to_y > from_y) ? 1 : -1;
        
        size_t x = from_x;
        size_t y = from_y;
        
        while (x != to_x && y != to_y) {
            x = static_cast<size_t>(static_cast<int>(x) + dx);
            y = static_cast<size_t>(static_cast<int>(y) + dy);
            
            const FieldCell* cell = field->GetCell(x, y);
            if (cell && !cell->IsPassable()) {
                return false;
            }
        }
    }
    
    return true;
}

void GameLogic::InitializeGame() {
    
    size_t start_x, start_y;
    GameField* field = game_state_.GetField();
    
    if (FindValidSpawnPosition(start_x, start_y, 1, 3)) {
        auto player_entity = std::make_unique<Player>(100, start_x, start_y);
        Player* player_ptr = player_entity.get();
        player_ptr->SetDead(false);
        
        auto starting_spell = std::make_unique<DirectDamageSpell>(50, 30);
        player_ptr->addSpellToHand(std::move(starting_spell));

        field->PlaceEntity(start_x, start_y, std::move(player_entity));
        game_state_.SetPlayer(player_ptr);
        std::cout << "Player spawned at (" << start_x << ", " << start_y << ")\n";
    } else {
        std::cerr << "ERROR: Cannot find valid spawn position for player!\n";
        return;
    }

    for (int i = 0; i < 3; ++i) {
        size_t enemy_x, enemy_y;
        if (FindValidSpawnPosition(enemy_x, enemy_y, field->GetHeight() - 3, field->GetHeight() - 2)) {
            auto enemy = std::make_unique<Enemy>(50, 15, enemy_x, enemy_y);
            field->PlaceEntity(enemy_x, enemy_y, std::move(enemy));
        }
    }

    
    for (int i = 0; i < 2; ++i) {
        size_t building_x, building_y;
        if (FindValidSpawnPosition(building_x, building_y, field->GetHeight() / 3, field->GetHeight() / 2)) {
            auto building = std::make_unique<EnemyBuilding>(200, building_x, building_y, 8);
            field->PlaceEntity(building_x, building_y, std::move(building));
        }
    }

    for (int i = 0; i < 2; ++i) {
        size_t tower_x, tower_y;
        if (FindValidSpawnPosition(tower_x, tower_y, field->GetHeight() - 5, field->GetHeight() - 3)) {
            auto tower = std::make_unique<EnemyTower>(150, tower_x, tower_y, 5);
            field->PlaceEntity(tower_x, tower_y, std::move(tower));
        }
    }
}

bool GameLogic::FindValidSpawnPosition(size_t& x, size_t& y, size_t min_y, size_t max_y) {
    GameField* field = game_state_.GetField();
    if (!field) return false;

    std::random_device rd;
    std::mt19937 gen(rd());
    
    if (min_y >= field->GetHeight()) min_y = 1;
    if (max_y >= field->GetHeight()) max_y = field->GetHeight() - 2;
    if (min_y > max_y) std::swap(min_y, max_y);
    
    std::uniform_int_distribution<size_t> dist_x(1, field->GetWidth() - 2);
    std::uniform_int_distribution<size_t> dist_y(min_y, max_y);

    for (int attempts = 0; attempts < 50; ++attempts) {
        size_t try_x = dist_x(gen);
        size_t try_y = dist_y(gen);
        
        FieldCell* cell = field->GetCell(try_x, try_y);
        if (cell && cell->IsEmpty() && cell->IsPassable()) {
            x = try_x;
            y = try_y;
            return true;
        }
    }

    for (size_t try_y = min_y; try_y <= max_y; ++try_y) {
        for (size_t try_x = 1; try_x < field->GetWidth() - 1; ++try_x) {
            FieldCell* cell = field->GetCell(try_x, try_y);
            if (cell && cell->IsEmpty() && cell->IsPassable()) {
                x = try_x;
                y = try_y;
                return true;
            }
        }
    }

    return false;
}