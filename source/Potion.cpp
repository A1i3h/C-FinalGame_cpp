#include "Potion.h"
#include "Player.h"

Potion::Potion(const std::wstring& item_name, const std::wstring& desc, int health, int buy_price)
{
    m_sItemName = item_name;
    m_sDescription = desc;
    m_ItemValue = health;
    m_BuyPrice = buy_price;
    m_SellPrice = buy_price / 2;
    SetType(ItemType::HEALTH);
}

Potion::~Potion()
{
}

bool Potion::OnUse(Player& player)
{
    if (player.GetHP() < player.GetMaxHP())
    {
        player.HealHP(50); // Heal 50 HP
        return true;
    }
    return false;
}
