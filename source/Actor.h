#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "Stats.h"
#include "Equipment.h"
#include <memory>

class Actor
{
public:
    enum class ActorType
    {
        WARRIOR = 0, MAGE, ASSASSIN, KNIGHT,
    };

private:
    const std::vector<std::wstring> m_EquipmentSlotLabels{ L"Weapon", L"HeadGear", L"Armour", L"FootWear", L"Relic" };
    const std::vector<std::wstring> m_StatLabels{ L"Attack: ", L"Strength", L"Intelligence", L"Speed ", L"WillPower" , L"Stamina" };

protected:
    std::wstring m_sName, m_sID;
    int m_Level, m_XP, m_XPToNextLevel, m_HP, m_MaxHP;
    int m_MP, m_MaxMP;

    bool m_bDead;

    ActorType m_eActorType;

    std::unordered_map<Stats::EquipSlots, std::shared_ptr<Equipment>> m_mapEquipmentSlots;

    Stats m_Stats;

public:
    Actor();
    Actor(const std::wstring& name, const std::wstring& id, int level, int max_hp, ActorType type = ActorType::WARRIOR);

    ~Actor() = default;

    inline const int GetHP() const { return m_HP; }
    inline const int GetMaxHP() const { return m_MaxHP; }

    inline const std::vector<std::wstring>& GetEquipSlotLabels() const { return m_EquipmentSlotLabels; }
    inline const std::vector<std::wstring>& GetStatLabels() const { return m_StatLabels; }

    std::unordered_map < Stats::EquipSlots, std::shared_ptr<Equipment>>& GetEquippedItemSlots() { return m_mapEquipmentSlots; }

    Stats& GetStats() { return m_Stats; }

    inline const bool IsDead() const { return m_bDead; }
    inline const std::wstring& GetName() const { return m_sName; }
    inline const std::wstring& GetID() const { return m_sID; }

    const int GetLevel() const { return m_Level; }
    const int GetXP() const { return m_XP; }
    const int GetMP() const { return m_MP; }
    const int GetMaxMP() const { return m_MaxMP; }
    const int GetXPToNextLevel() const { return m_XPToNextLevel; }

    void HealHP(int hp);
    void TakeDamage(int hp);

    bool UseMP(int mp);

    // New methods for level progression
    void GainXP(int xp);
    void LevelUp();
};
