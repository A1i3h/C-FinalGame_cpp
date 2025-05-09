#include "ItemMenuState.h"
#include "../Console.h"
#include "../Player.h"
#include "StateMachine.h"
#include "../Inputs/Keyboard.h"
#include "../utility/trpg_utilities.h"

using namespace std::placeholders;

void ItemState::DrawInventory()
{
    m_Console.DrawPanelHorz(m_PanelBarX - 1, 1, PANEL_BARS + 1, BLUE);
    int menu_x_pos = m_CenterScreenW - (MENU_SIZE / 2);
    int y_pos = 2; // Start drawing from Y=2 to leave space for other UI elements
    m_Console.Write(menu_x_pos, y_pos++, L"/ \/__ __\/  __// \__/|/ ___\ ", GREEN);  // M
    m_Console.Write(menu_x_pos, y_pos++, L"| |  / \  |  \  | |\/|||    \ ", GREEN);  // E
    m_Console.Write(menu_x_pos, y_pos++, L"| |  | |  |  /_ | |  ||\___ | ", GREEN);  // N
    m_Console.Write(menu_x_pos, y_pos++, L"\_/  \_/  \____\\_/  \|\____/", GREEN);  // U
    m_Console.DrawPanelHorz(18, 7, PANEL_BARS, BLUE);

    m_Console.DrawPanelHorz(m_PanelBarX - 1, 9, PANEL_BARS + 1, BLUE);
    m_Console.DrawPanelHorz(m_PanelBarX - 1, 11, PANEL_BARS + 1, BLUE);
    m_Console.DrawPanelHorz(m_PanelBarX - 1, 13, PANEL_BARS + 1, BLUE);

    m_Console.DrawPanelHorz(m_PanelBarX - 1, (m_ScreenHeight - 10), PANEL_BARS + 1, BLUE);
    m_Console.DrawPanelHorz(m_PanelBarX - 1, (m_ScreenHeight - 2), PANEL_BARS + 1, BLUE);

    m_Console.DrawPanelVert(m_PanelBarX - 1, 2, 44, BLUE);
    m_Console.DrawPanelVert(m_PanelBarX + PANEL_BARS, 2, 44, BLUE);
}

void ItemState::DrawPlayerInfo()
{
    const auto& name = m_Player.GetName();
    const auto& hp = m_Player.GetHP();
    const auto& hp_max = m_Player.GetMaxHP();
    const auto& level = std::to_wstring(m_Player.GetLevel());
    const auto& xp = std::to_wstring(m_Player.GetXP());
    const auto& xp2Next = std::to_wstring(m_Player.GetXPToNextLevel());

    WORD hp_colour = BLACK;
    if (hp <= hp_max * 0.3f)
        hp_colour = RED;
    else if (hp <= hp_max * 0.6)
        hp_colour = WHITE;

    std::wstring hp_string = L"HP: " + std::to_wstring(hp) + L" / " + std::to_wstring(hp_max);
    std::wstring level_string = L"Lvl: " + level + L" XP: " + xp + L" / " + xp2Next;

    m_Console.Write(26, 3 + m_ScreenHeight - 10, name);
    m_Console.Write(26, 4 + m_ScreenHeight - 10, hp_string);
    m_Console.Write(26, 5 + m_ScreenHeight - 10, level_string);
}

void ItemState::SelectorFunc(int index, SelectType type)
{
    switch (static_cast<ItemChoice>(index))
    {
    case ItemChoice::ITEM:
    {
        if (type == SelectType::PROCESS_INPUTS)
            m_ItemSelector.ProcessInputs();
        else if (type == SelectType::DRAW)
            m_ItemSelector.Draw();
        else if (type == SelectType::SHOW)
            m_ItemSelector.ShowCursor();
        else if (type == SelectType::HIDE)
            m_ItemSelector.HideCursor();
        break;
    }
    case ItemChoice::KEY_ITEM:
    {
        break;
    }
    default:
        break;
    }
}

void ItemState::OnMenuSelect(int index, std::vector<std::wstring> data)
{
    m_MenuSelector.HideCursor();
    m_bInMenuSelect = false;
    SelectorFunc(index, SelectType::SHOW);
}

void ItemState::OnItemSelect(int index, std::vector<std::shared_ptr<Item>> data)
{
    auto& items = m_Player.GetInventory();

    if (items.GetItems().empty())
        return;

    // Call the inventory to use the item
    items.UseItem(index, m_Player);

    // Get the count of the item
    const auto item_count = data[index]->GetCount();
    if (item_count <= 0)
    {
        remove(data, index);
        m_ItemSelector.SetData(items.GetItems());

        // Clear the buffer
        m_Console.ClearBuffer();
    }
}


void ItemState::RenderItem(int x, int y, std::shared_ptr<Item> item)
{
    int index = m_ItemSelector.GetIndex();
    const auto& data = m_ItemSelector.GetData();

    const std::wstring& item_name = item->GetItemName();
    m_Console.Write(x, y, item_name);
    m_Console.Write(x + static_cast<int>(item_name.size() + 1), y, std::to_wstring(item->GetCount()));

    if (index < data.size())
    {
        const std::wstring& item_desc = item->GetDescription();
        m_Console.Write(m_CenterScreenW - static_cast<int>(item_desc.size() / 2), 12, item_desc, LIGHT_BLUE);
    }
}

void ItemState::FocusOnMenu()
{
    m_bInMenuSelect = true;
    SelectorFunc(m_MenuSelector.GetIndex(), SelectType::HIDE);
    m_MenuSelector.ShowCursor();
}

ItemState::ItemState(Player& player, Console& console, StateMachine& statMachine, Keyboard& keyboard)
    : m_Console(console), m_StateMachine(statMachine), m_Keyboard(keyboard), m_Player(player)
    , m_MenuSelector{
        console, keyboard,
        {L"Items", L"Key Items"},
        SelectorParams{40, 10, 2, 25, 0}
    }
    , m_ItemSelector{
        console, keyboard,
        std::bind(&ItemState::OnItemSelect, this, _1, _2),
        std::bind(&ItemState::RenderItem, this, _1, _2, _3),
        std::vector<std::shared_ptr<Item>>(),
        SelectorParams{ 30, 14, 2, 35, 2 }
    }
    , m_bExitGame{ false }, m_bInMenuSelect{ true }
    , m_ScreenWidth{ console.GetScreenWidth() }, m_ScreenHeight{ console.GetScreenHeight() }, m_CenterScreenW{ console.GetHalfWidth() }
    , m_PanelBarX{ m_CenterScreenW - (PANEL_BARS / 2) }
{
    m_MenuSelector.SetSelectionFunc(std::bind(&ItemState::OnMenuSelect, this, _1, _2));
    m_ItemSelector.SetData(m_Player.GetInventory().GetItems());
}

ItemState::~ItemState()
{
}

void ItemState::OnEnter()
{
    TRPG_LOG("Entered Item State")
    m_Console.ClearBuffer();
}

void ItemState::OnExit()
{
    TRPG_LOG("Exited Item State")
    m_Console.ClearBuffer();
}

void ItemState::Update()
{
}

void ItemState::Draw()
{
    m_Console.Draw();
    DrawInventory();
    DrawPlayerInfo();

    m_MenuSelector.Draw();

    if (!m_bInMenuSelect)
        SelectorFunc(m_MenuSelector.GetIndex(), SelectType::DRAW);
}

void ItemState::ProcessInputs()
{
    if (m_bInMenuSelect)
    {
        m_MenuSelector.ProcessInputs();
        if (m_Keyboard.IsKeyJustPressed(KEY_BACKSPACE))
        {
            m_StateMachine.PopState();
        }
    }
    else
    {
        if (m_Keyboard.IsKeyJustPressed(KEY_BACKSPACE))
        {
            FocusOnMenu();
            m_Console.ClearBuffer();
        }

        if (m_Keyboard.IsKeyJustPressed(KEY_P)) // Use 'P' to use a potion
        {
            int selectedIndex = m_ItemSelector.GetIndex();
            auto& items = m_Player.GetInventory().GetItems();
            if (selectedIndex >= 0 && selectedIndex < items.size())
            {
                OnItemSelect(selectedIndex, items);
            }
        }

        SelectorFunc(m_MenuSelector.GetIndex(), SelectType::PROCESS_INPUTS);
    }
}

bool ItemState::Exit()
{
    return false;
}
