#pragma once
#include "IState.h"
#include "../Selector.h"

class Party;
class Console;
class StateMachine;
class Keyboard;
struct ShopParameters;
class Item;
class Equipment;

class ShopState : public IState
{
private:
    const int PANEL_BARS = 100;
    const int MENU_SIZE = 27;

    Party& m_Party;
    Console& m_Console;
    StateMachine& m_StateMachine;
    Keyboard& m_Keyboard;

    std::unique_ptr<ShopParameters> m_pShopParameters;
    Selector<> m_ShopChoiceSelector;
    Selector<> m_BuySellSelector;
    Selector<std::shared_ptr<class Equipment>> m_EquipmentSelector;
    Selector<std::shared_ptr<class Item>> m_ItemSelector;

    int m_Quantity, m_Price, m_ScreenWidth, m_ScreenHeight, m_CenterScreenW, m_PanelBarX, m_AvailableSellQuantity;
    bool m_bInShopSelect, m_bInItemBuy, m_bInItemSell;
    bool m_bIsEquipmentShop, m_bExitShop, m_bBuySellItem;
    bool m_bSetFuncs;

    void DrawShop();
    void DrawBuyItems();
    void DrawItemsBox();
    void ResetSelections();

    void BuyEquipment();
    void SellEquipment();
    void BuyItems();
    void SellItems();

    void OnShopMenuSelect(int index, std::vector<std::wstring> data);
    void BuySellOptions(int index, std::vector<std::wstring> data);

    void OnBuyItemSelect(int index, std::vector<std::shared_ptr<class Item>> data);
    void OnBuyEquipmentSelect(int index, std::vector<std::shared_ptr<class Equipment>> data);

    void OnSellItemSelect(int index, std::vector<std::shared_ptr<class Item>> data);
    void OnSellEquipmentSelect(int index, std::vector<std::shared_ptr<class Equipment>> data);

    void RenderBuyItems(int x, int y, std::shared_ptr<class Item> item);
    void RenderBuyEquipment(int x, int y, std::shared_ptr<class Equipment> equipment);

    void RenderSellItems(int x, int y, std::shared_ptr<class Item> item);
    void RenderSellEquipment(int x, int y, std::shared_ptr<class Equipment> equipment);

    void UpdateBuyQuantity(int price);
    void UpdateSellQuantity(int totalAvailable);

public:
    ShopState(Party& party, Console& console, StateMachine& stateMachine, Keyboard& keyboard, const std::string& shopFilepath);
    ~ShopState();

    void OnEnter() override;
    void OnExit() override;
    void Update() override;
    void Draw() override;
    void ProcessInputs() override;
    bool Exit() override { return m_bExitShop; }
};
