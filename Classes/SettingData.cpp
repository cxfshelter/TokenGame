//
//  SettingData.cpp
//  TokenGame
//
//  Created by best on 15/5/29.
//
//

#include "SettingData.h"
#include "DataEncryptUtils.h"

SettingData * SettingData::sm_Instance = nullptr;
SettingData * SettingData::getInstance()
{
    if (sm_Instance == nullptr) {
        sm_Instance = new (std::nothrow) SettingData();
    }
    
    return sm_Instance;
}

bool SettingData::init()
{
    bool isSave = false;
    if (m_SettingDataVm == nullptr) {
        m_SettingDataVm = new (std::nothrow) ValueMap();
    }
    
    if (GetSettingData(SettingDataEnum::CurFileDocName) == nullptr) {
        m_SettingDataVm->insert(pair<string, Value>(m_EnumSetKeyString[(int)SettingDataEnum::CurFileDocName], Value("1.txt")));
        isSave = true;
    }
    
    if (GetSettingData(SettingDataEnum::SoundEffectIsOpen) == nullptr) {
        m_SettingDataVm->insert(pair<string, Value>(m_EnumSetKeyString[(int)SettingDataEnum::SoundEffectIsOpen], Value(true)));
        isSave = true;
    }
    
    if (GetSettingData(SettingDataEnum::BgMusicIsOpen) == nullptr) {
        m_SettingDataVm->insert(pair<string, Value>(m_EnumSetKeyString[(int)SettingDataEnum::BgMusicIsOpen], Value(true)));
        isSave = true;
    }
    
    if (GetSettingData(SettingDataEnum::BattleEffectIsOpen) == nullptr) {
        m_SettingDataVm->insert(pair<string, Value>(m_EnumSetKeyString[(int)SettingDataEnum::BattleEffectIsOpen], Value(true)));
        isSave = true;
    }
    
    if (isSave) SaveSettingData();
    return true;
}

bool SettingData::LoadSettingData()
{
    // 因为独立区分开存档，故并没加存档路径
    m_SettingDataDir = FileUtils::getInstance()->getWritablePath() + "AppData";
    m_SettingDataPath = m_SettingDataDir + "/" + DataEncryptUtils::getInstance()->encode("SettingData.txt");
    
    if( !FileUtils::getInstance()->isDirectoryExist(m_SettingDataDir))
    {
        FileUtils::getInstance()->createDirectory(m_SettingDataDir);
    }
    
    m_SettingDataVm = new (std::nothrow) ValueMap();
    *m_SettingDataVm = FileUtils::getInstance()->getValueMapFromFile(m_SettingDataPath);
    init();
    
    return true;
}

bool SettingData::SaveSettingData()
{
    if (m_SettingDataVm == nullptr || m_SettingDataPath.length() <= 0) {
        return false;
    }
    
    FileUtils::getInstance()->writeToFile(*m_SettingDataVm, m_SettingDataPath);
    return true;
}

Value* SettingData::GetSettingData(SettingDataEnum ude)
{
    Value* result = nullptr;
    auto key = m_EnumSetKeyString[(uint16_t)ude];
    auto iter = m_SettingDataVm->find(key);
    if (iter != m_SettingDataVm->end())
    {
        result = &iter->second;
    }
    return result;
}

