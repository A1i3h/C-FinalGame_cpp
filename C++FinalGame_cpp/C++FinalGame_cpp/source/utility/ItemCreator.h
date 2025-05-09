#pragma once
#include <memory>
#include <string>
#include <concepts>
#include "../Item.h"
#include "../Equipment.h"
#include "../Potion.h"

class ItemCreator
{
private:
    template <typename T, typename ...Args>
        requires std::constructible_from<T, Args...>
    static std::shared_ptr<T> Create(Args&& ...args)
    {
        return std::make_shared<T>(std::forward<Args>(args) ...);
    }

    template <typename T, typename ...Args>
    static std::shared_ptr<T> Create(Args&& ...args)
    {
        return nullptr;
    }

public:
    template <class ...Args>
    static std::shared_ptr<Item> CreateItem(Item::ItemType type, Args&& ...args)
    {
        switch (type)
        {
        case Item::ItemType::HEALTH:
            return Create<Potion>(std::forward<Args>(args) ...);
        case Item::ItemType::REVIVE:
            //return Create<Potion>(std::forward<Args>(args) ...);
        case Item::ItemType::STATUS_EFFECT:
            //return Create<STATUS_EFFECT>(std::forward<Args>(args) ...);
        case Item::ItemType::BATTLE:
            //return Create<BATTLE>(std::forward<Args>(args) ...);
        case Item::ItemType::KEY_ITEM:
            //return Create<KEY_ITEM>(std::forward<Args>(args) ...);
        case Item::ItemType::NO_TYPE:
            return nullptr;
        default:
            return nullptr;
        }
    }

    template <class ...Args>
    static std::shared_ptr<Equipment> CreateEquipment(Equipment::EquipType type, WeaponProperties weapon_props,
        ArmourProperties armour_props, StatModifier stat_mod, Args&& ...args)
    {
        switch (type)
        {
        case Equipment::EquipType::WEAPON:
            return Create<Weapon>(std::forward<Args>(args)..., weapon_props, stat_mod);
        case Equipment::EquipType::ARMOUR:
            return Create<Armour>(std::forward<Args>(args)..., armour_props, stat_mod);
        case Equipment::EquipType::RELIC:
            // return Create<Accessory>(std::forward<Args>(args)...);
        case Equipment::EquipType::NO_TYPE:
            return nullptr;
        default:
            return nullptr;
        }
    }
};
