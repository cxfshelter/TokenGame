//
//  TiledMapUI.h
//  TokenGame
//
//  Created by token on 15/3/20.
//
//

#ifndef __TokenGame__TiledMapUI__
#define __TokenGame__TiledMapUI__

#include "UIBase.h"
#include "Common.h"

class TiledMapUI
{

private:
    bool Init();
    
    bool InitTiledData();
    void InitToucheEvent();
    
    void InitLayerData(Layer* rootLayer);
    
    static TiledMapUI* sm_TiledMapUI;

    TiledMapUI& operator = (const TiledMapUI& other){return *this;};
    
private:
    
    Layer* m_RootLayer;
    //玩家图标Id;
    const int m_PlayImgId = 37;
    
    int m_MapCfgId;
    
    int m_MoveMapNum;
    
    Size m_VisibleSize;
    //瓦片地图的大小 字节
    Size m_TileMapContentSize;
    
    //玩家所在瓦片坐标
    Vec2 m_PlayTiledVec2;
    
    //瓦片地图中心坐标
    Size m_TileMapCenterPoint;
    
    //瓦片地图的中心 在可视区中心位置
    Size m_TileMapVisibleCenterPoint;
    
    //地图里的瓦片位置
    uint32_t* m_PlayTiles = nullptr;
    uint32_t* m_FlagTiles = nullptr;
    uint32_t* m_BuildTiles = nullptr;
    uint32_t* m_BackgroundTiles = nullptr;
    
//    TMXLayer* m_PlayLayer;
//    TMXLayer* m_FlagLayer;
//    TMXLayer* m_BuildLayer;
//    TMXLayer* m_BorderLayer;
//    TMXLayer* m_BackgroundLayer;
//    TMXTiledMap* m_TMXTiledMap;
    
    cocos2d::experimental::TMXLayer* m_PlayLayer;
    cocos2d::experimental::TMXLayer* m_FlagLayer;
    cocos2d::experimental::TMXLayer* m_BuildLayer;
    cocos2d::experimental::TMXLayer* m_BackgroundLayer;
    cocos2d::experimental::TMXTiledMap* m_TMXTiledMap;
    
    //保存地图上的 瓦片Id   建筑Id BuildStatus
    unordered_map<int, pair<int, int>>* m_MapBuildStatus;
    
    EventListenerTouchAllAtOnce* m_ListenerTouchAllAtOnce = nullptr;
    
    ////把配置的坐标转成 Rect
    Rect GetCfgRect(int cfgRect);
    
    // OpenGL坐标 转成 格子坐标
    Vec2 GLToTile(const Vec2& gl);
    
    //tile坐标 转成 瓦片格子中心的OpenGL坐标
    Vec2 TileToGL(const Vec2& tile);
    
    //检查当前瓦片是否障碍物，return false:不可前行，true:可继续停留在地图
    bool CheckTiledObstacle(int tiledIdx);
    
    //判断移动到的目标点后  是否看到了地图的地缘
    bool IsMoveToMapBorder(Vec2& position);
    
    //所有建筑随机点
    void AllBuildRandomPoint();
    
    //计算 城堡 墓园 传送阵位置
    void CommonBuildRandomPoint();
    
    //获取一个建筑的随机点
    Vec2 BuildRandomPoint(const ValueVector* vv_Range);
    
    //计算 9 x 9 边上的瓦片 mapSizeWidth：地图宽的瓦片数量
    void Tiled9X9Border(int mapSizeWidth, int centerX, int centerY, int cfgId);
    
    //根据瓦片索引 获取得x, y
    Vec2 PosToTiledXY(int tiledIdx);
    
    //根据x y 获取瓦片的索引
    int GetTiledIdx(int x , int y);
    
    //删除一个未占领的建筑
    void DelNotHoldBuild(int buildId);
    
    
public:

    static TiledMapUI* getInstance();
    
    bool Load(Layer* layer);
    
    void UnLoad();
    
    //启用或停用地图事件
    void SetTouchEnabled(bool enabled);
    
    //建筑状态
    enum class BuildStatus
    {
        //正常状态
        Normal,
        //占领后状态
        Occupy,
        //拜访后状态
        Visit,
    };
    
    //根据瓦片Id 获取得 buildId
    uint32_t GetBuildId(uint32_t tiledIdx);

    
    //获取建筑状态
    BuildStatus GetBuildStatus(uint32_t tiledIdx);
    
    //列新建筑图标
    void UpdateBuildStatus(uint32_t tiledIdx, BuildStatus buildStatus);
    
};

#endif /* defined(__TokenGame__TiledMapUI__) */
