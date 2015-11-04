//
//  BattleFinishUI.cpp
//  TokenGame
//
//  Created by token on 15/3/20.
//
//

#include "MapUI.h"
#include "BattleData.h"
#include "TiledMapUI.h"
#include "BattleFinishUI.h"
#include "RandomEngine.h"

BattleFinishUI* BattleFinishUI::sm_BattleFinishUI = nullptr;

BattleFinishUI* BattleFinishUI::getInstance()
{
    if (sm_BattleFinishUI == nullptr)
    {
        sm_BattleFinishUI = new (std::nothrow) BattleFinishUI();
        if (sm_BattleFinishUI->Init()) {}
    }
    return sm_BattleFinishUI;
}

bool BattleFinishUI::Init()
{
    return true;
}

void BattleFinishUI::UnLoad()
{
    MsgListNode::getInstance()->UnLoadMsg();
    MoveText::getInstance()->ClearTextData();
    TiledMapUI::getInstance()->SetTouchEnabled(true);
    delete m_BagItem; delete m_DropItem; UIBase::UnLoad();
}


void BattleFinishUI::UpdateUI()
{

    auto layout_Center = m_RootWidget->getChildByName<Layout*>("Layout_Center");
    auto layout_Head = layout_Center->getChildByName<Layout*>("Layout_Head");
    
    auto text_BagSpace = layout_Head->getChildByName<Text*>("Text_BagSpace");
    auto langId = (int)LanguageIdEnum::BagSpaceLangId;
    auto langText = MultiLanguage::getInstance()->GetText(langId);
    text_BagSpace->setString(langText);
    
    auto text_BagText = layout_Head->getChildByName<Text*>("Text_BagText");
    langId = (int)LanguageIdEnum::BagNameLangId;
    langText = MultiLanguage::getInstance()->GetText(langId);
    text_BagText->setString(langText);
    
    auto text_CollectText = layout_Head->getChildByName<Text*>("Text_CollectText");
    langId = (int)LanguageIdEnum::DropOutLangId ;
    langText = MultiLanguage::getInstance()->GetText(langId);
    text_CollectText->setString(langText);

    RandomDropOut();
}

void BattleFinishUI::RandomDropOut()
{
    m_BagItem = new (std::nothrow) vector<pair<int, int>>();
    m_DropItem = new (std::nothrow) vector<pair<int, int>>();
    
    auto buildId = BattleData::getInstance()->m_BuildId;
    auto monsterId = BattleData::getInstance()->m_MonsterId;
    auto monsterLevel = BattleData::getInstance()->m_NextBuildLayer -1;
    
    CfgDataRow dataRow(monsterId);
    
    auto minDI = dataRow.GetValue(CfgField::MinItemnum)->asInt();
    auto maxDI = dataRow.GetValue(CfgField::MaxItemnum)->asInt();
    
    auto vv_dropItem = &dataRow.GetValue(CfgField::DropItemID_V)->asValueVector();
    
    
    int cfgDINum = (int)vv_dropItem->size();
    
    minDI = minDI < 0 ? 0 : minDI;
    
    maxDI = maxDI > cfgDINum ? maxDI : cfgDINum;
    
    auto nowDINum = RandomEngine::getInstance()->GetDefaultRandomValue(minDI, maxDI);
    
    nowDINum = nowDINum < minDI ? minDI : nowDINum;
    
    for (auto idx = 0; idx < nowDINum; idx++)
    {
        auto diId = vv_dropItem->at(idx).asInt();
        CfgDataRow diDataRow(diId);
        
        auto dp = diDataRow.GetValue(CfgField::DropPro)->asInt();
        
        auto rd = RandomEngine::getInstance()->GetDefaultRandomValue(1, 1000);
        
        if(rd < dp)
        {
            auto minVal = diDataRow.GetValue(CfgField::DropMin)->asInt();
            
            auto maxVal = diDataRow.GetValue(CfgField::DropMax)->asInt();
            
            rd = RandomEngine::getInstance()->GetDefaultRandomValue(minVal, maxVal);
            
            if (rd > 0)
            {
                auto itemId = diDataRow.GetValue(CfgField::ItemID)->asInt();
                
                if (buildId == (int)CfgIdEnum::EternalArena && itemId == (int)CfgIdEnum::GoldId)
                {
                    rd += (int)(1.3 * (monsterLevel - 1) + 0.5);
                }
                
                m_DropItem->push_back(pair<int, int>(itemId, rd));
            }
        }
    }
    if (m_DropItem->empty())
    {
        UnLoad();
        auto buildId = BattleData::getInstance()->m_BuildId;
        if (buildId > 0)
        {
            auto tiledIdx = BattleData::getInstance()->m_TiledIdx;
            auto nextBuildLayier = BattleData::getInstance()->m_NextBuildLayer;
            MapUI::getInstance()->ShowBattleDialog(tiledIdx, buildId, nextBuildLayier);
        }

    }else
    {
        auto bag_vm = &UserData::getInstance()->GetUserData(UserDataEnum::BackPack)->asValueMap();
        for (auto iter = bag_vm->begin(); iter != bag_vm->end(); iter++)
        {
            auto itemId = atoi(iter->first.c_str());
            if (itemId / 1000000 == 205) continue;
            m_BagItem->push_back(pair<int, int>(itemId,iter->second.asInt()));
        }
        
        ShowItemList(m_BagItem, true);
        ShowItemList(m_DropItem, false);
    }
    
    
}


void BattleFinishUI::UpdateBagItemNum()
{
    auto layout_Center = m_RootWidget->getChildByName<Layout*>("Layout_Center");
    auto layout_Head = layout_Center->getChildByName<Layout*>("Layout_Head");
    auto text_BagItemNum = layout_Head->getChildByName<Text*>("Text_BagItemNum");
    
    auto nowNum = GetBagItemNum();
    auto bag_maxNum = UserData::getInstance()->GetUserData(UserDataEnum::BackPackMaxNum)->asInt();
    text_BagItemNum->setString(StringUtils::format("%d/%d", nowNum, bag_maxNum));
    
}

//itemVect 物品 与数量 direction 左：true 右: false
void BattleFinishUI::ShowItemList(vector<pair<int, int>>* itemVect, bool direction)
{
    auto layout_Template = m_RootWidget->getChildByName<Layout*>("Layout_Template");
    auto layout_Center = m_RootWidget->getChildByName<Layout*>("Layout_Center");
    auto Layout_BtnItem = layout_Template->getChildByName<Layout*>("Layout_BtnItem");

    
    ListView* lview;
    if (direction)
    {
        lview = layout_Center->getChildByName<ListView*>("LView_Bag");
    }else
    {
        lview = layout_Center->getChildByName<ListView*>("LView_DropOut");
    }
    lview->removeAllChildren();
    for (auto iter = itemVect->begin(); iter != itemVect->end(); iter++)
    {
        auto itemId = iter->first;
        auto itemNum = iter->second;
        
        auto layout_BtnItem = Layout_BtnItem->clone();
        auto btn_Item = layout_BtnItem->getChildByName<Button*>("Btn_Item");
        btn_Item->addClickEventListener([this, direction, itemId](Ref* sender)
           {
               if (direction)
               {
                   // 不占据背包空间物品不能丢弃
                   if (CfgDataRow(itemId).GetValue(CfgField::NonSpace)->asInt() == 0) {
                       return;
                   }
                   
                   auto iter = m_BagItem->begin();
                   while (iter != m_BagItem->end())
                   {
                       if(iter->first == itemId)
                       {
                           break;
                       }
                       iter++;
                   }
                   if (iter != m_BagItem->end())
                   {
                       
                       iter->second -= 1;
                       if(iter->second == 0)
                       {
                           m_BagItem->erase(iter);
                       }
                       
                       iter = m_DropItem->begin();
                       while (iter != m_DropItem->end())
                       {
                           if(iter->first == itemId)
                           {
                               break;
                           }
                           iter++;
                       }
                       if(iter == m_DropItem->end())
                       {
                           m_DropItem->push_back(pair<int, int>( itemId, 1));
                       }else
                       {
                           iter->second += 1;
                       }
                   }
                   
               }else
               {
                   //判断是否金币
                   if (itemId == (int)CfgIdEnum::GoldId)
                   {
                       SaveGold();
                   }else
                   {
                       auto iter = m_DropItem->begin();
                       while (iter != m_DropItem->end())
                       {
                           if(iter->first == itemId)
                           {
                               break;
                           }
                           iter++;
                       }
                       if (iter != m_DropItem->end())
                       {
                           if (IsSaveBag(itemId))
                           {
                               iter->second -= 1;
                               if(iter->second == 0)
                               {
                                   m_DropItem->erase(iter);
                               }
                           }
                       }
                   }
               }
               ShowItemList(m_BagItem, true);
               ShowItemList(m_DropItem, false);
           });
        
        auto text_Name = layout_BtnItem->getChildByName<Text*>("Text_ItemName");
        CfgDataRow dataRow(itemId);
        auto langId = dataRow.GetValue(CfgField::Name)->asInt();
        auto langText = MultiLanguage::getInstance()->GetText(langId);
        text_Name->setString(langText);
        
        auto text_Num = layout_BtnItem->getChildByName<Text*>("Text_ItemNum");
        text_Num->setString(StringUtils::format("x%d", itemNum));
        lview->addChild(layout_BtnItem);
        UpdateBagItemNum();
    }
}


bool BattleFinishUI::IsSaveBag(int itemId)
{
    CfgDataRow itemRow(itemId);
    // 是否占据格子
    bool isOty = itemRow.GetValue(CfgField::NonSpace)->asInt() == 1;
    bool result = false;
    auto nowNum = GetBagItemNum();
    auto bag_maxNum = UserData::getInstance()->GetUserData(UserDataEnum::BackPackMaxNum)->asInt();
    if(nowNum < bag_maxNum || !isOty)
    {
        auto iter = m_BagItem->begin();
        while (iter != m_BagItem->end())
        {
            if(iter->first == itemId) break;
            iter ++;
        }
        
        if(iter == m_BagItem->end())
        {
            m_BagItem->push_back(pair<int, int>( itemId, 1));
        }else
        {
            iter->second += 1;
        }
        result = true;
    }
    else {
        auto langId = (int)LanguageIdEnum::BakcPackSpaceNotEnoughId;
        auto langText = MultiLanguage::getInstance()->GetText(langId);
        //背包 空间不够
        MoveText::getInstance()->AddDefaultTextData(m_RootWidget, langText);
    }
    return result;
}

void BattleFinishUI::SaveGold()
{
    auto goldNum = 0;
    auto goldId =(int)CfgIdEnum::GoldId;
    auto iter = m_DropItem->begin();
    while (iter != m_DropItem->end())
    {
        if(iter->first == goldId)
        {
            goldNum = iter->second;
            m_DropItem->erase(iter);
            break;
        }
        iter++;
    }
    UserData::getInstance()->Update_VMI_Fun(UserDataEnum::Storehouse, (int)CfgIdEnum::GoldId, goldNum);
    
    CfgDataRow dataRow((int)CfgIdEnum::GoldId);
    auto langId = dataRow.GetValue(CfgField::Name)->asInt();
    auto langText = MultiLanguage::getInstance()->GetText(langId);
    auto text = StringUtils::format("%s:%d", langText.c_str(), goldNum);
    MoveText::getInstance()->AddDefaultTextData(m_RootWidget, text);

}

//获取背包里物品数量
int BattleFinishUI::GetBagItemNum()
{
    int nowNum = 0;
    auto iter = m_BagItem->begin();
    while (iter != m_BagItem->end())
    {
        CfgDataRow bagItem(iter->first);
        if (bagItem.GetValue(CfgField::NonSpace)->asInt() == 1) {
            nowNum += iter->second;
        }
        ++iter;
    }
    return nowNum;
}

void BattleFinishUI::LoadFinish()
{
    TiledMapUI::getInstance()->SetTouchEnabled(false);
    MsgListNode::getInstance()->LoadMsg(m_RootWidget);
    
    auto layout_Bottom = m_RootWidget->getChildByName<Layout*>("Layout_Bottom");

    auto btn_Close = layout_Bottom->getChildByName<Button*>("Btn_Close");
    auto langId = (int)LanguageIdEnum::CloseLangId;
    auto langText = MultiLanguage::getInstance()->GetText(langId);
    btn_Close->setTitleText(langText);
    
    auto btn_Collect = layout_Bottom->getChildByName<Button*>("Btn_Collect");
    langId = (int)LanguageIdEnum::PickUpAllLangId;
    langText = MultiLanguage::getInstance()->GetText(langId);
    btn_Collect->setTitleText(langText);

    
    btn_Close->addClickEventListener([this](Ref* sender)
     {
         SaveBagItem();
         auto buildId = BattleData::getInstance()->m_BuildId;
         if (buildId > 0)
         {
             auto tiledIdx = BattleData::getInstance()->m_TiledIdx;
             auto nextBuildLayier = BattleData::getInstance()->m_NextBuildLayer;
             MapUI::getInstance()->ShowBattleDialog(tiledIdx, buildId, nextBuildLayier);
         }
         this->UnLoad();
     });
    
    btn_Collect->addClickEventListener([this](Ref* sender)
       {

           auto nowNum = GetBagItemNum();
           auto bag_maxNum = UserData::getInstance()->GetUserData(UserDataEnum::BackPackMaxNum)->asInt();
           auto saveItemNum = bag_maxNum - nowNum;
           
           auto iter = m_DropItem->begin();
           while (iter != m_DropItem->end())
           {
               int cfgId = iter->first;
               if (cfgId == (int)CfgIdEnum::GoldId)
               {
                   SaveGold(); continue;
               }
               
               CfgDataRow dropItem(cfgId);
               // 是否占据背包格子，1：占据
               bool isOcy = dropItem.GetValue(CfgField::NonSpace)->asInt() == 1;
               if (saveItemNum <= 0 && isOcy)
               {
                   auto langId = (int)LanguageIdEnum::BakcPackSpaceNotEnoughId;
                   auto langText = MultiLanguage::getInstance()->GetText(langId);
                   //背包 空间不够
                   MoveText::getInstance()->AddDefaultTextData(m_RootWidget, langText);
                   break;
               }

               auto bag_Iter = m_BagItem->begin();
               while (bag_Iter != m_BagItem->end())
               {
                   if (bag_Iter->first == iter->first)
                   {
                       break;
                   }
                   bag_Iter++;
               }
               
               if (bag_Iter == m_BagItem->end())
               {
                   //背包里没有这种物品
                   if (!isOcy) {
                       m_BagItem->push_back(pair<int, int>(cfgId, iter->second));
                       iter = m_DropItem->erase(iter);
                   }
                   else {
                       if (iter->second <= saveItemNum)
                       {
                           saveItemNum -= iter->second;
                           //背包空间不够保存一部分物品
                           m_BagItem->push_back(pair<int, int>(iter->first, iter->second));
                           iter = m_DropItem->erase(iter);
                       }else
                       {
                           m_BagItem->push_back(pair<int, int>(iter->first, saveItemNum));
                           iter->second -= saveItemNum; saveItemNum = 0;
                       }
                   }
                   
               }else
               {
                   //背包里有这种物品
                   if (!isOcy) {
                       bag_Iter->second += iter->second;
                       iter = m_DropItem->erase(iter);
                   }
                   else {
                       if (iter->second <= saveItemNum)
                       {
                           saveItemNum -= iter->second;
                           bag_Iter->second += iter->second;
                           iter = m_DropItem->erase(iter);
                       }else
                       {
                           bag_Iter->second += saveItemNum;
                           //背包空间不够保存一部分物品 删除保存到背包的物品
                           iter->second -= saveItemNum; saveItemNum = 0;
                       }
                   }
               }
            }
           
           ShowItemList(m_BagItem, true);
           ShowItemList(m_DropItem, false);
           
        });

}

void BattleFinishUI::SaveBagItem()
{
    auto bag_vm = &UserData::getInstance()->GetUserData(UserDataEnum::BackPack)->asValueMap();
    
    auto iter = m_BagItem->begin();
    while (iter != m_BagItem->end())
    {
        auto key = StringUtils::format("%d", iter->first);
        auto bag_iter = bag_vm->find( key);
        if (bag_iter == bag_vm->end())
        {
            bag_vm->insert(pair<string, Value>(key, Value(iter->second)));
        }else
        {
            bag_iter->second = Value(iter->second);
        }
        iter++;
    }
//    UserData::getInstance()->SaveUserData();
    UserData::getInstance()->SaveCorUserData(false);
    
}


