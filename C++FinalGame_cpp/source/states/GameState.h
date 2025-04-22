#pragma once
#include "IState.h"
#include "../Selector.h"
#include "../Player.h"
#include "../Inventory.h"
#include "../Party.h"
#include <memory>
#include "../utility/timer.h"
#include "../utility/TypeWriter.h"

class Console;
class Keyboard;
class StateMachine;

class GameState : public IState
{
private:
    Console& m_Console;
    Keyboard& m_keyboard;
    StateMachine& m_Statemachine;

    Selector<> m_Selector;
    std::unique_ptr<Player> m_TestPlayer;
    Inventory m_TestInventory;
    Party m_Party;
    Timer m_Timer;

public:
    GameState(Console& console, Keyboard& keyboard, StateMachine& stateMachine);
    ~GameState();

    void OnEnter() override;
    void OnExit() override;
    void Update() override;
    void Draw() override;
    void ProcessInputs() override;
    bool Exit() override;
};
