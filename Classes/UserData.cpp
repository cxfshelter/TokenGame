//
//  UserData.cpp
//  TokenGame
//
//  Created by token on 15/3/17.
//
//

#include "UserData.h"
#include "EnunClass.h"
#include "UserEventContainer.h"
#include "SettingData.h"
#include "DataEncryptUtils.h"

UserData* UserData::sm_UserData = nullptr;

UserData* UserData::getInstance()
{
    if (sm_UserData == nullptr)
    {
        sm_UserData = new (std::nothrow) UserData();
        if(sm_UserData->Init() == false)
        {
            
        }
        
    }
    return sm_UserData;
}

// 清空相关变量
void UserData::releaseData()
{
    if (m_UserDataVm != nullptr)
    {
        m_UserDataVm->clear();
        m_UserDataVm = nullptr;
    }
    
    if (m_UserMapVm != nullptr) {
        m_UserMapVm->clear();
        m_UserMapVm = nullptr;
    }
    
    delete sm_UserData;
    sm_UserData = nullptr;
}

void UserData::setRankName(string rankName)
{
    if (m_RankName.compare("") == 0) {
        m_RankName = rankName;
    }
    
}

const string UserData::getRankName()
{
    return m_RankName;
}

bool UserData::Init()
{
    if (m_UserDataVm == nullptr)
    {
        m_UserDataVm = new (std::nothrow) ValueMap();
        m_UserDataVm->reserve((int)UserDataEnum::EnumClassMaxVal - (int)UserMapDataEnum::EnumMapMaxVal);
    }
    
    if (m_UserMapVm == nullptr)
    {
        m_UserMapVm = new (std::nothrow) ValueMap();
        m_UserMapVm->reserve((int)UserMapDataEnum::EnumMapMaxVal);
    }
    
    if(GetUserData(UserDataEnum::UserId) == nullptr)
    {
        m_UserDataVm->insert(pair<string, Value>(getEnumEncodeString(UserDataEnum::UserId), Value("")));
    }
    
    if(GetUserData(UserDataEnum::UserName) == nullptr)
    {
        m_UserDataVm->insert(pair<string, Value>(getEnumEncodeString(UserDataEnum::UserName), Value("")));
    }
    
    if(GetUserData(UserDataEnum::BackPack) == nullptr)
    {
        m_UserDataVm->insert(pair<string, Value>(getEnumEncodeString(UserDataEnum::BackPack), Value(ValueMap())));
    }
    
    if(GetUserData(UserDataEnum::MakeBuild) == nullptr)
    {
        m_UserDataVm->insert(pair<string, Value>(getEnumEncodeString(UserDataEnum::MakeBuild), Value(ValueMap())));
    }
    
    if(GetUserData(UserDataEnum::DeathHero) == nullptr)
    {
        m_UserDataVm->insert(pair<string, Value>(getEnumEncodeString(UserDataEnum::DeathHero), Value(ValueMap())));
    }
    
    if(GetUserData(UserDataEnum::LiveHero) == nullptr)
    {
        m_UserDataVm->insert(pair<string, Value>(getEnumEncodeString(UserDataEnum::LiveHero), Value(ValueMap())));
    }
    
    if(GetUserData(UserDataEnum::MakeItem) == nullptr)
    {
        m_UserDataVm->insert(pair<string, Value>(getEnumEncodeString(UserDataEnum::MakeItem), Value(ValueMap())));
    }
    
    if(GetUserData(UserDataEnum::Production) == nullptr)
    {
        m_UserDataVm->insert(pair<string, Value>(getEnumEncodeString(UserDataEnum::Production), Value(ValueMap())));
    }
    
    if(GetUserData(UserDataEnum::ShopItem) == nullptr)
    {
        m_UserDataVm->insert(pair<string, Value>(getEnumEncodeString(UserDataEnum::ShopItem), Value(ValueMap())));
    }
    
    if(GetUserData(UserDataEnum::Storehouse) == nullptr)
    {
        m_UserDataVm->insert(pair<string, Value>(getEnumEncodeString(UserDataEnum::Storehouse), Value(ValueMap())));
    }
    
    if(GetUserData(UserDataEnum::FinishAchievement) == nullptr)
    {
        m_UserDataVm->insert(pair<string, Value>(getEnumEncodeString(UserDataEnum::FinishAchievement), Value(ValueVector())));
    }
    
    if(GetUserData(UserDataEnum::UnFinishAchievement) == nullptr)
    {
        m_UserDataVm->insert(pair<string, Value>(getEnumEncodeString(UserDataEnum::UnFinishAchievement), Value(ValueMap())));
    }
    
    if(GetUserData(UserDataEnum::TalentList) == nullptr)
    {
        m_UserDataVm->insert(pair<string, Value>(getEnumEncodeString(UserDataEnum::TalentList), Value(ValueVector())));
    }
    
    if(GetUserData(UserDataEnum::UnLockIdList) == nullptr)
    {
        m_UserDataVm->insert(pair<string, Value>(getEnumEncodeString(UserDataEnum::UnLockIdList), Value(ValueVector())));
    }
    
    // map
    if(GetUserData(UserDataEnum::ExploreMaps) == nullptr)
    {
        m_UserMapVm->insert(pair<string, Value>(getEnumEncodeString(UserDataEnum::ExploreMaps), Value(ValueMap())));
    }
    
    // map
    if(GetUserData(UserDataEnum::MistMaps) == nullptr)
    {
        m_UserMapVm->insert(pair<string, Value>(getEnumEncodeString(UserDataEnum::MistMaps), Value(ValueMap())));
    }
    
    // map
    if(GetUserData(UserDataEnum::TotalExploreValue) == nullptr)
    {
        m_UserMapVm->insert(pair<string, Value>(getEnumEncodeString(UserDataEnum::TotalExploreValue), Value(0)));
    }
    
    if(GetUserData(UserDataEnum::RanksMaxNum) == nullptr)
    {
        m_UserDataVm->insert(pair<string, Value>(getEnumEncodeString(UserDataEnum::RanksMaxNum), Value(1)));
    }
    
    if(GetUserData(UserDataEnum::BackPackMaxNum) == nullptr)
    {
        m_UserDataVm->insert(pair<string, Value>(getEnumEncodeString(UserDataEnum::BackPackMaxNum), Value(20)));
    }
    
    if(GetUserData(UserDataEnum::CollectMaxNum) == nullptr)
    {
        m_UserDataVm->insert(pair<string, Value>(getEnumEncodeString(UserDataEnum::CollectMaxNum), Value(15)));
    }
    
    if(GetUserData(UserDataEnum::CollectMinNum) == nullptr)
    {
        m_UserDataVm->insert(pair<string, Value>(getEnumEncodeString(UserDataEnum::CollectMinNum), Value(5)));
    }
    
    if(GetUserData(UserDataEnum::AlchemyGoldNum) == nullptr)
    {
        // 1为非自动
        m_UserDataVm->insert(pair<string, Value>(getEnumEncodeString(UserDataEnum::AlchemyGoldNum), Value(1)));
    }
    
    if(GetUserData(UserDataEnum::AchievementNum) == nullptr)
    {
        m_UserDataVm->insert(pair<string, Value>(getEnumEncodeString(UserDataEnum::AchievementNum), Value(1)));
    }
    
    if(GetUserData(UserDataEnum::AutoAttack) == nullptr)
    {
        m_UserDataVm->insert(pair<string, Value>(getEnumEncodeString(UserDataEnum::AutoAttack), Value(1)));
    }
    
    if(GetUserData(UserDataEnum::BreadRevertHp) == nullptr)
    {
        m_UserDataVm->insert(pair<string, Value>(getEnumEncodeString(UserDataEnum::BreadRevertHp), Value(5)));
    }
    
    // map
    if(GetUserData(UserDataEnum::MapNotHoldBuild) == nullptr)
    {
        m_UserMapVm->insert(pair<string, Value>(getEnumEncodeString(UserDataEnum::MapNotHoldBuild), Value(ValueMap())));
    }
    
    if(GetUserData(UserDataEnum::OnlyItemList) == nullptr)
    {
        m_UserDataVm->insert(pair<string, Value>(getEnumEncodeString(UserDataEnum::OnlyItemList), Value(ValueVector())));
    }
    
    if(GetUserData(UserDataEnum::RandomEventTime) == nullptr)
    {
        auto timeStamp = (int)time(nullptr);
        m_UserDataVm->insert(pair<string, Value>(getEnumEncodeString(UserDataEnum::RandomEventTime), Value(timeStamp)));
    }
    
    if(GetUserData(UserDataEnum::LastProductionTime) == nullptr)
    {
        auto timeStamp = (int)time(nullptr);
        m_UserDataVm->insert(pair<string, Value>(getEnumEncodeString(UserDataEnum::LastProductionTime), Value(timeStamp)));
    }
    
    if(GetUserData(UserDataEnum::EternalArenaMaxLevel) == nullptr)
    {
        m_UserDataVm->insert(pair<string, Value>(getEnumEncodeString(UserDataEnum::EternalArenaMaxLevel), Value(0)));
    }
    
    if(GetUserData(UserDataEnum::PlayerInMapId) == nullptr)
    {
        m_UserDataVm->insert(pair<string, Value>(getEnumEncodeString(UserDataEnum::PlayerInMapId), Value(909000001)));
    }

    if(GetUserData(UserDataEnum::PlayerInMapTiledId) == nullptr)
    {
        m_UserDataVm->insert(pair<string, Value>(getEnumEncodeString(UserDataEnum::PlayerInMapTiledId), Value(-1)));
    }
    
    if(GetUserData(UserDataEnum::IsPlayerInMap) == nullptr)
    {
        m_UserDataVm->insert(pair<string, Value>(getEnumEncodeString(UserDataEnum::IsPlayerInMap), Value(false)));
    }
    
    if(GetUserData(UserDataEnum::PlayerInMapXAxis) == nullptr)
    {
        m_UserDataVm->insert(pair<string, Value>(getEnumEncodeString(UserDataEnum::PlayerInMapXAxis), Value(0)));
    }
    
    if(GetUserData(UserDataEnum::PlayerInMapYAxis) == nullptr)
    {
        m_UserDataVm->insert(pair<string, Value>(getEnumEncodeString(UserDataEnum::PlayerInMapYAxis), Value(0)));
    }
    
    // map
    if(GetUserData(UserDataEnum::UnLockBuildMaps) == nullptr)
    {
        m_UserMapVm->insert(pair<string, Value>(getEnumEncodeString(UserDataEnum::UnLockBuildMaps), Value(ValueMap())));
    }
    
    if(GetUserData(UserDataEnum::UnLockTriggerList) == nullptr)
    {
        m_UserDataVm->insert(pair<string, Value>(getEnumEncodeString(UserDataEnum::UnLockTriggerList), Value(ValueVector())));
    }
    
    if(GetUserData(UserDataEnum::FirstTopUp) == nullptr)
    {
        m_UserDataVm->insert(pair<string, Value>(getEnumEncodeString(UserDataEnum::FirstTopUp), Value(ValueMap())));
    }
    
    if(GetUserData(UserDataEnum::FirstLoginSeconds) == nullptr)
    {
        m_UserDataVm->insert(pair<string, Value>(getEnumEncodeString(UserDataEnum::FirstLoginSeconds), Value(0)));
    }
    
    if(GetUserData(UserDataEnum::LoginAwardState) == nullptr)
    {
        m_UserDataVm->insert(pair<string, Value>(getEnumEncodeString(UserDataEnum::LoginAwardState), Value(ValueVector())));
        
        for (int i = 0; i < 7; ++i) {
            Add_VI_Fun(UserDataEnum::LoginAwardState, 0);
        }
    }
    
    return true;
}

bool UserData::isMapData(UserDataEnum ude)
{
    bool isMapData = false;
    if (ude == UserDataEnum::ExploreMaps || ude == UserDataEnum::MistMaps || ude == UserDataEnum::MapNotHoldBuild || ude == UserDataEnum::UnLockBuildMaps || ude == UserDataEnum::TotalExploreValue) {
        isMapData = true;
    }
    return isMapData;
}

//加载用户数据
bool UserData::LoadUserData()
{
    m_UserDataDir = FileUtils::getInstance()->getWritablePath()+"AppData";
    if( !FileUtils::getInstance()->isDirectoryExist(m_UserDataDir) )
    {
        FileUtils::getInstance()->createDirectory(m_UserDataDir);
    }
    
    *m_UserDataVm = FileUtils::getInstance()->getValueMapFromFile(getUserDataFilePath());
    *m_UserMapVm = FileUtils::getInstance()->getValueMapFromFile(getUserMapFilePath());
    
    Init(); //if (m_UserDataVm->empty() || m_UserDataVm->size() == 0) Init();
    return true;
}

//保存用户数据
bool UserData::SaveUserData()
{
    FileUtils::getInstance()->writeToFile(*m_UserDataVm, getUserDataFilePath());
    FileUtils::getInstance()->writeToFile(*m_UserMapVm, getUserMapFilePath());
    return true;
}

bool UserData::SaveCorUserData(bool isMapData)
{
    if (isMapData) {
        FileUtils::getInstance()->writeToFile(*m_UserMapVm, getUserMapFilePath());
    }
    else {
        FileUtils::getInstance()->writeToFile(*m_UserDataVm, getUserDataFilePath());
    }
    return true;
}

Value* UserData::GetUserData(UserDataEnum ude)
{
    Value* result = nullptr;
    auto key = getEnumEncodeString(ude);
    
    ValueMap* curVm = nullptr;
    if (isMapData(ude)) {
        curVm = m_UserMapVm;
    }
    else {
        curVm = m_UserDataVm;
    }
    
    auto iter = curVm->find(key);
    if (iter != curVm->end())
    {
        result = &iter->second;
    }
    
    return result;
}

//查找 ValueVector 中 是否有 val（全是int）
bool UserData::Check_VI_Fun(UserDataEnum userDataEnum, Value val)
{
    return Check_VI_Fun(&GetUserData(userDataEnum)->asValueVector(), val);
}

//查找 ValueVector 中 是否有 val
bool UserData::Check_VI_Fun(ValueVector* vv, Value val)
{
    bool result = false;
    if (find(vv->begin(), vv->end(), val) != vv->end())
    {
        result = true;
    }
    return result;
}

//用于只增加 vector<int> 数据
bool UserData::Add_VI_Fun(UserDataEnum userDataEnum, int val)
{
    return Add_VI_Fun(&GetUserData(userDataEnum)->asValueVector(), val, isMapData(userDataEnum));
}

bool UserData::Delete_VI_Fun(UserDataEnum userDataEnum, Value val)
{
    return Delete_VI_Fun(&GetUserData(userDataEnum)->asValueVector(), val);
}

//查找 ValueVector 中 是否有 val
bool UserData::Delete_VI_Fun(ValueVector* vv, Value val)
{
    bool result = false;
    auto iter = find(vv->begin(), vv->end(), val);
    if (iter != vv->end())
    {
        vv->erase(iter);    result = true;
    }
    return result;
}

//用于只增加 vector<int> 数据
bool UserData::Add_VI_Fun(ValueVector* vv, int val, bool isMapData)
{
    int result = false;
    
    if (find(vv->begin(), vv->end(), Value(val)) == vv->end())
    {
        result = true;
        vv->push_back(Value(val));
    }
//    SaveUserData();
    SaveCorUserData(isMapData);
    
    return result;
}

//用于只增加 vector<string> 数据
bool UserData::Add_VS_Fun(ValueVector* vs, const string& val)
{
    int result = false;
    if (find(vs->begin(), vs->end(), Value(val)) == vs->end())
    {
        result = true;
        vs->push_back(Value(val));
    }
    SaveUserData();
    return result;
}

//删除 ValueMap中的值
bool UserData::Delete_VMI_Fun(UserDataEnum userDataEnum, int key)
{
    return Delete_VMI_Fun(&GetUserData(userDataEnum)->asValueMap(), key, isMapData(userDataEnum));
}

//删除 ValueMap中的值
bool UserData::Delete_VMI_Fun(ValueMap* vm, int key, bool isMapData)
{
    return Delete_VMS_Fun(vm, StringUtils::format("%d", key), isMapData);
}

//删除 ValueMap中的值
bool UserData::Delete_VMS_Fun(ValueMap* vm, const string& key, bool isMapData)
{
    
    bool result = false;
    auto iter = vm->find(key);
    if (iter == vm->end())
    {
        result = false;
    }else
    {
        result = true;
        vm->erase(key);
    }
//    SaveUserData();
    SaveCorUserData(isMapData);
    
    return result;
}

//把 ValueMap 转成排序后的 viv  ValueMap key 必须要能转成 int
void UserData::VMSKToVIV_Sort(const ValueMap* vm, vector<pair<int, Value>>* viv)
{
    auto iter = vm->begin();
    while (iter != vm->end())
    {
        viv->push_back(make_pair(atoi(iter->first.c_str()), iter->second));
        iter++;
    }
    
    std::sort(viv->begin(), viv->end(), [&](const pair<int, Value> pair1, const pair<int, Value> pair2)
      {
          return pair1.first < pair2.first;
      });
}

//更新 探索度数量
//队伍最大人数 背包最大容量
//采集最小数量 采集最大数量
void UserData::Update_Value(UserDataEnum userDataEnum, Value val, bool isSave)
{
    *GetUserData(userDataEnum)  = val;
//    if(isSave) SaveUserData();
    if(isSave)  SaveCorUserData(isMapData(userDataEnum));
}


//用于更新ValueMap数据 key不存 val大于0就增加 key存在值叠加,
//isReplace=true 替换值 isVal0DelKey 值为0时 是否删除key isSave:是否保存
int UserData::Update_VMI_Fun(UserDataEnum userDataEnum, const int key, int val, bool isReplace, bool isVal0DelKey, bool isSave)
{
    return Update_VMI_Fun(&GetUserData(userDataEnum)->asValueMap(), key, val, isReplace, isVal0DelKey, isSave, isMapData(userDataEnum));
}

int UserData::Update_VMV_Encode(UserDataEnum ude, const int key, int val, bool isReplace, bool isSave)
{
    auto vm = &GetUserData(ude)->asValueMap();
    string keyStr = StringUtils::format("%d", key);
    string valueStr;    // 加密后
    int result = 0;
    
    auto iter = vm->find(keyStr);
    if (iter == vm->end()) {
        if (val > 0) {
            result = val;
            valueStr = DataEncryptUtils::getInstance()->encode(val);
            vm->insert(make_pair(keyStr, Value(valueStr)));
        }
    }
    else
    {
        int oriInt = DataEncryptUtils::getInstance()->decode(iter->second.asString());
        
        if (isReplace) {
            result = val;
        }
        else {
            result = oriInt + val;
        }
        
        if (result >= 0) {
            valueStr = DataEncryptUtils::getInstance()->encode(result);
            iter->second = Value(valueStr);
        }
    }
    
    if(isSave) SaveCorUserData(false);
    return result;
}

//用于更新ValueMap数据 key不存 val大于0就增加 key存在值叠加,
//isReplace=true 替换值 isVal0DelKey 值为0时 是否删除key isSave:是否保存
int UserData::Update_VMI_Fun(ValueMap* vm, const int key, int val, bool isReplace, bool isVal0DelKey, bool isSave, bool isMapData)
{
    return Update_VMS_Fun(vm, StringUtils::format("%d", key), val, isReplace, isVal0DelKey, isSave, isMapData);
}

//用于更新ValueMap数据 key不存 val大于0就增加 key存在值叠加,
//isReplace=true 替换值 isVal0DelKey 值为0时 是否删除 key isSave:是否保存
int UserData::Update_VMS_Fun(ValueMap* vm, const string& key, int val, bool isReplace, bool isVal0DelKey, bool isSave, bool isMapData)
{
    int result = 0;
    
    auto iter = vm->find(key);
    if (iter == vm->end())
    {
        result = val;
        if (val >= 0)//修改了
        {
            vm->insert(make_pair(key, Value(val)));
        }
    }else
    {
        auto data = iter->second;
        if (data.getType() == Value::Type::INTEGER)
        {
            if (isReplace)
            {
                result = val;
            }else
            {
                result = data.asInt() + val;
            }
            
            if (isVal0DelKey && result == 0)
            {
                vm->erase(key);
            }
            
            if (result >= 0)
            {
                iter->second = Value(result);
            }

        }
        
    }
    
    if(isSave) SaveCorUserData(isMapData);
    return result;
}

//获取ValueMap的数据
const Value UserData::Get_MVI_Value(UserDataEnum userDataEnum, int key, bool isEncode)
{
    auto oriVal = Get_MVI_Value(&GetUserData(userDataEnum)->asValueMap(), key);
    if (isEncode) {
        int realVal = DataEncryptUtils::getInstance()->decode(oriVal.asString());
        return Value(realVal);
    }
    
    return oriVal;
}

//获取ValueMap的数据
const Value UserData::Get_MVI_Value(const ValueMap* vm, int key)
{
    string strKey = StringUtils::format("%d", key);
    return Get_MVS_Value(vm, strKey);
}

//获取ValueMap的数据
const Value UserData::Get_MVS_Value(const ValueMap* vm, const string& key)
{
    const Value* val = nullptr;
    auto iter = vm->find(key);
    if (iter == vm->end())
    {
        val = &Value::Null;
        CCLOGERROR("ValueMap Not Key:%s", key.c_str());
    }else
    {
        val = &iter->second;
    }
    return *val;
}

//检查仓库是否有这些物品 ValueMap 物品Id 物品数量 lackData缺少数据
bool UserData::CheckStorehouseData(const ValueMap* vm, vector<pair<int, int>>* lackData)
{
    auto storehouse = &GetUserData(UserDataEnum::Storehouse)->asValueMap();
    for (auto iter = vm->begin(); iter != vm->end(); iter++)
    {
        auto si = storehouse->find(iter->first);
        if (si == storehouse->end())
        {
            lackData->push_back(make_pair(atoi(iter->first.c_str()), iter->second.asInt()));
        }else
        {
            if (si->second.asInt() < iter->second.asInt())
            {
                lackData->push_back(make_pair(atoi(iter->first.c_str()), iter->second.asInt() - si->second.asInt()));
            }
        }
    }
    return lackData->empty() ? true : false;
}

//删除仓库里这些物品  ValueMap 物品Id 物品数量  lackData缺少数据 isSave是否保存
bool UserData::DeleteStorehouseData(const ValueMapIntMultiKey* vmi, vector<pair<int, int>>* lackData, bool isSave)
{
    ValueMap vm;
    for (auto iter = vmi->begin(); iter != vmi->end(); iter++)
    {
        vm[StringUtils::format("%d", iter->first)] = iter->second;
    }
    return DeleteStorehouseData(&vm, lackData, isSave);
}
//删除仓库里这些物品  ValueMap 物品Id 物品数量  lackData缺少数据
bool UserData::DeleteStorehouseData(const ValueMap* vm, vector<pair<int, int>>* lackData, bool isSave)
{
    bool resutl = CheckStorehouseData(vm, lackData);
    if ( resutl )
    {
        int val;
        auto storeVm = &GetUserData(UserDataEnum::Storehouse)->asValueMap();
        for (auto iter = vm->begin(); iter != vm->end(); iter++)
        {
            val = (*storeVm)[iter->first].asInt();
            val = val - iter->second.asInt();
            if (val == 0)
            {
                storeVm->erase(iter->first);
            }else
            {
                (*storeVm)[iter->first] = Value(val);
            }
        }
    }
//    if(isSave) SaveUserData();
    if(isSave) SaveCorUserData(false);
    
    return resutl;
}

//批量增加数据到 仓库
bool UserData::AddStorehouseData(const ValueMapIntMultiKey* vmi)
{
    string key;
    bool result = true;
    auto storeVm = &GetUserData(UserDataEnum::Storehouse)->asValueMap();
    for (auto data : *vmi)
    {
        Update_VMI_Fun(storeVm, data.first, data.second.asInt(), false, true, false, false);
    }
//    SaveUserData();
    SaveCorUserData(false);
    
    return result;
}

string UserData::getEnumEncodeString(UserDataEnum ude)
{
    string keyStr = m_EnumKeyString[(uint16_t)ude];
    
    return DataEncryptUtils::getInstance()->encode(keyStr);
}

string UserData::getUserDataFilePath()
{
    string curFileName = SettingData::getInstance()->GetSettingData(SettingDataEnum::CurFileDocName)->asString();
    string userDataPath = m_UserDataDir + "/" + DataEncryptUtils::getInstance()->encode(USERDATA_FILE_NAME + curFileName) ;
    return userDataPath;
}

string UserData::getUserMapFilePath()
{
    string curFileName = SettingData::getInstance()->GetSettingData(SettingDataEnum::CurFileDocName)->asString();
    string userMapFilePath = m_UserDataDir + "/" + DataEncryptUtils::getInstance()->encode(USERMAP_FILE_NAME + curFileName);
    return userMapFilePath;
}

