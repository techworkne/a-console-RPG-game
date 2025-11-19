#ifndef GAME_GAMESERIALIZE_HPP
#define GAME_GAMESERIALIZE_HPP

#include <iostream>

#include "Entity.hpp"
#include "Player.hpp"
#include "Enemy.hpp"
#include "EnemyBuilding.hpp"
#include "EnemyTower.hpp"
#include "GameField.hpp"
#include "GameState.hpp"
#include "Spell.hpp"

class GameSerialize{
    public:
        static void SerializeGameState(const GameState& state, std::ostream& stream);
        static void SerializePlayer(const Player& player, std::ostream& stream);
        static void SerializeEnemyEntity(const Entity& entity, std::ostream& stream);
        static void SerializeField(const GameField& field, std::ostream& stream);
        static void SerializeSpell(const Spell& spell, std::ostream& stream);


        static void DeserializeGameState( GameState& state, std::istream& stream);
        static std::unique_ptr<Spell> DeserializeSpell(std::istream& stream);
        static std::unique_ptr <Player> DeserializePlayer(std::istream& stream);
        static std::unique_ptr <GameField> DeserializeField(std::istream& stream);
        static std::unique_ptr <Entity> DeserializeEnemyEntity(std::istream& stream);
        static std::unique_ptr <Enemy> DeserializeEnemy(std::istream& stream);
        static std::unique_ptr <EnemyTower> DeserializeTower(std::istream& stream);
        static std::unique_ptr <EnemyBuilding> DeserializeBuilding(std::istream& stream);
};

#endif