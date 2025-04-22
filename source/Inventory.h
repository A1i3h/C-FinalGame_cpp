#pragma once

#include "Item.h"
#include "Equipment.h"
#include <vector>
#include <memory>

class Player;

class Inventory
{
private:
    std::vector<std::shared_ptr<Item>> m_Items;
    std::vector<std::shared_ptr<Equipment>> m_Equipment;

public:
    Inventory(); // Declaration only
    ~Inventory() = default;

    const std::vector<std::shared_ptr<Item>>& GetItems() const { return m_Items; }
    const std::vector<std::shared_ptr<Equipment>>& GetEquipment() const { return m_Equipment; }

    bool AddItem(std::shared_ptr<Item> newItem);
    bool AddEquipment(std::shared_ptr<Equipment> newEquipment);
    bool UseItem(int index, Player& player);
};

