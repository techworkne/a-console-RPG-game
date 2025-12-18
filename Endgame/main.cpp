#include "GameLogic.hpp"
#include "GameManager.hpp"
#include "InputHandler.hpp"
#include "InputConfig.hpp"
#include "Render.hpp"
#include <iostream>
#include <memory>

int main() {
    try {
        std::cout << "===== Console Game =====\n";
        
        auto config = std::make_shared<InputConfig>();
        if (!config->LoadFromFile("controls.ini")) {
            std::cout << "Using default controls.\n";
            config->LoadDefaults();
            config->SaveToFile("controls.ini");
        }
        
        auto input_handler = std::make_unique<InputHandler>(config);
        
        GameLogic game_logic(15, 15);
        
        GameManager<InputHandler, Renderer> game_manager(game_logic, std::move(input_handler));
        
        game_manager.Run();
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}