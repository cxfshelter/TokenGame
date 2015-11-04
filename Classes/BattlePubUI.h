//
//  BattlePubUI.h
//  TokenGame
//
//  Created by token on 15/3/20.
//
//

#ifndef __TokenGame__BattlePubUI__
#define __TokenGame__BattlePubUI__

#include "UIBase.h"
#include "Common.h"

//远征酒馆
class BattlePubUI : public UIBase
{

private:
    bool Init();
    static BattlePubUI* sm_BattlePubUI;
    BattlePubUI& operator = (const BattlePubUI& other){return *this;};
    
    // 长按提示次数
    int m_longTextShowNum = 0;
    
private:
    
    void ShowButton();
    
    //处理 英雄升级后 背包里的英雄数量要调整
    void UpdateBackPackHeroNum();
    
    void UpdateBackPackText_RanksText();
    
    //更新英雄数量 反回 -1 英雄不够 -2 队伍已满
    int UpdateLiveHero(int cfgId, int num);
    
    //更新物品数量 反回 -1 物品不够 -2 背包已满
    int UpdateBackPackNum(int cfgId, int num);
    
    void BtnAddReduceEvent(int cfgId, Widget* layoutItem, int addNum);
    
protected:
    virtual void UnLoad();
    virtual void UpdateUI();
    virtual void LoadFinish();
    
public:
    
    static BattlePubUI* getInstance();
    
    //获取背包容量和队伍人数
    tuple<int, int> GetBackPackNowNum_RanksNowNum();
    
    //获取背包中的英雄数量
    int GetBackPackHeroNum();
    //获取背包中的所有英雄
    void GetBackPackHero(vector<pair<int, int>>* heroVec);
    
    
};

#endif /* defined(__TokenGame__BattlePubUI__) */
