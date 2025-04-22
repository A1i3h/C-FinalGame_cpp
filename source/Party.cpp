#include "Party.h"
#include "Player.h"
#include "Logger.h"
#include "utility/trpg_utilities.h"

Party::Party()
    : m_PartyMembers{}
    , m_Inventory{}
    , m_Gold{ 500 }
    , m_NumActiveMembers{ 0 }
{
}

Party::~Party()
{
}

bool Party::AddMember(std::shared_ptr<Player> newMember)
{
    if (m_PartyMembers.size() >= MAX_MEMBERS)
    {
        TRPG_ERROR("There are already [" + std::to_string(MAX_MEMBERS) + "] members in the party!\nPlease remove!");
        return false;
    }

    for (const auto& member : m_PartyMembers)
    {
        if (member->GetID() == newMember->GetID())
        {
            TRPG_ERROR("Member is already in the Party!");
            return false;
        }
    }

    if (m_NumActiveMembers < MAX_ACTIVE_MEMBERS)
    {
        newMember->SetActive(true);
        m_NumActiveMembers++;
        newMember->SetPartyPosition(m_NumActiveMembers);
    }

    m_PartyMembers.push_back(std::move(newMember));

    return true;
}

bool Party::BuyEquipment(int price, std::shared_ptr<Equipment> equipment)
{
    // Check if the party has enough gold
    if (m_Gold < price)
    {
        return false; // Not enough gold
    }

    // Deduct the gold
    m_Gold -= price;

    // Add the equipment to the inventory
    if (!m_Inventory.AddEquipment(equipment))
    {
        // If adding the equipment fails, refund the gold
        m_Gold += price;
        return false;
    }

    return true; // Purchase successful
}
#include "Party.h"
#include "Item.h"

bool Party::BuyItem(int price, std::shared_ptr<Item> item)
{
    // Check if the party has enough gold
    if (m_Gold < price)
    {
        return false; // Not enough gold
    }

    // Deduct the gold
    m_Gold -= price;

    // Add the item to the inventory
    if (!m_Inventory.AddItem(item))
    {
        // If adding the item fails, refund the gold
        m_Gold += price;
        return false;
    }

    return true; // Purchase successful
}
