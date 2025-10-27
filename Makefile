CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra
TARGET = game.exe

SOURCES = main.cpp GameLogic.cpp GameState.cpp InputHandler.cpp Render.cpp RewardSystem.cpp\
          GameField.cpp FieldCell.cpp Entity.cpp Player.cpp Enemy.cpp EnemyBuilding.cpp EnemyArchitecture.cpp EnemyTower.cpp\
          Spell.cpp DirectDamageSpell.cpp AreaDamageSpell.cpp TrapSpell.cpp PlayerHand.cpp

HEADERS = GameLogic.hpp GameState.hpp InputHandler.hpp Render.hpp GameField.hpp RewardSystem.hpp\
          FieldCell.hpp Entity.hpp Player.hpp Enemy.hpp EnemyBuilding.hpp EnemyArchitecture.hpp EnemyTower.hpp \
          EntityType.hpp FieldCellType.hpp CombatMode.hpp Direction.hpp Command.hpp \
          Spell.hpp DirectDamageSpell.hpp AreaDamageSpell.hpp TrapSpell.hpp PlayerHand.hpp  

$(TARGET): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)
