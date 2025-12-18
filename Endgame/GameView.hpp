#ifndef GAME_GAMEVIEW_HPP_
#define GAME_GAMEVIEW_HPP_

#include "GameLogic.hpp"
#include "Render.hpp"
#include <iostream>

template<typename RendererType>
class GameView {
public:
    GameView(GameLogic& logic) 
        : logic_(logic), renderer_() {}
    
    ~GameView() = default;
    
    void Render() {
        const GameField* field = logic_.GetField();
        const Player* player = logic_.GetPlayer();
        int level = logic_.GetGameState()->GetLevel();
        
        if (field && player) {
            renderer_.Render(*field, player, level);
        } else {
            std::cout << "Cannot render: game state not initialized.\n";
        }
    }
    
    void RenderStats() const {
        const Player* player = logic_.GetPlayer();
        int level = logic_.GetGameState()->GetLevel();
        
        if (player) {
            renderer_.RenderGameStats(player, level);
            renderer_.RenderSpells(player);
        }
    }
    
    void RenderGameOver() const {
        renderer_.RenderGameOver(logic_.GetScore(), logic_.GetGameState()->GetLevel());
    }
    
private:
    GameLogic& logic_;
    RendererType renderer_;
};

#endif