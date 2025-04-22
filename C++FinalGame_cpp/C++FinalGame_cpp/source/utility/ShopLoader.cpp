#include "ShopLoader.h"
#include "../Logger.h"
#include "../utility/trpg_utilities.h"
#include "ItemCreator.h"
#include "Equipment.Loader.h"
#include "ItemLoader.h"

using namespace tinyxml2;

ShopLoader::ShopLoader()
{
	m_pXMLDoc = std::make_unique<tinyxml2::XMLDocument>();
}

ShopLoader::~ShopLoader()
{
}

std::unique_ptr<ShopParameters> ShopLoader::CreateShopParametersFromFile(const std::string& shop_filepath)
{
	if (LoadFile(shop_filepath) != tinyxml2::XML_SUCCESS)
	{
		TRPG_ERROR("Failed to Load shop Parameters from [" + shop_filepath + "]");
		return nullptr;
	}

	XMLElement* pRootElement = m_pXMLDoc->RootElement();

	if (!pRootElement)
	{
		TRPG_ERROR("Failed to get the root element");
		return nullptr;
	}

	XMLElement* pShopParams = pRootElement->FirstChildElement("ShopParameters");

	if (!pShopParams)
	{
		TRPG_ERROR("Failed to get the Shop Parameters");
		return nullptr;
	}

	XMLElement* pShopType = pShopParams->FirstChildElement("ShopType");

	if (!pShopType)
	{
		TRPG_ERROR("Failed to get the Shop Type");
		return nullptr;
	}

	std::string shopTypeStr = pShopType->GetText();

	XMLElement* pInventory = pShopParams->FirstChildElement("Inventory");

	if (!pInventory)
	{
		TRPG_ERROR("Failed to get the Shop Inventory");
		return nullptr;
	}

	std::shared_ptr<Inventory> inventory = std::make_shared<Inventory>();
	ShopParameters::ShopType shopType = ShopTypeFromString(shopTypeStr);
	std::string definitionLocation{ "" };
	bool itemLoader{ false };
	bool weaponLoader{ false };

	switch (shopType)
	{
	case ShopParameters::ShopType::WEAPON:
		definitionLocation = "./assets/xml_files/WeaponDefs.xml";
		weaponLoader = true;
		break;
	case ShopParameters::ShopType::ARMOUR:
		definitionLocation = "./assets/xml_files/ArmourDefs.xml";
		break;
	case ShopParameters::ShopType::RELIC:
		definitionLocation = "./assets/xml_files/RelicDefs.xml";
		break;
	case ShopParameters::ShopType::ITEM:
		definitionLocation = "./assets/xml_files/ItemDefs.xml";
		itemLoader = true;
		break;
	case ShopParameters::ShopType::NOT_A_SHOP:
		TRPG_ERROR("Invalid Shop Type");
		return nullptr;
	}

	std::unique_ptr<ItemLoader> itemLoaderPtr{ nullptr };
	if (itemLoader)
		itemLoaderPtr = std::make_unique<ItemLoader>(definitionLocation);

	std::unique_ptr<EquipmentLoader> equipmentLoaderPtr{ nullptr };
	if (!itemLoader)
		equipmentLoaderPtr = std::make_unique<EquipmentLoader>(definitionLocation, weaponLoader);

	XMLElement* pShopItem = pInventory->FirstChildElement("ShopItem");

	if (!pShopItem)
	{
		TRPG_ERROR("Failed to get the Shop Item");
		return nullptr;
	}

	XMLElement* pItem = pShopItem->FirstChildElement(shopTypeStr.c_str());
	if (!pItem)
	{
		TRPG_ERROR("Failed to get the first Item");
		return nullptr;
	}

	while (pItem)
	{
		XMLElement* pName = pItem->FirstChildElement("Name");
		if (!pName)
		{
			TRPG_ERROR("Failed to get item name!");
			return nullptr;
		}

		std::string name{ pName->GetText() };

		if (itemLoader)
		{
			auto newItem = itemLoaderPtr->CreateObjectFromFile(name);

			if (newItem)
				inventory->AddItem(std::move(newItem));
		}
		else
		{
			auto newEquipment = equipmentLoaderPtr->CreateObjectFromFile(name);
			if (newEquipment)
				inventory->AddEquipment(std::move(newEquipment));
		}
		pItem = pItem->NextSiblingElement(shopTypeStr.c_str());
	}

	std::unique_ptr<ShopParameters> shop_parameters = std::make_unique<ShopParameters>(std::move(inventory), shopType);

	if (!shop_parameters)
	{
		TRPG_ERROR("Failed to create shop Parameters!");
		return nullptr;
	}

	return shop_parameters;
}

std::shared_ptr<ShopParameters> ShopLoader::CreateObjectFromFile(const std::string& objName)
{
	return std::shared_ptr<ShopParameters>();
}
