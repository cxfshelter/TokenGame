//
//  HoldBuildUI.h
//  TokenGame
//
//  Created by token on 15/3/20.
//
//

#ifndef __TokenGame__HoldBuildUI__
#define __TokenGame__HoldBuildUI__

#include "UIBase.h"
#include "Common.h"

class HoldBuildUI : public UIBase
{

private:
    bool Init();
    static HoldBuildUI* sm_HoldBuildUI;
    HoldBuildUI& operator = (const HoldBuildUI& other){return *this;};
    
private:
    uint32_t m_TiledIdx = 0;
    
    vector<pair<int, int>>* m_BagItem;
    vector<pair<int, int>>* m_DropItem;
    
    void SaveGold();
    
    void OutputItems();
    
    void SaveBagItem();
    int GetBagItemNum();
    void RandomDropOut();
    void UpdateBagItemNum();
    
    bool IsSaveBag(int itemId);
    void ShowItemList(vector<pair<int, int>>* itemVect, bool direction);
    
protected:
    virtual void UnLoad();
    virtual void UpdateUI();
    virtual void LoadFinish();
    
public:
    static HoldBuildUI* getInstance();
    
};

#endif /* defined(__TokenGame__HoldBuildUI__) */
