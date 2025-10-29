#ifndef SPELL_HPP
#define SPELL_HPP

#include "EntityType.hpp"
#include <memory>
#include <string>

class GameField;
class Entity;

class Spell {
    public:
        virtual ~Spell() = default;
        
        virtual bool cast(GameField& field, Entity* caster, size_t target_x, size_t target_y) = 0;
        
        
        // получение инфы о заклинание
        virtual int getManaCost() const = 0;
        virtual std::string getName() const = 0;
        
        protected:
        int mana_cost_;
        

};

#endif