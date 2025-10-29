#ifndef GAME_RENDERER_HPP_
#define GAME_RENDERER_HPP_

#include "GameField.hpp"
#include "Player.hpp"
#include <iostream>

class Renderer {
public:
    Renderer() = default;
    ~Renderer() = default;
    
    void Render(const GameField& field, const Player* player) const;
    void RenderGameStats(const Player* player) const;
    void RenderSpells(const Player* player) const;
    void RenderControls() const;
    void RenderGameOver(int score) const;

private:
    char GetCellSymbol(const FieldCell* cell) const;
};

#endif