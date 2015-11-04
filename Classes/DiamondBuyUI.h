//
//  DiamondBuyUI.h
//  TokenGame
//
//  Created by best on 15/6/24.
//
//

#ifndef __TokenGame__DiamondBuyUI__
#define __TokenGame__DiamondBuyUI__

#include "UIBase.h"

// 钻石购买及充值钻石
class DiamondBuyUI : public UIBase
{
public:
    static DiamondBuyUI* getInstance();
    
//    void setIOSPayDes(std::string identi, std::string addNum, std::string price);
    
    // 打开充值界面
    void showTopUpLayout();
    
protected:
    virtual void UnLoad();
    virtual void UpdateUI();
    virtual void LoadFinish();
    
private:
    bool Init();
    
    void saveDiamondData(int cfgID, Value val);
    
    // cfgId购买项目的id
    void showBuyLayout(int cfgId);
    
    
    
private:
    static DiamondBuyUI* sm_Instance;
    
    DiamondBuyUI& operator = (const DiamondBuyUI& other){return *this;};
    
};

#endif /* defined(__TokenGame__DiamondBuyUI__) */
