#include "Equipment.Loader.h"
#include "trpg_utilities.h"
#include "ItemCreator.h"
#include "../Logger.h"
#include <cassert>
#include <memory> 

using namespace tinyxml2;

WeaponProperties EquipmentLoader::CreateWeaponProperties(tinyxml2::XMLElement* xmlElement)
{
	XMLElement* pWeaponProps = xmlElement->FirstChildElement("WeaponProperties");
	if (!pWeaponProps)
		return WeaponProperties();

	XMLElement* pAttackPwr = pWeaponProps->FirstChildElement("AttackPwr");
	int attackPwr = pAttackPwr ? atoi(pAttackPwr->GetText()) : 0;

	XMLElement* pWeaponType = pWeaponProps->FirstChildElement("WeaponType");
	const std::string weaponTypeStr = pWeaponType ? std::string{ pWeaponType->GetText() } : "Sword";
	WeaponProperties::WeaponType type = WeaponTypeFromString(weaponTypeStr);

	return WeaponProperties(attackPwr, type);
}

ArmourProperties EquipmentLoader::CreateArmorProperties(tinyxml2::XMLElement* xmlElement)
{
	XMLElement* pArmourProps = xmlElement->FirstChildElement("ArmourProperties");
	if (!pArmourProps)
		return ArmourProperties();

	XMLElement* pDefencePwr = pArmourProps->FirstChildElement("DefencePwr");
	int defencePwr = pDefencePwr ? atoi(pDefencePwr->GetText()) : 0;

	XMLElement* pArmourType = pArmourProps->FirstChildElement("ArmourType");
	const std::string armour_type_str = pArmourType ? std::string{ pArmourType->GetText() } : "";
	ArmourProperties::ArmourType type = ArmourTypeFromString(armour_type_str);

	return ArmourProperties(defencePwr, type);
}

StatModifier EquipmentLoader::CreateStatModifier(tinyxml2::XMLElement* xmlElement)
{
	XMLElement* pStatModifier = xmlElement->FirstChildElement("StatModifier");
	if (!pStatModifier)
		return StatModifier();

	XMLElement* pModifierVal = pStatModifier->FirstChildElement("ModValue");
	int mod_value = pModifierVal ? atoi(pModifierVal->GetText()) : 0;

	XMLElement* pModifierType = pStatModifier->FirstChildElement("ModType");
	const std::string mod_type_str = pModifierType ? std::string{ pModifierType->GetText() } : "";
	StatModifier::ModifierType mod_type = ModifierTypeFromString(mod_type_str);

	XMLElement* pElementalType = pStatModifier->FirstChildElement("ElementalType");
	const std::string elemental_type_str = pElementalType ? std::string{ pElementalType->GetText() } : "";
	StatModifier::ElementalType elemental_type = ElementalTypeFromString(elemental_type_str);

	return StatModifier(mod_value, mod_type, elemental_type);
}

EquipmentLoader::EquipmentLoader(const std::string& filepath, bool weapons)
	: m_sFilepath{ filepath }
	, m_bWeaponLoader{ weapons }
{
	m_pXMLDoc = std::make_unique<tinyxml2::XMLDocument>();
}

EquipmentLoader::~EquipmentLoader()
{
}

std::shared_ptr<Equipment> EquipmentLoader::CreateObjectFromFile(const std::string& objName)
{
	if (LoadFile(m_sFilepath) != XML_SUCCESS)
	{
		std::string error{ m_pXMLDoc->ErrorStr() };
		TRPG_ERROR("Failed to load the equipment file – [" + m_sFilepath + "] – " + error);
		return nullptr;
	}

	XMLElement* pRootElement = m_pXMLDoc->RootElement();
	if (!pRootElement)
	{
		std::string error{ m_pXMLDoc->ErrorStr() };
		TRPG_ERROR("Failed to get the root element for – [" + m_sFilepath + "] – " + error);
		return nullptr;
	}

	XMLElement* pEquipment = pRootElement->FirstChildElement(m_bWeaponLoader ? "Weapons" : "Arms");
	if (!pEquipment)
	{
		std::string error{ m_pXMLDoc->ErrorStr() };
		TRPG_ERROR("Failed to get the Equipment" + error);
		return nullptr;
	}

	XMLElement* pItem = pEquipment->FirstChildElement(m_bWeaponLoader ? "Weapon" : "Armour");
	if (!pItem)
	{
		std::string error{ m_pXMLDoc->ErrorStr() };
		TRPG_ERROR("Failed to get the first Item! " + error);
		return nullptr;
	}

	std::shared_ptr<Equipment> newEquipment{ nullptr };

	while (pItem)
	{
		XMLElement* pName = pItem->FirstChildElement("Name");
		if (!pName)
		{
			std::string error{ m_pXMLDoc->ErrorStr() };
			TRPG_ERROR("Failed to get name of Item !" + error);
			return nullptr;
		}

		std::string name{ pName->GetText() };
		if (name != objName)
		{
			pItem = pItem->NextSiblingElement(m_bWeaponLoader ? "Weapon" : "Armour");
			continue;
		}

		std::wstring equipName = CharToWide(name.c_str());

		XMLElement* pType = pItem->FirstChildElement("Type");
		if (!pType)
		{
			std::string error{ m_pXMLDoc->ErrorStr() };
			TRPG_ERROR("Failed to get type of Item !" + error);
			return nullptr;
		}

		Equipment::EquipType equipType{ EquipTypeFromString(pType->GetText()) };

		XMLElement* pDescription = pItem->FirstChildElement("Description");
		if (!pDescription)
		{
			std::string error{ m_pXMLDoc->ErrorStr() };
			TRPG_ERROR("Failed to get description of Item !" + error);
			return nullptr;
		}
		std::wstring desc = CharToWide(pDescription->GetText());

		XMLElement* pBuyPrice = pItem->FirstChildElement("BuyPrice");
		if (!pBuyPrice)
		{
			std::string error{ m_pXMLDoc->ErrorStr() };
			TRPG_ERROR("Failed to get buy price of Item !" + error);
			return nullptr;
		}

		int buy_price = atoi(pBuyPrice->GetText());
		if (buy_price < 1)
		{
			TRPG_ERROR("Item Buy Price was below minimum!");
			return nullptr;
		}

		WeaponProperties weapon_props = CreateWeaponProperties(pItem);
		ArmourProperties armour_props = CreateArmorProperties(pItem);
		StatModifier stat_mod = CreateStatModifier(pItem);

		newEquipment = ItemCreator::CreateEquipment(
			equipType,
			weapon_props,
			armour_props,
			stat_mod,
			equipName, desc, buy_price
		);

		return newEquipment;
	}

	TRPG_ERROR("Item - [" + objName + "] - does not exist");
	assert(newEquipment);
	return newEquipment;
}

