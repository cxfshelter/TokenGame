//
//  DiamondBuyData.cpp
//  TokenGame
//
//  Created by best on 15/6/29.
//
//

#include "DiamondBuyData.h"
#include "CfgData.h"
#include "CfgDataRow.h"
#include "SettingData.h"
#include "UserData.h"
#include "MakeBuildUI.h"
#include "ProductionItem.h"
#include "AchievementSystem.h"
#include "DiamondBuyScene.h"
#include "DataEncryptUtils.h"
#include "DiamondBuyUI.h"

DiamondBuyData * DiamondBuyData::sm_Instance = nullptr;
DiamondBuyData * DiamondBuyData::getInstance()
{
    if (sm_Instance == nullptr) {
        sm_Instance = new (std::nothrow) DiamondBuyData();
    }
    
    return sm_Instance;
}

void DiamondBuyData::init()
{
    bool isSave = false;
    if (m_DiamondDataVm == nullptr) {
        m_DiamondDataVm = new (std::nothrow) ValueMap();
    }
    
    auto diamondTableList = CfgData::getInstance()->GetCfgTableId("DiamondItem");
    int cfgId = 0;
    for (auto iter = diamondTableList->begin(); iter != diamondTableList->end(); ++iter)
    {
        /*  进度通用性与类型：
                    1 - int（进度通用则存储是否购买, 1为已购买，0为未购买）
                    0 - map（进度不可通用则存储map，[进度, 购买次数]，进度：1或2或3）
         */
        cfgId = *iter;
        if (GetDiamondData(cfgId) == nullptr) {
            CfgDataRow itemRow(cfgId);
            if (itemRow.GetValue(CfgField::GenericID)->asInt() == 1) {
                m_DiamondDataVm->insert(pair<string, Value>(StringUtils::format("%d", *iter), Value(0)));
            }
            else {
                m_DiamondDataVm->insert(pair<string, Value>(StringUtils::format("%d", *iter), Value(ValueMap())));
            }
            
            isSave = true;
        }
    }
    
    if (isSave) SaveDiamondData();
}

bool DiamondBuyData::LoadDiamondData()
{
    // 因为独立区分开存档，故并没加存档路径
    m_DiamondDataDir = FileUtils::getInstance()->getWritablePath() + "AppData";
//    m_DiamondDataPath = m_DiamondDataDir + "/DiamondData.txt";
    m_DiamondDataPath = m_DiamondDataDir + "/" + DataEncryptUtils::getInstance()->encode("DiamondData.txt");
    
    if ( !FileUtils::getInstance()->isDirectoryExist(m_DiamondDataDir)) {
        FileUtils::getInstance()->createDirectory(m_DiamondDataDir);
    }
    
    m_DiamondDataVm = new (std::nothrow) ValueMap();
    *m_DiamondDataVm = FileUtils::getInstance()->getValueMapFromFile(m_DiamondDataPath);
    init();
    
    MakeComEffectx();
    return true;
}

bool DiamondBuyData::SaveDiamondData()
{
    if (m_DiamondDataVm == nullptr || m_DiamondDataPath.length() <= 0) {
        return false;
    }
    
    FileUtils::getInstance()->writeToFile(*m_DiamondDataVm, m_DiamondDataPath);
    return true;
}

Value* DiamondBuyData::GetDiamondData(int itemID)
{
    Value* result = nullptr;
    string key = StringUtils::format("%d", itemID);
    auto iter = m_DiamondDataVm->find(key);
    if (iter != m_DiamondDataVm->end()) {
        result = &iter->second;
    }
    
    return result;
}

int DiamondBuyData::GetProduceShowID()
{
    Value * result = nullptr;
    for (int iterID : m_EnumProduceID)
    {
        result = GetDiamondData(iterID);
        if (result == nullptr || result->asInt() == 1) continue;
        if (result->asInt() == 0) {
            return iterID;
        }
    }
    
    return -1;
}

bool DiamondBuyData::BuyEvent(int cfgID)
{
    CfgDataRow data(cfgID);
    int costNum = data.GetValue(CfgField::PriceID_M)->asIntMultiKeyMap().begin()->second.asInt();
    if (!costDiamond(costNum)) return false;
    
    int isCom = data.GetValue(CfgField::GenericID)->asInt();
    Value * oriVal = DiamondBuyData::getInstance()->GetDiamondData(cfgID);
    if (isCom == 1) {
        *oriVal = Value(1);
    }
    else {
        string key = SettingData::getInstance()->GetSettingData(SettingDataEnum::CurFileDocName)->asString();
        auto oriMap = &oriVal->asValueMap();
        auto iter = oriMap->find(key);
        if (iter == oriMap->end()) {
            oriMap->insert(pair<string, Value>(key, Value(1)));
        }
        else {
            int oriNum = iter->second.asInt();
            iter->second = Value(++oriNum);
        }
    }
    SaveDiamondData();
    
    MakeEffect(cfgID);
    return true;
}

bool DiamondBuyData::costDiamond(int costNum)
{
    // best test
//    return true;
    
    int curNum = UserData::getInstance()->Update_VMI_Fun(UserDataEnum::Storehouse, (int)CfgIdEnum::GemId, -costNum);
    if (curNum < 0)
    {
        // 钻石不足，跳到钻石充值界面
        return false;
    }
    
    return true;
}

void DiamondBuyData::goToTopUp(string oriUIName)
{
    m_isShowTopUp = true;
    m_oriUIName = oriUIName;
    auto scene = DiamondBuyScene::createScene();
    Director::getInstance()->replaceScene(CCTransitionMoveInL::create(0.18f, scene));
    RecordUtils::getInstance()->sdkRecord((int)umengEventID::Open_TopUp, "非点击进入");
}

void DiamondBuyData::androidPaySuc(int feeCode)
{
	int goldNum = 0;
	switch((BuyProductID)feeCode)
	{
		case BuyProductID::BuyID_First:
		{
//			if (std::atoi(UserData::getInstance()->Get_MVI_Value(UserDataEnum::FirstTopUp, 1, true).asString().c_str()) >= 1) {
//				goldNum = 100;
//			}
//			else
//			{
//				// 首次充值
//				goldNum = 100 * 2;
//				UserData::getInstance()->Update_VMV_Encode(UserDataEnum::FirstTopUp, 1, 1);
//			}
			goldNum = 100;
		}
		break;
		case BuyProductID::BuyID_Second:
		{
//			if (std::atoi(UserData::getInstance()->Get_MVI_Value(UserDataEnum::FirstTopUp, 2, true).asString().c_str()) >= 1) {
//				goldNum = 300;
//			}
//			else
//			{
//				// 首次充值
//				goldNum = 300 * 2;
//				UserData::getInstance()->Update_VMV_Encode(UserDataEnum::FirstTopUp, 2, 1);
//			}
			goldNum = 320;
		}
		break;
		case BuyProductID::BuyID_Third:
		{
//			if (std::atoi(UserData::getInstance()->Get_MVI_Value(UserDataEnum::FirstTopUp, 3, true).asString().c_str()) >= 1) {
//				goldNum = 1000;
//			}
//			else
//			{
//				// 首次充值
//				goldNum = 1000 * 2;
//				UserData::getInstance()->Update_VMV_Encode(UserDataEnum::FirstTopUp, 3, 1);
//			}
			goldNum = 550;
		}
		break;
		case BuyProductID::BuyID_Fourth:
		{
			if (std::atoi(UserData::getInstance()->Get_MVI_Value(UserDataEnum::FirstTopUp, 3, true).asString().c_str()) >= 1) {
				// 按道理不应该出现这里，因为超值礼包只能买一次
			}
			else
			{
				// 首次充值
				goldNum = 5;
				UserData::getInstance()->Update_VMI_Fun(UserDataEnum::Storehouse, (int)CfgIdEnum::GoldId, 1000);
				UserData::getInstance()->Update_VMV_Encode(UserDataEnum::FirstTopUp, 4, 1);
			}

		}
		break;
	}
	UserData::getInstance()->Update_VMI_Fun(UserDataEnum::Storehouse, (int)CfgIdEnum::GemId, goldNum);
	DiamondBuyUI::getInstance()->showTopUpLayout();
}

bool DiamondBuyData::isProduceID(int itemID)
{
    for (int iterID : m_EnumProduceID)
    {
        if (iterID == itemID) {
            return true;
        }
    }
    
    return false;
}

void DiamondBuyData::MakeEffect(int itemID)
{
    CfgDataRow dataRow(itemID);
    auto functionMap = dataRow.GetValue(CfgField::FunctionID_M)->asIntMultiKeyMap();
    auto numVector = dataRow.GetValue(CfgField::Numbe_V)->asValueVector();
    int idx = 0;
    // 是否进度通用
    bool isCom = dataRow.GetValue(CfgField::GenericID)->asInt() == 1;
    for (auto iter = functionMap.begin(); iter != functionMap.end(); ++iter) {
        int cfgID = iter->second.asInt();
        int addNum = numVector[idx++].asInt();
        switch (iter->first) {
            case 100700010:     // 解锁地图
            {
                UserData::getInstance()->Update_VMI_Fun(UserDataEnum::ExploreMaps, cfgID, 0);
            }
                break;
            case 100700011:     // 获得建筑
            {
                UserData::getInstance()->Update_VMI_Fun(UserDataEnum::MakeBuild, cfgID, (int)MakeBuildUI::MakeBuildStatus::MakeFinish, true);
            }
                break;
            case 100700012:     // 获得科技
            {
                UserData::getInstance()->Add_VI_Fun(UserDataEnum::TalentList, cfgID);
                CfgDataRow talentDataRow(cfgID);
                auto langId = talentDataRow.GetValue(CfgField::Schedule)->asInt();
                auto langText = MultiLanguage::getInstance()->GetText(langId);
                MsgListNode::getInstance()->NewMsg( langText );
                AchievementSystem::getInstance()->CheckAchievement(UserEventType::StudyTalent, cfgID);
            }
                break;
            case 100700013:     // 增加离线生产时间
            {
                ProductionItem::getInstance()->setMaxSleepProductionTime(addNum);
            }
                break;
            case 100700014:     // 获得物品
            {
                UserData::getInstance()->Update_VMI_Fun(UserDataEnum::Storehouse, cfgID, addNum);
            }
                break;
            case 100700015:     // 获得兵种
            {
                UserData::getInstance()->Update_VMI_Fun(UserDataEnum::LiveHero, cfgID, addNum);
            }
                break;
            default:
                break;
        }
        
    }
}

void DiamondBuyData::MakeComEffectx()
{
    auto diamondTableList = CfgData::getInstance()->GetCfgTableId("DiamondItem");
    int cfgId = 0;
    for (auto iter = diamondTableList->begin(); iter != diamondTableList->end(); ++iter)
    {
        cfgId = *iter;
        CfgDataRow itemRow(cfgId);
        if (itemRow.GetValue(CfgField::GenericID)->asInt() != 1) continue;  // 非进度通用
        if (GetDiamondData(cfgId)->asInt() != 1) continue;                  // 还未购买
        
        // 检查其功能有没生效
        auto functionMap = itemRow.GetValue(CfgField::FunctionID_M)->asIntMultiKeyMap();
        auto numVector = itemRow.GetValue(CfgField::Numbe_V)->asValueVector();
        int idx = 0;
        for (auto iter = functionMap.begin(); iter != functionMap.end(); ++iter) {
            int itemID = iter->second.asInt();
            int addNum = numVector[idx++].asInt();
            switch (iter->first) {
                case 100700010:     // 解锁地图
                {
                    auto checkIter = UserData::getInstance()->Get_MVI_Value(UserDataEnum::ExploreMaps, itemID);
                    if (checkIter == Value::Null) {
                        UserData::getInstance()->Update_VMI_Fun(UserDataEnum::ExploreMaps, itemID, 0);
                    }
                }
                    break;
                case 100700011:     // 获得建筑
                {
                    auto checkIter = UserData::getInstance()->Get_MVI_Value(UserDataEnum::MakeBuild, itemID);
                    if (checkIter == Value::Null) {
                        UserData::getInstance()->Update_VMI_Fun(UserDataEnum::MakeBuild, itemID, (int)MakeBuildUI::MakeBuildStatus::MakeFinish, true);
                    }
                }
                    break;
                case 100700012:     // 获得科技
                {
                    bool checkBool = UserData::getInstance()->Check_VI_Fun(UserDataEnum::TalentList, Value(itemID));
                    if (!checkBool) {
                        UserData::getInstance()->Add_VI_Fun(UserDataEnum::TalentList, itemID);
                        CfgDataRow talentDataRow(itemID);
                        auto langId = talentDataRow.GetValue(CfgField::Schedule)->asInt();
                        auto langText = MultiLanguage::getInstance()->GetText(langId);
                        MsgListNode::getInstance()->NewMsg( langText );
                        AchievementSystem::getInstance()->CheckAchievement(UserEventType::StudyTalent, itemID);
                    }
                }
                    break;
                case 100700013:     // 增加离线生产时间
                {
                    ProductionItem::getInstance()->setMaxSleepProductionTime(addNum);
                }
                    break;
                default:
                    break;
            }
        }
        
    }
}




