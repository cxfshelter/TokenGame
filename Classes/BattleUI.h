//
//  BattleUI.h
//  TokenGame
//
//  Created by token on 15/3/20.
//
//

#ifndef __TokenGame__BattleUI__
#define __TokenGame__BattleUI__

#include "UIBase.h"
#include "Common.h"
#include "ProgressBar.h"

class BattleUI : public UIBase
{
    
private:
    bool Init();
    static BattleUI* sm_BattleUI;
    BattleUI& operator = (const BattleUI& other){return *this;};
protected:
    
    virtual void UpdateUI();
    virtual void LoadFinish();
    
private:
    // 是否播放特效
    bool m_IsPlayAnim = true;
    
    int m_ScheduleNameIdx = 0;
    
    bool m_IsAutoAttack = false;
    
    const string m_Stay = "Stay";
    const string m_Attack = "Attack";
    const string m_Beating = "Beating";
    const string m_Dead    = "Dead";
    
    Armature* m_MonsterArmature;
    
    vector<ProgressBar*>* m_VecProgressBar;
    
    void InitHeroUI();
    
    void InitMonsterUI(int cfgId);
    
    void BufferEffect(int bufferId);
    
    //每次加+1
    string GetNewScheduleName();
    
    //创建一个英雄动画
    Armature* CreateHeroArmature(int cfgId);
    
    //创建一个英雄
    void CreateHeroEvent(int cfgId, int idx);
    
    //创建一个怪物动画
    Armature* CreateMonsterArmature(int cfgId);
    
    
public:
    
    virtual void UnLoad();
    
    static BattleUI* getInstance();
    
    //战斗结束
    void BattleFinish(bool success);
    //每场战斗结束要删除
    string m_MonsterArmatureFileInfo;
    
    enum class AttackType
    {
        //英雄未命中
        HeroNotHit,
        //英雄攻击
        HeroAttack,
        //英雄职业相克攻击
        HeroProAttack,
        //英雄治疗攻击
        HeroHealAttack,
        
        //怪物未命中
        MonsterNotHit,
        //怪物攻击
        MonsterAttack,
        //怪物职业相克攻击
        MonsterProAttack,
        
    };
    
    //回城 Item To Storehouse
    void TownPortal();
    
    
    //死亡掉落物品 breadHeath:没面包 死亡
    void HeroDeath(bool breadHeath = false);
    
       //显示攻击文本
    void ShowAttackText(int attackVal, AttackType attackType);
    
    //战斗回城信息文本
    void GoMainUITextInfo(vector<pair<int, int>>* dropOut, LanguageIdEnum langId = (LanguageIdEnum)0);
    
};

#endif /* defined(__TokenGame__BattleUI__) */
