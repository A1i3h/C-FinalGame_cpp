#include "GameState.h"
#include "StateMachine.h"
#include "GameMenuState.h"
#include "IntroGameState.h"
#include "../Logger.h"
#include "../Console.h"
#include "../inputs/Keyboard.h"
#include "../Party.h"
#include "../Potion.h"
#include "../utility/ItemCreator.h"
#include "GameMenuState.h"
#include "../Player.h"
#include "../utility/ItemLoader.h"
#include "../utility/Equipment.Loader.h"
#include "../utility/ShopLoader.h"
#include <cassert>
#include "../utility/ShopParameters.h"

using namespace std::placeholders;

GameState::GameState(Console& console, Keyboard& keyboard, StateMachine& stateMachine)
    : m_Console(console)
    , m_keyboard(keyboard)
    , m_Statemachine(stateMachine)
    , m_Selector(console, keyboard, { L"Start", L"Settings", L"Exit" })
    , m_TestInventory()
    , m_Party()
    , m_Timer()
{
    auto potion = ItemCreator::CreateItem(Item::ItemType::HEALTH, L"Potion", L"Heals a small amount of Health", 25, 50);

    // Initialize and add two players to the party
    auto player1 = std::make_shared<Player>(L"Player", L"test_player", m_TestInventory, 100, 200);
    auto John = std::make_shared<Player>(L"John", L"warrior", m_TestInventory, 50, 200);
    auto Ali = std::make_shared<Player>(L"Ali", L"Mage", m_TestInventory, 50, 200);

    m_Party.AddMember(player1);
    m_Party.AddMember(John);
    m_Party.AddMember(Ali);

    m_Party.GetInventory().AddItem(potion);
    m_Party.GetInventory().AddItem(potion);
}

GameState::~GameState()
{
}

void GameState::OnEnter()
{
    m_Console.ClearBuffer();
    std::vector<std::wstring> titleArt = {
        L"           _ _ _       _____  _____   _____ ",
        L"     /\\   | (_| )     |  __ \\|  __ \\ / ____|",
        L"    /  \\  | |_|/ ___  | |__) | |__) | |  __ ",
        L"   / /\\ \\ | | | / __| |  _  /|  ___/| | |_ |",
        L"  / ____ \\| | | \\__ \\ | | \\ \\| |    | |__| |",
        L" /_/    \\_\\_|_| |___/ |_|  \\_\\_|     \\_____|",
        L"",
        L"           Welcome to Ali's RPG"
    };

    int startY = 2;
    for (size_t i = 0; i < titleArt.size(); ++i) {
        m_Console.Write(5, startY + static_cast<int>(i), titleArt[i], BLUE);
    }
}

void GameState::OnExit()
{
    m_Console.ClearBuffer();
}

void GameState::Update()
{
    //not required
}

void GameState::Draw()
{
    std::wstring time_ms = L"MS: " + std::to_wstring(m_Timer.ElapsedMS());
    std::wstring time_sec = L"SEC:" + std::to_wstring(m_Timer.ElapsedSec());

    m_Console.Write(25, 25, time_ms, RED);
    m_Console.Write(25, 26, time_sec, RED);

    m_Selector.Draw();

    m_Console.Draw();
}

void GameState::ProcessInputs()
{
    // Check if "M" key is pressed
    if (m_keyboard.IsKeyJustPressed(VK_M)) {
        Logger::Log("M key pressed. Transitioning to GameMenuState...");
        m_Statemachine.PushState(std::make_unique<GameMenuState>(m_Party, m_Console, m_Statemachine, m_keyboard));
    }

    // Check if "X" key is pressed
    if (m_keyboard.IsKeyJustPressed(VK_X)) {
        Logger::Log("X key pressed. Transitioning to IntroGameState...");
        m_Statemachine.PushState(std::make_unique<GameMenuState>(m_Party, m_Console, m_Statemachine, m_keyboard));
    }

    // Process other inputs (e.g., menu navigation)
    m_Selector.ProcessInputs();
}

bool GameState::Exit()
{
    return false;
}