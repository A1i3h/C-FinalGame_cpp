#include "GameMenuState.h"
#include "../Logger.h"

GameMenuState::GameMenuState(Party& party, Console& console, StateMachine& stateMachine, Keyboard& keyboard)
    : m_Party(party),
    m_Console(console),
    m_StateMachine(stateMachine),
    m_Keyboard(keyboard),
    m_MenuSelector(console, keyboard, { L"Items", L"Magic", L"Equipment", L"Stats", L"Order", L"Exit" }),
    m_PlayerSelector(console, keyboard, {})
{
    m_bExitGame = false;
    m_bInMenuSelect = true;

    m_ScreenWidth = m_Console.GetScreenWidth();
    m_ScreenHeight = m_Console.GetScreenHeight();
    m_CenterScreenW = m_ScreenWidth / 2;
    m_PanelBarX = m_CenterScreenW - PANEL_BARS / 2;

    m_FirstChoice = -1;
    m_SecondChoice = -1;

    m_eSelectType = SelectType::NONE;

    // ? Removed recursive PushState (causes infinite loop)
}

GameMenuState::~GameMenuState()
{
}

void GameMenuState::OnEnter()
{
    m_Console.ClearBuffer();
    Logger::Log("Entered GameMenuState.");
}

void GameMenuState::OnExit()
{
    m_Console.ClearBuffer();
}

void GameMenuState::Update()
{
    // Optional logic can go here
}

void GameMenuState::Draw()
{
    DrawPanels();
    DrawPlayerInfo();
    m_MenuSelector.Draw();
    if (!m_bInMenuSelect) {
        m_PlayerSelector.Draw();
    }
    m_Console.Draw();
}

void GameMenuState::ProcessInputs()
{
    if (m_bInMenuSelect) {
        m_MenuSelector.ProcessInputs();
    }
    else {
        m_PlayerSelector.ProcessInputs();
    }

    if (m_Keyboard.IsKeyJustPressed(VK_ESCAPE)) {
        Logger::Log("Exiting GameMenuState...");
        m_StateMachine.PopState();
    }
}

bool GameMenuState::Exit()
{
    return m_bExitGame;
}

void GameMenuState::DrawPanels()
{
    m_Console.DrawPanel(m_PanelBarX, 1, PANEL_BARS, MENU_SIZE, WHITE);
}

void GameMenuState::DrawPlayerInfo()
{
    // Placeholder for drawing player information
}

void GameMenuState::OnMenuSelect(int index, std::vector<std::wstring> data)
{
    // Handle menu selection
}

void GameMenuState::OnPlayerSelect(int index, std::vector<std::shared_ptr<Player>> data)
{
    // Handle player selection
}

void GameMenuState::OnDrawPlayerSelect(int x, int y, std::shared_ptr<Player> player)
{
    // Draw player selection
}

void GameMenuState::SetOrderPlacement(int playerPosition)
{
    // Set order placement
}

void GameMenuState::UpdatePlayerOrder()
{
    // Update player order
}
