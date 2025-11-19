#include "GameLoop.hpp"
#include "InputHandler.hpp"
#include "Render.hpp"
#include "GameLogic.hpp"

GameLoop::GameLoop(GameLogic& logic, InputHandler& input, Renderer& renderer) : logic_(logic), input_handler_(input), renderer_(renderer) {}

void GameLoop::ProcessTurn() {
    Player* player = logic_.GetPlayer();
    if (!player || logic_.IsGameOver()) return;

    if (player->GetHealth() <= 0 && player->IsDead()){
        player->SetDead(true);
        logic_.HandlePlayerDeath();
        return;
    }

    if (player->IsDead()) {
        logic_.HandlePlayerDeath();
        return;
    }
    UpdateEnemies();
    UpdateBuildings();
    UpdateTowers();
}

void GameLoop::Run() {
    std::cout << "======================================================================================================================\n";
    std::cout << "Welcome to the Console Game!\n";
    renderer_.RenderControls();

    HandleMainMenu();

    while (!logic_.IsGameOver()) {
        if (IsLevelComplete()) {
            HandleLevelComplete();
            if (logic_.IsGameOver()) break;
            continue;
        }
        const GameField* field = logic_.GetField();
        renderer_.Render(*field, logic_.GetPlayer(), current_level_);
        
        Player* player = logic_.GetPlayer();
        if (player && player->GetHealth() <= 0 && !player->IsDead() && player->GetLives() > 0) {
            std::cout << "Player health reached 0! Handling death...\n";
            logic_.HandlePlayerDeath();
            continue;
        }
        
        if (player && player->IsDead()) {
            std::cout << "Player is dead! Press Enter to continue...";
            std::string input;
            std::getline(std::cin, input);
            logic_.HandlePlayerDeath();
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
                    logic_.HandlePlayerInput(Direction::kUp);
                    turnProcessed = true;
                }
                break;
            case Command::kMoveDown:
                if (!player->IsDead()) {
                    logic_.HandlePlayerInput(Direction::kDown);
                    turnProcessed = true;
                }
                break;
            case Command::kMoveLeft:
                if (!player->IsDead()) {
                    logic_.HandlePlayerInput(Direction::kLeft);
                    turnProcessed = true;
                }
                break;
            case Command::kMoveRight:
                if (!player->IsDead()) {
                    logic_.HandlePlayerInput(Direction::kRight);
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
                logic_.HandlePlayerAttack();
                turnProcessed = true;
                break;
            case Command::kCastSpell:
                logic_.HandleSpellSelection();
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
                if (logic_.SaveToFile("savegame.txt")) {
                    std::cout << "Game saved successfully!\n";
                } else {
                    std::cout << "Failed to save game!\n";
                }
                std::cout << "Quitting game...\n";
                exit(0);
            case Command::kSave:{
                std::cout << "Enter save file name (or press Enter for default): ";
                std::string filename;
                std::getline(std::cin, filename);
                
                if (filename.empty()) filename = "savegame.txt";
                
                if (logic_.SaveToFile(filename)) {
                    std::cout << "Game saved successfully!\n";
                } else {
                    std::cout << "Failed to save game!\n";
                }
                break;}
            case Command::kLoad:{
                std::cout << "Enter save file name (or press Enter for default): ";
                std::string filename;
                std::getline(std::cin, filename);
                
                if (filename.empty()) filename = "savegame.txt";
                
                if (logic_.LoadFromFile(filename)) {
                    current_level_ = logic_.GetGameState()->GetLevel();
                    std::cout << "Game loaded! Current level: " << current_level_ << "\n";
                    
                    //const GameField* field = logic_.GetField();
                    //renderer_.Render(*field, logic_.GetPlayer(), current_level_);
                } else {
                    std::cout << "Failed to load game!\n";
                }
                break;}
            case Command::kInvalid:
                std::cout << "Invalid command.\n";
                continue;
        }

        if (turnProcessed && !player->IsDead()) {
            ProcessTurn();
        }
    }

    renderer_.RenderGameOver(logic_.GetScore(), current_level_);
    game_started_ = false;
    HandleMainMenu();
    
}

void GameLoop::HandleMainMenu() {
    while (!game_started_) {
        std::cout << "\n=== MAIN MENU ===\n";
        std::cout << "1. Start New Game\n";
        std::cout << "2. Load Game\n";
        std::cout << "3. Exit\n";
        std::cout << "Choose: ";
        
        std::string input;
        std::getline(std::cin, input);

        if (input == "1") {
            ResetLevel();
            logic_.GetGameState()->ResetLevel();
            game_started_ = true;
            std::cout << "Starting new game...\n";
            InitializeNewLevel();
            Run();
        } else if (input == "2"){
            game_started_ = true;
            std::cout << "Enter save file name (or press Enter for default): ";
            std::string filename;
            std::getline(std::cin, filename);
            
            if (filename.empty()) filename = "savegame.txt";
            
            if (logic_.LoadFromFile(filename)) {
                current_level_ = logic_.GetGameState()->GetLevel();
                std::cout << "Game loaded! Current level: " << current_level_ << "\n";
                
                const GameField* field = logic_.GetField();
                renderer_.Render(*field, logic_.GetPlayer(), current_level_);
                game_started_ = true;
                return;
            } else {
                std::cout << "Failed to load game!\n";
            }
        }
        else if (input == "3") {
            if (logic_.SaveToFile("savegame.txt")) {
                std::cout << "Game saved successfully!\n";
            } else {
                std::cout << "Failed to save game!\n";
            }
            std::cout << "Goodbye!\n";
            exit(0);
        } else {
            std::cout << "Invalid choice. Please enter 1, 2 or 3.\n";
        }
    }
}

void GameLoop::UpdateEnemies() {
    GameField* field = logic_.GetField();
    Player* player = logic_.GetPlayer();
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
            logic_.GetMovementSystem()->MoveEnemyTowardsPlayer(enemy, player);
        }
    }
}

void GameLoop::UpdateTowers() {
    GameField* field = logic_.GetField();
    Player* player = logic_.GetPlayer();
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

void GameLoop::UpdateBuildings() {
    GameField* field = logic_.GetField();
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
                                auto new_enemy = std::make_unique<Enemy>(50 + current_level_, 15 + current_level_*2, adj_x, adj_y);
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

void GameLoop::HandleLevelComplete() {
    std::cout << "LEVEL COMPLETE! All enemies defeated!\n";
    std::cout << "Final score: " << logic_.GetScore() << "\n";
    
    UpgradeAfterLevel();
}

void GameLoop::InitializeNewLevel() {
    std::cout << "=== LEVEL " << current_level_ << " ===\n";
    size_t level_size = (current_level_ + 15 > 25) ? 25 : current_level_ + 15; 

    logic_.InitializeNewLevel(level_size, level_size, current_level_);
    std::cout << "New level initialized! Map: " << level_size << "x" << level_size << "\n";
}

void GameLoop::LoadNextLevel() {
    
    Player* player = logic_.GetPlayer();
    PlayerHand& hand = player->getHand();
    int current_score = player->GetScore();
    int current_lives = player->GetLives();
    int current_mana = player->getMana();
    
    if (hand.getSpellCount() > 1){
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<size_t> dist(0, hand.getSpellCount() - 1);
        hand.removeSpell(dist(gen));
    }
    

    AddLevel();
    logic_.GetGameState()->AddLevel();
    InitializeNewLevel();
    
    player->SetScore(current_score + 100);
    player->SetLives(current_lives);
    player->setMana(current_mana + 50);
    player->ResetHealth(100);
    player->AddLives();
    std::cout << "Starting next level! You have lost some magical abilities...\n";
    std::cout << "Bonus: +100 points, +50 mana, +1 live, restore health <(+.+)>\n";
}

void GameLoop::UpgradeAfterLevel() {
    int player_score = logic_.GetScore();
    std::cout << "\nCongratulations! Choose upgrade:\n";
    std::cout << "=========================================================================================================================================\n";
    std::cout << "Your score: " << player_score << ".\n";
    std::cout << "0. Skip. Dont get better.\n";
    std::cout << "1. Small mana potion. +50 mana. Cost: 100 points.\n";
    std::cout << "2. Mana potion. +100 mana. Cost: 200 points.\n";
    std::cout << "3. Strengthening the cursed energy. +2 cursed dagger strike power. Cost: 300 points.\n";
    std::cout << "4. The introduction of an improved curse to increase kinetic energy. +2 to the power of the Desert Eagle Mark XIX shot. Cost: 450 points.\n";

    std::string input;
    std::getline(std::cin, input);

    if (input == "0")
    {
        LoadNextLevel();
    } else if (input == "1")
    {
        std::cout << "\nYou bought small mana potion.\n";
        logic_.GetPlayer()->addMana(50);
    } else if (input == "2")
    {
        std::cout << "\nYou bought mana potion.\n";
        logic_.GetPlayer()->addMana(100);
    } else if (input == "3")
    {
        std::cout << "\nYou bought strengthening the cursed energy.\n";
        logic_.GetPlayer()->AddMeleeDamage(2);
    } else if (input == "4")
    {
        std::cout << "\nYou bought the introduction of an improved curse to increase kinetic energy.\n";
        logic_.GetPlayer()->AddRangeDamage(2);
    }
    
    
    LoadNextLevel();
    
}

// void GameLoop::HandleGameEnd() {
//     while (true) {
//         std::cout << "\nChoose option:\n";
//         std::cout << "1. Restart Game\n";
//         std::cout << "2. Load save game\n";
//         std::cout << "3. Exit\n";
//         std::cout << "Choose: ";
        
//         std::string input;
//         std::getline(std::cin, input);
        
//         if (input == "1") {
//             game_started_ = false;
//             ResetLevel();
//             logic_.GetGameState()->ResetLevel();
//             logic_.InitializeNewLevel(15, 15, current_level_);
//             Run();
//             break;
//         } else if (input == "2"){
//             std::cout << "Enter save file name (or press Enter for default): ";
//             std::string filename;
//             std::getline(std::cin, filename);
            
//             if (filename.empty()) filename = "savegame.txt";
            
//             if (logic_.LoadFromFile(filename)) {
//                 current_level_ = logic_.GetGameState()->GetLevel();
//                 std::cout << "Game loaded! Current level: " << current_level_ << "\n";
                
//                 const GameField* field = logic_.GetField();
//                 renderer_.Render(*field, logic_.GetPlayer(), current_level_);
                
//                 game_started_ = true;
//                 Run();
//                 break;
//             } else {
//                 std::cout << "Failed to load game! Please try again.\n";
//             }
//         }else if (input == "3") {
//             if (logic_.SaveToFile("savegame.txt")) {
//                 std::cout << "Game saved successfully!\n";
//             } else {
//                 std::cout << "Failed to save game!\n";
//             }
//             std::cout << "Thanks for playing! Goodbye!\n";
//             exit(0);
//         } else {
//             std::cout << "Invalid choice. Please enter 1 or 2.\n";
//         }
//     }
// }

bool GameLoop::AreAllEnemiesDefeated() const {
    GameField* field = logic_.GetField();
    if (!field) return false;

    for (size_t y = 0; y < field->GetHeight(); ++y) {
        for (size_t x = 0; x < field->GetWidth(); ++x) {
            FieldCell* cell = field->GetCell(x, y);

            if (cell && !cell->IsEmpty()) {
                Entity* entity = cell->GetEntity();

                if (entity && entity->IsAlive()) {
                    EntityType type = entity->GetType();
                    if (type == EntityType::kEnemy || type == EntityType::kBuilding || type == EntityType::kTower) {
                        return false;
                    }
                }
            }
        }
    }
    return true;
}

bool GameLoop::IsLevelComplete() const {
    return AreAllEnemiesDefeated();
}
