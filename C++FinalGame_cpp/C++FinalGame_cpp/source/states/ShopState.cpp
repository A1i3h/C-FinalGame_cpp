#include "ShopState.h"
#include <cassert>
#include "../Party.h"
#include "../Console.h"
#include "StateMachine.h"
#include "../Inputs/Keyboard.h"
#include "../Equipment.h"
#include "../Item.h"

#include "../utility/ShopLoader.h"
#include "../utility/ShopParameters.h"
#include "../utility/ItemCreator.h"
#include "../Logger.h"

using namespace std::placeholders;

constexpr int PANEL_BARS = 100;

ShopState::ShopState(Party& party, Console& console, StateMachine& stateMachine, Keyboard& keyboard, const std::string& shopFilepath)
    : m_Party(party), m_Console(console), m_StateMachine(stateMachine), m_Keyboard(keyboard)
    , m_pShopParameters{ nullptr }
    , m_ShopChoiceSelector{
        console,
        keyboard, {L"BUY", L"SELL", L"EXIT"},
        SelectorParams{42, 10, 3}
    }
    , m_BuySellSelector{ console, keyboard, {L"OK", L"Cancel"},
        SelectorParams{80, 30, 2, 5 } }
    , m_EquipmentSelector{ console, keyboard, std::vector<std::shared_ptr<Equipment>>(), SelectorParams{30, 18, 1, 20, 2} }
    , m_ItemSelector{ console, keyboard, std::vector<std::shared_ptr<Item>>(), SelectorParams{30, 18, 1, 20, 2} }
    , m_Quantity{ 0 }, m_Price{ 0 }, m_ScreenWidth{ console.GetScreenWidth() }, m_ScreenHeight{ console.GetHalfHeight() }, m_CenterScreenW{ console.GetHalfWidth() }
    , m_bInShopSelect{ true }, m_bInItemBuy{ false }, m_bInItemSell{ false }, m_bSetFuncs{ false }
    , m_bIsEquipmentShop{ false }, m_bBuySellItem{ false }, m_bExitShop{ false }
    , m_AvailableSellQuantity{ 0 } // Fix for uninitialized variable
    , m_PanelBarX{ 0 }             // Fix for uninitialized variable
{
    ShopLoader shopLoader{};
    m_pShopParameters = std::move(shopLoader.CreateShopParametersFromFile(shopFilepath));

    if (!m_pShopParameters)
    {
        TRPG_ERROR("Failed to Load Shop parameters from [" + shopFilepath + "]");
        assert(m_pShopParameters);
        m_bExitShop = true;
    }

    switch (m_pShopParameters->shopType)
    {
    case ShopParameters::ShopType::WEAPON:
    case ShopParameters::ShopType::ARMOUR:
    case ShopParameters::ShopType::RELIC:
        m_EquipmentSelector.SetData(m_pShopParameters->inventory->GetEquipment());
        m_bIsEquipmentShop = true;
        break;
    case ShopParameters::ShopType::ITEM:
    {
        m_ItemSelector.SetData(m_pShopParameters->inventory->GetItems());
        m_bIsEquipmentShop = false;

        // Debug log to verify items
        const auto& items = m_pShopParameters->inventory->GetItems();
        if (items.empty())
        {
            TRPG_ERROR("No items loaded into the shop inventory!");
        }
        else
        {
            TRPG_LOG("Items loaded into the shop inventory:");
            for (const auto& item : items)
            {
                TRPG_LOG("Item: " + std::string(item->GetItemName().begin(), item->GetItemName().end()) +
                    ", Price: " + std::to_string(item->GetBuyPrice()));
            }
        }
        break;
    }
    case ShopParameters::ShopType::NOT_A_SHOP:
        assert(false && "Shop Type must be set!");
        break;
    }

    m_ShopChoiceSelector.SetSelectionFunc(std::bind(&ShopState::OnShopMenuSelect, this, _1, _2));
    m_BuySellSelector.SetSelectionFunc(std::bind(&ShopState::BuySellOptions, this, _1, _2));
    m_BuySellSelector.HideCursor();
}


ShopState::~ShopState()
{
}

void ShopState::OnEnter()
{
    m_Console.ClearBuffer();
}

void ShopState::OnExit()
{
    m_Console.ClearBuffer();
}

void ShopState::Update()
{
    if (m_bExitShop)
    {
        m_StateMachine.PopState();
        return;
    }

    if (m_bBuySellItem)
    {
        if (m_bInItemBuy)
            UpdateBuyQuantity(m_Price);
        else if (m_bInItemSell)
			UpdateSellQuantity(m_AvailableSellQuantity);
    }
}

void ShopState::Draw()
{
    DrawShop();
    m_ShopChoiceSelector.Draw();

    if ((m_bInItemBuy || m_bInItemSell) && m_bSetFuncs)
    {
        if (m_bIsEquipmentShop)
            m_EquipmentSelector.Draw();
        else
            m_ItemSelector.Draw();

        DrawItemsBox();
    }

    if (m_bBuySellItem)
    {
        DrawBuyItems();
        m_BuySellSelector.Draw();
    }

    m_Console.Draw();
}

void ShopState::ProcessInputs()
{
    if (m_bInShopSelect)
    {
        m_ShopChoiceSelector.ProcessInputs();
        return;
    }

    if (m_bInItemBuy && !m_bBuySellItem)
    {
        if (!m_bSetFuncs)
        {
            m_EquipmentSelector.SetSelectionFunc(std::bind(&ShopState::OnSellEquipmentSelect, this, _1, _2));
            m_EquipmentSelector.SetDrawFunc(std::bind(&ShopState::RenderSellEquipment, this, _1, _2, _3));
            m_EquipmentSelector.SetData(m_Party.GetInventory().GetEquipment());
        }
        else
        {
            m_ItemSelector.SetSelectionFunc(std::bind(&ShopState::OnSellItemSelect, this, _1, _2));
            m_ItemSelector.SetDrawFunc(std::bind(&ShopState::RenderSellItems, this, _1, _2, _3));
            m_ItemSelector.SetData(m_Party.GetInventory().GetItems());
        }

        m_bSetFuncs = true;
    }

    if (m_Keyboard.IsKeyJustPressed(KEY_BACKSPACE))
    {
        ResetSelections();
        return;
    }

    if (!m_bBuySellItem && m_bIsEquipmentShop && (m_bInItemBuy || m_bInItemSell))
        m_EquipmentSelector.ProcessInputs();
    else if (!m_bBuySellItem && !m_bIsEquipmentShop && (m_bInItemBuy || m_bInItemSell))
        m_ItemSelector.ProcessInputs();
    else if (m_bBuySellItem)
        m_BuySellSelector.ProcessInputs();
}

void ShopState::DrawShop()
{
    if (m_pShopParameters->shopType == ShopParameters::ShopType::ITEM)
    {
        const int pos_x = m_CenterScreenW - 25;
        m_Console.Write(pos_x, 3, L" |_   _|__   __|  ____|  \\/  |/ ____| |  | |/ __ \\|  __ \\ ", RED);
        m_Console.Write(pos_x, 4, L"   | |    | |  | |__  | \\  / | (___ | |__| | |  | | |__) |", RED);
        m_Console.Write(pos_x, 5, L"   | |    | |  |  __| | |\\/| |\\___ \\|  __  | |  | |  ___/ ", RED);
        m_Console.Write(pos_x, 6, L"  _| |_   | |  | |____| |  | |____) | |  | | |__| | |     ", RED);
        m_Console.Write(pos_x, 7, L" |_____|  |_|  |______|_|  |_|_____/|_|  |_|\\____/|_|     ", RED);
    }
    else if (m_pShopParameters->shopType == ShopParameters::ShopType::WEAPON)
    {
        const int pos_x = m_CenterScreenW - 33;
        m_Console.Write(pos_x, 3, L" \\ \\        / /  ____|   /\\   |  __ \\ / __ \\| \\ | |  __ \\ \\   / /", RED);
        m_Console.Write(pos_x, 4, L"  \\ \\  /\\  / /| |__     /  \\  | |__) | |  | |  \\| | |__) \\ \\_/ / ", RED);
        m_Console.Write(pos_x, 5, L"   \\ \\/  \\/ / |  __|   / /\\ \\ |  ___/| |  | | . ` |  _  / \\   /  ", RED);
        m_Console.Write(pos_x, 6, L"    \\  /\\  /  | |____ / ____ \\| |    | |__| | |\\  | | \\ \\  | |   ", RED);
        m_Console.Write(pos_x, 7, L"     \\/  \\/   |______/_/    \\_\\_|     \\____/|_| \\_|_|  \\_\\ |_|   ", RED);
    }
    else if (m_pShopParameters->shopType == ShopParameters::ShopType::ARMOUR)
    {
        const int pos_x = m_CenterScreenW - 31;
        m_Console.Write(pos_x, 3, L"     /\\   |  __ \\|  \\/  |/ __ \\| |  | |  __ \\ \\   / /", RED);
        m_Console.Write(pos_x, 4, L"    /  \\  | |__) | \\  / | |  | | |  | | |__) \\ \\_/ / ", RED);
        m_Console.Write(pos_x, 5, L"   / /\\ \\ |  _  /| |\\/| | |  | | |  | |  _  / \\   /  ", RED);
        m_Console.Write(pos_x, 6, L"  / ____ \\| | \\ \\| |  | | |__| | |__| | | \\ \\  | |   ", RED);
        m_Console.Write(pos_x, 7, L" /_/    \\_\\_|  \\_\\_|  |_|\\____/ \\____/|_|  \\_\\ |_|   ", RED);
    }

    if (m_pShopParameters->shopType == ShopParameters::ShopType::WEAPON)
    {
        const int pos_x = m_CenterScreenW - 20; // Center the title
    }

    // Draw menu options
    m_Console.Write(m_CenterScreenW - 10, 9, L"BUY", WHITE);
    m_Console.Write(m_CenterScreenW, 9, L"SELL", WHITE);
    m_Console.Write(m_CenterScreenW + 10, 9, L"EXIT", WHITE);

    // Draw gold display on the same line as menu options
    const int goldPosX = m_CenterScreenW + 30; // Adjust position to the right of "EXIT"
    m_Console.Write(goldPosX, 9, L"GOLD: " + std::to_wstring(m_Party.GetGold()), WHITE);

    // Panel constants
    constexpr int PANEL_BARS = 100;
    const int panelX = m_CenterScreenW - (PANEL_BARS + 4) / 2;
    m_PanelBarX = panelX;

    // Y positions
    const int topY = 1;
    const int bottomY = m_ScreenHeight + 18;

    // Top border
    m_Console.DrawPanelHorz(m_PanelBarX - 1, topY, PANEL_BARS + 2, BLUE);                   // Top
    m_Console.DrawPanelHorz(m_PanelBarX - 1, 9, PANEL_BARS + 2, BLUE);                      // Above selector
    m_Console.DrawPanelHorz(m_PanelBarX - 1, 11, PANEL_BARS + 2, BLUE);                     // Below selector
    m_Console.DrawPanelHorz(m_PanelBarX - 1, bottomY, PANEL_BARS + 2, BLUE);                // Bottom

    // Vertical sides
    m_Console.DrawPanelVert(m_PanelBarX - 1, topY + 1, bottomY - topY - 1, BLUE);           // Left side
    m_Console.DrawPanelVert(m_PanelBarX + PANEL_BARS, topY + 1, bottomY - topY - 1, BLUE);  // R
}

void ShopState::DrawBuyItems()
{
    // Draw quantity and total box
    const int boxX = m_CenterScreenW + 20; // Position the box on the right
    const int boxY = 20;

    m_Console.DrawPanel(boxX, boxY, 25, 10, BLUE); // Outer box

    // Draw headers
    m_Console.Write(boxX + 2, boxY + 2, L"QUANTITY:", WHITE);
    m_Console.Write(boxX + 2, boxY + 4, L"TOTAL:", WHITE);

    // Draw horizontal lines under the headers
    m_Console.DrawPanelHorz(boxX + 2, boxY + 3, 20, WHITE, L"-"); // Line under "QUANTITY"
    m_Console.DrawPanelHorz(boxX + 2, boxY + 5, 20, WHITE, L"-"); // Line under "TOTAL"

    // Draw values
    m_Console.Write(boxX + 12, boxY + 2, std::to_wstring(m_Quantity), WHITE);
    m_Console.Write(boxX + 12, boxY + 4, std::to_wstring(m_Price * m_Quantity), WHITE);

    // Draw OK/Cancel options
    m_Console.Write(boxX + 2, boxY + 6, L"-> OK", WHITE);
    m_Console.Write(boxX + 12, boxY + 6, L"CANCEL", WHITE);
}

void ShopState::DrawItemsBox()
{
    // Draw the item list box
    const int boxX = m_CenterScreenW - 40; // Position the box on the left
    const int boxY = 14;

    m_Console.DrawPanel(boxX, boxY, 50, 20, BLUE); // Outer box

    // Draw headers
    m_Console.Write(boxX + 5, boxY + 1, L"ITEM:", WHITE);
    m_Console.Write(boxX + 30, boxY + 1, L"PRICE:", WHITE);

    // Draw a horizontal line under the headers using dashes
    m_Console.DrawPanelHorz(boxX + 2, boxY + 2, 46, BLUE, L"-"); 

    // Draw gold display
    const auto& goldStr = std::to_wstring(m_Party.GetGold());
    m_Console.Write(boxX + 60, boxY + 1, L"GOLD: " + goldStr, WHITE);
}

void ShopState::ResetSelections()
{
	m_bInItemBuy = false;
	m_bInItemSell = false;
	m_bSetFuncs = false;
	m_bBuySellItem = false;
	m_bInShopSelect = true;

	m_ItemSelector.HideCursor();
	m_EquipmentSelector.HideCursor();
	m_ShopChoiceSelector.ShowCursor();
	m_Console.ClearBuffer();
}

void ShopState::BuyEquipment()
{
    int itemIndex = m_EquipmentSelector.GetIndex();
    const auto& item = m_EquipmentSelector.GetData()[itemIndex];

    if (item->GetCount() + m_Quantity - 1 > item->GetMaxCount())
        return;

    auto newItem = ItemCreator::CreateEquipment(
        item->GetType(),
        item->GetWeaponProperties(),
        item->GetArmourProperties(),
        item->GetStatModifier(),
        item->GetName(),
        item->GetDescription(),
        item->GetBuyPrice(),
        item->GetSellPrice() // Added missing argument
    );
    assert(newItem, &"Failed to create new item!");
    
    newItem->Add(m_Quantity - 1);

    auto& Inventory = m_Party.GetInventory();

    for (const auto& it : Inventory.GetEquipment())
    {
        if (newItem->GetName() != it->GetName())
            continue;

        if (newItem->GetCount() + it->GetCount() > it->GetMaxCount())
        {
            return;
        }
        break;
    }

    if (!m_Party.BuyEquipment(m_Quantity * m_Price, std::move(newItem)))
    {
        m_Console.Write(80, 34, L"Failed to buy the Equipment!", RED);
        return;
    }
}


void ShopState::SellEquipment()
{
}

void ShopState::BuyItems()
{
    int itemIndex = m_ItemSelector.GetIndex();
    const auto& item = m_ItemSelector.GetData()[itemIndex];

    if (item->GetCount() + m_Quantity - 1 > item->GetMaxCount())
        return;

    auto newItem = ItemCreator::CreateItem(
        item->GetType(),
        item->GetItemName(),
        item->GetDescription(),
        item->GetItemValue(),
        item->GetBuyPrice()
    );
    assert(newItem, &"Failed to create new item!");

    newItem->AddItem(m_Quantity - 1);

    auto& Inventory = m_Party.GetInventory();

    for (const auto& it : Inventory.GetItems())
    {
        if (newItem->GetItemName() != it->GetItemName())
            continue;

        if (newItem->GetCount() + it->GetCount() > it->GetMaxCount())
        {
            return;
        }
        break;
    }

    if (!m_Party.BuyItem(m_Quantity * m_Price, std::move(newItem)))
    {
        m_Console.Write(80, 34, L"Failed to buy the Item!", RED);
        return;
    }
}

void ShopState::SellItems()
{

}

void ShopState::OnShopMenuSelect(int index, std::vector<std::wstring> data)
{
	if (index > data.size() - 1 || data.empty())
		return;

	switch (index)
	{
	case 0:
		m_bInItemBuy = true;
		break; // Prevent fallthrough
	case 1:
		m_bInItemSell = true;
		break; // Prevent fallthrough
	case 2:
		m_bExitShop = true;
		break; // Prevent fallthrough
	default:
		return;
	}

	m_bInShopSelect = false;

	m_ShopChoiceSelector.HideCursor();

	if (m_bIsEquipmentShop)
		m_EquipmentSelector.ShowCursor();
	else
		m_ItemSelector.ShowCursor();
}

void ShopState::BuySellOptions(int index, std::vector<std::wstring> data)
{
	if (index == 1 || m_Quantity <= 0)
	{
		m_bBuySellItem = false;
		m_BuySellSelector.HideCursor();
		m_Quantity = 0;
		m_Price = 0;
		m_AvailableSellQuantity = 0;
		m_Console.ClearBuffer();
		return;
	}

	if (m_bInItemBuy)
	{
		if (m_bIsEquipmentShop)
			BuyEquipment();
		else
			BuyItems();
	}
	else if (m_bInItemSell)
	{
		if (m_bIsEquipmentShop)
			SellEquipment();
		else
			SellItems();
	}

    m_bBuySellItem = false;
    m_BuySellSelector.HideCursor();
    m_Quantity = 0;
    m_Price = 0;
    m_AvailableSellQuantity = 0;
    m_Console.ClearBuffer();
    return;
}

void ShopState::OnBuyItemSelect(int index, std::vector<std::shared_ptr<Item>> data)
{
	const auto& item = data[index];
	const auto& price = item->GetBuyPrice();
	m_bBuySellItem = true;
	m_BuySellSelector.ShowCursor();
	m_Price = price;
}

void ShopState::OnBuyEquipmentSelect(int index, std::vector<std::shared_ptr<Equipment>> data)
{
	const auto& item = data[index];
	const auto& price = item->GetBuyPrice();
	m_bBuySellItem = true;
	m_BuySellSelector.ShowCursor();
	m_Price = price;
}

void ShopState::OnSellItemSelect(int index, std::vector<std::shared_ptr<Item>> data)
{
	const auto& item = data[index];
	const auto& price = item->GetSellPrice();
	m_bBuySellItem = true;
	m_BuySellSelector.ShowCursor();
	m_Price = price;
    m_AvailableSellQuantity = item->GetCount();
}

void ShopState::OnSellEquipmentSelect(int index, std::vector<std::shared_ptr<Equipment>> data)
{
    const auto& item = data[index];
    const auto& price = item->GetSellPrice();
    m_bBuySellItem = true;
    m_BuySellSelector.ShowCursor();
    m_Price = price;
    m_AvailableSellQuantity = item->GetCount();
}

void ShopState::RenderBuyItems(int x, int y, std::shared_ptr<Item> item)
{
    const auto& name = item->GetItemName();
    const auto&& price = std::to_wstring(item->GetBuyPrice());

    m_Console.Write(x, y, name);
    m_Console.Write(x + 25, y, price);
}


void ShopState::RenderBuyEquipment(int x, int y, std::shared_ptr<Equipment> item)
{
	const auto& name = item->GetName();
	const auto&& price = std::to_wstring(item->GetBuyPrice());

	m_Console.Write(x, y, name);
	m_Console.Write(x + 25, y, price);
}

void ShopState::RenderSellItems(int x, int y, std::shared_ptr<Item> item)
{
	const auto& name = item->GetItemName();
	const auto&& price = std::to_wstring(item->GetSellPrice());

	m_Console.Write(x, y, name);
	m_Console.Write(x + 25, y, price);
}

void ShopState::RenderSellEquipment(int x, int y, std::shared_ptr<Equipment> item)
{
	const auto& name = item->GetName();
	const auto&& price = std::to_wstring(item->GetSellPrice());

	m_Console.Write(x, y, name);
	m_Console.Write(x + 25, y, price);
}

void ShopState::UpdateBuyQuantity(int price)
{
	const auto& gold = m_Party.GetGold();

	if (m_Keyboard.IsKeyJustPressed(KEY_W) && gold > (m_Quantity + 1) * price)
	{
		m_Quantity++;
	}
	else if (m_Keyboard.IsKeyJustPressed(KEY_S) && m_Quantity > 0)
	{
		m_Quantity--;
		m_Quantity = std::clamp(m_Quantity, 0, 99);
	}
}

void ShopState::UpdateSellQuantity(int totalAvailable)
{
	if (m_Keyboard.IsKeyJustPressed(KEY_W) && m_Quantity < totalAvailable)
	{
		m_Quantity++;
	}
	else if (m_Keyboard.IsKeyJustPressed(KEY_S) && m_Quantity > 0)
	{
		m_Quantity--;
		m_Quantity = std::clamp(m_Quantity, 0, 99);
	}
}
