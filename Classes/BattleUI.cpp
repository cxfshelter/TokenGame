//
//  BattleUI.cpp
//  TokenGame
//
//  Created by token on 15/3/20.
//
//

#include "MapUI.h"
#include "Network.h"
#include "MainScene.h"
#include "BattleUI.h"
#include "BattleData.h"
#include "TiledMapUI.h"
#include "BattlePubUI.h"
#include "BattleFinishUI.h"
#include "BufferSchedule.h"
#include "AchievementSystem.h"


BattleUI* BattleUI::sm_BattleUI = nullptr;


BattleUI* BattleUI::getInstance()
{
    if (sm_BattleUI == nullptr)
    {
        sm_BattleUI = new (std::nothrow) BattleUI();
        if (sm_BattleUI->Init()) {}
    }
    return sm_BattleUI;
}

bool BattleUI::Init()
{
    return true;
}

void BattleUI::UnLoad()
{
    if (m_VecProgressBar == nullptr) return;
      
    for (auto pb : *m_VecProgressBar) delete pb;
    delete m_VecProgressBar;  m_VecProgressBar = nullptr;

    MsgListNode::getInstance()->UnLoadMsg();
    MoveText::getInstance()->ClearTextData();
    TiledMapUI::getInstance()->SetTouchEnabled(true);
    ArmatureDataManager::getInstance()->removeArmatureFileInfo(m_MonsterArmatureFileInfo.c_str());
    
    UIBase::UnLoad();
}

void BattleUI::UpdateUI()
{
    InitMonsterUI(BattleData::getInstance()->m_MonsterId);
    InitHeroUI();
}

void BattleUI::LoadFinish()
{
    m_IsPlayAnim = SettingData::getInstance()->GetSettingData(SettingDataEnum::BattleEffectIsOpen)->asBool();
    m_ScheduleNameIdx = 0;
    BattleData::getInstance()->InitHeroData();
    m_VecProgressBar = new vector<ProgressBar*>();
    MsgListNode::getInstance()->LoadMsg(m_RootWidget);
}


void BattleUI::InitHeroUI()
{
    ArmatureDataManager::getInstance()->addArmatureFileInfo("Texiao.ExportJson");
    
    auto layout_Bottom = m_RootWidget->getChildByName<Layout*>("Layout_Bottom");
    
    auto text_HeroText = layout_Bottom->getChildByName<Text*>("Text_HeroText");
    
    auto langId = (int)LanguageIdEnum::SelfLangId;
    auto langText = MultiLanguage::getInstance()->GetText(langId);
    text_HeroText->setString(langText);
    
    auto lbar_HeroHp = layout_Bottom->getChildByName<LoadingBar*>("LBar_HeroHp");
    lbar_HeroHp->setPercent(100);
    
    
    //自动或手动控制战斗
    auto btn_Auto = layout_Bottom->getChildByName<Button*>("Btn_Auto");
    auto btn_UnAuto = layout_Bottom->getChildByName<Button*>("Btn_UnAuto");
    if (BattleData::getInstance()->m_HeroAutoAttack)
    {
        btn_Auto->setVisible(true); btn_UnAuto->setVisible(false);
        
    }else
    {
        btn_Auto->setVisible(false); btn_UnAuto->setVisible(true);
    }
    
    langId = (int)LanguageIdEnum::AutoAttackLangId;
    langText = MultiLanguage::getInstance()->GetText(langId);
    btn_Auto->setTitleText(langText); btn_UnAuto->setTitleText(langText);
    btn_Auto->addClickEventListener([=](Ref* sender)
     {
         btn_Auto->setVisible(false);
         btn_UnAuto->setVisible(true);
         BattleData::getInstance()->m_HeroAutoAttack = false;
         UserData::getInstance()->Update_Value(UserDataEnum::AutoAttack, Value(0));
     });
    
    btn_UnAuto->addClickEventListener([=](Ref* sender)
    {
        btn_Auto->setVisible(true);
        btn_UnAuto->setVisible(false);
        for (auto pb : *m_VecProgressBar) pb->Resume();
        BattleData::getInstance()->m_HeroAutoAttack = true;
        UserData::getInstance()->Update_Value(UserDataEnum::AutoAttack, Value(1) );
    });
    
    auto btn_Bread = layout_Bottom->getChildByName<Button*>("Btn_Bread");
    auto text_BreadName = btn_Bread->getChildByName<Text*>("Text_BreadNAME");
    langId = (int)LanguageIdEnum::BreadLangId;
    langText = MultiLanguage::getInstance()->GetText(langId);
    text_BreadName->setString(langText);
    
    auto text_Bread = btn_Bread->getChildByName<Text*>("Text_Bread");
    
    
    auto bread_Num = 0;
    auto breadData = UserData::getInstance()->Get_MVI_Value(UserDataEnum::BackPack, (int)CfgIdEnum::BreadId);
    if(breadData != Value::Null) bread_Num = breadData.asInt();
    
    text_Bread->setString(StringUtils::format("%d", bread_Num));
    
    auto lbar_Bread = btn_Bread->getChildByName<LoadingBar*>("LBar_Bread");
    
    string pbName = "BreadHpSchedule";
    auto progressBar = new ProgressBar();
    progressBar->SetTotalTime(3); //3秒吃一个面包
    progressBar->SetScheduleName(pbName);
    progressBar->SetLoadingBar(lbar_Bread);
    
    m_VecProgressBar->push_back(progressBar);
    
    progressBar->SetCallBack([this, progressBar]()
     {
         progressBar->Restart();
         progressBar->GetLoadingBar()->setPercent(0);
         m_RootWidget->unschedule(progressBar->GetScheduleName());
     });
    
    
    btn_Bread->addClickEventListener([=](Ref* sender)
     {

         auto bread_Num = 0;
         auto breadData = UserData::getInstance()->Get_MVI_Value(UserDataEnum::BackPack, (int)CfgIdEnum::BreadId);
         if(breadData != Value::Null) bread_Num = breadData.asInt();

         if (bread_Num == 0)
         {
             auto langId = (int)LanguageIdEnum::BreadNotEnoughId;
             auto langText = MultiLanguage::getInstance()->GetText(langId);
             MoveText::getInstance()->AddDefaultTextData(m_RootWidget, langText);
         }else
         {
             if(progressBar->GetRemainTime() == progressBar->GetTotalTime())
             {
                 auto bread_Num = UserData::getInstance()->Update_VMI_Fun(
                                  UserDataEnum::BackPack, (int)CfgIdEnum::BreadId, -1);
                 if (bread_Num >= 0)
                 {
                     text_Bread->setString(StringUtils::format("%d", bread_Num));
                     //回血
                     BattleData::getInstance()->BreadRevertHp();
                     
                     m_RootWidget->schedule([this, progressBar](float f)
                        {
                            progressBar->Update(f);
                        }, progressBar->GetScheduleName());
                 }
             }
             AchievementSystem::getInstance()->CheckAchievement(UserEventType::BattleEatBread, 1);
         }
     });
    
    vector<pair<int, int> >heroVec;
    BattlePubUI::getInstance()->GetBackPackHero(&heroVec);
    auto idx = 0; //创建英雄技能
    for (auto heroData : heroVec)
    {
        auto heroId = heroData.first;
        auto heroNum = heroData.second;
        for (auto i = 0; i < heroNum ; i++)
        {
            CreateHeroEvent(heroId, idx);  idx++;
        }
    }

    auto talentTeamTotalHp = BattleData::getInstance()->m_TalentTeamTotalHp;
    auto text_HeroHp = layout_Bottom->getChildByName<Text*>("Text_HeroHp");
    text_HeroHp->setString(StringUtils::format("%d/%d", talentTeamTotalHp, talentTeamTotalHp));
    
    BattleData::getInstance()->m_HeroHpLBar = lbar_HeroHp;
    BattleData::getInstance()->m_TextHeroHp = text_HeroHp;

}

//英雄显示位置
void BattleUI::CreateHeroEvent(int cfgId, int idx)
{

    auto layout_Bottom = m_RootWidget->getChildByName<Layout*>("Layout_Bottom");
    
    string btn_Name = StringUtils::format("Btn_Skill%d", idx);
    auto Btn_Hero = layout_Bottom->getChildByName<Button*>(btn_Name);
    Btn_Hero->setVisible(true);
    
    auto lbar_Name = StringUtils::format("LBar_State%d", idx);
    auto lbar_State = Btn_Hero->getChildByName<LoadingBar*>(lbar_Name);
    lbar_State->setPercent(100);
    
    CfgDataRow dataRow(cfgId);
    auto textName = StringUtils::format("Text_Name%d", idx);
    
    auto text_Name = Btn_Hero->getChildByName<Text*>(textName);
    
    auto skillId = dataRow.GetValue(CfgField::SkillID_V)->asValueVector().begin()->asInt();
    CfgDataRow skillDataRow(skillId);
    auto langId = skillDataRow.GetValue(CfgField::Name)->asInt();
    auto langText = MultiLanguage::getInstance()->GetText(langId);
    text_Name->setString(langText);
    
    auto attack_Speed = dataRow.GetValue(CfgField::Spd_F)->asFloat();
    attack_Speed = 10 / attack_Speed;
    
    Armature* armature = CreateHeroArmature(cfgId);
    
    string pbName = StringUtils::format("HeroSkillSchedule%d", idx);
    auto playName = skillDataRow.GetValue(CfgField::SkillAtksfxID_S)->asString();
    
    auto isGetKong = UserData::getInstance()->Check_VI_Fun(UserDataEnum::TalentList, Value((int)CfgIdEnum::TalentEliminateCD));
    auto progressBar = new ProgressBar();
    progressBar->SetScheduleName(pbName);
    progressBar->SetTotalTime(attack_Speed, isGetKong);
    progressBar->SetLoadingBar(lbar_State);
    
    m_VecProgressBar->push_back(progressBar);
    
    progressBar->SetCallBack([=]()
       {
           progressBar->Restart();
           if (armature->getAnimation()->getAnimationData()->getMovement(playName) == nullptr)
           {
               CCLOGERROR("Armature:%s, not %s", armature->getName().c_str(), playName.c_str());
           }else if(m_IsPlayAnim)
           {
               armature->setVisible(true);
               armature->getAnimation()->play(playName);
           }
           
           if (m_MonsterArmature != nullptr && m_IsPlayAnim)
           {
               m_MonsterArmature->getAnimation()->play(m_Beating);
           }
           
           auto attackResult = BattleData::getInstance()->HeroAttack(cfgId);
           if (attackResult == -1)
           {
               BattleFinish(true);

           }else if(attackResult > 0)
           {
               BufferEffect(attackResult);
           }

           if (!BattleData::getInstance()->m_HeroAutoAttack)
           {
               m_RootWidget->unschedule(pbName);
           }
       });
    
    Btn_Hero->addClickEventListener([this, progressBar, pbName](Ref* sender)
        {
            if(progressBar->GetRemainTime() == progressBar->GetTotalTime())
            {
                m_RootWidget->schedule([this, progressBar](float f)
                   {
                       progressBar->Update(f);
                       
                   }, pbName);
            }
        });
    
    m_RootWidget->schedule([this, progressBar](float f)
       {
           progressBar->Update(f);
           
       }, pbName);
    
    if (BattleData::getInstance()->m_HeroAutoAttack)
    {
        progressBar->Resume();
    }
    
    UIUtils::getInstance()->showProperImg(Btn_Hero, cfgId);
    
}

Armature* BattleUI::CreateHeroArmature(int cfgId)
{
    if(ArmatureDataManager::getInstance()->getAnimationData("Texiao") == nullptr)
    {
        CCLOGERROR("not Armature:%s", "Texiao");
    }
    auto layout_Effect = m_RootWidget->getChildByName<Layout*>("Layout_Effect");
    Armature* armature_Hero = Armature::create("Texiao");
    
    armature_Hero->setVisible(false);
    
    armature_Hero->getAnimation()->setMovementEventCallFunc(
    [this, cfgId](Armature *armature, MovementEventType type, const std::string& id)
    {
        if (type == MovementEventType::COMPLETE)
        {
            armature->setVisible(false);
            armature->getAnimation()->resume();
        }
    });
    
    auto visible = Director::getInstance()->getVisibleSize();
    armature_Hero->setPosition(visible/2);
    layout_Effect->addChild(armature_Hero);
    return armature_Hero;
}

void BattleUI::InitMonsterUI(int cfgId)
{
    
    auto layout_Top = m_RootWidget->getChildByName<Layout*>("Layout_Top");
    
    auto text_Name = layout_Top->getChildByName<Text*>("Text_Name");
    auto text_Desc = layout_Top->getChildByName<Text*>("Text_Desc");
    
    auto text_Hp = layout_Top->getChildByName<Text*>("Text_Hp");

    auto lbar_MonsterHp = layout_Top->getChildByName<LoadingBar*>("LBar_Hp");
    lbar_MonsterHp->setPercent(100);

    auto Lbar_MonsterSkill = layout_Top->getChildByName<LoadingBar*>("LBar_Skill");
    Lbar_MonsterSkill->setPercent(100);
    
    CfgDataRow monsterDataRow(cfgId);
    auto langId = monsterDataRow.GetValue(CfgField::Name)->asInt();
    auto langText = MultiLanguage::getInstance()->GetText(langId);
    text_Name->setString(langText);
    
    langId = monsterDataRow.GetValue(CfgField::MonsterDescribe)->asInt();
    langText = MultiLanguage::getInstance()->GetText(langId);
    text_Desc->setString(langText);
    
    auto monsterHp = monsterDataRow.GetValue(CfgField::Hp)->asInt();
    
    auto buildId = BattleData::getInstance()->m_BuildId;
    //竞技场怪物
    if (buildId == (int)CfgIdEnum::EternalArena)
    {
        auto level = BattleData::getInstance()->m_NextBuildLayer -1;
        auto hpExt = monsterDataRow.GetValue(CfgField::HpCoefficient_F)->asFloat();
        monsterHp += (int)(monsterHp * hpExt * level + 0.5);
    }
    

    text_Hp->setString(StringUtils::format("%d/%d", monsterHp, monsterHp));
    
    auto attack_Speed = monsterDataRow.GetValue(CfgField::Spd_F)->asFloat();
    attack_Speed = 10 / attack_Speed;
    
    
    Armature* armture = CreateMonsterArmature(cfgId);
    string pbName = "MonsterSkillPbSchedule";
    
    auto progressBar = new ProgressBar();
    
    Lbar_MonsterSkill->setUserData(progressBar);
    
    progressBar->SetScheduleName(pbName);
    progressBar->SetTotalTime(attack_Speed);
    progressBar->SetLoadingBar(Lbar_MonsterSkill);
    
    m_VecProgressBar->push_back(progressBar);
    
    progressBar->SetCallBack([=]()
     {
         progressBar->Restart();
         
         if(m_IsPlayAnim) armture->getAnimation()->play(m_Attack);
         
         auto bf = BattleData::getInstance()->MonsterAttack();
         if (bf)
         {
             BattleFinish(false);
         }
     });
    
    m_RootWidget->schedule([this, progressBar](float f)
       {
           progressBar->Update(f);
       }, pbName);

    
    
    auto proType = monsterDataRow.GetValue(CfgField::PropertyID)->asInt();
    ImageView* img_Pro = layout_Top->getChildByName<ImageView*>(StringUtils::format("Img_Pro%d", proType));
    if (img_Pro)    img_Pro->setVisible(true);
    
    BattleData::getInstance()->m_MonsterId = cfgId;
    BattleData::getInstance()->m_MonsterMaxHp = monsterHp;
    BattleData::getInstance()->m_MonsterNowHp = monsterHp;
    BattleData::getInstance()->m_TextMonsterHp =  text_Hp;
    BattleData::getInstance()->m_MonsterHpLBar = lbar_MonsterHp;
    BattleData::getInstance()->m_MonSterProgressBar = progressBar;
}


Armature* BattleUI::CreateMonsterArmature(int cfgId)
{
    
    auto layout_Effect = m_RootWidget->getChildByName<Layout*>("Layout_Effect");
    CfgDataRow dataRow(cfgId);
    string armatureName = dataRow.GetValue(CfgField::MonsterIconName_S)->asString();
    
    m_MonsterArmatureFileInfo = armatureName + ".ExportJson";

    ArmatureDataManager::getInstance()->addArmatureFileInfo(m_MonsterArmatureFileInfo);
    
    if(ArmatureDataManager::getInstance()->getAnimationData(armatureName) == nullptr)
    {
        CCLOGERROR("Armature:%s", armatureName.c_str());
    }
    
    m_MonsterArmature = Armature::create(armatureName);
    m_MonsterArmature->getAnimation()->setMovementEventCallFunc(
       [this, cfgId](Armature *armature_Monster,MovementEventType type, const std::string& name)
       {
           if (type == MovementEventType::COMPLETE)
           {
               if (name == m_Attack)
               {
                    armature_Monster->getAnimation()->play(m_Stay);
               }
               if (name == m_Dead)
               {
                   UnLoad();
                   BattleFinishUI::getInstance()->Load(m_RootLayer, "BattleFinishUI.csb");
                   BattleFinishUI::getInstance()->Show();
                   
               }
           }
       });
    
    if (m_MonsterArmature->getAnimation()->getAnimationData()->getMovement("Stay") == nullptr)
    {
        CCLOGERROR("Armature:%s, not:%s", m_MonsterArmature->getName().c_str(), "Stay");
    }
    
    m_MonsterArmature->getAnimation()->play(m_Stay);
    auto visible = Director::getInstance()->getVisibleSize();
    
    visible = visible/ 2 ;
    m_MonsterArmature->setPosition(Size(visible.width, visible.height + 30));
    layout_Effect->addChild(m_MonsterArmature);
    return m_MonsterArmature;
}


//保存永恒竞技场最高层次
void SaveEternalArenaMaxLevel(int buildId, int level)
{
    if (buildId == (int)CfgIdEnum::EternalArena)
    {
        auto maxLevel = UserData::getInstance()->GetUserData(UserDataEnum::EternalArenaMaxLevel)->asInt();
//        if (maxLevel < level)
        if(true)
        {
            Network::getInstance()->UpdateAreanVal(level);
            UserData::getInstance()->Update_Value(UserDataEnum::EternalArenaMaxLevel, Value(level) );
            
        }
    }
}


//战斗结束
void BattleUI::BattleFinish(bool success)
{
    if (m_RootWidget != nullptr)
    {
        m_RootWidget->unscheduleAllCallbacks();
    }
    
    if (success)
    {
        if (m_MonsterArmature != nullptr)
        {
            m_MonsterArmature->getAnimation()->play(m_Dead);
        }
        m_MonsterArmature = nullptr;
        
        auto buildId = BattleData::getInstance()->m_BuildId;
        auto mosterId = BattleData::getInstance()->m_MonsterId;
        //怪物出现的层次
        auto buildLevel = BattleData::getInstance()->m_NextBuildLayer;
        
        SaveEternalArenaMaxLevel(buildId, buildLevel -1 );
        
        AchievementSystem::getInstance()->CheckAchievement(UserEventType::KillMonster, 1);
        
        AchievementSystem::getInstance()->CheckAchievement(UserEventType::DefeatMonster, mosterId);
        
        UserEventContainer::getInstance()->Send(UserEventType::BattleFinish, buildId, Value(success));
        
        SoundUtils::getInstance()->playMusicSound(MDEAD_EFFECT, false);
    }else
    {
        auto layout_Top = m_RootWidget->getChildByName<Layout*>("Layout_Top");
        auto lview_Buffer = layout_Top->getChildByName<ListView*>("LView_Buffer");
        for (auto layout : lview_Buffer->getChildren())
        {
            string name = layout->getName();
            if(name == "Buffer100801" || name == "Buffer100805")
            {
                auto bs = (BufferSchedule*)layout->getUserData();
                layout->unschedule( bs->m_ScheduleName ); delete bs;
            }
            
            if(name == "Buffer100802" || name == "Buffer100803")
            {
                if(layout->isVisible())
                {
                    auto bs = (BufferSchedule*)layout->getUserData();
                    layout->unschedule( bs->m_ScheduleName ); delete bs;
                }
            }
        }
        
        m_MonsterArmature = nullptr;
        
        HeroDeath();
        MapUI::getInstance()->UnLoad(); UnLoad();
        
        auto scene = MainScene::createScene();
        Director::getInstance()->replaceScene(CCTransitionFade::create(0.8f, scene, Color3B::BLACK));
    }

}


//死亡掉落物品 breadHeath:没面包 死亡
void BattleUI::HeroDeath(bool breadHeath)
{
    UserData::getInstance()->Update_Value(UserDataEnum::PlayerInMapTiledId, Value(-1));
    auto vm_BackPack = &UserData::getInstance()->GetUserData(UserDataEnum::BackPack)->asValueMap();
    auto vm_LiveHero = &UserData::getInstance()->GetUserData(UserDataEnum::LiveHero)->asValueMap();
    auto vm_DeathHero = &UserData::getInstance()->GetUserData(UserDataEnum::DeathHero)->asValueMap();
    
    vector<pair<int, int>> dropItem;
    vector<pair<int, int>> dropHero;
    for (auto iter = vm_BackPack->begin(); iter != vm_BackPack->end(); )
    {
        auto dataId = atoi(iter->first.c_str());
        auto dataNum = iter->second.asInt();
        if (dataId / 1000000 == 205) //英雄死亡
        {
            iter = vm_BackPack->erase(iter);

            dropHero.push_back(make_pair(dataId, dataNum));
            
            UserData::getInstance()->Update_VMI_Fun(vm_LiveHero, dataId, -dataNum);
            UserData::getInstance()->Update_VMI_Fun(vm_DeathHero, dataId, dataNum);
            
        }else
        {
            
            CfgDataRow dataRow(dataId);
            if(dataRow.GetValue(CfgField::DeadDrop)->asInt() == 1)
            {
                iter = vm_BackPack->erase(iter);
                dropItem.push_back(make_pair(dataId, dataNum));
                
            }else if(dataRow.GetValue(CfgField::BackItem)->asInt() == 1)
            {
                iter = vm_BackPack->erase(iter);
                
                dropItem.push_back(make_pair(dataId, dataNum));
                
                UserData::getInstance()->Update_VMI_Fun(UserDataEnum::Storehouse, dataId, dataNum);
            }
            else
            {
                iter++;
            }
        }
    }
//    UserData::getInstance()->SaveUserData();
    UserData::getInstance()->SaveCorUserData(false);

    string langText = "";
    if (breadHeath)
    {
        // MsgListNode::getInstance()->NewMsg(LanguageIdEnum::BreadHeathLangId);
        GoMainUITextInfo(&dropHero, LanguageIdEnum::BreadHeathLangId);
        // MsgListNode::getInstance()->NewMsg(LanguageIdEnum::LoseItems);
        GoMainUITextInfo(&dropItem, LanguageIdEnum::LoseItems);
        
    }else
    {
        // MsgListNode::getInstance()->NewMsg(LanguageIdEnum::BattleDeathHomeLangId);
        GoMainUITextInfo(&dropHero, LanguageIdEnum::BattleDeathHomeLangId);
        GoMainUITextInfo(&dropItem, LanguageIdEnum::LoseItems);
    }
    SoundUtils::getInstance()->playMusicSound(PDEAD_EFFECT, false);
}

//回城 Item To Storehouse
void BattleUI::TownPortal()
{
    UserData::getInstance()->Update_Value(UserDataEnum::PlayerInMapTiledId, Value(-1));
    auto vm_BackPack = &UserData::getInstance()->GetUserData(UserDataEnum::BackPack)->asValueMap();
    auto vm_Storehouse = &UserData::getInstance()->GetUserData(UserDataEnum::Storehouse)->asValueMap();
    vector<pair<int, int>> getItem;
    for (auto iter = vm_BackPack->begin(); iter != vm_BackPack->end(); )
    {
        auto dataId = atoi(iter->first.c_str());
        auto dataNum = iter->second.asInt();
        if (dataId / 1000000 == 205)
        {
            iter++; continue;
        }

        CfgDataRow dataRow(dataId);
        if(dataRow.GetValue(CfgField::BackItem)->asInt() == 1)
        {
            iter = vm_BackPack->erase(iter);
            getItem.push_back(make_pair(dataId, dataNum));
            UserData::getInstance()->Update_VMI_Fun(vm_Storehouse, dataId, dataNum);
        }else
        {
            iter++;
        }
    }
//    UserData::getInstance()->SaveUserData();
    UserData::getInstance()->SaveCorUserData(false);
    if (getItem.size() != 0) {
        // MsgListNode::getInstance()->NewMsg(LanguageIdEnum::BattleLiveHomeLangId);
        GoMainUITextInfo(&getItem, LanguageIdEnum::BattleLiveHomeLangId);
    }
    
}

//战斗回城信息文本
void BattleUI::GoMainUITextInfo(vector<pair<int, int>>* dropOut, LanguageIdEnum langIdEnum)
{
    auto dropOutNum = dropOut->size();
    if (dropOutNum == 0) return;
    
    auto idx = 0;
    int langId = 0;
    string langText = "";
    for (auto data : *dropOut)
    {
        idx ++;
        CfgDataRow dataRow(data.first);
        langId = dataRow.GetValue(CfgField::Name)->asInt();
        auto itemName = MultiLanguage::getInstance()->GetText(langId);
        
        if(idx == dropOutNum)
        {
            langText += StringUtils::format("%sx%d", itemName.c_str(), data.second);
        }else
        {
            langText += StringUtils::format("%sx%d,", itemName.c_str(), data.second);
        }
        
        UserEventContainer::getInstance()->Send(UserEventType::GetItemEvent, data.first);
    }
    string str = "";
    if ((int)langIdEnum != 0) {
        str = MultiLanguage::getInstance()->GetText((int)langIdEnum);
    }
    
    MsgListNode::getInstance()->NewMsg(str + langText);
}


void BattleUI::BufferEffect(int bufferId)
{
    
    CfgDataRow dataRow(bufferId);
    auto talentHit = BattleData::getInstance()->m_TalentHit;
    CfgDataRow monsterDataRow(BattleData::getInstance()->m_MonsterId);
    auto monsterEvasion = monsterDataRow.GetValue(CfgField::Evasion)->asInt();
    auto bufferHit = dataRow.GetValue(CfgField::BuffHit)->asInt();
    
    auto rvHit = (bufferHit + talentHit - monsterEvasion);
    auto rv = RandomEngine::getInstance()->GetDefaultRandomValue(1, 100);
    if (rv > rvHit) return; //判断buffer 是否命中
    
    auto layout_Top = m_RootWidget->getChildByName<Layout*>("Layout_Top");
    auto lview_Buffer = layout_Top->getChildByName<ListView*>("LView_Buffer");
    
    auto layout_Template = m_RootWidget->getChildByName<Layout*>("Template");
    
    auto bufferType = dataRow.GetValue(CfgField::Bufftype)->asInt();
    
    switch (bufferType)
    {
        case (int)BufferCfgId::PersistDamage: //持续伤害
        {
            auto buffer_Template = layout_Template->getChildByName<Layout*>("Buffer100801");
            auto layout_Buffer = (Layout*)buffer_Template->clone();
            auto text_Time = layout_Buffer->getChildByName<Text*>("Text_Time1");
            
            auto bs = new BufferSchedule();
            layout_Buffer->setUserData(bs);
            
            bs->m_TimeText = text_Time;
            bs->m_LayoutBuffer = layout_Buffer;
            bs->m_ScheduleName = GetNewScheduleName();
            bs->m_DelOrHidelLayoutBuffer = true;
            bs->m_TotalTime = dataRow.GetValue(CfgField::BuffDuration_F)->asFloat();
            bs->m_CbIntervalTime = dataRow.GetValue(CfgField::BuffInterval_F)->asFloat();
            bs->m_CallBack = ([=]()
               {
                   BattleData::getInstance()->BufferAttack(bufferId);
               });
            
            layout_Buffer->schedule([bs](float t)
            {
                bs->Update(t);
                
            }, bs->m_ScheduleName);
            
            layout_Buffer->setVisible(true);
            lview_Buffer->addChild(layout_Buffer);
            
            break;
        }
        case (int)BufferCfgId::ReduceDamage: //怪物中这个buffer 怪物攻击减少
        {
            auto layout_Buffer = (Layout*)lview_Buffer->getChildByName("Buffer100802");
            auto text_Time = layout_Buffer->getChildByName<Text*>("Text_Time2");
            
            auto bufferVal = dataRow.GetValue(CfgField::BuffMitigation)->asInt();
            
            BattleData::getInstance()->m_BufferReduceDamage = bufferVal;
            
            if(layout_Buffer->isVisible())
            {
                layout_Buffer->unschedule("BufferSchedule");
            }
            
            BufferSchedule* bs = new BufferSchedule();
            layout_Buffer->setUserData(bs);
            
            bs->m_TimeText = text_Time;
            bs->m_LayoutBuffer = layout_Buffer;
            bs->m_ScheduleName = "BufferSchedule";
            bs->m_DelOrHidelLayoutBuffer = false;
            bs->m_TotalTime = dataRow.GetValue(CfgField::BuffDuration_F)->asFloat();
            bs->m_CbIntervalTime = bs->m_TotalTime;
            bs->m_CallBack = ([=]()
              {
                  BattleData::getInstance()->m_BufferReduceDamage = 0;
              });
            
            layout_Buffer->schedule([=](float t)
            {
                bs->Update(t);
                
            }, bs->m_ScheduleName);

            lview_Buffer->setVisible(true);
            
            break;
        }
        case (int)BufferCfgId::Dizziness: //眩晕
        {
            auto Lbar_MonsterSkill = layout_Top->getChildByName<LoadingBar*>("LBar_Skill");
            auto pb = (ProgressBar*)Lbar_MonsterSkill->getUserData();
            pb->AddRemainTime(dataRow.GetValue(CfgField::BuffDuration_F)->asFloat());
            
            auto layout_Buffer = (Layout*)lview_Buffer->getChildByName("Buffer100803");
            
            auto text_Time = layout_Buffer->getChildByName<Text*>("Text_Time3");
            
            if(layout_Buffer->isVisible())
            {
                layout_Buffer->unschedule("BufferSchedule");
            }
            
            BufferSchedule* bs = new BufferSchedule();
            layout_Buffer->setUserData(bs);
            
            bs->m_TimeText = text_Time;
            bs->m_LayoutBuffer = layout_Buffer;
            bs->m_ScheduleName = "BufferSchedule";
            bs->m_DelOrHidelLayoutBuffer = false;
            bs->m_TotalTime = dataRow.GetValue(CfgField::BuffDuration_F)->asFloat();
            bs->m_CbIntervalTime = bs->m_TotalTime;

            bs->m_CallBack = ([=](){ /*delete bs;*/ });
            
            layout_Buffer->schedule([=](float t)
                {
                    bs->Update(t);
                }, bs->m_ScheduleName);
            
            layout_Buffer->setVisible(true);
            break;
        }
        case (int)BufferCfgId::PersistRecover: //持续恢复
        {
            auto buffer_Template = layout_Template->getChildByName<Layout*>("Buffer100805");
            
            auto layout_Buffer = (Layout*)buffer_Template->clone();
            
            auto bs = new BufferSchedule();
            layout_Buffer->setUserData(bs);
            
            bs->m_LayoutBuffer = layout_Buffer;
            bs->m_ScheduleName = GetNewScheduleName();
            bs->m_DelOrHidelLayoutBuffer = true;
            bs->m_TotalTime = dataRow.GetValue(CfgField::BuffDuration_F)->asFloat();
            bs->m_CbIntervalTime = dataRow.GetValue(CfgField::BuffInterval_F)->asFloat();
            bs->m_CallBack = ([=]()
              {
                  BattleData::getInstance()->BufferAttack(bufferId);
              });
            
            layout_Buffer->schedule([=](float t)
            {
                bs->Update(t);
            }, bs->m_ScheduleName);
            
            lview_Buffer->addChild(layout_Buffer);
            
            break;
        }
    }
    
}

//获取一个新回调名
string BattleUI::GetNewScheduleName()
{
    return StringUtils::format("BufferSchedule%d",m_ScheduleNameIdx++);
}

void BattleUI::ShowAttackText(int attackVal, AttackType attackType)
{
    if (attackType == AttackType::HeroAttack || attackType == AttackType::HeroProAttack) {
        SoundUtils::getInstance()->playMusicSound(PATTACK_EFFECT, false);
    }
    else if (attackType == AttackType::MonsterAttack || attackType == AttackType::MonsterProAttack) {
        SoundUtils::getInstance()->playMusicSound(MATTACK_EFFECT, false);
    }
    
    if( attackVal == 0 && attackType != AttackType::HeroNotHit && attackType != AttackType::MonsterNotHit)
    {
        attackVal = 1;
    }
    MoveText::TextData textData;
    textData.ParenNode = m_RootWidget;
    textData.TextInfo = StringUtils::format("%d", attackVal);
    textData.isBattleText = true;
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    switch (attackType)
    {
        case AttackType::HeroNotHit:
        {
            textData.TextInfo = "Miss";
            textData.FontSize = 24;
            textData.Color = Color3B::WHITE;
            textData.Direction = MoveText::Direction::Down;
            textData.PositionX = visibleSize.width /2 ;textData.PositionY = visibleSize.height - 200;
            break;
        }
        case AttackType::HeroAttack :
        {
            textData.FontSize = 24;
            textData.Color = Color3B::RED;
            textData.Direction = MoveText::Direction::Down;
            textData.PositionX = visibleSize.width /2 ;textData.PositionY = visibleSize.height - 200;
            break;
        }
        case AttackType::HeroProAttack :
        {
            textData.FontSize = 30;
            textData.Color = Color3B::RED;
            textData.Direction = MoveText::Direction::Down;
            textData.PositionX = visibleSize.width /2 ;textData.PositionY = visibleSize.height - 250;
            break;
        }
        case AttackType::HeroHealAttack :
        {
            textData.FontSize = 24;
            textData.Color = Color3B::GREEN;
            textData.Direction = MoveText::Direction::Up;
            textData.PositionX = visibleSize.width /2 ;textData.PositionY = 280;
            break;
        }
        case AttackType::MonsterNotHit :
        {
            textData.TextInfo = "Miss";
            textData.FontSize = 24;
            textData.Color = Color3B::WHITE;
            textData.Direction = MoveText::Direction::Up;
            textData.PositionX = visibleSize.width /2 ;textData.PositionY = 280;
            break;
        }
        case AttackType::MonsterAttack :
        {
            textData.FontSize = 24;
            textData.Color = Color3B::WHITE;
            textData.Direction = MoveText::Direction::Up;
            textData.PositionX = visibleSize.width /2 ;textData.PositionY = 280;
            break;
        }
        case AttackType::MonsterProAttack :
        {
            textData.FontSize = 30;
            textData.Color = Color3B::WHITE;
            textData.Direction = MoveText::Direction::Up;
            textData.PositionX = visibleSize.width /2 ;textData.PositionY = 280;
            break;
        }
    }
    MoveText::getInstance()->AddTextData(textData);
}

