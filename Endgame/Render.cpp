#include "Render.hpp"

void Renderer::Render(const GameField& field, const Player* player, int level) const {
    std::cout << "\n--- Game Field ---\n";
    RenderGameStats(player, level);
    
    for (size_t y = 0; y < field.GetHeight(); ++y) {
        for (size_t x = 0; x < field.GetWidth(); ++x) {
            const FieldCell* cell = field.GetCell(x, y);
            std::cout << GetCellSymbol(cell) << ' ';
        }
        std::cout << "\n";
    }
    std::cout << "------------------\n";
}

void Renderer::RenderGameStats(const Player* player, const int current_level) const {
    std::cout << "Score: " << (player ? player->GetScore() : 0) 
              << " | Lives: " << (player ? player->GetLives() : 0)
              << " | HP: " << (player ? player->GetHealth() : 0)
              << " | Mana: " << (player ? player->getMana() : 0)
              << " | Combat Mode: " << (player && player->GetCombatMode() == CombatMode::kMelee ? "Cursed dragger" : "Desert Eagle Mark XIX")
              << " | Stunned: " << (player && player->IsStunned() ? "Yes" : "No")
              << " | Status: " << (player && player->IsDead() ? "DEAD" : "ALIVE")
              << " | Level: " << current_level << "\n\n";
    RenderSpells(player);
}

void Renderer::RenderSpells(const Player* player) const {
    if (!player) return;
    
    const PlayerHand& hand = player->getHand();
    if (hand.getSpellCount() == 0) {
        std::cout << "Spells: None\n";
        return;
    }
    
    std::cout << "Spells: ";
    for (size_t i = 0; i < hand.getSpellCount(); ++i) {
        const Spell* spell = hand.getSpell(i);
        if (spell) {
            std::cout << (i + 1) << ": " << spell->getName() << "(" << spell->getManaCost() << " mana)";
            if (i < hand.getSpellCount() - 1) {
                std::cout << " ";
            }
        }
    }
    std::cout << "\n";
}

void Renderer::RenderControls() const {
    std::cout << "Controls: 'w', 'a', 's', 'd' to move. 'c' to switch combat mode. 'f' to attack. 'e' to cast spell. 'h' to help. 'q' to quit. 'S' to save. 'L' to load.\n";
    std::cout << "Legend: P = Player, E = Enemy, B = Enemy Building, T = Enemy Tower  # = Wall, ~ = Slowdown, * = Trap, . = Empty, D = Dead Player\n";
    std::cout << "Press Enter to skip a turn.\n";
}

void Renderer::RenderGameOver(int score, int level) const {
    std::cout << "\n--- GAME OVER ---\n";
    std::cout << "Final Score: " << score << "\n";
    std::cout << "Final Level: " << level << "\n";
    std::cout << "Thanks for playing!\n";
}

char Renderer::GetCellSymbol(const FieldCell* cell) const {
    if (!cell) return '?';
    
    if (!cell->IsEmpty()) {
        Entity* entity = cell->GetEntity();

        if(entity->GetType() == EntityType::kPlayer){
            Player* player = dynamic_cast<Player*>(entity);
            return (player && player->IsDead()) ? 'D' : 'P';
        }else{
            switch (entity->GetType())
            {
                case EntityType::kEnemy:
                    return 'E';
                case EntityType::kBuilding:
                    return 'B';
                case EntityType::kTower:
                    return 'T';
                default:
                    return '?';
                    break;
            }
        }
        

    }
    
    switch (cell->GetType()) {
        case FieldCellType::kNormal: return '.';
        case FieldCellType::kImpassable: return '#';
        case FieldCellType::kSlowdown: return '~';
        case FieldCellType::kTrap: return '*';
        default: return '?';
    }
}