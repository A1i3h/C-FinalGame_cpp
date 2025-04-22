#pragma once
#include "IState.h"
#include "../Console.h"
#include "../inputs/Keyboard.h"
#include "StateMachine.h"

class IntroGameState : public IState
{
private:
    Console& m_Console;
    Keyboard& m_keyboard;
    StateMachine& m_Statemachine;

public:
    IntroGameState(Console& console, Keyboard& keyboard, StateMachine& stateMachine);
    ~IntroGameState();

    void OnEnter() override;
    void OnExit() override;
    void Update() override;
    void Draw() override;
    void ProcessInputs() override;
    bool Exit() override;
};
