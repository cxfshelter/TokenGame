//
//  SoundUtils.cpp
//  TokenGame
//
//  Created by best on 15/6/16.
//
//

#include "SoundUtils.h"
#include "SimpleAudioEngine.h"
#include "UserData.h"
#include "SettingData.h"

SoundUtils* SoundUtils::m_Instance = nullptr;

SoundUtils* SoundUtils::getInstance()
{
    if (m_Instance == nullptr) {
        m_Instance = new (std::nothrow)SoundUtils();
    }
    return m_Instance;
}

bool SoundUtils::playMusicSound(const char* pszFilePath, bool isMusic, bool isLoop)
{
    // 设置里面是否播放
    bool isPlay;
    if (isMusic) {
        isPlay = SettingData::getInstance()->GetSettingData(SettingDataEnum::BgMusicIsOpen)->asBool();
    }
    else {
        isPlay = SettingData::getInstance()->GetSettingData(SettingDataEnum::SoundEffectIsOpen)->asBool();
    }
    
    if (isPlay) {
        if (isMusic) {
            CocosDenshion::SimpleAudioEngine::getInstance()->preloadBackgroundMusic(pszFilePath);
            CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic(pszFilePath, isLoop);
        }
        else {
            CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect(pszFilePath);
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(pszFilePath, isLoop);
        }
    }
    
    return isPlay;
}