#ifndef GAME_GAMELOOP_HPP_
#define GAME_GAMELOOP_HPP_
#include <iostream>
#include <random>

class GameLogic;
class InputHandler;
class Renderer;
class GameLoop {
    public:
        GameLoop(GameLogic& logic, InputHandler& input, Renderer& renderer);
        void Run();
        void ProcessTurn();
        
        void AddLevel(){ current_level_++;}
        void ResetLevel(){ current_level_ = 1;}
        int GetLevel() const {return current_level_;}
    private:
        void UpdateEnemies();
        void UpdateTowers();
        void UpdateBuildings();

        void HandleMainMenu();
        void HandleLevelComplete();
        void HandleGameEnd();
        void LoadNextLevel();
        void UpgradeAfterLevel();
        void InitializeNewLevel();

        bool IsLevelComplete() const;
        bool AreAllEnemiesDefeated() const;
        GameLogic& logic_;
        InputHandler& input_handler_;
        Renderer& renderer_;
        int current_level_ = 1;
        bool game_started_ = false;
    };
#endif