//
//  BattleData.cpp
//  TokenGame
//
//  Created by token on 15/4/8.
//
//

#include "BattleUI.h"
#include "BattleData.h"
#include "RandomEngine.h"

BattleData* BattleData::sm_BattleData = nullptr;

BattleData* BattleData::getInstance()
{
    if (sm_BattleData == nullptr)
    {
        sm_BattleData = new (std::nothrow) BattleData();
        if (sm_BattleData->Init()) {}
    }
    return sm_BattleData;
}

bool BattleData::Init()
{
    
    return true;
}

void BattleData::releaseData()
{
    delete sm_BattleData;
    sm_BattleData = nullptr;
}

void BattleData::InitHeroData()
{
    
     m_MonsterNowHp = 0;
    
     m_MonsterMaxHp = 0;
    
    //提升命中率
     m_TalentHit = 0;
    
    //提升攻击伤害
     m_TalentAttack = 0;
    
    //战斗开始就绪
     m_TalentBattleReady = 0;
    
    //天赋血量 加 团队 当前HP
     m_TalentTeamNowHp = 0;
    
    //天赋血量 加 团队HP
     m_TalentTeamTotalHp = 0;
    
    //天赋闪避 加 团队闪避
     m_TalentTeamEvasion = 0;
    
    //减少受到伤害
     m_TalentReduceDamage = 0;
    
    //buffer 减少伤害
     m_BufferReduceDamage = 0;
    
    //----------------------获取英雄总闪避-------------------------
    
    auto vm_BackPack = &UserData::getInstance()->GetUserData(UserDataEnum::BackPack)->asValueMap();
    for (auto iter = vm_BackPack->begin(); iter != vm_BackPack->end(); iter++)
    {
        auto cfgId = atoi(iter->first.c_str());
        if (cfgId / 1000000 == 205)
        {
            CfgDataRow dataRow(cfgId);
            m_TalentTeamEvasion += dataRow.GetValue(CfgField::Evasion)->asInt();
        }
    }
    //----------------------获取英雄总血量---------------------------
    auto vm_LiveHero = &UserData::getInstance()->GetUserData(UserDataEnum::BackPack)->asValueMap();
    for (auto item : *vm_LiveHero)
    {
        for (auto i = 0; i < item.second.asInt(); i++)
        {
            CfgDataRow dataRow(atoi(item.first.c_str()));
            m_TalentTeamTotalHp += dataRow.GetValue(CfgField::Hp)->asInt();
        }
    }

    //----------------------获取自动攻击还是手动攻击---------------------------
    auto auto_val = UserData::getInstance()->GetUserData(UserDataEnum::AutoAttack)->asInt();
    m_HeroAutoAttack = auto_val == 0 ? false : true;
    
    // 同步天赋计算数值
    initTalentNum(true);
    
    m_TalentTeamNowHp = m_TalentTeamTotalHp;
}

//怪物攻击 反回英雄血量是否为0
bool BattleData::MonsterAttack()
{
    CfgDataRow dataRow(m_MonsterId);

    auto monsterHit = dataRow.GetValue(CfgField::Hit)->asInt();
    
    //命中 - 闪避
    auto rhit = monsterHit - m_TalentTeamEvasion;
    auto rv = RandomEngine::getInstance()->GetDefaultRandomValue(1, 100);
    if (rv <= rhit)
    {
        auto atk = dataRow.GetValue(CfgField::Atk)->asInt();
        
        //竞技场怪物
        if (m_BuildId == (int)CfgIdEnum::EternalArena)
        {
            auto atkExt = dataRow.GetValue(CfgField::AtkCoefficient_F)->asFloat();
            atk += (int)(atk * atkExt * m_NextBuildLayer - 1 + 0.5);
        }

        // 若怪物的攻击小于或等于己方的减伤值，强制扣1血
        auto attackVal = (atk - m_TalentReduceDamage - m_BufferReduceDamage);
        if(attackVal < 1) attackVal = 1;

        m_TalentTeamNowHp -= attackVal;
        
        BattleUI::getInstance()->ShowAttackText(attackVal, BattleUI::AttackType::MonsterAttack);
        
        m_TalentTeamNowHp = m_TalentTeamNowHp < 0 ? 0 : m_TalentTeamNowHp;
        
        m_HeroHpLBar->setPercent(((float)m_TalentTeamNowHp / (float)m_TalentTeamTotalHp) * 100);
        m_TextHeroHp->setString(StringUtils::format("%d/%d",m_TalentTeamNowHp, m_TalentTeamTotalHp));
    }else
    {
        BattleUI::getInstance()->ShowAttackText(0, BattleUI::AttackType::MonsterNotHit);
    }
    auto result = m_TalentTeamNowHp == 0 ? true : false;
    if (result) m_TalentTeamNowHp = m_TalentTeamTotalHp;
    return result;
}

//面包回血
void BattleData::BreadRevertHp()
{
    auto revertHp = UserData::getInstance()->GetUserData(UserDataEnum::BreadRevertHp)->asInt();
    m_TalentTeamNowHp += revertHp;
    
    BattleUI::getInstance()->ShowAttackText(revertHp, BattleUI::AttackType::HeroHealAttack);
    
    if (m_TalentTeamNowHp > m_TalentTeamTotalHp)
    {
        m_TalentTeamNowHp =  m_TalentTeamTotalHp;
    }
    m_HeroHpLBar->setPercent(((float)m_TalentTeamNowHp / (float)m_TalentTeamTotalHp) * 100);
    m_TextHeroHp->setString(StringUtils::format("%d/%d",m_TalentTeamNowHp, m_TalentTeamTotalHp));
}

//英雄攻击
//反回-1 怪物血量为0
//反回0 没中buffer 大于零为bufferId
int BattleData::HeroAttack(int cfgId)
{
    CfgDataRow heroDataRow(cfgId);
    CfgDataRow monsterDataRow(m_MonsterId);
    
    // 怪物与英雄属性
    auto heroProId = heroDataRow.GetValue(CfgField::PropertyID)->asInt();
    auto monsterProId = monsterDataRow.GetValue(CfgField::PropertyID)->asInt();
    
    auto heroHit = heroDataRow.GetValue(CfgField::Hit)->asInt();
    auto monsterEvasion = monsterDataRow.GetValue(CfgField::Evasion)->asInt();
    auto vv_Skills = &heroDataRow.GetValue(CfgField::SkillID_V)->asValueVector();
    
    auto bufferId = 0;
    auto skillId = 0;
    auto bufferAttack = GetBufferAttack(vv_Skills, bufferId, skillId);
    auto rvHit = (heroHit + m_TalentHit - monsterEvasion);
    
    auto rv = RandomEngine::getInstance()->GetDefaultRandomValue(1, 100);
    if (rv <= rvHit)
    {
        auto atk = heroDataRow.GetValue(CfgField::Atk)->asInt();
        
        auto proVal = GetProVal(heroProId, monsterProId);
        BattleUI::AttackType attackType;
        
        if (proVal <= 1)
        {
            attackType = BattleUI::AttackType::HeroAttack;
        }else
        {
            attackType = BattleUI::AttackType::HeroProAttack;
        }
        
        auto attackVal = ( atk + m_TalentAttack - bufferAttack) * proVal;
        // attack 此时为float，需对其四舍五入，取整
        attackVal = (int)(attackVal + 0.5);
        attackVal = attackVal < 1 ? 1 : attackVal;
        
        CfgDataRow skill_DataRow(skillId);
        auto skillType = skill_DataRow.GetValue(CfgField::Skilltype)->asInt();
        switch (skillType)
        {
            case (int)SkillType::AttackTarget :
                m_MonsterNowHp -= attackVal;
                BattleUI::getInstance()->ShowAttackText(attackVal, attackType);
                break;
            case (int)SkillType::HealHeroHP :
                m_TalentTeamNowHp += attackVal;
                BattleUI::getInstance()->ShowAttackText(attackVal, BattleUI::AttackType::HeroHealAttack);
                break;
            default:
                break;
        }
        
        if(bufferId > 0) //判断是否吸血buffer
        {
            CfgDataRow bufferDataRow(bufferId);
            if (bufferDataRow.GetValue(CfgField::Bufftype)->asInt() == (int)BufferCfgId::Vampire)
            {
                auto bufferHit = bufferDataRow.GetValue(CfgField::BuffHit)->asInt();
                rvHit = (bufferHit + m_TalentHit - monsterEvasion);
                rv = RandomEngine::getInstance()->GetDefaultRandomValue(1, 100);
                if (rv <= rvHit)
                {
                    bufferId = 0;
                    attackVal = bufferDataRow.GetValue(CfgField::BuffAttack)->asInt();
                    m_TalentTeamNowHp += attackVal;
                    
                    BattleUI::getInstance()->ShowAttackText(attackVal, BattleUI::AttackType::HeroHealAttack);
                    
                }
            }
        }
        
        if(m_TalentTeamNowHp > m_TalentTeamTotalHp) m_TalentTeamNowHp = m_TalentTeamTotalHp;
        
        m_HeroHpLBar->setPercent(((float)m_TalentTeamNowHp / (float)m_TalentTeamTotalHp) * 100);
        m_TextHeroHp->setString(StringUtils::format("%d/%d",m_TalentTeamNowHp, m_TalentTeamTotalHp));
        
        m_MonsterNowHp = m_MonsterNowHp < 0 ? 0 : m_MonsterNowHp;
        m_MonsterHpLBar->setPercent(((float)m_MonsterNowHp / (float)m_MonsterMaxHp) * 100);
        m_TextMonsterHp->setString(StringUtils::format("%d/%d", m_MonsterNowHp, m_MonsterMaxHp));
        
        return m_MonsterNowHp == 0 ? -1 : bufferId;
    }
    else
    {
        BattleUI::getInstance()->ShowAttackText(0, BattleUI::AttackType::HeroNotHit);
        return  -2; //未命中
    }
    
}

//获得第一个技能 第一个Buff的威力
float BattleData::GetBufferAttack(const ValueVector* vv_Skills, int& bufferId, int& skillId)
{
    auto result = 0;
    if (!vv_Skills->empty())
    {
        skillId = vv_Skills->begin()->asInt();
        
        CfgDataRow skillDataRow(skillId);
        auto vv_Buffer = &skillDataRow.GetValue(CfgField::BuffID_V)->asValueVector();
        if (!vv_Buffer->empty())
        {
            bufferId = vv_Buffer->begin()->asInt();
            
            CfgDataRow bufferDataRow(bufferId);
            result += bufferDataRow.GetValue(CfgField::BuffAttack)->asInt();
        }
        
    }
    return result;
}

//获取属性相克数据
float BattleData::GetProVal(int heroProId, int monsterProId)
{
    float result = 1.0;
    int cfgId = (int)ProCfgId::CfgBaseId + heroProId;
    CfgDataRow dataRow(cfgId);
    switch (monsterProId)
    {
        case (int)ProCfgId::MeleeCo :
            result = dataRow.GetValue(CfgField::MeleeCo_F)->asFloat();
            break;
        case (int)ProCfgId::FighterCo :
            result = dataRow.GetValue(CfgField::FighterCo_F)->asFloat();
            break;
        case (int)ProCfgId::MagicCo :
            result = dataRow.GetValue(CfgField::MagicCo_F)->asFloat();
            break;
        case (int)ProCfgId::RangedCo :
            result = dataRow.GetValue(CfgField::RangedCo_F)->asFloat();
            break;
        case (int)ProCfgId::FlyingCo :
            result = dataRow.GetValue(CfgField::FlyingCo_F)->asFloat();
            break;
    }
    return  result;
}

//buffer攻击 反回 false 表示怪物死亡
bool BattleData::BufferAttack(int bufferId)
{
    bool result = false;
    CfgDataRow dataRow(bufferId);
    auto bufferType = dataRow.GetValue(CfgField::Bufftype)->asInt();
    switch (bufferType)
    {
        case (int)BufferCfgId::PersistDamage: //持续伤害
        {
            auto attackVal = dataRow.GetValue(CfgField::BuffAttack)->asInt();
            m_MonsterNowHp -= attackVal;
            BattleUI::getInstance()->ShowAttackText(attackVal, BattleUI::AttackType::HeroAttack);
            m_MonsterNowHp = m_MonsterNowHp < 0 ? 0 : m_MonsterNowHp;
            if (m_MonsterNowHp == 0)
            {
                result = true;
            }
            m_MonsterHpLBar->setPercent(((float)m_MonsterNowHp / (float)m_MonsterMaxHp) * 100);
            m_TextMonsterHp->setString(StringUtils::format("%d/%d",m_MonsterNowHp, m_MonsterMaxHp));
            break;
        }
        case (int)BufferCfgId::PersistRecover: //持续恢复
        {
            auto attackVal = dataRow.GetValue(CfgField::PerHealing)->asInt();
            m_TalentTeamNowHp += attackVal;
            BattleUI::getInstance()->ShowAttackText(attackVal, BattleUI::AttackType::HeroHealAttack);
            
            if(m_TalentTeamNowHp > m_TalentTeamTotalHp) m_TalentTeamNowHp = m_TalentTeamTotalHp;
            m_HeroHpLBar->setPercent(((float)m_TalentTeamNowHp / (float)m_TalentTeamTotalHp) * 100);
            m_TextHeroHp->setString(StringUtils::format("%d/%d",m_TalentTeamNowHp, m_TalentTeamTotalHp));
            break;
        }
    }
    return result;
}

void BattleData::initTalentNum(bool isBattle)
{
    auto vv_Talent = UserData::getInstance()->GetUserData(UserDataEnum::TalentList)->asValueVector();
    for (auto talentId : vv_Talent)
    {
        CfgDataRow dataRow(talentId.asInt());
        auto vm = dataRow.GetValue(CfgField::FunctionID_M)->asIntMultiKeyMap();
        if (isBattle) {
            for (auto   kv : vm)
            {
                switch (kv.first)
                {
                    case (int)TalentCfgId::AdditionHp :         //提升团队HP
                        m_TalentTeamTotalHp += kv.second.asInt();
                        break;
                    case (int)TalentCfgId::AdditionAttack :     //提升攻击伤害
                        m_TalentAttack += kv.second.asInt();
                        break;
                    case (int)TalentCfgId::ReduceDamage :       //减少受到伤害
                        m_TalentReduceDamage += kv.second.asInt();
                        break;
                    case (int)TalentCfgId::AdditionHit :        //提升命中率
                        m_TalentHit += kv.second.asInt();
                        break;
                    case (int)TalentCfgId::AdditionEvasion :    //提升闪避率
                        m_TalentTeamEvasion = kv.second.asInt();
                        break;
                    case (int)TalentCfgId::BattleReady :        //战斗开始就绪
                        m_TalentBattleReady += kv.second.asInt();
                        break;
                    default:
                        break;
                }
            }
        }
        else {
            for (auto   kv : vm)
            {
                switch (kv.first)
                {
                    case (int)TalentCfgId::AdditionExplore :    //增加探索范围
                        m_TalentExplore = kv.second.asInt();
                        break;
                    case (int)TalentCfgId::ReduceBread :        //减少面包消耗（百分比）
                        m_TalentReduceBread = 1;
                        break;
                    default:
                        break;
                }
            }
        }
        
    }
}




