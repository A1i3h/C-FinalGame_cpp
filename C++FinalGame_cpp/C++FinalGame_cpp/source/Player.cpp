#include "Player.h"

Player::Player(const std::wstring& name, const std::wstring& id, Inventory& inventory, int level, int max_hp)
    : Actor(name, id, level, max_hp, ActorType::WARRIOR) // Call the base class constructor
    , m_Inventory(inventory) // Initialize the reference member
    , m_bActive(false)
    , m_PartyPosition(0)
{
    // No need to reassign inventory here
}

Player::~Player()
{
}


