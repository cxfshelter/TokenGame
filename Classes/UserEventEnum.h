//
//  UserEventEnum.h
//  TokenGame
//
//  Created by token on 15/3/16.
//
//

#ifndef  _UserEventEnum_H_
#define  _UserEventEnum_H_

//事件类型
enum class UserEventType
{
    //建造建筑
    MakeBuildEvent = 0,// = 300001,
    
    //获得物品
    GetItemEvent,// = 300002,
    
    //野外占领建筑
    CaptureBuildEvent,// =  300003,
    
    //到达新地图
    GoToNewMapEvent,// = 300004,

    //上面4个的位置不要动
    
    //炼金
    Aichemy,
    
    //获得金币
    AddGold,
    
    //消耗金币
    DelGold,
    
    //更新用户数据
    UpdateUserData,
    
    //采集
    CollectEvent,
    
    //探索
    ExploreMap,
    
    //赌博胜利
    GambleVictory,
    
    //竞技场层数
    AthleticsLayer,
    
    //战斗中吃面包
    BattleEatBread,
    
    //埋葬(星级)
    BuryHero,
    
    //杀死怪物数
    KillMonster,
    
    //学习天赋
    StudyTalent,
    
    //击败怪物
    DefeatMonster,
    
    //训练兵种
    TrainHero,
    
    //完成地图
    //占领指定建筑物
    FinishMap,
    
    //战斗结束
    BattleFinish,
    
    //加载完一个UI
    LoadFinishUI,
    
    //剧情文字播放完毕
    PlotTxtLoadFinish,
    
    EnumClassMaxVal,
};


//事件优先级
enum class UserEventPriority : uint8_t
{
    Low = 0,
    Normal,
    High,
    EnumClassMaxVal
};


#endif /* defined(_UserEventEnum_H_) */
