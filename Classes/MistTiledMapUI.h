//
//  MistTiledMap.h
//  TokenGame
//
//  Created by token on 15/4/22.
//
//

#ifndef __TokenGame__MistTiledMap__
#define __TokenGame__MistTiledMap__

#include "UIBase.h"
#include "Common.h"
#include "TiledFourCell.h"

class MistTiledMapUI
{
private:
    
    bool Init();
    
    static MistTiledMapUI* sm_MistTiledMapUI;
    
    MistTiledMapUI& operator = (const MistTiledMapUI& other){return *this;};
    
    
private:
    
    //人物所在瓦片Id
    Point m_PlayVec2;
   
    //总瓦片数量
    uint32_t m_MapTiledNum;
    
    // 瓦片图片gid与瓦片顶点数值总和对应
    vector<int>* m_GidVector;
    
    //存放迷雾数据
    ValueVector* m_MistTiledData = nullptr;
    
     //瓦片坐标和瓦片顶点对象 键值对
    unordered_map<int, TiledFourCell*>* m_TiledCellsMap;
    
    //地图里的瓦片位置
    uint32_t* m_MistTiles = nullptr;
    cocos2d::experimental::TMXLayer* m_MistLayer;
    cocos2d::experimental::TMXTiledMap* m_TMXTiledMap;
    
    // 天赋解开迷雾次数
    int m_CountNum = 0;
    
    void changeCloudTiled4( cocos2d::experimental::TMXLayer* layer, Point pos );
    void changeCloudTiled8( cocos2d::experimental::TMXLayer* layer, Point pos );
    void changeCloudTiled1( cocos2d::experimental::TMXLayer* layer, Point pos );
    void changeCloudTiled2( cocos2d::experimental::TMXLayer* layer, Point pos );
    
    
    TiledFourCell* GetCellByTiledPos(Point pos);
    
    //Point GetMapTiledPos( TMXTiledMap* map, Point pos );
    
    void InitMist(uint32_t mapId, uint32_t mapTiledNum);
    
    void SetGidByTotalNum( cocos2d::experimental::TMXLayer* layer, Point pos, int iTotalNum );
    
    void MovePoint1X1(uint32_t tiledIdx);
    void MovePoint3X3(uint32_t tiledIdx);
    
    // 地图适配，获取正确的计算参数
    // isWidth: true-计算宽度，false-计算高度
    float getProperNum(const float oriNum, bool isWidth);
    
public:
    
    void UnLoad();
    
    bool Load(Layer* layer);
    
    static MistTiledMapUI* getInstance();
    
    //移动地图
    void MovePosition(Vec2& vec);
    
    //移动到指定瓦片索引 tiledIdx:瓦片索引
    void MovePoint(uint32_t tiledIdx);
    
    void InitLayerData(int mapId, Size mapTiledNum, Size tileMapContentSize, Size tileMapVisibleCenterPoint);
    
};

#endif /* defined(__TokenGame__MistTiledMap__) */
