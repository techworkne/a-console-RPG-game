#ifndef GAME_GAMEMANAGER_HPP_
#define GAME_GAMEMANAGER_HPP_

#include "GameLogic.hpp"
#include "InputProvider.hpp"
#include "GameView.hpp"
#include "CommandUtils.hpp"
#include "MovementEntity.hpp"
#include "Enemy.hpp"
#include "EnemyBuilding.hpp"
#include "EnemyTower.hpp"
#include <memory>
#include <iostream>
#include <string>
#include <limits>
#include <random>
#include <vector>

template<typename InputProviderType, typename RendererType>
class GameManager {
public:
    GameManager(GameLogic& logic, std::unique_ptr<InputProviderType> input_provider)
        : logic_(logic), 
          input_provider_(std::move(input_provider)),
          game_view_(std::make_unique<GameView<RendererType>>(logic_)) {
        
        current_level_ = logic_.GetGameState()->GetLevel();
        game_started_ = false;
    }
    
    ~GameManager() = default;
    
    void Run() {
        std::cout << "======================================================================\n";
        std::cout << "Welcome to the Console Game!\n";
        
        ShowControls();
        
        while (true) {
            if (!game_started_) {
                HandleMainMenu();
                if (!game_started_) continue;
            }
            
            if (logic_.IsGameOver()) {
                HandleGameOver();
                game_started_ = false;
                continue;
            }
            
            if (IsLevelComplete()) {
                HandleLevelComplete();
                continue;
            }
            
            if (!ProcessGameTick()) {
                break;
            }
        }
    }
    
private:
    void ShowControls() const {
        std::cout << "\n=== Controls ===\n";
        const auto& bindings = input_provider_->GetKeyBindings();
        
        for (const auto& [command, key] : bindings) {
            if (command != Command::kInvalid) {
                std::cout << key << " - " << CommandToString(command) << "\n";
            }
        }
        std::cout << "Enter - Skip turn\n";
        std::cout << "================\n\n";
    }
    
    bool ProcessGameTick() {
        game_view_->Render();
        
        Player* player = logic_.GetPlayer();
        
        if (player && player->GetHealth() <= 0 && !player->IsDead() && player->GetLives() > 0) {
            std::cout << "Player health reached 0! Handling death...\n";
            logic_.HandlePlayerDeath();
            return true;
        }
        
        if (player && player->IsDead()) {
            std::cout << "Player is dead! Press Enter to continue...";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            logic_.HandlePlayerDeath();
            return true;
        }
        if (player && player->IsAlive() && player->IsStunned()) {
            std::cout << "\nPlayer is stunned! Skipping turn...\n";
            std::cout << "Press Enter to continue...";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            
            UpdateEnemies();
            UpdateBuildings();
            UpdateTowers();
            
            player->UnStun();
            return true;
        }
        
        Command command = input_provider_->GetCommand();
        
        bool turnProcessed = false;
        
        if (player && player->IsDead()) {
            std::cout << "Player is dead! Cannot perform actions.\n";
            return true;
        }
        
        switch (command) {
            case Command::kMoveUp:
                logic_.HandlePlayerInput(Direction::kUp);
                turnProcessed = true;
                break;
            case Command::kMoveDown:
                logic_.HandlePlayerInput(Direction::kDown);
                turnProcessed = true;
                break;
            case Command::kMoveLeft:
                logic_.HandlePlayerInput(Direction::kLeft);
                turnProcessed = true;
                break;
            case Command::kMoveRight:
                logic_.HandlePlayerInput(Direction::kRight);
                turnProcessed = true;
                break;
            case Command::kSwitchMode:
                if (player && !player->IsStunned()) {
                    player->SwitchCombatMode();
                    std::cout << "Combat mode: " << (player->GetCombatMode() == CombatMode::kMelee ? 
                                "Cursed dagger" : "Desert Eagle Mark XIX") << "\n";
                    turnProcessed = true;
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
                ShowControls();
                break;
            case Command::kQuit:
                return HandleQuit();
            case Command::kSave:
                HandleSaveGame();
                break;
            case Command::kLoad:
                HandleLoadGame();
                break;
            case Command::kInvalid:
                std::cout << "Invalid command.\n";
                break;
        }
        
        if (turnProcessed && (!player || !player->IsDead())) {
            UpdateEnemies();
            UpdateBuildings();
            UpdateTowers();
        }
        
        return true;
    }
    
    void UpdateEnemies() {
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
    
    void UpdateTowers() {
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
    
    void UpdateBuildings() {
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
                                    auto new_enemy = std::make_unique<Enemy>(
                                        50 + current_level_, 
                                        15 + current_level_ * 2, 
                                        adj_x, adj_y
                                    );
                                    field->PlaceEntity(adj_x, adj_y, std::move(new_enemy));
                                    std::cout << "Building spawned enemy at (" 
                                              << adj_x << ", " << adj_y << ")\n";
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    bool HandleQuit() {
        std::cout << "\nSave game before quitting? (y/n): ";
        std::string response;
        std::getline(std::cin, response);
        
        if (response == "y" || response == "Y") {
            if (logic_.SaveToFile("autosave.txt")) {
                std::cout << "Game saved to autosave.txt\n";
            }
        }
        
        std::cout << "Goodbye!\n";
        return false;
    }
    
    void HandleSaveGame() {
        std::cout << "Enter save file name (or press Enter for 'savegame.txt'): ";
        std::string filename;
        std::getline(std::cin, filename);
        
        if (filename.empty()) filename = "savegame.txt";
        
        if (logic_.SaveToFile(filename)) {
            std::cout << "Game saved to " << filename << "\n";
        } else {
            std::cout << "Failed to save game!\n";
        }
    }
    
    void HandleLoadGame() {
        std::cout << "Enter save file name (or press Enter for 'savegame.txt'): ";
        std::string filename;
        std::getline(std::cin, filename);
        
        if (filename.empty()) filename = "savegame.txt";
        
        if (logic_.LoadFromFile(filename)) {
            current_level_ = logic_.GetGameState()->GetLevel();
            std::cout << "Game loaded! Current level: " << current_level_ << "\n";
            game_view_ = std::make_unique<GameView<RendererType>>(logic_);
        } else {
            std::cout << "Failed to load game!\n";
        }
    }
    
    void HandleMainMenu() {
        while (!game_started_) {
            std::cout << "\n=== MAIN MENU ===\n";
            std::cout << "1. Start New Game\n";
            std::cout << "2. Load Game\n";
            std::cout << "3. Configure Controls\n";
            std::cout << "4. Exit\n";
            std::cout << "Choose: ";
            
            std::string input;
            std::getline(std::cin, input);
            
            if (input == "1") {
                StartNewGame();
            } else if (input == "2") {
                game_started_ = true;
                std::cout << "Enter save file name: ";
                std::string filename;
                std::getline(std::cin, filename);
                
                if (filename.empty()) filename = "savegame.txt";
                
                if (logic_.LoadFromFile(filename)) {
                    current_level_ = logic_.GetGameState()->GetLevel();
                    std::cout << "Game loaded! Level: " << current_level_ << "\n";
                } else {
                    std::cout << "Failed to load game!\n";
                    game_started_ = false;
                }
            } else if (input == "3") {
                ConfigureControls();
            } else if (input == "4") {
                if (logic_.SaveToFile("autosave.txt")) {
                    std::cout << "Game saved.\n";
                }
                std::cout << "Goodbye!\n";
                exit(0);
            } else {
                std::cout << "Invalid choice.\n";
            }
        }
    }
    
    void StartNewGame() {
        current_level_ = 1;
        logic_.GetGameState()->ResetLevel();
        InitializeNewLevel();
        game_started_ = true;
        std::cout << "New game started! Level " << current_level_ << "\n";
    }
    
    void InitializeNewLevel() {
        std::cout << "=== LEVEL " << current_level_ << " ===\n";
        size_t level_size = (current_level_ + 15 > 25) ? 25 : current_level_ + 15;
        
        logic_.InitializeNewLevel(level_size, level_size, current_level_);
        std::cout << "New level initialized! Map: " << level_size << "x" << level_size << "\n";
    }
    
    void HandleLevelComplete() {
        std::cout << "LEVEL COMPLETE! All enemies defeated!\n";
        std::cout << "Final score: " << logic_.GetScore() << "\n";
        
        UpgradeAfterLevel();
    }
    
    void UpgradeAfterLevel() {
        int player_score = logic_.GetScore();
        std::cout << "\nCongratulations! Choose upgrade:\n";
        std::cout << "Your score: " << player_score << ".\n";
        std::cout << "0. Skip. Dont get better.\n";
        std::cout << "1. Small mana potion. +50 mana. Cost: 100 points.\n";
        std::cout << "2. Mana potion. +100 mana. Cost: 200 points.\n";
        std::cout << "3. Strengthening the cursed energy. +2 cursed dagger strike power. Cost: 300 points.\n";
        std::cout << "4. The introduction of an improved curse to increase kinetic energy. +2 to the power of the Desert Eagle Mark XIX shot. Cost: 450 points.\n";

        std::string input;
        std::getline(std::cin, input);

        Player* player = logic_.GetPlayer();
        if (!player) return;

        if (input == "1") {
            player->addMana(50);
        } else if (input == "2") {
            player->addMana(100);
        } else if (input == "3") {
            player->AddMeleeDamage(2);
        } else if (input == "4") {
            player->AddRangeDamage(2);
        }
        
        LoadNextLevel();
    }
    
    void LoadNextLevel() {
        Player* player = logic_.GetPlayer();
        if (!player) return;
        
        PlayerHand& hand = player->getHand();
        if (hand.getSpellCount() > 1) {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<size_t> dist(0, hand.getSpellCount() - 1);
            hand.removeSpell(dist(gen));
        }
        
        current_level_++;
        logic_.GetGameState()->AddLevel();
        InitializeNewLevel();
        
        player->SetScore(player->GetScore() + 100);
        player->setMana(player->getMana() + 50);
        player->ResetHealth(100);
        player->AddLives();
        
        std::cout << "Starting next level! Bonus: +100 points, +50 mana, +1 live, restore health '-(+-+)-'\n";
    }
    
    bool AreAllEnemiesDefeated() const {
        GameField* field = logic_.GetField();
        if (!field) return false;

        for (size_t y = 0; y < field->GetHeight(); ++y) {
            for (size_t x = 0; x < field->GetWidth(); ++x) {
                FieldCell* cell = field->GetCell(x, y);
                if (cell && !cell->IsEmpty()) {
                    Entity* entity = cell->GetEntity();
                    if (entity && entity->IsAlive()) {
                        EntityType type = entity->GetType();
                        if (type == EntityType::kEnemy || 
                            type == EntityType::kBuilding || 
                            type == EntityType::kTower) {
                            return false;
                        }
                    }
                }
            }
        }
        return true;
    }
    
    bool IsLevelComplete() const {
        return AreAllEnemiesDefeated();
    }
    
    void HandleGameOver() {
        std::cout << "\n=== GAME OVER ===\n";
        std::cout << "Final Score: " << logic_.GetScore() << "\n";
        std::cout << "Final Level: " << current_level_ << "\n";
        
        std::cout << "\n1. Restart\n";
        std::cout << "2. Return to main menu\n";
        std::cout << "Choose: ";
        
        std::string choice;
        std::getline(std::cin, choice);
        
        if (choice == "1") {
            StartNewGame();
        } else {
            game_started_ = false;
        }
    }
    
    void ConfigureControls() {
        std::cout << "\n=== Configure Controls ===\n";
        std::cout << "1. Load from file\n";
        std::cout << "2. Reset to defaults\n";
        std::cout << "3. Back to menu\n";
        std::cout << "Choose: ";
        
        std::string choice;
        std::getline(std::cin, choice);
        
        if (choice == "1") {
            std::cout << "Enter config file name: ";
            std::string filename;
            std::getline(std::cin, filename);
            
            if (filename.empty()) filename = "controls.ini";
            
            auto config = std::make_shared<InputConfig>();
            if (config->LoadFromFile(filename)) {
                InputHandler* handler = dynamic_cast<InputHandler*>(input_provider_.get());
                if (handler) {
                    handler->SetConfig(config);
                    std::cout << "Controls configured from " << filename << "\n";
                }
            }
        } else if (choice == "2") {
            auto config = std::make_shared<InputConfig>();
            config->LoadDefaults();
            
            InputHandler* handler = dynamic_cast<InputHandler*>(input_provider_.get());
            if (handler) {
                handler->SetConfig(config);
                std::cout << "Controls reset to defaults.\n";
            }
        }
    }
    
    GameLogic& logic_;
    std::unique_ptr<InputProviderType> input_provider_;
    std::unique_ptr<GameView<RendererType>> game_view_;
    
    int current_level_;
    bool game_started_;
};

#endif