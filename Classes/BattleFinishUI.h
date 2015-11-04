//
//  BattleFinishUI.h
//  TokenGame
//
//  Created by token on 15/3/20.
//
//

#ifndef __TokenGame__BattleFinishUI__
#define __TokenGame__BattleFinishUI__

#include "UIBase.h"
#include "Common.h"

class BattleFinishUI : public UIBase
{
    
private:
    bool Init();
    static BattleFinishUI* sm_BattleFinishUI;
    BattleFinishUI& operator = (const BattleFinishUI& other){return *this;};
    
private:
    
    vector<pair<int, int>>* m_BagItem;
    vector<pair<int, int>>* m_DropItem;
    
    void SaveGold();
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
    //怪物Id
    static BattleFinishUI* getInstance();
    
};

#endif /* defined(__TokenGame__BattleFinishUI__) */
