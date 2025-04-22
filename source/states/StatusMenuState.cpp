#include "StatusMenuState.h"
#include "../Console.h"
#include "../Player.h"
#include "../Inputs/Keyboard.h"
#include "StateMachine.h"
#include <cassert>

void StatusMenuState::DrawStatusPanel()
{
    m_Console.DrawPanelHorz(m_PanelBarX - 1, 1, PANEL_BARS + 1, RED);
    int menu_x_pos = m_CenterScreenW - (STATUS_SIZE / 2);
    int y_pos = 2;
    m_Console.Write(menu_x_pos, 2, L"   _____ _        _                 ", GREEN);
    m_Console.Write(menu_x_pos, 3, L"  / ____| |      | |                ", GREEN);
    m_Console.Write(menu_x_pos, 4, L" | (___ | |_ __ _| |_ _   _ ___     ", GREEN);
    m_Console.Write(menu_x_pos, 5, L"  \\___ \\| __/ _` | __| | | / __|    ", GREEN);
    m_Console.Write(menu_x_pos, 6, L"  ____) | || (_| | |_| |_| \\__ \\   ", GREEN);
    m_Console.Write(menu_x_pos, 7, L" |_____/ \\__\\__,_|\\__|\\__,_|___/   ", GREEN);
  
    m_Console.DrawPanelHorz(m_PanelBarX - 1, 9, PANEL_BARS + 2, RED);
    m_Console.DrawPanelHorz(m_PanelBarX - 1, 11, PANEL_BARS + 2, RED);
    
    m_Console.DrawPanelHorz(m_PanelBarX - 1, (m_ScreenHeight - 2), PANEL_BARS + 2, RED);

    m_Console.DrawPanelVert(m_PanelBarX - 1, 2, 44, RED);
    m_Console.DrawPanelVert(m_PanelBarX + PANEL_BARS, 2, 44, RED);
}

auto slot2str = [&](Stats::EquipSlots slot) {
    switch (slot)
    {
    case Stats::EquipSlots::WEAPON:
        return L"WEAPON ";
    case Stats::EquipSlots::HEADGEAR:
        return L"HEADGEAR ";
    case Stats::EquipSlots::CHEST_BODY:
        return L"CHEST ";
    case Stats::EquipSlots::FOOTWEAR:
        return L"FOOTWEAR ";
    case Stats::EquipSlots::RELIC:
        return L"RELIC ";
    case Stats::EquipSlots::NO_SLOT:
        assert(false, &"Should have a slot!");
            return L"NO_SLOT: ";
    default:
        assert(false, &"Should have a slot!");
        return L" ";
    }

};

void StatusMenuState::DrawPlayerInfo()
{
    const std::wstring& player_name = m_Player.GetName();
    m_Console.Write(m_CenterScreenW - static_cast<int>(player_name.size() / 2), 10, player_name);

    std::wstring level = std::to_wstring(m_Player.GetLevel());
    std::wstring hp = std::to_wstring(m_Player.GetHP());
    std::wstring hp_max = std::to_wstring(m_Player.GetMaxHP());
    std::wstring mp = std::to_wstring(m_Player.GetMP());
    std::wstring mp_max = std::to_wstring(m_Player.GetMaxMP()); // fixed
    std::wstring xp = std::to_wstring(m_Player.GetXP());
    std::wstring xp_next = std::to_wstring(m_Player.GetXPToNextLevel());

    m_Console.Write(STAT_LABEL_X_POS, 14, L"LEVEL: " + level);
    m_Console.Write(STAT_LABEL_X_POS, 15, L"HP: " + hp + L" / " + hp_max);
    m_Console.Write(STAT_LABEL_X_POS, 16, L"MP: " + mp + L" / " + mp_max);
    m_Console.Write(STAT_LABEL_X_POS, 17, L"XP: " + xp + L" / " + xp_next);

    // EQUIPMENT section
    m_Console.Write(STAT_LABEL_X_POS, 19, L"EQUIPMENT", BLUE);
    m_Console.Write(STAT_LABEL_X_POS, 20, L"=========", BLUE);

    int equipment_lines = 0;
    const auto& equipment = m_Player.GetEquippedItemSlots();
    for (const auto& [slot, item] : equipment)
    {
        m_Console.Write(STAT_LABEL_X_POS, 21 + equipment_lines, slot2str(slot));
        m_Console.Write(STAT_VAL_X_POS, 21 + equipment_lines, item ? item->GetName() : L"Empty");
        ++equipment_lines;
    }

    
    const int attr_start_y = 21 + equipment_lines + 2;
    int attr_index = 0;

    m_Console.Write(STAT_LABEL_X_POS, attr_start_y - 2, L"ATTRIBUTES", BLUE);
    m_Console.Write(STAT_LABEL_X_POS, attr_start_y - 1, L"=========", BLUE);

    const auto& stat_list = m_Player.GetStats().GetStatList();
    for (const auto& [stat, value] : stat_list)
    {
        int mod = m_Player.GetStats().GetModifier(stat);
        std::wstring value_str = std::to_wstring(value + mod);
        m_Console.Write(STAT_LABEL_X_POS, attr_start_y + attr_index, stat);
        m_Console.Write(STAT_VAL_X_POS, attr_start_y + attr_index, value_str);
        ++attr_index;
    }
}


StatusMenuState::StatusMenuState(Player& player, Console& console, StateMachine& stateMachine, Keyboard& keyboard)
	: m_Player(player), m_Console(console), m_StateMachine(stateMachine), m_Keyboard(keyboard)
	, m_ScreenWidth{ console.GetScreenWidth() }, m_ScreenHeight{ console.GetScreenHeight() }
	, m_CenterScreenW{ console.GetHalfWidth() }, m_PanelBarX{ m_CenterScreenW - (PANEL_BARS / 2) }
	, m_DiffPosY{ 0 }
{
}


StatusMenuState::~StatusMenuState()
{
}

void StatusMenuState::OnEnter()
{
    m_Console.ClearBuffer();
}

void StatusMenuState::OnExit()
{
    m_Console.ClearBuffer();
}

void StatusMenuState::Update()
{
}

void StatusMenuState::Draw()
{
    DrawStatusPanel();
    DrawPlayerInfo();

    m_Console.Draw();
}

void StatusMenuState::ProcessInputs()
{
	if (m_Keyboard.IsKeyJustPressed(KEY_BACKSPACE))
	{
		m_StateMachine.PopState();
	}
}

bool StatusMenuState::Exit()
{
	return false;
}
