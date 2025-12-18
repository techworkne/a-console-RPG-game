#ifndef GAME_INPUTHANDLER_HPP_
#define GAME_INPUTHANDLER_HPP_

#include "InputProvider.hpp"
#include "InputConfig.hpp"
#include <memory>

class InputHandler : public InputProvider {
    public:
        InputHandler();
        InputHandler(std::shared_ptr<InputConfig> config);
        ~InputHandler() override = default;
        
        Command GetCommand() const override;
        Direction GetAttackDirection() const override;
        void UpdateKeyBindings(const std::unordered_map<Command, char>& newBindings) override;
        const std::unordered_map<Command, char>& GetKeyBindings() const override;
        
        void SetConfig(std::shared_ptr<InputConfig> config);
        std::shared_ptr<InputConfig> GetConfig() const { return config_; }
        
    private:
        std::shared_ptr<InputConfig> config_;
        
        char GetSingleCharInput() const;
};

#endif