//
//  ProductionItem.cpp
//  TokenGame
//
//  Created by token on 15/4/19.
//
//

#include "MapUI.h"
#include "MainUI.h"
#include "ProductionItem.h"


ProductionItem* ProductionItem::sm_ProductionItem = nullptr;

ProductionItem* ProductionItem::getInstance()
{
    if (sm_ProductionItem == nullptr)
    {
        sm_ProductionItem = new (std::nothrow) ProductionItem();
        if (sm_ProductionItem->Init()) {}
    }
    return sm_ProductionItem;
}

bool ProductionItem::Init()
{
    return true;
}

void ProductionItem::setMaxSleepProductionTime(float newMaxTime)
{
    // 低于1小时或大于24小时退出
    if (newMaxTime < 60 * 60 || newMaxTime > 60 * 24) {
        return;
    }
    m_MaxSleepProductionTime = newMaxTime;
    
}

void ProductionItem::setCBIntervalTime(bool isBuild)
{
    if (isBuild) m_CbIntervalTime = 10;
}

//循环计算一个长时间段内所有生产数据
void ProductionItem::LoopProductionItem()
{
    if(!UserData::getInstance()->m_SleepStatus) return;
    
    UserData::getInstance()->m_SleepStatus = false;
    
    //最后一次生产时间
    auto lastProductionTime = UserData::getInstance()->GetUserData(UserDataEnum::LastProductionTime)->asInt();
    
    int nowTimeStamp = (int)time(nullptr);
    
//    m_MaxSleepProductionTime = 60 * 60; 一小时
    
    auto totalProductionTime = nowTimeStamp - lastProductionTime;
    if (totalProductionTime  > m_MaxSleepProductionTime)
    {
        totalProductionTime = m_MaxSleepProductionTime;
    }
    
    map<int, int> expendItems;
    ValueMapIntMultiKey vm_outputItems;

    auto ProductionItemNum =  totalProductionTime / m_CbIntervalTime; //
    
    m_SurplusIntervalTime = (int)totalProductionTime % (int)m_CbIntervalTime;
    
    auto st = time(nullptr);
    
    for (auto i = 0; i < ProductionItemNum; i++)
    {
        ComputeProductionItemData(&vm_outputItems, &expendItems, false);
    }
    
    auto et = time(nullptr);
    
    
    auto  t = st - et;
    
    CCLOGERROR("run time:%ld", t);
    
    //UserData::getInstance()->AddStorehouseData(&vm_outputItems);
    
    auto iter_out = vm_outputItems.begin();
    while (iter_out != vm_outputItems.end())
    {
        auto iter_expend = expendItems.find(iter_out->first);
        if (iter_expend != expendItems.end())
        {
            if (iter_out->second.asInt() - iter_expend->second < 0)
            {
                CCLOGERROR(" iter_out < iter_expend");
            }
            iter_out->second = Value(iter_out->second.asInt() - iter_expend->second);
            
            expendItems.erase(iter_expend);
        }
        iter_out++;
    }
    
    UserData::getInstance()->Update_Value(UserDataEnum::LastProductionTime, Value(nowTimeStamp));
    
    auto langId = (int)LanguageIdEnum::ProductionItemLangId;
    auto langText = MultiLanguage::getInstance()->GetText(langId);
    
    auto itemId = 0; auto itemNum = 0;
    for (auto iter : vm_outputItems)
    {
        itemId = iter.first;
        itemNum = iter.second.asInt();
        if (itemNum > 0)
        {
            langId = CfgData::getInstance()->GetValue(itemId, CfgField::Name)->asInt();
            langText += MultiLanguage::getInstance()->GetText(langId);
            langText = StringUtils::format("%s x%d ", langText.c_str(), itemNum);

        }
    }
    if (!vm_outputItems.empty())
    {
        MsgListNode::getInstance()->NewMsg(langText);
    }

}

//每20秒计算一次生产数据  outputItems:生产的物品 expendItems:消耗的物品
void ProductionItem::ComputeProductionItemData(ValueMapIntMultiKey* outputItems, map<int, int>* expendItems, bool isUpdateTime)
{
    ValueMapIntMultiKey* vm_outputItems;
    if (outputItems == nullptr)
    {
        ValueMapIntMultiKey vm;
        vm_outputItems = &vm;
    }else
    {
        vm_outputItems = outputItems;
    }
    
    auto itemId = 0; auto itemNum = 0;
    auto vm_Storehouse = &UserData::getInstance()->GetUserData(UserDataEnum::Storehouse)->asValueMap();
    auto vm_ProductionItem = &UserData::getInstance()->GetUserData(UserDataEnum::Production)->asValueMap();
    auto iter = vm_ProductionItem->begin();
    while (iter != vm_ProductionItem->end())
    {
        itemId = atoi(iter->first.c_str());
        itemNum = iter->second.asInt(); iter++;
        
        for (auto i = 0; i < itemNum; i++)
        {
            CfgDataRow dataRow(itemId);
            vector<pair<int, int>> lackData;
            auto imkm_ItemId = &dataRow.GetValue(CfgField::MaterialID_M)->asIntMultiKeyMap();
            bool delResutl = UserData::getInstance()->DeleteStorehouseData(imkm_ItemId, &lackData, false);
            
            if (!delResutl) break;
            if(expendItems != nullptr)
            {
                auto iter_Expend = imkm_ItemId->begin();
                while (iter_Expend != imkm_ItemId->end())
                {
                    auto iter = expendItems->find(iter_Expend->first);
                    if(iter!= expendItems->end())
                    {
                        iter->second += iter_Expend->second.asInt();
                    }
                    else
                    {
                        expendItems->insert(make_pair(iter_Expend->first, iter_Expend->second.asInt()));
                    }
                    iter_Expend++;
                }
            }
            
            auto outputItemNum = dataRow.GetValue(CfgField::Output)->asInt();
            
            auto output_iter = vm_outputItems->find(itemId);
            
            UserData::getInstance()->Update_VMI_Fun(vm_Storehouse, itemId, outputItemNum, false, true, false);
            
            if (output_iter != vm_outputItems->end())
            {
                output_iter->second = Value(output_iter->second.asInt() + outputItemNum);
                
            }else
            {
                vm_outputItems->insert(make_pair(itemId, Value(outputItemNum)));
            }
            

        }
        
    }
    
    if(isUpdateTime)
    {
        //UserData::getInstance()->AddStorehouseData(vm_outputItems);
        time_t timeStamp = time(nullptr); auto intTimeStamp = (int)timeStamp;
        UserData::getInstance()->Update_Value(UserDataEnum::LastProductionTime, Value(intTimeStamp));
    }
}


//设置生产调度结节
void ProductionItem::SetSchedule(Node* node, Text* text)
{
    LoopProductionItem();
    
    m_Node = node;
    if (text != nullptr)
    {
        text->setString(StringUtils::format("00:%.2d", (int)m_SurplusIntervalTime));
    }
    
    m_Node->schedule([=](float f)
       {
           m_SurplusIntervalTime -= f;
           if (text != nullptr)
           {
               text->setString(StringUtils::format("00:%.2d", (int)m_SurplusIntervalTime));
           }
           if (m_SurplusIntervalTime <= 0)
           {
               m_SurplusIntervalTime = m_CbIntervalTime;
               
               ValueMapIntMultiKey vm_outputItems;

               ComputeProductionItemData(&vm_outputItems);
               
               map<int, int>outItems;
               ComputeProductionItemList(&outItems);
               
               if (m_Node != MapUI::getInstance()->m_RootWidget)
               {
                   auto itemId = 0;
                   auto itemNum = 0;
                   for (auto iter : outItems)
                   {
                       itemId = iter.first;
                       itemNum = iter.second;
                       if (itemNum > 0)
                       {
                           if (m_Node == MainUI::getInstance()->m_RootWidget)
                           {
                               MainUI::getInstance()->UpdateStorehouse();
                           }
                           MoveText::getInstance()->GetResText(m_Node, itemId, itemNum);
                       }
                   }
               }
           }
           
       }, 0.5, m_ScheduleName);
}

//取消调度器
void ProductionItem::UnSchedule()
{
    if (m_Node != nullptr)
    {
        m_Node->unschedule(m_ScheduleName);
        MoveText::getInstance()->ClearTextData();
    }
}

//显示生产列表信息
void ProductionItem::ComputeProductionItemList(map<int, int>* outItems)
{
    //消耗的物品
    map<int, int> expendItems;
    //生产出来的物品
    map<int, int> ProductionItemList;
    
    auto itemId = 0; auto itemNum = 0;
    auto vm_ProductionItem = &UserData::getInstance()->GetUserData(UserDataEnum::Production)->asValueMap();
    auto iter = vm_ProductionItem->begin();
    while (iter != vm_ProductionItem->end())
    {
        itemId = atoi(iter->first.c_str());
        itemNum = iter->second.asInt(); iter++;
        
        //生产出来的物品
        CfgDataRow dataRow(itemId);
        auto outputItemNum = dataRow.GetValue(CfgField::Output)->asInt();
        auto iter_out = ProductionItemList.find(itemId);
        if (iter_out != ProductionItemList.end())
        {
            iter_out->second += itemNum * outputItemNum;
        }else
        {
            ProductionItemList.insert(make_pair(itemId, itemNum * outputItemNum));
        }
        
        //要消耗的物品
        auto imkm_ItemId = &dataRow.GetValue(CfgField::MaterialID_M)->asIntMultiKeyMap();
        auto iter_CfgItem = imkm_ItemId->begin();
        while (iter_CfgItem != imkm_ItemId->end())
        {
            auto expendNum = iter_CfgItem->second.asInt() * itemNum;
            auto iter_Expend = expendItems.find(iter_CfgItem->first);
            if (iter_Expend != expendItems.end())
            {
                iter_Expend->second += expendNum;
            }else
            {
                expendItems.insert(make_pair(iter_CfgItem->first, expendNum));
            }
            iter_CfgItem++;
        }

    }
    
    auto iter_out = ProductionItemList.begin();
    while (iter_out != ProductionItemList.end())
    {
        auto iter_expend = expendItems.find(iter_out->first);
        if (iter_expend != expendItems.end())
        {
            iter_out->second -= iter_expend->second;
            expendItems.erase(iter_expend);
        }
        outItems->insert(make_pair(iter_out->first, iter_out->second));
        iter_out++;
    }
    
    auto iter_Expend = expendItems.begin();
    while (iter_Expend != expendItems.end())
    {
        outItems->insert(make_pair(iter_Expend->first, -iter_Expend->second));
        iter_Expend++;
    }
    
}

