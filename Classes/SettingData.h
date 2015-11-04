//
//  SettingData.h
//  TokenGame
//
//  Created by best on 15/5/29.
//
//

#ifndef __TokenGame__SettingData__
#define __TokenGame__SettingData__

#include "cocos2d.h"
#include <unordered_map>
#include "EnunClass.h"

USING_NS_CC;
using namespace std;

class SettingData
{
public:
    static SettingData * getInstance();
    
    /*  加载设置里面的数据
     */
    bool LoadSettingData();
    
    /*  保存用户数据
     */
    bool SaveSettingData();
    
    Value* GetSettingData(SettingDataEnum ude);
    
private:
    bool init();
    
private:
    static SettingData * sm_Instance;
    
    string m_SettingDataDir;
    
    string m_SettingDataPath;
    
    // 设置基本数据（单独存在，否则切换存档会丢失）
    ValueMap * m_SettingDataVm = nullptr;
    
    const vector<string> m_EnumSetKeyString =
    {
        /*  存档
         *  string
         */
        "CurUserDataFileName",
        
        /*  音效
         *  bool
         */
        "SoundEffectIsOpen",
        
        /*  背景音乐
         *  bool
         */
        "BgMusicIsOpen",
        
        /*  战斗特效
         *  bool
         */
        "BattleIsOpen",
        
    };
    
};



#endif /* defined(__TokenGame__SettingData__) */
