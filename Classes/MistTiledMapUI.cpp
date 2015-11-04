//
//  MistTiledMap.cpp
//  TokenGame
//
//  Created by token on 15/4/22.
//
//

#include "MistTiledMapUI.h"
#include "BattleData.h"


MistTiledMapUI* MistTiledMapUI::sm_MistTiledMapUI = nullptr;

MistTiledMapUI* MistTiledMapUI::getInstance()
{
    if (sm_MistTiledMapUI == nullptr)
    {
        sm_MistTiledMapUI = new (std::nothrow) MistTiledMapUI();
        if (sm_MistTiledMapUI->Init()) {}
    }
    return sm_MistTiledMapUI;
}

bool MistTiledMapUI::Init()
{
    bool result = false;

    return result;
}

bool MistTiledMapUI::Load(Layer* rootLayer)
{
    
    m_TMXTiledMap = cocos2d::experimental::TMXTiledMap::create("MistTiledMap.tmx");
    if (m_TMXTiledMap != nullptr)
    {
        rootLayer->addChild(m_TMXTiledMap, -1);
        //m_TMXTiledMap->setVisible(false);
        
        m_GidVector= new vector<int>();
        // 以数据值为索引，可以取得瓦片图素
        m_GidVector->push_back(1);//顶点总和0，对应图素ID为1
        m_GidVector->push_back(5);//顶点总和1，对应图素ID为5
        m_GidVector->push_back(9);//顶点总和2，对应图素ID为9
        m_GidVector->push_back(13);//顶点总和3，对应图素ID为13
        m_GidVector->push_back(2);//顶点总和4，对应图素ID为2
        m_GidVector->push_back(6);//顶点总和5，对应图素ID为6
        m_GidVector->push_back(10);
        m_GidVector->push_back(14);
        m_GidVector->push_back(3);
        m_GidVector->push_back(7);
        m_GidVector->push_back(11);
        m_GidVector->push_back(15);
        m_GidVector->push_back(4);
        m_GidVector->push_back(8);
        m_GidVector->push_back(12);
        m_GidVector->push_back(16);
        
        m_TiledCellsMap = new unordered_map<int, TiledFourCell*>();
    }

    return true;
}

void MistTiledMapUI::UnLoad()
{
//    //地图里的瓦片位置
//    delete [] m_MistTiles;
//    m_MistTiles = nullptr;
    
    delete m_GidVector;
    m_GidVector = nullptr;
    
    for (auto tiledFourCel : *m_TiledCellsMap)
    {
        delete tiledFourCel.second;
    }
    delete m_TiledCellsMap;
    
    if( m_TMXTiledMap != nullptr)
    {
        if (m_TMXTiledMap->getParent())
        {
            m_TMXTiledMap->removeFromParent();
        }
        m_TMXTiledMap = nullptr;
    }
}


void MistTiledMapUI::InitLayerData(int mapId, Size mapTiledSize, Size tileMapContentSize, Size tileMapVisibleCenterPoint)
{
    mapTiledSize = mapTiledSize + Size(2, 2);
    m_TMXTiledMap->setMapSize(mapTiledSize);
    
    auto tiledSize = m_TMXTiledMap->getTileSize();
    tileMapContentSize = Size(mapTiledSize.width * tiledSize.width, mapTiledSize.height * tiledSize.height);
    //auto visibleSize = Director::getInstance()->getVisibleSize();
    //地图显示在视屏中心点坐标
    //tileMapVisibleCenterPoint = Size(visibleSize.width /2 , tileMapContentSize.height + visibleSize.height /2);
    
    m_TMXTiledMap->setContentSize(tileMapContentSize);
    m_TMXTiledMap->setAnchorPoint(Vec2(0.5, 0.5));
    
    //迷雾瓦片要偏移半块
//    tileMapVisibleCenterPoint.width += mapTiledSize.width;
//    tileMapVisibleCenterPoint.height += 3 * mapTiledSize.height + 15;
    tileMapVisibleCenterPoint.width += getProperNum(mapTiledSize.width, true);
    tileMapVisibleCenterPoint.height += getProperNum(mapTiledSize.height, false);
    m_TMXTiledMap->setPosition(tileMapVisibleCenterPoint);
    
    m_MistLayer = m_TMXTiledMap->getLayer("MistLayer");
    m_MistLayer->removeTileAt(Vec2(0,0));
    m_MistLayer->setLayerSize(mapTiledSize);

    //迷雾瓦片要偏移半块 这里要多加一块才可以盖住地图
    InitMist(mapId, mapTiledSize.width * mapTiledSize.height);
}

void MistTiledMapUI::InitMist(uint32_t mapId, uint32_t mapTiledNum)
{
    m_MapTiledNum = mapTiledNum;

    auto vm_Mist = &UserData::getInstance()->GetUserData(UserDataEnum::MistMaps)->asValueMap();
    auto strMapId = StringUtils::format("%d", mapId);
    auto iter_Mist = vm_Mist->find(strMapId);
    if (iter_Mist == vm_Mist->end())
    {
        // 尚未开启该地图，将迷雾填充地图
        auto newCenter = 0;
        auto center = m_MapTiledNum / 2;
        auto width = m_MistLayer->getLayerSize().width;
        
        vector<int> mistVec = vector<int>(m_MapTiledNum, 1);
        
        for (auto i = -5; i <= 3; i++)
        {
            newCenter = center + width * i;
            for (auto j = -4; j < 3; j++)
            {
                mistVec[newCenter + j] = 0;
            }
        }
        
//        for (auto i = -3; i <= 3; i++)
//        {
//            newCenter = center + width * i;
//            for (auto j = -3; j <= 3; j++)
//            {
//                mistVec[newCenter + j] = 0;
//                CCLOGERROR("center :%d", newCenter + j);
//            }
//        }
        
        ValueVector saveMist;
        for (auto idx = 0; idx < m_MapTiledNum; idx++)
        {
            if (mistVec.at(idx) == 1)
            {
                saveMist.push_back(Value(idx));
            }
        }
        
        vm_Mist->insert(make_pair(strMapId, Value(saveMist)));
        m_MistTiledData =  &vm_Mist->find(strMapId)->second.asValueVector();
        
    }else
    {
        m_MistTiledData =  &iter_Mist->second.asValueVector();
    }
    
    m_MistTiles = new uint32_t[m_MapTiledNum]();
    for (auto i = 0; i < m_MapTiledNum; i++)
    {
        // GID说明其用的是迷雾的哪个图，1说明是全黑
        m_MistTiles[i] = 1;
    }
    m_MistLayer->setTiles(m_MistTiles);
    m_MistLayer->setupTiles();
    
    for (auto i = 0; i < m_MapTiledNum; i++)
    {
        if (find(m_MistTiledData->begin(), m_MistTiledData->end(), Value(i)) == m_MistTiledData->end())
        {
            // 在保存的该地图迷雾数据中未找到该格子，说明该格子的迷雾已被打开
            MovePoint1X1(i);
        }
    }
    
}

//移动地图
void MistTiledMapUI::MovePosition(Vec2& vec)
{
    //迷雾层 瓦片要偏移 半块
    auto width = m_MistLayer->getLayerSize().width;
    auto height = m_MistLayer->getLayerSize().height;
//    vec.x +=  width;
//    vec.y += height * 3 + 15;
    vec.x += getProperNum(width, true);
    vec.y += getProperNum(height, false);
    
    m_TMXTiledMap->setPosition(vec);
}

//移动到指定瓦片索引 tiledIdx:瓦片索引 isSave:是否存盘
void MistTiledMapUI::MovePoint3X3(uint32_t tiledIdx)
{
    // 计算 以 tiledIdx 为中心格 3X3的格子
    auto w = (int)m_MistLayer->getLayerSize().width;
    m_PlayVec2 = Point(tiledIdx % w, tiledIdx / w);
    
    int left = tiledIdx -1; // 左边
    int right = tiledIdx +1 ; //右边
    int top = tiledIdx - w;//顶部
    int bottom = tiledIdx + w;//底部
    int leftTop = top - 1; // 左上角
    int leftBottom = bottom -1;// 左下角
    int rightTop = top + 1; // 右上角
    int rightBottom = bottom +1; // 右下角
    
    MovePoint1X1(tiledIdx);
    if(left >= 0 && left <= m_MapTiledNum) MovePoint1X1(left);
    if(right >= 0 && right <= m_MapTiledNum) MovePoint1X1(right);
    if(top >= 0 && top <= m_MapTiledNum) MovePoint1X1(top);
    if(bottom >= 0 && bottom <= m_MapTiledNum) MovePoint1X1(bottom);
    if(leftTop >= 0 && leftTop <= m_MapTiledNum) MovePoint1X1(leftTop);
    if(leftBottom >= 0 && leftBottom <= m_MapTiledNum) MovePoint1X1(leftBottom);
    if(rightTop >= 0 && rightTop <= m_MapTiledNum) MovePoint1X1(rightTop);
    if(rightBottom >= 0 && rightBottom <= m_MapTiledNum) MovePoint1X1(rightBottom);
    
    // 天赋增加迷雾解开范围
    while (m_CountNum-- > 0) {
        MovePoint3X3(left);
        MovePoint3X3(right);
        MovePoint3X3(top);
        MovePoint3X3(bottom);
        MovePoint3X3(leftTop);
        MovePoint3X3(leftBottom);
        MovePoint3X3(rightTop);
        MovePoint3X3(rightBottom);
    }
}

// 地图适配，获取正确的计算参数
float MistTiledMapUI::getProperNum(const float oriNum, bool isWidth)
{
    int mapId = UserData::getInstance()->GetUserData(UserDataEnum::PlayerInMapId)->asInt();
    float addNum = 0;
    float mulNum = 0;
    float result = 0;
    
    if (isWidth) {
        addNum = 0;
        if (mapId == 909000004) {
            addNum = -15;
        }
        else if (mapId == 909000005 || mapId == 909000006 || mapId == 909000007) {
            addNum = -15;
        }
        else if (mapId == 909000008 || mapId == 909000009) {
            addNum = -20;
        }
        else if (mapId == 909000010 || mapId == 909000011) {
            addNum = -22;
        }
        else if (mapId == 909000012) {
            addNum = 10;
        }
        result = oriNum + addNum;
    }
    else {
        addNum = 15;
        // 25 * 25
        if (mapId == 909000001) {
            mulNum = 3;
        }
        // 31 * 31
        else if (mapId == 909000002 || mapId == 909000003) {
            mulNum = 2.4;
        }
        // 37 * 37
        else if (mapId == 909000004) {
            mulNum = 2.2;
        }
        // 41 * 41
        else if (mapId == 909000005 || mapId == 909000006 || mapId == 909000007) {
            mulNum = 2;
        }
        // 47 * 47
        else if (mapId == 909000008 || mapId == 909000009) {
            mulNum = 1.5;
        }
        // 51 * 51
        else if (mapId == 909000010 || mapId == 909000011) {
            mulNum = 1.4;
        }
        // 17 * 17
        else if (mapId == 909000012) {
            mulNum = 3.5;
        }
        result = mulNum * oriNum + addNum;
    }
    
    return result;
}

// 移动到指定瓦片索引 tiledIdx:瓦片索引 isSave:是否存盘
// 开始解开迷雾
void MistTiledMapUI::MovePoint(uint32_t tiledIdx)
{
    // 根据天赋获得迷雾解开偏移量
    m_CountNum = BattleData::getInstance()->m_TalentExplore;
    
    //每行相差2格
    auto width = (int)m_MistLayer->getLayerSize().width;
    
    tiledIdx += (tiledIdx / ( width - 2)) * 2;
    MovePoint3X3(tiledIdx);
    
//    UserData::getInstance()->SaveUserData();
    UserData::getInstance()->SaveCorUserData(true);
}

//移动到指定瓦片索引 tiledIdx:瓦片索引 isSave:是否存盘
void MistTiledMapUI::MovePoint1X1(uint32_t tiledIdx)
{
    auto w = (int)m_MistLayer->getLayerSize().width;
    Vec2 vec2(tiledIdx % w, tiledIdx / w);
    
    /* 设置当前瓦片的数据值，以及设置旁白瓦片的数据值 */
    changeCloudTiled4(m_MistLayer, Point(vec2.x, vec2.y));
    changeCloudTiled8(m_MistLayer, Point(vec2.x + 1, vec2.y));
    changeCloudTiled1(m_MistLayer, Point(vec2.x, vec2.y + 1));
    changeCloudTiled2(m_MistLayer, Point(vec2.x + 1, vec2.y + 1));

    // 因为移动该瓦片块已被解开迷雾，所以迷雾数组要将其去除
    auto iter_Mist = find(m_MistTiledData->begin(), m_MistTiledData->end(), Value((int)tiledIdx));
    if (iter_Mist != m_MistTiledData->end())
    {
        m_MistTiledData->erase(iter_Mist);
    }
    
}
//
//Point MistTiledMapUI::GetMapTiledPos(TMXTiledMap* map, Point pos )
//{
//    Size mapSize = map->getMapSize();
//    Size tiledSize = map->getTileSize();
//    
//    int iMapHeight = mapSize.height * tiledSize.height;
//    
//    /* pos为笛卡尔坐标系的坐标，所以y轴需要修正 */
//    int x = pos.x / tiledSize.width;
//    int y = (iMapHeight - pos.y) / tiledSize.height;
//    
//    return Point(x, y);
//}

TiledFourCell* MistTiledMapUI::GetCellByTiledPos(Point pos)
{
    Size mapSize = m_MistLayer->getLayerSize();//m_TMXTiledMap->getMapSize();
    
    /* 瓦片坐标原本是二维坐标，转换为一维数值 */
    int index = mapSize.width * pos.x + pos.y;
    
    TiledFourCell* cell;
    auto tiledCell = m_TiledCellsMap->find(index);
    if(tiledCell == m_TiledCellsMap->end())
    {
        /* 如果该瓦片没有顶点数据对象，则创建一个顶点数据值默认都为0 */
        cell = new TiledFourCell();
        m_TiledCellsMap->insert(make_pair(index, cell));
    }
    else
    {
        cell = tiledCell->second;
    }
    return cell;
}

void MistTiledMapUI::changeCloudTiled4( cocos2d::experimental::TMXLayer* layer, Point pos )
{
    
    TiledFourCell* cell = GetCellByTiledPos(pos);
    
    /* 数值4，在瓦片右下角 */
    cell->setiRightBottom(4);
    
    /* 根据瓦片4个顶点之和设置地图瓦片的图片 */
    SetGidByTotalNum(layer, pos, cell->getiTotalNum());
    
}

void MistTiledMapUI::changeCloudTiled8( cocos2d::experimental::TMXLayer* layer, Point pos )
{
    TiledFourCell* cell = GetCellByTiledPos(pos);
    
    /* 数值8，在瓦片左下角 */
    cell->setiLeftBottom(8);
    
    /* 根据瓦片4个顶点之和设置地图瓦片的图片 */
    SetGidByTotalNum(layer, pos, cell->getiTotalNum());
    
}

void MistTiledMapUI::changeCloudTiled1( cocos2d::experimental::TMXLayer* layer, Point pos )
{
    TiledFourCell* cell = GetCellByTiledPos(pos);
    
    /* 数值1，在瓦片右上角 */
    cell->setiRightTop(1);
    
    /* 根据瓦片4个顶点之和设置地图瓦片的图片 */
    SetGidByTotalNum(layer, pos, cell->getiTotalNum());
}

void MistTiledMapUI::changeCloudTiled2( cocos2d::experimental::TMXLayer* layer, Point pos )
{
    TiledFourCell* cell = GetCellByTiledPos(pos);
    
    /* 数值2，在瓦片左上角 */
    cell->setiLeftTop(2);
    
    /* 根据瓦片4个顶点之和设置地图瓦片的图片 */
    SetGidByTotalNum(layer, pos, cell->getiTotalNum());
}

void MistTiledMapUI::SetGidByTotalNum( cocos2d::experimental::TMXLayer* layer, Point pos, int iTotalNum )
{
    Size mapSize = m_MistLayer->getLayerSize();//m_TMXTiledMap->getMapSize();
    
    /* 避免超出范围 */
    
    if(pos.x < 0 || pos.y < 0)
    {
        return;
    }
    
    if(pos.x >= mapSize.width || pos.y >= mapSize.height)
    {
        return;
    }
    
    //iTotalNum是瓦片的4个顶点数据值的总和，
    //通过这个值获得对应的瓦片图素
    auto gid = m_GidVector->at(iTotalNum);

    /* 用新的瓦片图素替换pos坐标上的瓦片图素 */
    layer->setTileGID(gid, pos);
}
