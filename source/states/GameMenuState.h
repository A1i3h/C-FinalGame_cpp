#pragma once
#include "IState.h"
#include "../Selector.h"
#include "../Party.h"
#include "../Console.h"
#include "../Inputs/Keyboard.h"
#include "StateMachine.h"

class GameMenuState : public IState
{
    enum class SelectType {
        NONE,
        ITEM,
        MAGIC,
        EQUIPMENT,
        STATS,
        ORDER
    };

private:
    const int PANEL_BARS = 90;
    const int MENU_SIZE = 27;
    const int CLEAR_ROW = 90;
    const int SMALL_PANEL_BAR = 29;

    Party& m_Party;
    Console& m_Console;
    StateMachine& m_StateMachine;
    Keyboard& m_Keyboard;

    Selector<> m_MenuSelector;
    Selector<std::shared_ptr<Player>> m_PlayerSelector;

    bool m_bExitGame;
    bool m_bInMenuSelect;
    int m_ScreenWidth;
    int m_ScreenHeight;
    int m_CenterScreenW; 
    int m_PanelBarX;
    int m_FirstChoice, m_SecondChoice;

    SelectType m_eSelectType;

    void DrawPanels();
    void DrawPlayerInfo();
    void OnMenuSelect(int index, std::vector<std::wstring> data);
    void OnPlayerSelect(int index, std::vector<std::shared_ptr<Player>> data);
    void OnDrawPlayerSelect(int x, int y, std::shared_ptr<Player> player);
    void SetOrderPlacement(int playerPosition);
    void UpdatePlayerOrder();

public:
    GameMenuState(Party& party, Console& console, StateMachine& stateMachine, Keyboard& keyboard);
    ~GameMenuState();

    void OnEnter() override;
    void OnExit() override;
    void Update() override;
    void Draw() override;
    void ProcessInputs() override;
    bool Exit() override;
};
