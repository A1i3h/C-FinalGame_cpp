#include "GameMenuState.h"
#include "ItemMenuState.h"
#include "EquipmentMenuState.h"
#include "StatusMenuState.h"

#include "../Party.h"
#include "../Player.h"
#include "../Console.h"
#include "../Inputs/Keyboard.h"
#include "StateMachine.h"
#include "IState.h"
#include "../utility/Globals.h"

using namespace std::placeholders;

const int PANEL_BARS = 50;

void GameMenuState::DrawPanels()
{
    // Draw Opening Bar
    m_Console.DrawPanelHorz(m_PanelBarX - 1, 1, PANEL_BARS + 1, BLUE);
    int menu_x_pos = m_CenterScreenW - (MENU_SIZE / 2);
    int y_pos = 2; // Start drawing from Y=2 to leave space for other UI elements
    m_Console.Write(menu_x_pos, y_pos++, L" _  _  ____  __ _  _  _ ", GREEN);  // M
    m_Console.Write(menu_x_pos, y_pos++, L"( \\/ )(  __)(  ( \\/ )( \\", GREEN);  // E
    m_Console.Write(menu_x_pos, y_pos++, L"/ \\/ \\ ) _) /    /) \\/ (", GREEN);  // N
    m_Console.Write(menu_x_pos, y_pos++, L"\\_)(_/(____)\\_)__)\\____/", GREEN);  // U
    m_Console.DrawPanelHorz(18, 7, PANEL_BARS, BLUE);

    // Move this line further down
    m_Console.DrawPanelHorz(m_PanelBarX - 1, 40, SMALL_PANEL_BAR, BLUE);

    m_Console.DrawPanelHorz(m_PanelBarX - 1, (m_ScreenHeight - 2), PANEL_BARS + 1, BLUE);

    m_Console.DrawPanelVert(m_PanelBarX - 1, 2, 44, BLUE);
    m_Console.DrawPanelVert(m_PanelBarX + PANEL_BARS, 2, 44, BLUE);
    m_Console.DrawPanelVert(48, 2, 44, BLUE);

    // Adjust timer position to be below EXIT
    const auto& time_str = L"TIME: " + TRPG_Globals::GetInstance().GetTime();
    // Assuming "EXIT" is drawn at a Y position of 38, let's draw the timer at Y=42 or lower
    m_Console.Write(26, 42, time_str);
}

void GameMenuState::DrawPlayerInfo()
{
    int i = 1;
    for (const auto& player : m_Party.GetParty())
    {
        if (!player->IsActive())
            continue;

        const auto& name = player->GetName();
        const auto& hp = std::to_wstring(player->GetHP());
        const auto& hp_max = std::to_wstring(player->GetMaxHP());
        const auto& level = std::to_wstring(player->GetLevel());
        const auto& xp = std::to_wstring(player->GetXP());
        const auto& xp_to_next = std::to_wstring(player->GetXPToNextLevel());

        std::wstring hp_string = L"HP: " + hp + L" / " + hp_max;
        std::wstring level_string = L"Lvl: " + level + L" Exp: " + xp + L" / " + xp_to_next;

        m_Console.Write(75, 12 + i, name, PURPLE);
        m_Console.Write(75, 13 + i, hp_string, PURPLE);
        m_Console.Write(75, 14 + i, level_string, PURPLE);
        i += 10;
    }

    const auto& gold = std::to_wstring(m_Party.GetGold());

    std::wstring gold_str = L"GOLD: " + gold;
    m_Console.Write(26, 43, gold_str);
}

void GameMenuState::OnMenuSelect(int index, std::vector<std::wstring> data)
{
    switch (index)
    {
    case 0:
        m_eSelectType = SelectType::ITEM;
        break;
    case 1:
        m_eSelectType = SelectType::MAGIC;
        break;
    case 2:
        m_eSelectType = SelectType::EQUIPMENT;
        break;
    case 3:
        m_eSelectType = SelectType::STATS;
        break;
    case 4:
        m_eSelectType = SelectType::ORDER;
        break;
    case 5:
        break;
    case 6:
        m_bExitGame = true;
        return;
    default:
        return;
    }
    if (m_eSelectType != SelectType::NONE)
    {
        m_MenuSelector.HideCursor();
        m_PlayerSelector.ShowCursor();
        m_bInMenuSelect = false;
    }
}

void GameMenuState::OnPlayerSelect(int index, std::vector<std::shared_ptr<Player>> data)
{
    const auto& player = data[index];
    switch (m_eSelectType)
    {
    case SelectType::ITEM:
        m_StateMachine.PushState(std::make_unique<ItemState>(*player, m_Console, m_StateMachine, m_Keyboard));
        break;
    case SelectType::MAGIC:
        break;
    case SelectType::EQUIPMENT:
        m_StateMachine.PushState(std::make_unique<EquipmentMenuState>(*player, m_Console, m_StateMachine, m_Keyboard));
        break;
    case SelectType::STATS:
        m_StateMachine.PushState(std::make_unique<StatusMenuState>(*player, m_Console, m_StateMachine, m_Keyboard));
        break;
    case SelectType::ORDER:
        SetOrderPlacement(player->GetPartyPosition());
        break;
    default:
        break;
    }
}

void GameMenuState::OnDrawPlayerSelect(int x, int y, std::shared_ptr<Player> player)
{
    //not required
}

void GameMenuState::SetOrderPlacement(int playerPosition)
{
    if (m_FirstChoice < 0)
        m_FirstChoice = playerPosition;
    else if (m_SecondChoice < 0)
        m_SecondChoice = playerPosition;
}

void GameMenuState::UpdatePlayerOrder()
{
    if (m_FirstChoice < 0 || m_SecondChoice < 0)
        return;

    for (auto& member : m_Party.GetParty())
    {
        if (member->GetPartyPosition() == m_FirstChoice)
        {
            member->SetPartyPosition(m_SecondChoice);
        }
        else if (member->GetPartyPosition() == m_SecondChoice)
        {
            member->SetPartyPosition(m_FirstChoice);
        }
    }

    std::sort(m_Party.GetParty().begin(), m_Party.GetParty().end(),
        [&](std::shared_ptr<Player>& rh, std::shared_ptr<Player>& lh)
        {
            return rh->GetPartyPosition() < lh->GetPartyPosition();
        });

    std::sort(m_PlayerSelector.GetData().begin(), m_PlayerSelector.GetData().end(),
        [&](std::shared_ptr<Player>& rh, std::shared_ptr<Player>& lh)
        {
            return rh->GetPartyPosition() < lh->GetPartyPosition();
        });

    m_FirstChoice = m_SecondChoice = -1;
    m_bInMenuSelect = true;
    m_eSelectType = SelectType::NONE;
    m_MenuSelector.ShowCursor();
    m_PlayerSelector.HideCursor();
    m_Console.ClearBuffer();
}

GameMenuState::GameMenuState(Party& party, Console& console, StateMachine& stateMachine, Keyboard& keyboard)
    : m_Party(party)
    , m_Console(console)
    , m_StateMachine(stateMachine)
    , m_Keyboard(keyboard)
    , m_MenuSelector{
        console,
        keyboard,
        {L"Items", L"Magic", L"Equipment", L"Stats", L"Order", L"Save", L"Exit"},
        SelectorParams{30, 8, 1, 0, 0} }
        , m_PlayerSelector{
            console,
            keyboard,
            std::bind(&GameMenuState::OnPlayerSelect, this, _1, _2),
            std::bind(&GameMenuState::OnDrawPlayerSelect, this, _1, _2, _3),
            party.GetParty(),
            SelectorParams{69, 13, 1, 0, 10} }
            , m_bExitGame{ false }
    , m_bInMenuSelect{ true }
    , m_ScreenWidth{ console.GetScreenWidth() }
    , m_ScreenHeight{ console.GetScreenHeight() }
    , m_CenterScreenW{ console.GetHalfWidth() }
    , m_PanelBarX{ m_CenterScreenW - (PANEL_BARS / 2) }
    , m_FirstChoice{-1}, m_SecondChoice{-1}
    , m_eSelectType{ SelectType::NONE }
{
    m_MenuSelector.SetSelectionFunc(std::bind(&GameMenuState::OnMenuSelect, this, _1, _2));
}

GameMenuState::~GameMenuState()
{
}

void GameMenuState::OnEnter()
{
    if (m_bInMenuSelect)
        m_PlayerSelector.HideCursor();

    m_Console.ClearBuffer();
}

void GameMenuState::OnExit()
{
    m_Console.ClearBuffer();
}

void GameMenuState::Update()
{
    UpdatePlayerOrder();
}

void GameMenuState::Draw()
{
    DrawPanels();
    DrawPlayerInfo();

    m_MenuSelector.Draw();
    m_PlayerSelector.Draw();

    m_Console.Draw();
}

void GameMenuState::ProcessInputs()
{
    if (m_bInMenuSelect)
    {
        if (m_Keyboard.IsKeyJustPressed(KEY_BACKSPACE))
        {
            m_StateMachine.PopState();
            return;
        }

        m_MenuSelector.ProcessInputs();
    }
    else
    {
        if (m_Keyboard.IsKeyJustPressed(KEY_BACKSPACE))
        {
            m_PlayerSelector.HideCursor();
            m_bInMenuSelect = true;
            m_eSelectType = SelectType::NONE;
            m_FirstChoice = m_SecondChoice = -1;
            m_MenuSelector.ShowCursor();
            m_Console.ClearBuffer();
            return;
        }

        m_PlayerSelector.ProcessInputs();
    }
}

bool GameMenuState::Exit()
{
    return m_bExitGame;
}
