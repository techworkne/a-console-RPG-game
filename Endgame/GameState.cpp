#include "GameState.hpp"
#include <stdexcept>

GameState::GameState(size_t width, size_t height) : game_field_(std::make_unique<GameField>(width, height)), player_(nullptr) {}

bool GameState::IsGameOver() const {
    return !player_ || (player_->GetLives() <= 0 && player_->IsDead());
}

void GameState::ResetField(size_t width, size_t height){
    game_field_ = std::make_unique<GameField>(width, height);
    player_ = nullptr;
}

int GameState::GetScore() const {
    return player_ ? player_->GetScore() : 0;
}