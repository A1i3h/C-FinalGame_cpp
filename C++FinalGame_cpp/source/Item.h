#pragma once
#include <string>
#include "Equipment.h"

class Player;

class Item 
{
public: enum ItemType { HEALTH = 0, STATUS_EFFECT, REVIVE, BATTLE, KEY_ITEM, NO_TYPE };
private:
    const int MAX_COUNT = 99;
    ItemType m_eItemType;
protected:
    int m_Count{ 1 }, m_BuyPrice{ 0 }, m_SellPrice{ 0 }, m_ItemValue{ 0 };
    std::wstring m_sItemName{ L"Item_Name" };
    std::wstring m_sDescription{ L"Item description goes here!" };

    void SetType(ItemType type) { m_eItemType = type; }

public:
    virtual ~Item() = default;
    virtual bool OnUse(Player& player) = 0;

    const int GetCount() const { return m_Count; }

    inline bool Decrement(int num = 1)
    {
        if (m_Count <= 0)
            return false;
        if (m_Count - num < 0)
            return false;

        m_Count -= num;

        if (m_Count <= 0)
            m_Count = 0;
        return true;
    }

    bool AddItem(int num = 1) {
		if (num < 1)
			num = 1;

        if (m_Count + num > MAX_COUNT)
            return false;

        m_Count += num;
        return true;
    }

    inline const std::wstring& GetItemName() const { return m_sItemName; }
    inline const std::wstring& GetDescription() const { return m_sDescription; }
    inline const int GetBuyPrice() const { return m_BuyPrice; }
    inline const int GetSellPrice() const { return m_SellPrice; }
    inline const int GetMaxCount() const { return MAX_COUNT; }
    inline const ItemType GetType() const { return m_eItemType;  }
    inline const int GetItemValue() const { return m_ItemValue;  }
};
