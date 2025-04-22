#include "GameState.h"
#include "StateMachine.h"
#include "ShopState.h"
#include "../Logger.h"
#include "../Console.h"
#include "../inputs/keyboard.h"
#include "../Party.h"
#include "../Potion.h"
#include "../utility/ItemCreator.h"
#include "GameMenuState.h"
#include "../Player.h"
#include "../utility/ItemLoader.h"
#include "../utility/Equipment.Loader.h"
#include "../utility/ShopLoader.h" // Added missing include for ShopLoader
#include <cassert>
#include "../utility/ShopParameters.h"

GameState::GameState(Console& console, Keyboard& keyboard, StateMachine& stateMachine)
    : m_Console(console)
    , m_keyboard(keyboard)
    , m_Statemachine(stateMachine)
    , m_Selector(console, keyboard, { L"Start", L"Settings", L"Exit" })
    , m_TestInventory()
    , m_Party() // Initialize Party object
    , m_Timer()
    , m_Typewriter{ console, 45, 15,
        L"This is the new Typewriter\n"
        L"The Typewriter will be used for different dialogs!"
        L"Used for talking!", 60, 50, WHITE, BLUE
    }
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

    // Equipment creation
    auto sword1 = ItemCreator::CreateEquipment(
        Equipment::EquipType::WEAPON,
        WeaponProperties(15, WeaponProperties::WeaponType::SWORD),
        ArmourProperties(),
        StatModifier(3, StatModifier::ModifierType::STRENGTH),
        L"Short Sword", L"A small sword, not very effective", 100
    );

    auto sword2 = ItemCreator::CreateEquipment(
        Equipment::EquipType::WEAPON,
        WeaponProperties(25, WeaponProperties::WeaponType::SWORD),
        ArmourProperties(),
        StatModifier(5, StatModifier::ModifierType::STRENGTH),
        L"Long Sword", L"A long sword, more effective", 200
    );

    auto staff1 = ItemCreator::CreateEquipment(
        Equipment::EquipType::WEAPON,
        WeaponProperties(10, WeaponProperties::WeaponType::STAFF),
        ArmourProperties(),
        StatModifier(4, StatModifier::ModifierType::INTELLIGENCE),
        L"Wooden Staff", L"A basic wooden staff", 80
    );

    auto staff2 = ItemCreator::CreateEquipment(
        Equipment::EquipType::WEAPON,
        WeaponProperties(20, WeaponProperties::WeaponType::STAFF),
        ArmourProperties(),
        StatModifier(8, StatModifier::ModifierType::INTELLIGENCE),
        L"Enchanted Staff", L"A staff with magical properties", 160
    );

    auto chest1 = ItemCreator::CreateEquipment(
        Equipment::EquipType::ARMOUR,
        WeaponProperties(),
        ArmourProperties(10, ArmourProperties::ArmourType::CHEST_BODY),
        StatModifier(3, StatModifier::ModifierType::STRENGTH),
        L"Leather Armour", L"A well-worn piece of armour", 100
    );

    auto chest2 = ItemCreator::CreateEquipment(
        Equipment::EquipType::ARMOUR,
        WeaponProperties(),
        ArmourProperties(20, ArmourProperties::ArmourType::CHEST_BODY),
        StatModifier(6, StatModifier::ModifierType::STRENGTH),
        L"Steel Armour", L"A strong piece of armour", 200
    );

    auto helmet1 = ItemCreator::CreateEquipment(
        Equipment::EquipType::ARMOUR,
        WeaponProperties(),
        ArmourProperties(5, ArmourProperties::ArmourType::HEADGEAR),
        StatModifier(2, StatModifier::ModifierType::STRENGTH),
        L"Iron Helmet", L"A sturdy iron helmet", 80
    );

    auto helmet2 = ItemCreator::CreateEquipment(
        Equipment::EquipType::ARMOUR,
        WeaponProperties(),
        ArmourProperties(10, ArmourProperties::ArmourType::HEADGEAR),
        StatModifier(4, StatModifier::ModifierType::STRENGTH),
        L"Steel Helmet", L"A strong steel helmet", 160
    );

    // Add equipment to players' inventories
    player1->GetInventory().AddEquipment(sword1);
    player1->GetInventory().AddEquipment(sword2);
    player1->GetInventory().AddEquipment(staff1);
    player1->GetInventory().AddEquipment(staff2);
    player1->GetInventory().AddEquipment(chest1);
    player1->GetInventory().AddEquipment(chest2);
    player1->GetInventory().AddEquipment(helmet1);
    John->GetInventory().AddEquipment(helmet2);
}

GameState::~GameState()
{
}

void GameState::OnEnter()
{
    m_Console.ClearBuffer();

    Logger::Log("Attempting to load 'Potion' from Weapons.xml...");
    EquipmentLoader el{ "./assets/xml_files/WeaponsDefs.xml" };
    auto equipment = el.CreateObjectFromFile("Potion");

    if (!equipment)
    {
        Logger::Log("ERROR: Failed to load equipment item 'Potion' from Weapons.xml");
    }
    else
    {
        Logger::Log("Successfully loaded 'Potion' equipment from file.");
    }
}

void GameState::OnExit()
{
    m_Console.ClearBuffer();
}

void GameState::Update()
{
    m_Typewriter.UpdateText(); // Corrected to match the header file
}

void GameState::Draw()
{
    std::wstring time_ms = L"MS: " + std::to_wstring(m_Timer.ElapsedMS());
    std::wstring time_sec = L"SEC:" + std::to_wstring(m_Timer.ElapsedSec());

    m_Console.Write(25, 25, time_ms, RED);
    m_Console.Write(25, 26, time_sec, RED);

    m_Selector.Draw();

    m_Typewriter.Draw(); // Corrected to match the header file

    m_Console.Draw();
}

void GameState::ProcessInputs()
{
    if (m_keyboard.IsKeyJustPressed(KEY_ESCAPE))
    {
        m_Statemachine.PopState();
        return;
    }

    if (m_keyboard.IsKeyJustPressed(KEY_M))
    {
        m_Statemachine.PushState(std::make_unique<GameMenuState>(m_Party, m_Console, m_Statemachine, m_keyboard));
        return;
    }

    if (m_keyboard.IsKeyJustPressed(KEY_ENTER))
    {
        m_Statemachine.PushState(std::make_unique<ShopState>(m_Party, m_Console, m_Statemachine, m_keyboard, "./assets/xml_files/WeaponShopDef_1.xml"));
        return;
    }

    if (m_keyboard.IsKeyJustPressed(KEY_ENTER))
    {
        m_Timer.Start();
    }
    else if (m_keyboard.IsKeyJustPressed(KEY_P))
    {
        m_Timer.Pause();
    }
    else if (m_keyboard.IsKeyJustPressed(KEY_R))
    {
        m_Timer.Resume();
    }
    else if (m_keyboard.IsKeyJustPressed(KEY_T))
    {
        m_Timer.Stop();
        m_Console.ClearBuffer();
    }

    m_Selector.ProcessInputs();
}

bool GameState::Exit()
{
    return false;
}
