//
//  BattleData.h
//  TokenGame
//
//  Created by token on 15/4/8.
//
//

#ifndef __TokenGame__BattleData__
#define __TokenGame__BattleData__

#include "Common.h"
#include "ProgressBar.h"

class BattleData
{

private:
    bool Init();
    static BattleData* sm_BattleData;
    BattleData& operator = (const BattleData& other){return *this;};
    
private:
    
    //职业相克 数据计算
    float GetProVal(int heroProId, int monsterProId);
    ////获得第一个技能 第一个Buff的威力
    float GetBufferAttack(const ValueVector* vv_Skills, int& bufferId, int& skillId);
public:
    
    static BattleData* getInstance();
    
    void releaseData();
    
    uint32_t m_TiledIdx;
    uint32_t m_MonsterId;
    uint32_t m_BuildId;
    uint32_t m_NextBuildLayer;
    
    int m_MonsterNowHp;
    
    int m_MonsterMaxHp;
    
    Text* m_TextMonsterHp;
    LoadingBar* m_MonsterHpLBar;
    
    ProgressBar* m_MonSterProgressBar;
//    //怪物被攻击后的Buffer
//    //      bufferId time
//    vector<pair<int, float>> m_MonsterBuffer;
    
    //自动攻击还是手动攻击
    // false为手动
    bool m_HeroAutoAttack;
    
    //提升命中率
    int m_TalentHit = 0;
    
    //探索范围
    int m_TalentExplore = 0;
    
    //提升攻击伤害
    int m_TalentAttack = 0;
    
    //战斗开始就绪
    int m_TalentBattleReady = 0;
    
    //天赋血量 加 团队 当前HP
    int m_TalentTeamNowHp = 0;
    
    //天赋血量 加 团队HP
    int m_TalentTeamTotalHp = 0;
    
    //天赋闪避 加 团队闪避
    int m_TalentTeamEvasion = 0;
    
    //减少面包消耗
    int m_TalentReduceBread = 0;
    
    //减少受到伤害
    int m_TalentReduceDamage = 0;
    
    //buffer 减少伤害
    int m_BufferReduceDamage = 0;

    Text* m_TextHeroHp;
    LoadingBar* m_HeroHpLBar;

    //初始化英雄的战斗
    void InitHeroData();
    
    //面包回血
    void BreadRevertHp();
    
    //怪物攻击 反回英雄血量是否为0
    bool MonsterAttack();
    
    //英雄攻击
    //反回-1 怪物血量为0
    //反回0 没中buffer 大于零为bufferId
    int HeroAttack(int cfgId);
    
    //buffer攻击 反回 false 表示怪物死亡
    bool BufferAttack(int bufferId);
    
    /* 同步天赋效果，isBattle: true - 同步天赋战斗数据，false - 同步天赋非战斗数据
    */
    void initTalentNum(bool isBattle);
    
};

#endif /* defined(__TokenGame__BattleData__) */
