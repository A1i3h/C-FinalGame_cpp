#include "IntroGameState.h"
#include "../Logger.h"

IntroGameState::IntroGameState(Console& console, Keyboard& keyboard, StateMachine& stateMachine)
    : m_Console(console), m_keyboard(keyboard), m_Statemachine(stateMachine)
{
}

IntroGameState::~IntroGameState()
{
}

void IntroGameState::OnEnter()
{
    m_Console.ClearBuffer();
    Logger::Log("Entered IntroGameState.");
}

void IntroGameState::OnExit()
{
    m_Console.ClearBuffer();
}

void IntroGameState::Update()
{
}

void IntroGameState::Draw()
{
    m_Console.Write(10, 10, L"Welcome to the Intro Page!", WHITE);
    m_Console.Draw();
}

void IntroGameState::ProcessInputs()
{
    if (m_keyboard.IsKeyJustPressed(VK_ESCAPE)) {
        Logger::Log("Exiting IntroGameState...");
        m_Statemachine.PopState();
    }
}

bool IntroGameState::Exit()
{
    return false;
}