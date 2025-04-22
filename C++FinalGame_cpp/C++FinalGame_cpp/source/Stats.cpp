#include "Stats.h"
#include "Logger.h"
#include <iostream>

Stats::Stats()
	: Stats(0, 0, 0, 0, 0)
{

}

Stats::Stats(int strength, int intelligence, int speed, int willpower, int stamina)
	: m_StatList{
		{L"Attack", 0},
		{L"Defense", 0},
		{L"Magic", 0},
		{L"Strength", strength},
		{L"Speed", speed},
		{L"Intelligence", intelligence},
		{L"WillPower", willpower},
		{L"Stamina", stamina},
	},
m_StatModifierList{
		{L"Strength", 0},
		{L"Speed", 0},
		{L"Intelligence", 0},
		{L"WillPower", 0},
		{L"Stamina", 0},
	},
	m_EquipSlotList{
		{EquipSlots::WEAPON, 0},
		{EquipSlots::HEADGEAR, 0},
		{EquipSlots::CHEST_BODY, 0},
		{EquipSlots::FOOTWEAR, 0},
		{EquipSlots::RELIC, 0},

}
{
	//initialise
	UpdateStats();
}

const int Stats::GetStat(const std::wstring& key)
{
	if (m_StatList.find(key) == m_StatList.end())
	{
		//TRPG_ERROR("[" + key + "] -- Is not valid!");
		std::wcout << L"[" + key + L"] -- Is not valid!\n";
		return 0;
	}
	return m_StatList[key];
}

const int Stats::GetModifier(const std::wstring& key)
{
		if (m_StatModifierList.find(key) == m_StatModifierList.end())
		{
			//TRPG_ERROR("[" + key + "] -- Is not valid!");
			std::wcout << L"[" + key + L"] -- Is not valid!\n";
			return 0;
		}
		return m_StatModifierList[key];
}

const int Stats::GetEquipmentValue(EquipSlots slot)
{
	if (m_EquipSlotList.find(slot) == m_EquipSlotList.end())
	{
		//TRPG_ERROR("[" + key + "] -- Is not valid!");
		std::wcout << L"--Invalid slot!\n";
		return 0;
	}
	return m_EquipSlotList[slot];
}

void Stats::SetModifier(const std::wstring& key, int value)
{

	if (m_StatModifierList.find(key) == m_StatModifierList.end())
	{
		//TRPG_ERROR("[" + key + "] -- Is not valid!");
		std::wcout << L"[" + key + L"] --Is not valid!\n";
		return;
	}
	m_StatModifierList[key] = value;
}

void Stats::SetEquipmentValue(EquipSlots slot, int value)
{
	if (m_EquipSlotList.find(slot) == m_EquipSlotList.end())
	{
		//TRPG_ERROR("[" + key + "] -- Is not valid!");
		std::wcout << L"--Invalid slot!\n";
		return;
	}
	m_EquipSlotList[slot] = value ;
}

void Stats::SetStat(const std::wstring& key, int value)
{
	if (m_StatList.find(key) == m_StatList.end())
	{
		//TRPG_ERROR("[" + key + "] -- Is not valid!");
		std::wcout << L"[" + key + L"] -- Is not valid!\n";
		return;
	}
	 m_StatList[key] = value;
}

void Stats::UpdateStats()
{

	m_StatList[L"Attack"] = m_EquipSlotList[EquipSlots::WEAPON] + (m_StatList[L"Strength"] + m_StatModifierList[L"Strength"]) +
		((m_StatList[L"Intelligence"] + m_StatModifierList[L"Intelligence"]) / 5) +
		((m_StatList[L"WillPower"] + m_StatModifierList[L"WillPower"]) / 5);
	
	//defense stats

	m_StatList[L"Defense"] = m_EquipSlotList[EquipSlots::HEADGEAR] + m_EquipSlotList[EquipSlots::CHEST_BODY] + m_EquipSlotList[EquipSlots::FOOTWEAR] +
		((m_StatList[L"Strength"] + m_StatModifierList[L"Strength"]) / 5 ) +
		((m_StatList[L"Intelligence"] + m_StatModifierList[L"Intelligence"]) / 5) +
		((m_StatList[L"Speed"] + m_StatModifierList[L"Speed"]) / 5) +
		((m_StatList[L"WillPower"] + m_StatModifierList[L"WillPower"]) / 5);
}