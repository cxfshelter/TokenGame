//
//  SettingUI.h
//  TokenGame
//
//  Created by best on 15/5/22.
//
//

#ifndef __TokenGame__SettingUI__
#define __TokenGame__SettingUI__

#include "UIBase.h"

// 设置
class SettingUI : public UIBase
{

public:
    static SettingUI* getInstance();
    
protected:
    virtual void UnLoad();
    virtual void UpdateUI();
    virtual void LoadFinish();
    
    
private:
    bool Init();
    
    static SettingUI* sm_Instance;
    
    SettingUI& operator = (const SettingUI& other){return *this;};
    
    // 显示是否确定重启
    void showConfirmDialog(bool isShow);
    
    // 重新开始
    void restartGame();
    
    // 切换存档
    void switchGame();
    
    // 保存对应的数值
    void saveSettingData(SettingDataEnum setType, Value val);
    
    //
    void showSwitchFileLayout();
    
    
};


#endif /* defined(__TokenGame__SettingUI__) */
