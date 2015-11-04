//
//  MapUI.h
//  TokenGame
//
//  Created by token on 15/3/20.
//
//

#ifndef __TokenGame__MapUI__
#define __TokenGame__MapUI__

#include "UIBase.h"
#include "Common.h"

class MapUI : public UIBase
{

private:
    bool Init();
    static MapUI* sm_MapUI;

    MapUI& operator = (const MapUI& other){return *this;};
protected:
    

    virtual void UpdateUI();
    
    virtual void LoadFinish();
    
private:

    int32_t m_MapId = 0;
    
    //当前地图总探索值
    uint32_t m_MapTotalExplore = 0;
    
    //移动的瓦片数量 用于计算消耗面包
    uint16_t m_BreadNowMoveTiledNum = 0;
    
    //移动多少瓦片 消耗一个面包
    uint32_t m_BreadMaxMoveTiledNum = 0;
    
    //更新地图探索数据 exploreVal:增加探索值
    void UpdateMapExplore(int addExploreVal = -1);
    
    //显示离开对话框
    void ShowExitDialog(uint32_t titleLangId, uint32_t descLangId);
    
    // 红遮罩相关
    int opacityNum = 255;
    bool isUp = false;
    
    // 剧情相关
    int m_index = 0;
    void addPlotMsg(string msg, Vec2 originPos);
    
    // 野外商店物品购买数量
    unordered_map<int, unordered_map<int, int>> * m_StoreBuyNumMap;
    
    //显示缺少钥匙不能进入建筑界面
    // isLock：是否已解锁；
    void ShowLackKeyUI(int tiled, int buildId, vector<pair<int, int> >* lackData, bool isLock, bool isOccupy);
    
    // 进入解锁相关界面，返回是否成功解锁
    void GoUnLockUI(int tiled, int buildId);
    
    // 进入解锁建筑，删除解锁物品
    void DeleteUnLockItem(int buildId);
    
    // 检测是否需要购买地图扩展包
    bool checkIsBuyMap(int curMapId);
    
public:

    virtual void UnLoad();
    
    static MapUI* getInstance();
    
    void ReleaseInstance();
    
    // isDirect: true直接回城不消耗卷轴
    void GoMainUI(bool isDirect);
    
    //显示面包数量
    void ShowBreadNum();
    
    //显示野外占领建筑后文字提示
    void ShowBattleSucUI(int buildId, bool isSuccess);
    
    //更新面包数量
    //反回是否有足够的面包
    bool UpdateBreadNum();
    
    //判断是否解锁
    bool IsUnLockBuild(int buildId,vector<pair<int, int> >* lackItemData);
    
    //是否显示红色遮罩（面包小于5）
    void ShowRedBlood(bool isShow);
    
    //重置野外商店购买次数
    void ResetMarketBuyNum();
    
    /*  设置野外商店对应物品购买次数
     *  num: -1，说明购买一次
     *  返回: 购买过后剩余的次数; (-1: 并无该物品)
     */
    int SetMarketBuyNum(int storeId, int fieldStoreID, int num);
    
    //更新背包容量
    void UpdateBackPack();
    
    void HoldBuild(uint32_t tiledIdx ,uint32_t cfgId);
    
    void OtherBuild(uint32_t tiledIdx ,uint32_t cfgId);
    
    void PortalBuild(uint32_t tiledIdx, uint32_t cfgId);
    
    void ShowOptionMap(uint32_t tiledIdx,uint32_t cfgId);
    
    void DeathBuild(uint32_t tiledIdx ,uint32_t cfgId);
    
    void MarketBuild(uint32_t tiledIdx ,uint32_t cfgId);

    void WineBarBuild(uint32_t tiledIdx ,uint32_t cfgId);
    
    void MonsterBuild(uint32_t tiledIdx ,uint32_t cfgId);
    
    //随机一个永恒竞技场怪物
    int RandomEternalArenaMonsetr();
    
    //计算一个随机怪物 cfgId怪物出现表配置Id layerId层数
    int RandomMonsetr(int cfgId, int layerId);
    
    // 缺少物品数据lackData，大小是需要解锁物品的数量，pair:id, num
    // 只要背包对应解锁物品不足，lackData就会被插入数据，故若lackData为空，说明背包有足够的解锁物品
    void CheckBackPackUnLoackItem(const ValueMapIntMultiKey* unLockItem, vector<pair<int, int> >* lackData);
    
    //在地图上碰到了障碍 buildIdOrEnemyId true是建筑Id false 敌方Id
    void MapObstacle(uint32_t tiledIdx, uint32_t cfgId, bool buildIdOrEnemyId);
    //显示战斗对话框
    void ShowBattleDialog(uint32_t tiledIdx, uint32_t cfgId, uint16_t nowBuileLayer);
    
    enum class MoveDirection
    {
        Left,
        Right,
        Up,
        Down
    };
    
    void HideMoveDirection();
    Layout* GetLayoutDirection();
    void ShowMoveDirection(MoveDirection md);
    
};

#endif /* defined(__TokenGame__MapUI__) */
