#pragma once

#include <memory>
#include <vector>
#include <string>
#include "Inventory.h"

class Player;

class Party
{
private:
    std::vector<std::shared_ptr<Player>> m_PartyMembers;
    Inventory m_Inventory;
    const int MAX_ACTIVE_MEMBERS = 4;
    const int MAX_MEMBERS = 16;
    const int MAX_GOLD = 99999;

    int m_Gold;
    int m_NumActiveMembers;

public:
    Party(); // Declaration only
    ~Party();

    bool AddMember(std::shared_ptr<Player> newMember); // Declaration only
    bool RemoveMember(const std::wstring& memberID, bool eliminate = false);
    Inventory& GetInventory() { return m_Inventory; }
    std::vector<std::shared_ptr<Player>>& GetParty() { return m_PartyMembers; }
    const int GetGold() const { return m_Gold; }
    void AddGold(int gold);
    const int GetNumActiveMembers() const { return m_NumActiveMembers; }

    bool BuyItem(int price, std::shared_ptr<Item> item);
    bool BuyEquipment(int price, std::shared_ptr<Equipment> equipment);
};
