#include "ItemLoader.h"
#include "trpg_utilities.h"
#include "ItemCreator.h"
#include "../Potion.h"
#include "../Logger.h"
#include <cassert>

using namespace tinyxml2;

ItemLoader::ItemLoader(const std::string& filepath)
    : m_sFilePath{ filepath }
{
    m_pXMLDoc = std::make_unique<tinyxml2::XMLDocument>();
}

ItemLoader::~ItemLoader()
{
}

tinyxml2::XMLError ItemLoader::LoadFile(const std::string& filepath)
{
    return m_pXMLDoc->LoadFile(filepath.c_str());
}

std::shared_ptr<Item> ItemLoader::CreateObjectFromFile(const std::string& objName)
{
    if (LoadFile(m_sFilePath) != XML_SUCCESS)
    {
        std::string error{ m_pXMLDoc->ErrorStr() };
        TRPG_ERROR("Failed to Load Item -{" + m_sFilePath + "] -- " + error);
        return nullptr;
    }

    // Get Root Element
    XMLElement* pRootElement = m_pXMLDoc->RootElement();

    if (!pRootElement)
    {
        std::string error{ m_pXMLDoc->ErrorStr() };
        TRPG_ERROR("Failed to get the root Element! - " + error);
        return nullptr;
    }

    XMLElement* pItems = pRootElement->FirstChildElement("Items");

    if (!pItems)
    {
        std::string error{ m_pXMLDoc->ErrorStr() };
        TRPG_ERROR("Failed to get the Items! - " + error);
        return nullptr;
    }

    // Making Loop
    XMLElement* pItem = pItems->FirstChildElement("Item");

    if (!pItem)
    {
        std::string error{ m_pXMLDoc->ErrorStr() };
        TRPG_ERROR("Failed to get the First Item! - " + error);
        return nullptr;
    }

    std::shared_ptr<Item> newItem{ nullptr };

    while (pItem)
    {
        XMLElement* pName = pItem->FirstChildElement("Name");
        if (!pName)
        {
            std::string error{ m_pXMLDoc->ErrorStr() };
            TRPG_ERROR("Failed to get Item Name! - " + error);
            return nullptr;
        }

        std::string name{ pName->GetText() };

        if (name != objName)
        {
            pItem = pItem->NextSiblingElement("Item");
            continue;
        }

        XMLElement* pType = pItem->FirstChildElement("Type");
        if (!pType)
        {
            std::string error{ m_pXMLDoc->ErrorStr() };
            TRPG_ERROR("Failed to get Item Type! - " + error);
            return nullptr;
        }

        Item::ItemType type = ItemTypeFromString(pType->GetText());

        XMLElement* pDesc = pItem->FirstChildElement("Description");
        if (!pDesc)
        {
            std::string error{ m_pXMLDoc->ErrorStr() };
            TRPG_ERROR("Failed to get Item Description! - " + error);
            return nullptr;
        }
        std::wstring desc = CharToWide(pDesc->GetText());

        XMLElement* pValue = pItem->FirstChildElement("Value");
        if (!pValue)
        {
            std::string error{ m_pXMLDoc->ErrorStr() };
            TRPG_ERROR("Failed to get Item Value! - " + error);
            return nullptr;
        }

        int value = atoi(pValue->GetText());

        if (value < -1)
        {
            TRPG_ERROR("Item value below minimum!");
            return nullptr;
        }

        XMLElement* pBuyPrice = pItem->FirstChildElement("BuyPrice");
        if (!pBuyPrice)
        {
            std::string error{ m_pXMLDoc->ErrorStr() };
            TRPG_ERROR("Failed to get Item Buy Price! - " + error);
            return nullptr;
        }

        int buy_price = atoi(pBuyPrice->GetText());
        if (buy_price < 1)
        {
            TRPG_ERROR("Item Buy Price was below minimum!");
            return nullptr;
        }

        newItem = ItemCreator::CreateItem(type, CharToWide(name.c_str()), desc, value, buy_price);
        return newItem;
    }

    assert(newItem);
    return nullptr;
}


