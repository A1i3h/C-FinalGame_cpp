#pragma once

#include <string>

class Player;

struct StatModifier {
    enum class ModifierType { STRENGTH, SPEED, INTELLIGENCE, WILLPOWER, ELEMENTAL, STAMINA,NO_TYPE };
    enum class ElementalType { FIRE = 0, EARTH, WIND, ICE, WATER, LIGHTNING, NO_TYPE };

    int statModifierVal;
    ModifierType modifierType;
    ElementalType elementalType;
    std::wstring modifierTypeStr;

    StatModifier(int val = 0, ModifierType mod_type = ModifierType::NO_TYPE, ElementalType elemental_type = ElementalType::NO_TYPE)
        : statModifierVal(val), modifierType(mod_type), elementalType(elemental_type) {

        switch (mod_type) {
        case ModifierType::STRENGTH:
            modifierTypeStr = L"Strength";
            break;
        case ModifierType::SPEED:
            modifierTypeStr = L"Speed";
            break;
        case ModifierType::INTELLIGENCE:
            modifierTypeStr = L"Intelligence";
            break;
        case ModifierType::WILLPOWER:
            modifierTypeStr = L"Dexterity";
            break;
        case ModifierType::ELEMENTAL:
            modifierTypeStr = L"Elemental";
            break;
        case ModifierType::STAMINA:
            modifierTypeStr = L"Stamina";
            break;
        case ModifierType::NO_TYPE:
            modifierTypeStr = L"No Type";  // Fixed incorrect "L Strength"
            break;
        default:
            break;
        }
    }
};


struct WeaponProperties {
    enum class WeaponType { SWORD, DAGGER, BOW, STAFF, NOT_A_WEAPON };

    int attackPwr;
    WeaponType weaponType;

    WeaponProperties(int attack_pwr = 0, WeaponType weapon_type = WeaponType::NOT_A_WEAPON)
        : attackPwr(attack_pwr), weaponType(weapon_type) {
    }
};

struct ArmourProperties {
    enum class ArmourType { HEADGEAR, CHEST_BODY, FOOTWEAR, NOT_ARMOUR };

    int defensePwr;
    ArmourType armourType;

    ArmourProperties(int defense_pwr = 0, ArmourType armor_type = ArmourType::NOT_ARMOUR)
        : defensePwr(defense_pwr), armourType(armor_type) {
    }
};

class Equipment
{
public:
    enum class EquipType { WEAPON = 0, ARMOUR, RELIC, NO_TYPE };

private:
    const int MAX_COUNT = 50;
    Equipment::EquipType m_eEquipType{ EquipType::NO_TYPE };

protected:
    std::wstring m_sName{ L"" }, m_sDescription{ L"" };
    int m_BuyPrice{ 0 }, m_SellPrice{ 0 }, m_Count{ 1 }, m_weight{ 0 };
    bool m_bEquipped{ false };
    WeaponProperties m_WeaponProperties;
    ArmourProperties m_ArmourProperties;
    StatModifier m_StatModifier;

    void SetEquipType(EquipType type) { m_eEquipType = type; }

public:
    virtual ~Equipment() {}
    virtual const int GetValue() const = 0;
    virtual bool OnEquip(Player& player) = 0;
    virtual bool OnRemove(Player& player) = 0;

    inline void Remove() { if (m_bEquipped) m_bEquipped = false; }
    inline void Equip() { if (!m_bEquipped) m_bEquipped = true; } // Fixed logic for Equip
    inline const bool IsEquipped() const { return m_bEquipped; }
    inline const std::wstring& GetName() const { return m_sName; }
    inline const std::wstring& GetDescription() const { return m_sDescription; } // Fixed typo in method name
    inline bool Add(int num = 1) 
    {
        if (num < 1)
            num = 1;

        if (m_Count + num > MAX_COUNT)
            return false;

        m_Count += num;
        return true;
    }

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

    inline const int GetCount() const { return m_Count; }
    inline const Equipment::EquipType GetType() const { return m_eEquipType; }
    inline const WeaponProperties& GetWeaponProperties() const { return m_WeaponProperties; }
    inline const ArmourProperties& GetArmourProperties() const { return m_ArmourProperties; }
    inline const StatModifier& GetStatModifier() const { return m_StatModifier; }
    inline const int GetBuyPrice() const { return m_BuyPrice;  }
    inline const int GetSellPrice() const { return m_SellPrice;  }
    inline const int GetMaxCount() const { return MAX_COUNT; }
};


class Weapon : public Equipment
{
public:
    Weapon();
    Weapon(const std::wstring& name, const std::wstring& description, int buy_price, WeaponProperties weapon_properties = WeaponProperties(),
        StatModifier stat_modifier = StatModifier());
    ~Weapon() = default;

    inline const int GetValue() const override { return m_WeaponProperties.attackPwr; }
    bool OnEquip(Player& player) override;
    bool OnRemove(Player& player) override;
};

class Armour : public Equipment
{
public:
    Armour();
    Armour(const std::wstring& name, const std::wstring& description, int buy_price, ArmourProperties armour_properties = ArmourProperties(),
        StatModifier stat_modifier = StatModifier());
    ~Armour() = default;

    inline const int GetValue() const override { return m_ArmourProperties.defensePwr; }
    bool OnEquip(Player& player) override;
    bool OnRemove(Player& player) override;
};