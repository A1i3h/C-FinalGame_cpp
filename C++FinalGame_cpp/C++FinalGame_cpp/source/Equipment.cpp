#include "Equipment.h"
#include "Player.h"
#include "Logger.h"

Weapon::Weapon()
	: Weapon(L"Weapon_Name", L"Weapon description", 0, 0)
{
}

Weapon::Weapon(const std::wstring& name, const std::wstring& description, int buy_price, WeaponProperties weapon_properties, StatModifier stat_modifier)
{
	m_sName = name;
	m_sDescription = description;
	m_BuyPrice = buy_price;
	m_SellPrice = buy_price / 2;
	m_StatModifier = stat_modifier;
	SetEquipType(EquipType::WEAPON);
	m_WeaponProperties = weapon_properties;
	m_ArmourProperties = ArmourProperties();
}

bool Weapon::OnEquip(Player& player)
{
	const auto& item_pwr = GetValue();
	auto& player_stats = player.GetStats();

	player_stats.SetEquipmentValue(Stats::EquipSlots::WEAPON, item_pwr);

	const auto& stat_modifier = GetStatModifier();

	if (stat_modifier.modifierType != StatModifier::ModifierType::NO_TYPE)
		player_stats.SetModifier(stat_modifier.modifierTypeStr, stat_modifier.statModifierVal);

	Equip();

	player_stats.UpdateStats();

	return true;
}

bool Weapon::OnRemove(Player& player)
{
	auto& player_stats = player.GetStats();
	player_stats.SetEquipmentValue(Stats::EquipSlots::WEAPON, 0);

	const auto& stat_modifier = GetStatModifier();

	if (stat_modifier.modifierType != StatModifier::ModifierType::NO_TYPE)
		player_stats.SetModifier(stat_modifier.modifierTypeStr, 0);

	Remove();

	player_stats.UpdateStats();

	return true;
}

Armour::Armour()
	: Armour(L"Armour_name", L"Armour Description", 0, 0)
{

}

Armour::Armour(const std::wstring& name, const std::wstring& description, int buy_price, ArmourProperties armour_properties, StatModifier stat_modifier)
{
	m_sName = name;
	m_sDescription = description;
	m_BuyPrice = buy_price;
	m_SellPrice = buy_price / 2;
	m_StatModifier = stat_modifier;
	SetEquipType(EquipType::WEAPON);
	m_WeaponProperties = WeaponProperties();
	m_ArmourProperties = armour_properties;
}

bool Armour::OnEquip(Player& player)
{
	const auto& item_pwr = GetValue();
	auto& player_stats = player.GetStats();

	Stats::EquipSlots slot = Stats::EquipSlots::NO_SLOT;
	switch (m_ArmourProperties.armourType)
	{
	case ArmourProperties::ArmourType::HEADGEAR:
			slot = Stats::EquipSlots::HEADGEAR;
			break;
	case ArmourProperties::ArmourType::CHEST_BODY:
		slot = Stats::EquipSlots::CHEST_BODY;
		break;
	case ArmourProperties::ArmourType::FOOTWEAR:
		slot = Stats::EquipSlots::FOOTWEAR;
		break;
	case ArmourProperties::ArmourType::NOT_ARMOUR:
		slot = Stats::EquipSlots::NO_SLOT;
		return false;
	default:
		slot = Stats::EquipSlots::NO_SLOT;
		return false;
	}

	const auto& stat_modifier = GetStatModifier();

	if (stat_modifier.modifierType != StatModifier::ModifierType::NO_TYPE)
		player_stats.SetModifier(stat_modifier.modifierTypeStr, stat_modifier.statModifierVal);

	player_stats.SetEquipmentValue(slot, item_pwr);

	Equip();

	player_stats.UpdateStats();

	return true;
}

bool Armour::OnRemove(Player& player)
{
	auto& player_stats = player.GetStats();

	Stats::EquipSlots slot = Stats::EquipSlots::NO_SLOT;
	switch (m_ArmourProperties.armourType)
	{
	case ArmourProperties::ArmourType::HEADGEAR:
		slot = Stats::EquipSlots::HEADGEAR;
		break;
	case ArmourProperties::ArmourType::CHEST_BODY:
		slot = Stats::EquipSlots::CHEST_BODY;
		break;
	case ArmourProperties::ArmourType::FOOTWEAR:
		slot = Stats::EquipSlots::FOOTWEAR;
		break;
	case ArmourProperties::ArmourType::NOT_ARMOUR:
		slot = Stats::EquipSlots::NO_SLOT;
		return false;
	default:
		slot = Stats::EquipSlots::NO_SLOT;
		return false;
	}

	const auto& stat_modifier = GetStatModifier();

	if (stat_modifier.modifierType != StatModifier::ModifierType::NO_TYPE)
		player_stats.SetModifier(stat_modifier.modifierTypeStr, 0);

	player_stats.SetEquipmentValue(slot, 0);

	Remove();

	player_stats.UpdateStats();

	return true;
	
}
