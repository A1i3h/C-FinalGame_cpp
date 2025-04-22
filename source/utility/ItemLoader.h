#pragma once
#include <string>
#include <memory>
#include <tinyxml2.h>
#include "../Item.h"

class ItemLoader
{
public:
    ItemLoader(const std::string& filepath);
    ~ItemLoader();

    std::shared_ptr<Item> CreateObjectFromFile(const std::string& objName);

private:
    tinyxml2::XMLError LoadFile(const std::string& filepath);

    std::string m_sFilePath;
    std::unique_ptr<tinyxml2::XMLDocument> m_pXMLDoc;
};
