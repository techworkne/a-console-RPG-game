#include "GameSerialize.hpp"
#include "AreaDamageSpell.hpp"
#include "TrapSpell.hpp"

void GameSerialize::SerializeGameState(const GameState& state, std::ostream& stream) {
    stream << "GAME_SAVE\n";
    stream << state.GetLevel() << " " ;
    
    const Player* player = state.GetPlayer();
    if (player) {
        SerializePlayer(*player, stream);
    }
    
    const GameField* field = state.GetField();
    if (field) {
        SerializeField(*field, stream);
    }
}

void GameSerialize::SerializeSpell(const Spell& spell, std::ostream& stream) {
    stream << static_cast<int>(spell.getType()) << " ";
    
    stream << spell.getManaCost() << " ";
    
    stream << spell.getDamage() << " ";
}

void GameSerialize::SerializePlayer(const Player &player, std::ostream &stream){
    stream << player.GetX() << " " << player.GetY() << " ";
    stream << player.GetLives() << " " << player.GetHealth() << " " << player.GetScore() << " ";
    stream << player.getMana() << " ";
    stream << static_cast<int>(player.GetCombatMode()) << " ";
    stream << (player.IsStunned() ? 1 : 0) << " ";

    stream << player.getHand().getSpellCount() << " ";
    for (size_t i = 0; i < player.getHand().getSpellCount(); ++i) {
        const Spell* spell = player.getHand().getSpell(i);
        SerializeSpell(*spell, stream);
    }
    
    stream << "\n";
}

void GameSerialize::SerializeEnemyEntity(const Entity& entity, std::ostream& stream){
    stream << static_cast<int>(entity.GetType()) << " ";
    stream << entity.GetX() << " " << entity.GetY() << " ";
    stream << entity.GetHealth() << " ";

    switch (entity.GetType()){
        case EntityType::kEnemy:
            stream << dynamic_cast<const Enemy&>(entity).GetDamage() << " ";
            break;
        case EntityType::kBuilding:
            stream << dynamic_cast<const EnemyBuilding&>(entity).GetSpawnInterval() << " ";
            break;
        case EntityType::kTower: {
            const EnemyTower& tower = dynamic_cast<const EnemyTower&>(entity);
            stream << tower.GetAttackRadius() << " " << tower.GetAttackCooldown() << " ";
            break;
        }
        case EntityType::kPlayer:
            break;
        case EntityType::kEmpty:
            break;
        default:
            break;
        }
    stream << "\n";
}

void GameSerialize::SerializeField(const GameField& field, std::ostream& stream){
    stream << field.GetWidth() << " " << field.GetHeight() << " ";
    
    for (size_t y = 0; y < field.GetHeight(); ++y) {
        for (size_t x = 0; x < field.GetWidth(); ++x) {
            const FieldCell* cell = field.GetCell(x, y);
            stream << static_cast<int>(cell->GetType()) << " ";
        }
    }

    std::vector<const Entity*> entities;
    for (size_t y = 0; y < field.GetHeight(); ++y) {
        for (size_t x = 0; x < field.GetWidth(); ++x) {
            const FieldCell* cell = field.GetCell(x, y);
            if (cell && !cell->IsEmpty()) {
                const Entity* entity = cell->GetEntity();
                if (entity && entity->GetType() != EntityType::kPlayer) {
                    entities.push_back(entity);
                }
            }
        }
    }

    for (const Entity* entity : entities) {
        SerializeEnemyEntity(*entity, stream);
    }
}

//===============================================================================================================================================

void GameSerialize::DeserializeGameState(GameState& state, std::istream& stream){
    std::string header;
    stream >> header;
    
    if (header != "GAME_SAVE") {
        throw std::runtime_error("Invalid save file format");
    }
    
    int level;
    stream >> level;
    
    state.SetLevel(level);

    auto player = DeserializePlayer(stream);
    auto field = DeserializeField(stream);
    
    
    state.ResetField(field->GetWidth(), field->GetHeight());
    GameField* current_field = state.GetField();

    for (size_t y = 0; y < field->GetHeight(); ++y) {
        for (size_t x = 0; x < field->GetWidth(); ++x) {
            const FieldCell* src_cell = field->GetCell(x, y);
            FieldCell* dest_cell = current_field->GetCell(x, y);
            if (src_cell && dest_cell) {
                dest_cell->SetType(src_cell->GetType());
            }
        }
    }

    if (player) {
        size_t player_x = player->GetX();
        size_t player_y = player->GetY();
        
        if (player_x < current_field->GetWidth() && player_y < current_field->GetHeight()) {
            current_field->PlaceEntity(player_x, player_y, std::move(player));
            
            FieldCell* player_cell = current_field->GetCell(player_x, player_y);
            if (player_cell && player_cell->GetEntity()) {
                Player* player_ptr = dynamic_cast<Player*>(player_cell->GetEntity());
                state.SetPlayer(player_ptr);
            }
        }
    }

    while (stream.peek() != EOF && stream.peek() != '\n') {
        auto entity = DeserializeEnemyEntity(stream);
        if (entity && entity->IsAlive()) {
            size_t entity_x = entity->GetX();
            size_t entity_y = entity->GetY();
            
            if (entity_x < current_field->GetWidth() && entity_y < current_field->GetHeight()) {
                current_field->PlaceEntity(entity_x, entity_y, std::move(entity));
            }
        }
    }
}

std::unique_ptr<Spell> GameSerialize::DeserializeSpell(std::istream& stream) {
    size_t type_num, mana_cost, damage;
    stream >> type_num >> mana_cost >> damage;

    SpellType type = static_cast<SpellType>(type_num);

    switch (type) {
        case SpellType::DirectDamage:
            return std::make_unique<DirectDamageSpell>(damage, mana_cost);
        case SpellType::AreaDamage:
            return std::make_unique<AreaDamageSpell>(damage, mana_cost);
        case SpellType::TrapSpell:
            return std::make_unique<TrapSpell>(damage, mana_cost);
        default:
            std::cerr << "Error: deserialize spells";
            return std::make_unique<DirectDamageSpell>(0, 0);
    }
}

std::unique_ptr<Player> GameSerialize::DeserializePlayer(std::istream& stream) {
    size_t x, y;
    int lives, health, score, mana;
    int combat_mode, stunned;
    
    stream >> x >> y >> lives >> health >> score >> mana >> combat_mode >> stunned;
    
    auto player = std::make_unique<Player>(health, x, y);
    player->SetLives(lives);
    player->SetScore(score);
    player->setMana(mana);
    player->SetCombatMode(static_cast<CombatMode>(combat_mode));
    if (stunned) player->Stun();
    
    
    size_t spell_count;
    stream >> spell_count;
    for (size_t i = 0; i < spell_count; ++i) {
        auto spell = DeserializeSpell(stream);
        player->addSpellToHand(std::move(spell));
    }
    
    return player;
}

std::unique_ptr<Entity> GameSerialize::DeserializeEnemyEntity(std::istream& stream) {
    int type_val;
    stream >> type_val;
    EntityType type = static_cast<EntityType>(type_val);
    
    switch (type) {
        case EntityType::kEnemy:
            return DeserializeEnemy(stream);
        case EntityType::kBuilding:
            return DeserializeBuilding(stream);
        case EntityType::kTower:
            return DeserializeTower(stream);
        default:
            return nullptr;
    }
}

std::unique_ptr<Enemy> GameSerialize::DeserializeEnemy(std::istream& stream){
    size_t x, y, health, damage;
    stream >> x >> y >> health >> damage;
    auto enemy = std::make_unique<Enemy>(health, damage, x, y);
    return enemy;
}

std::unique_ptr<EnemyBuilding> GameSerialize::DeserializeBuilding(std::istream& stream){
    size_t x, y, health, spawn_interval;
    stream >> x >> y >> health >> spawn_interval;
    auto building = std::make_unique<EnemyBuilding>(health, x, y, spawn_interval);
    return building;
}

std::unique_ptr<EnemyTower> GameSerialize::DeserializeTower(std::istream& stream){
    size_t x, y, health, attack_radius, attack_cooldown;
    stream >> x >> y >> health >> attack_radius >> attack_cooldown; 
    auto tower = std::make_unique<EnemyTower>(health, x , y, attack_radius);
    tower->SetAttackCooldown(attack_cooldown);
    return tower;
}

std::unique_ptr<GameField> GameSerialize::DeserializeField(std::istream& stream) {
    size_t width, height;
    stream >> width >> height;
    
    auto field = std::make_unique<GameField>(width, height);
    
    
    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            int cell_type;
            stream >> cell_type;
            auto type = static_cast<FieldCellType>(cell_type);
            field->GetCell(x, y)->SetType(type);
        }
    }
    
    return field;
}