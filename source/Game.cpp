#include "Game.h"
#include <iostream>
#include "Logger.h"
#include "states/GameState.h"
#include "utility/Globals.h"

bool Game::Init()
{
    try
    {
        m_pConsole = std::make_unique<Console>();
    }
    catch (const std::exception& e)
    {
        TRPG_ERROR(std::string("Exception in Init(): ") + e.what());
        return false;
    }

    m_hConsoleIn = GetStdHandle(STD_INPUT_HANDLE);
    m_pKeyboard = std::make_unique<Keyboard>();
    m_pStateMachine = std::make_unique<StateMachine>(); // Fixed variable name

    m_pStateMachine->PushState(std::make_unique<GameState>(*m_pConsole, *m_pKeyboard, *m_pStateMachine));

    return true;
}

void Game::ProcessEvents()
{
    if (!GetNumberOfConsoleInputEvents(m_hConsoleIn, &m_NumRead))
    {
        DWORD error = GetLastError();
        TRPG_ERROR("Unable to get console events: " + std::to_string(error));
        return;
    }
    if (m_NumRead <= 0)
        return;

    if (!PeekConsoleInput(m_hConsoleIn, m_InRecBuf, 128, &m_NumRead))
    {
        DWORD error = GetLastError();
        TRPG_ERROR("Failed to peek console input: " + std::to_string(error));
        return;
    }

    for (DWORD i = 0; i < m_NumRead; i++)
    {
        switch (m_InRecBuf[i].EventType)
        {
        case KEY_EVENT:
            KeyEventProcess(m_InRecBuf[i].Event.KeyEvent);
            break;
        default:
            break;
        }
    }
    FlushConsoleInputBuffer(m_hConsoleIn);
}

void Game::ProcessInputs()
{
    if (m_pKeyboard->IsKeyJustPressed(KEY_ESCAPE))
        m_bIsRunning = false;

    if (m_pStateMachine->Empty())
    {
        TRPG_ERROR("NO STATES TO PROCESS!");
        m_bIsRunning = false;
        return;
    }

    m_pStateMachine->GetCurrentState()->ProcessInputs();
}

void Game::Update()
{
    if (m_pStateMachine->Empty())
    {
        TRPG_ERROR("NO STATES TO UPDATE!");
        m_bIsRunning = false;
        return;
    }

    m_pStateMachine->GetCurrentState()->Update();
    m_pKeyboard->Update();

    TRPG_Globals::GetInstance().Update();
}

void Game::Draw()
{
    if (m_pStateMachine->Empty())
    {
        TRPG_ERROR("NO STATES TO DRAW!");
        m_bIsRunning = false;
        return;
    }

    m_pStateMachine->GetCurrentState()->Draw();
    m_pConsole->Draw();
}

void Game::KeyEventProcess(KEY_EVENT_RECORD keyEvent)
{
    if (keyEvent.bKeyDown)
        m_pKeyboard->OnKeyDown(keyEvent.wVirtualKeyCode);
    else
        m_pKeyboard->OnKeyUp(keyEvent.wVirtualKeyCode);
}

Game::Game()
    : m_bIsRunning{ true }
    , m_pConsole{ nullptr }
    , m_pKeyboard{ nullptr }
    , m_pStateMachine{ nullptr } // Fixed variable name
{
}

Game::~Game()
{
    m_pConsole = nullptr;
    m_pKeyboard = nullptr;
    m_pStateMachine = nullptr; // Fixed variable name
}

void Game::Run()
{
    if (!Init())
        m_bIsRunning = false;

    while (m_bIsRunning)
    {
        ProcessEvents();
        ProcessInputs();
        Update();
        Draw();
    }

    std::cout << "Bye Bye!\n";
}
