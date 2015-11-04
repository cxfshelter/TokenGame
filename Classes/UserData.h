//
//  UserData.h
//  TokenGame
//
//  Created by token on 15/3/17.
//
//

#ifndef _USERDATA_H_
#define _USERDATA_H_

#include <vector>
#include <unordered_map>

#include "cocos2d.h"
#include "EnunClass.h"

USING_NS_CC;
using namespace std;


class UserData
{
private:
    
    static UserData* sm_UserData;
    
    string m_UserDataDir;
    
    string m_UserDataPath;
    
    //用户基本数据
    ValueMap* m_UserDataVm = nullptr;
    
    //用户地图相关数据
    ValueMap* m_UserMapVm = nullptr;
    

    const vector<string> m_EnumKeyString =
    {
        //用户Id
        //string
        "UserId",
        
        //用户名
        //string
        "UserName",
        
        //背包 id num
        //map<string, int>
        "BackPack",
        
        //仓库 id num
        //map<string, int>
        "Storehouse",
        
        //建筑id
        //0有新建筑可建
        //1有建筑可建
        //2已建好
        //map<string, int>
        "MakeBuild",
        
        //商店可购买的Id
        //true是new物品
        //map<string, bool>
        "ShopItem",
        
        //制作Id
        //true是new物品可制作
        //map<string, bool>
        "MakeItem",
        
        //活着的英雄Id,数量
        //map<string, int>
        "LiveHero",
        
        //死亡的英雄Id,数量
        //map<string, int>
        "DeathHero",
        
        //生产Id
        //true是new物品可生产
        //map<string, bool>
        "Production",
        
        //天赋列表
        //vector<int>
        "TalentList",
        
        //已完成的成就
        //vector<int>
        "FinishAchievement",
        
        //已开始但未完成的成就
        //    成就Id 完成度
        //map<string, int>
        "UnFinishAchievement",

        
        //保存已解锁的Id
        //vector<int>
        "UnLockIdList",
        
        //到达过的地图
        "ExploreMaps",
        
        //地图上没去掉的迷雾
        "MistMaps",
        
        //所有地图总探索值
        // int
        "TotalExploreValue",
        
        //队伍最大人数
        "RanksMaxNum",
        
        //背包最大容量
        "BackPackMaxNum",
        
        //采集最小数量
        "CollectMinNum",
        
        //采集最大数量
        "CollectMaxNum",
        
        //练金获取金币数
        "AlchemyGoldNum",
        
        //成就总数量
        "AchievementNum",
        
        //是否自动攻击
        //0,手动攻击
        //1,自动攻击
        "AutoAttack",
        
        //面包回血值
        "BreadRevertHp",
        
        //地图上未占领的建筑
        "MapNotHoldBuild",
        
        //已拥有的唯一物品
        //或已消耗的唯一物品
        //vector<int>
        "OnlyItemList",
        
        //最后一次随机事件时间戳
        //int
        "RandomEventTime",
        
        //最后一次生产时间戳
        //int
        "LastProductionTime",
        
        //通关永恒竞技场最高层次
        "EternalArenaMaxLevel",
        
        //玩家所在地图Id
        "PlayerInMapId",
        
        //玩家所在瓦片Id
        //如果-1表示在中间
        "PlayerInMapTiledId",
        
        //玩家是否在地图中
        //bool
        "IsPlayerInMap",
        
        //玩家所在地图X坐标
        //如果瓦片在中间值无效
        "PlayerInMapXAxis",
        
        //玩家所在地图Y坐标
        //如果瓦片在中间值无效
        "PlayerInMapYAxis",
        
        //地图上没解锁的建筑
        //地图Id       建筑Id
        //map<string, vector<int>>
        "UnLockBuildMaps",
        
        //用于保存多个解锁条件的TriggerId
        //vector<int>
        "UnLockTriggerList",
        
        //首充状态
        //map<string, bool>
        "FirstTopUp",
        
        //首次登陆时间戳（秒）
        //int，最大值2147483647（即：2038年1月9号等，可用）
        "FirstLoginSeconds",
        
        //登陆领奖的领取状态
        //vector<int>    0代表未领取，非0（即1）代表已领取
        "LoginAwardState",
        
        "EnumClassMaxVal"
    };
    
private:
    
    bool Init();
    
    /*  判断是否要另存文件的数据类型（目前地图相关的均另存地图）
     */
    bool isMapData(UserDataEnum ude);
    
    //获得ValueMap中的值
    const Value Get_MVS_Value(const ValueMap* vm, const string& key);
    
    // 对存储的键进行加密
    string getEnumEncodeString(UserDataEnum udem);
    
    
public:
    
    // 排行榜名字
    string m_RankName = "";
    void setRankName(string);
    const string getRankName();
    
    //登陆或休眠状态为true
    bool m_SleepStatus = true;
    
    static UserData* getInstance();
    
    // 清空相关变量
    void releaseData();
    
    //加载用户数据
    bool LoadUserData();
    
    //保存用户数据
    bool SaveUserData();
    
    // 保存相应地用户数据
    bool SaveCorUserData(bool isMapData);
    
    // isEncode：true返回解密，false返回数据不需要解密
    Value* GetUserData(UserDataEnum ude);
    
    
    //用于只增加 vector<int> 数据
    bool Add_VI_Fun(ValueVector* vi, int val, bool isMapData = false);
    
    //用于只增加 vector<int> 数据（暂无用到）
    bool Add_VS_Fun(ValueVector* vs, const string& val);
    
    //用于只增加 vector<int> 数据
    bool Add_VI_Fun(UserDataEnum userDataEnum, int val);
    
    bool Delete_VI_Fun(UserDataEnum userDataEnum, Value val);
    
    //查找 ValueVector 中 是否有 val
    bool Delete_VI_Fun(ValueVector* vv, Value val);

    //删除 ValueMap中的值
    bool Delete_VMI_Fun(ValueMap* vm, int key, bool isMapData = false);
    
    //删除 ValueMap中的值
    bool Delete_VMS_Fun(ValueMap* vm, const string& key, bool isMapData = false);
    
    //删除 ValueMap中的值
    bool Delete_VMI_Fun(UserDataEnum userDataEnum, int key);
    
    
    //获取ValueMap的数据（全是int）
    // isEncode: true解密， false不解密
    const Value Get_MVI_Value(UserDataEnum userDataEnum, int key, bool isEncode = false);
    
    //获得ValueMap中的值
    const Value Get_MVI_Value(const ValueMap* vm, int key);
    
    //查找 ValueVector 中 是否有 val
    bool Check_VI_Fun(ValueVector* vv, Value val);
    
    //查找 ValueVector 中 是否有 val
    bool Check_VI_Fun(UserDataEnum userDataEnum, Value val);
    
    //把 ValueMap 转成排序后的 viv  ValueMap key 必须要能转成 int
    void VMSKToVIV_Sort(const ValueMap* vm, vector<pair<int, Value>>* viv);

    //更新 基本值类型
    void Update_Value(UserDataEnum userDataEnum, Value val, bool isSave = true);
    
    //用于更新ValueMap数据 key不存 val大于0就增加 key存在值叠加,
    //isReplace=true 替换值 isVal0DelKey 值为0时 是否删除 key
    int Update_VMI_Fun(ValueMap* vm, const int key, int val, bool isReplace = false, bool isVal0DelKey = true, bool isSave = true, bool isMapData = false);
    
    //用于更新ValueMap数据 key不存 val大于0就增加 key存在值叠加,
    //isReplace=true 替换值 isVal0DelKey 值为0时 是否删除 key
    int Update_VMS_Fun(ValueMap* vm, const string& key, int val, bool isReplace = false, bool isVal0DelKey = true, bool isSave = true, bool isMapData = false);
    
    //用于更新ValueMap数据 key不存 val大于0就增加 key存在值叠加,
    //isReplace=true 替换值 isVal0DelKey 值为0时 是否删除key isSave:是否保存
    int Update_VMI_Fun(UserDataEnum userDataEnum, const int key, int val, bool isReplace = false, bool isVal0DelKey = true, bool isSave = true);
    
    //--- encode ----
    // encode test
    // 加密专用通道，对于ValueMap
    // 只加int值（且目前只对个别值进行加密，如采集数量、钻石数量等）
    // val为加密前正确数据
    int Update_VMV_Encode(UserDataEnum ude, const int key, int val, bool isReplace = false, bool isSave = true);
    
    //--- encode ----
    
    ////用于更新ValueMap数据 key不存就增加 key存在值叠加, isReplace=true 替换值
    bool CheckStorehouseData(const ValueMap* vm, vector<pair<int, int>>* lackData);
    
    //批量增加数据到 仓库
    bool AddStorehouseData(const ValueMapIntMultiKey* vmi);
    
    //删除仓库里这些物品  ValueMap 物品Id 物品数量
    bool DeleteStorehouseData(const ValueMap* vm, vector<pair<int, int>>* lackData, bool isSave = true);
    
    //删除仓库里这些物品  ValueMap 物品Id 物品数量
    bool DeleteStorehouseData(const ValueMapIntMultiKey* vmi, vector<pair<int, int>>* lackData, bool isSave = true);
    
    // 获取用户基本数据文件路径
    string getUserDataFilePath();
    
    // 获取用户地图数据文件路径
    string getUserMapFilePath();
};

#endif /* defined(_USERDATA_H_) */
