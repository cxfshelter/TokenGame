//
//  EnunClass.h
//  TokenGame
//
//  Created by token on 15/3/23.
//
//

#ifndef TokenGame_EnunClass_h
#define TokenGame_EnunClass_h

#define LIST_ITEM_HORWORD   19
#define LIST_ITEM_HEIGHT    32
#define LIST_MOVE_DURATION  0.5

#define BTN_NOT_LONGBTN     0
#define BTN_IS_LONGBTN      1

#define LONG_BTN_TIME  0.6	// 长按判定最低时间
#define BTN_ADD_TIME   0.1	// 长按增加数量间隔

// 背景音乐
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#define MUSIC_FILE        "MusicMain.ogg"
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#define MUSIC_FILE        "MusicMain.caf"
#else
#define MUSIC_FILE        "MusicMain.mp3"
#endif

// 采集音效
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#define GOLD_EFFECT        "gold.ogg"
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#define GOLD_EFFECT        "gold.caf"
#else
#define GOLD_EFFECT        "gold.mp3"
#endif

// 提炼音效
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#define TILIAN_EFFECT        "tilian.ogg"
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#define TILIAN_EFFECT        "tilian.caf"
#else
#define TILIAN_EFFECT        "tilian.mp3"
#endif

// 玩家死亡音效
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#define PDEAD_EFFECT        "idead.ogg"
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#define PDEAD_EFFECT        "idead.caf"
#else
#define PDEAD_EFFECT        "idead.mp3"
#endif

// 玩家攻击音效
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#define PATTACK_EFFECT        "ihit.ogg"
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#define PATTACK_EFFECT        "ihit.caf"
#else
#define PATTACK_EFFECT        "ihit.mp3"
#endif

//  怪物死亡音效
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#define MDEAD_EFFECT        "mdead.ogg"
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#define MDEAD_EFFECT        "mdead.caf"
#else
#define MDEAD_EFFECT        "mdead.mp3"
#endif

//  怪物攻击音效
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#define MATTACK_EFFECT        "mhit.ogg"
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#define MATTACK_EFFECT        "mhit.caf"
#else
#define MATTACK_EFFECT        "mhit.mp3"
#endif

// 地图行走音效
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#define WALK_EFFECT        "walk.ogg"
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#define WALK_EFFECT        "walk.caf"
#else
#define WALK_EFFECT        "walk.mp3"
#endif

#define USERDATA_FILE_NAME  "UserBaseData"
#define USERMAP_FILE_NAME   "UserMapData"

//用户数据相关的
enum class UserDataEnum
{
    
    //用户Id
    //string
    UserId,
    
    //用户名
    //string
    UserName,
    
    //背包 id num
    //map<string, int>
    BackPack,
    
    //仓库 id num
    //map<string, int>
    Storehouse,
    
    //建筑id
    //0有新建筑可建
    //1有建筑可建
    //2已建好
    //map<string, int>
    MakeBuild,
    
    //商店可购买的Id
    //true是new物品
    //map<string, bool>
    ShopItem,
    
    //制作Id
    //true是new物品可制作
    //map<string, bool>
    MakeItem,
    
    //活着的英雄Id,数量
    //map<string, int>
    LiveHero,
    
    //死亡的英雄Id,数量
    //map<string, int>
    DeathHero,
    
    //生产Id
    //生产数量
    //map<string, int>
    Production,
    
    //天赋列表
    //vector<int>
    TalentList,
    
    //已完成的成就
    //vector<int>
    FinishAchievement,
    
    //已开始但未完成的成就
    //    成就Id 完成度
    //map<string, int>
    UnFinishAchievement,
    
    //保存已解锁的Id
    //vector<int>
    UnLockIdList,
    
    //到达过的地图id
    //地图Id 探索值
    //map<string, int>
    ExploreMaps,
    
    //地图上没去掉的迷雾
    //地图Id       瓦片Id
    //map<string, vector<int>>
    MistMaps,
    
    //所有地图的探索值
    //int
    TotalExploreValue,
    
    //队伍最大人数
    RanksMaxNum,
    
    //背包最大容量
    BackPackMaxNum,
    
    //采集最小数量
    CollectMinNum,
    
    //采集最大数量
    CollectMaxNum,
    
    //练金获取金币
    AlchemyGoldNum,
    
    //成就总值
    AchievementNum,
    
    //是否自动攻击
    //0,手动攻击
    //1,自动攻击
    AutoAttack,
    
    //面包回血值
    BreadRevertHp,
    
    //地图上未占领的建筑
    //地图Id 未占领的建筑Id
    //map<string, vector<int>>
    MapNotHoldBuild,
    
    //已拥有的唯一物品
    //或已消耗的唯一物品
    //vector<int>
    OnlyItemList,
    
    //最后一次随机事件时戳
    //int
    RandomEventTime,
    
    //最后一次生产时间戳
    //int
    LastProductionTime,
    
    //通关永恒竞技场最高层次
    EternalArenaMaxLevel,
    
    //玩家所在地图Id
    PlayerInMapId,

    //玩家所在瓦片Id
    //如果-1表示在中间
    PlayerInMapTiledId,
    
    //玩家是否在地图中
    //bool
    IsPlayerInMap,
    
    //玩家所在地图X坐标
    PlayerInMapXAxis,
    
    //玩家所在地图Y坐标
    PlayerInMapYAxis,
    
    //地图上没解锁的建筑
    //地图Id       建筑Id
    //map<string, vector<int>>
    UnLockBuildMaps,

    //用于保存多个解锁条件的TriggerId
    //vector<int>
    UnLockTriggerList,
    
    //首充状态
    //id(1/2/3) : 是否首充(0/1)
    //map<string, string>
    FirstTopUp,
    
    //首次登陆时间日期（距离1970年1月1日多少天）
    //int，最大值2147483647（即：2038年1月9号等，可用）
    FirstLoginDays,
    
    //登陆领奖的领取状态
    //vector<int>    0代表未领取，非0（即1）代表已领取
    LoginAwardState,
    
    EnumClassMaxVal,
};

// 用户地图数据相关
enum class UserMapDataEnum
{
    //到达过的地图id
    //地图Id 探索值
    //map<string, int>
    ExploreMaps,
    
    //地图上没去掉的迷雾
    //地图Id       瓦片Id
    //map<string, vector<int>>
    MistMaps,
    
    //所有地图的探索值
    //int
    TotalExploreValue,
    
    //地图上未占领的建筑
    //地图Id 未占领的建筑Id
    //map<string, vector<int>>
    MapNotHoldBuild,
    
    //地图上没解锁的建筑
    //地图Id       建筑Id
    //map<string, vector<int>>
    UnLockBuildMaps,
    
    EnumMapMaxVal,
};

enum class SettingDataEnum
{
    /*  存档
     *  string
     *  内容：1.txt或者2.txt或者3.txt
     */
    CurFileDocName,
    
    /*  音效
     *  bool
     */
    SoundEffectIsOpen,
    
    /*  背景音乐
     *  bool
     */
    BgMusicIsOpen,
    
    /*  战斗特效
     *  bool
     */
    BattleEffectIsOpen,
    
    SetEnumMax
    
};

//配置表Id相关的
enum class  CfgIdEnum: int32_t
{
    GoldId = 100000000,
    
    GemId =  100000100,
    //面包id
    BreadId = 100000002,
    
    //仓库Id
    StorehouseId = 100001000,
    
    //训练营Id
    TrainingCampId = 100001105,
    
    //酒馆
    BuildPubId = 100001104,
    
    //1级民宅Id
    WorkerHouseMinId = 100001001,
    
    //最大级民宅Id
    WorkerHouseMaxId = 100001040,
    
    //工匠包 最小Id
    BackPackMinId = 100002000,
    
    //工匠包 最小Id
    BackPackMaxId = 100002013,
    
    //农场建筑Id
    FarmId = 100001100,
    
    //商店建筑Id
    ShopId = 100001101,
    
    //木头
    WoodId = 100000006,
    
    //石头
    StoneId = 100000007,
    
    //石头 木头
    Collect_Default_Wood_Stone_Min = 5,
    
    //石头 木头
    Collect_Default_Wood_Stone_Max = 15,
    
    //皮革
    LeatherId = 100000003,

    //马厩Id
    StableHoumeId = 100001102,
    
    
    //解锁建造建筑
    UnlockBuild = 100700001,
    
    //解锁 制作物品
    UnlockMake = 100700002,
    
    //解锁 商店物品
    UnLockShop =100700003,
    
    //解锁 生产
    UnLockProduce =100700006,
    
//    //冒险者Id
//    RiskTakersId = 205000000,
    
    //地图城堡建筑
    MapCastleBuildId = 900000001,
    
    //地图墓园建筑
    MapDeadBuildId = 900000002,
    
    //地图传送阵建筑
    MapPortalBuildId = 900000003,
    
    // 开启新地图建筑 后面第4位是4
    BuildOpenNewMap = 4,
    
    // 需要解锁（2：任务解锁，3：钥匙解锁（万能解锁器可解开））
    BuildLock1Last4 = 2,
    BuildLock2Last4 = 3,
    
    //酒馆 后面第4位
    BuildWineBarIdLast4 = 5,
    
    //市场后面第4位是6
    BuildMarketIdLast4 = 6,

    //竞技场后面第4位
    BuildAthleticsIdLast4 = 7,
    
    //复活配置表Id
    //当前Id 加 复活的兵种等级
    ReviveHero =  904000000,
    
    //招募 壮汉
    RecruitStrongManId = 205001000,
    
    //招募 灵能者
    RecruitPsykerId = 205003000,
    
    //招募 战斗机甲
    RecruitMechaId = 205005000,
    
    //兵工厂建筑Id
    ArsenalId =  100001106,
    
    //虚空竞技场Id
    EternalArena = 900000090,
    
    //拓展包：天启之殇
    BuyMap6ID = 202000001,
    
    //拓展包：腥红审判
    BuyMap8ID = 202000002,
    
    //拓展包：最终进化
    BuyMap10ID = 202000003,
    
    //智能生产线ID
    BuildItlProduceID = 100001200,
    
    //回城能源ID
    BackPackBackHome = 100000024,
    
    // 万能解码器id
    TalentDecompilerID = 100530000,
    
    // 清除第一次战斗技能cd
    TalentEliminateCD = 100520000,
    
};

//多语言相关的Id
enum class LanguageIdEnum
{
    //技能：
    SkillId = 25001,
    
    //生命：
    LifeId = 25002,
    
    //威力：
    PowerId = 25003,
    
    //速度：
    SpeedId = 25004,
    
    //附带效果：
    BufferId = 25005,
    
    //招募一个冒险者！
    Recruit_RiskerId = 25006,
    
    //请选择要升级的兵种！
    OptionHeroId = 25007,
    
    //训练一个%s？
    TrainingId = 25008,
    
    //点数达到%d会获得神秘天赋
    NextAchievementVal = 25009,
    
    //成就点数
    NowAchievementVal = 25010,
    
    //奖励成就点数
    RewardsAchievementVal = 25011,

    //当前进度：
    NowSchedule = 25012,
    
    //你学会了
    LearnXXX = 25013,
    
    //你购买了
    BuyXXX = 25014,
    
    //我方 语言配置Id
    SelfLangId = 25015,
    
    //自动攻击
    AutoAttackLangId = 25016,
    
    //背包容量
    BagSpaceLangId = 25017,
    
    //背包
    BagNameLangId = 25018,
    
    //掉落
    DropOutLangId = 25019,
    
    //关闭
    CloseLangId = 25020,
    
    //全部拾取
    PickUpAllLangId = 25021,
    
    //进入
    EnterLangId = 25022,
    
    //离开
    ExitLangId = 25023,
    
    //回城卷轴购买
    BackHomeId = 25069,
    
    //取消
    CancelLangId = 25024,
    
    //面包语言配置Id
    BreadLangId =  100000002,
    
    //见艰难的选择
    DifficultyOption = 900022,
    
    //你要怎么处理
    HowToDealLangId = 900023,
    
    //复活
    ReviveLangId = 900024,
    
    //消耗
    ConsumeLangid = 900025,
    
    //埋葬
    BuryLangId = 900026,
    
    //愿逝者安息
    DeadPeace = 900027,
    
    //提炼
    CollectLangId = 25025,
    
    //下次收获
    NextHarvest = 25026,
    
    //工匠：
    WorkerLangId = 25027,
    
    //队伍人数
    TeamsNumLangId = 25028,
    
    //回城
    GoHomeLangId = 25029,
    
    //面包：
    MapBreadLangId = 25030,
    
    //探索度：
    ExploreLangId = 25031,
    
    //本图探索：
    MapExploreLangId = 25032,
    
    //学习
    StudyLangId = 25033,
    
    //没兴趣
    NotInterest = 25034,
    
    //怀着悲伤的心情埋葬你的战友，获得
    BuryDescLangId = 900028,
    
    // 你的战友复活的了，消耗
    ReviveDescLangId = 900029,
    
    //缺少
    LackResLangId =  25038,
    
    //获得了
    GetResLangId = 25039,
    
    //冒险失败，您的战士倒下了，
    //您可以在墓园选择复活他们或者埋葬他们
    BattleDeathHomeLangId = 25040,
    
    //在此次冒险中，您带回了
    BattleLiveHomeLangId = 25041,
    
    //在您离开的这段时间，随从们为您生产了
    ProductionItemLangId = 25042,
    
    //壮汉 可进化为禁卫兵
    StrongManEvolutionId = 25043,
    
    //灵能者 可进化为步枪手或剑豪
    PsykerEvolutionId = 25044,
    
    //战斗机甲 可进化为医疗机甲或哨兵机甲
    MechaEvolutionId = 25045,
    
    //背包已满
    BakcPackSpaceNotEnoughId = 25056,
    
    //必须携带食品才能进行探险
    MustCarryFoodLangId = 25057,
    
    //您的队伍必须至少有一名作战单位才能进行探险
    MustCarryHeroLangId = 25058,
    
    //战斗食品回血 食品不足
    BreadNotEnoughId = 25059,
    
    //食品消耗殆尽，您的战友倒下了，您可以在墓园复活或埋葬他们</string>
    BreadHeathLangId = 25060,
    
    //采集 按钮语言Id
    BtnCollectLangId = 25046,
    
    //建造 按钮语言Id
    BtnMakeBuildLangId = 25047,
    
    //交易 按钮语言Id
    BtnTradeLangId = 25048,
    
    //制造 按钮语言Id
    BtnMakeItemLangId = 25049,
    
    //训练 按钮语言Id
    BtnTrainLangId = 25050,
    
    //钻石商店 按钮语言Id
    BtnGemShopLangId = 25051,
    
    //排行榜 按钮语言Id
    BtnRankingListLangId = 25052,
    
    //勋章 按钮语言Id
    BtnMedalLangId = 25053,
    
    //科技 按钮语言Id
    BtnScienceLangId = 25054,
    
    //出发 按钮语言Id
    BtnGoBattleLangId = 25055,
    
    //物品或英雄不够
    NotEnoughLangId = 25061,
    
    //已升级为顶级兵种
    HaveMaxLevelHeroId = 25062,
    
    //队伍已满
    TeamSpaceNotEnoughId = 25063,
    
    //确定 语言Id
    ConfirmLangId = 25064,
    
    //您在此次冒险中丢失了物品
    LoseItems = 25065 ,
    
    //生产 - 消耗：
    ProductionExpend = 25066,
    
    //生产 - 产出：
    ProductionOutput = 25067,
    
    //首次进入剧情
    PlotFirstTime = 25068,
    
    //已达成勋章：xx
    AchievementText = 25076,
    
    // 您还缺少%d钻石，请立即补充钻石
    Diamond_Lack = 25125,
    
    // 消耗%d钻石可立即获得
    Diamond_ToGet = 25127,
    
    // 建造黑市过程中...
    BuildShopGetGem = 25141,
};

//成就类型
enum class AchievementType
{
    //炼金
    Aichemy = 100570,
    
    //探索
    ExploreMap = 100571,
    
    //赌博胜利
    GambleVictory = 100572,
    
    //竞技场层数
    AthleticsLayer = 100573,
    
    //战斗中吃面包
    BattleEatBread = 100574,
    
    //采集
    CollectEvent = 100575,
    
    //埋葬(星级)
    BuryHero  = 100576,
    
    //杀死怪物数
    KillMonster  = 100577,
    
    //学习天赋
    StudyTalent  = 100581,
    
    //击败怪物
    DefeatMonster  = 100582,
    
    //训练兵种
    TrainHero  = 100583,
    
    //完成地图
    FinishMap = 100584,
    
    //目前enum数量为12
    EnumClassMaxVal = 12,

};

enum class TalentCfgId
{
    //提升团队HP
    AdditionHp = 3001,
    //提升攻击伤害
    AdditionAttack = 3002,
    //减少受到伤害
    ReduceDamage = 3003,
    //提升命中率
    AdditionHit = 3004,
    //提升闪避率
    AdditionEvasion = 3005,
    
    //增加探索范围
    AdditionExplore = 3011,
    
    //减少面包消耗（百分比）
    ReduceBread = 3012,
    
    //战斗开始就绪
    BattleReady = 3013,

    
    //减少面包消耗天赋Id
    ReduceBreadId = 100500003
    
};
//职业相关配置
enum class ProCfgId
{
    
    //近战
    MeleeCo = 1001,
    
    //坦克攻击
    FighterCo = 1002,
    
    //法术攻击
    MagicCo = 1003,
    
    //射击攻击
    RangedCo = 1004,
    
    //飞行攻击
    FlyingCo = 1005,
    
    
    CfgBaseId = 908000000,
    
};

//Buffer相关配置
enum class BufferCfgId
{
    //持续伤害 同时存在多个buffer
    //只显示最新的持续伤害buffer时间
    PersistDamage = 100801,
    
    //怪物中这个buffer 怪物攻击减少
    //只保留 怪物攻击减少最多的buffer
    ReduceDamage = 100802,
    
    //眩晕 保留眩晕时间最长的
    Dizziness = 100803,
    
    //吸血
    Vampire = 100804,
    
    //持续恢复
    PersistRecover = 100805
    
};

//技能类型
enum class SkillType
{
    //伤害攻击目标
    AttackTarget = 2001,
    
    //回复己方血量
    HealHeroHP = 2002,
    
    
};

//建筑 通关 状态
enum class BuildPass
{
    NotPass = 1, //不通关
    PassNextMap = 2, //通关并解锁下幅地图
    PassNotNextMap = 3, //通关但不解锁
};

//设置状态
enum class SettingType
{
    SoundEffect         = 1,    // 按键音效
    BackGroundMusic     = 2,    // 背景音乐
    BattleEffect        = 3,    // 战斗特效
};

// 付费点id
enum class BuyProductID
{
	BuyID_First		 = 20047001,
	BuyID_Second	 = 20047002,
	BuyID_Third		 = 20047003,
	BuyID_Fourth 	 = 20047004,

};

// umeng sdk的自定义事件id
enum umengEventID
{
	Use_Diamond_Build 	= 1000,				// 消耗钻石建造建筑
	Use_Diamond_Buy 	= 1001,				// 消耗钻石购买物品
	Use_Diamond_Upgrade = 1002,				// 消耗钻石升级兵种

	Press_Back_Home		= 1003,				// 点击回城: 消耗物品、确定、取消
	Press_Shop			= 1004,				// 点击钻石商店
	Open_TopUp			= 1005,				// 打开充值界面: 点击、非点击（进入界面方式）

};

#endif
