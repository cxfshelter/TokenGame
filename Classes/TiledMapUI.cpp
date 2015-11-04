//
//  TiledMapUI.cpp
//  TokenGame
//
//  Created by token on 15/3/20.
//
//

#include "MapUI.h"
#include "TiledMapUI.h"
#include "RandomEngine.h"
#include "MistTiledMapUI.h"


TiledMapUI* TiledMapUI::sm_TiledMapUI = nullptr;


TiledMapUI* TiledMapUI::getInstance()
{
    if (sm_TiledMapUI == nullptr)
    {
        sm_TiledMapUI = new (std::nothrow) TiledMapUI();
        if (sm_TiledMapUI->Init()) {}
    }
    return sm_TiledMapUI;
}

bool TiledMapUI::Init()
{
    bool result = false;
    m_MapBuildStatus = new (std::nothrow) unordered_map<int, pair<int, int>>();
    return result;
}

bool TiledMapUI::Load(Layer* rootLayer)
{
    bool result = false;
    
    m_MapCfgId = UserData::getInstance()->GetUserData(UserDataEnum::PlayerInMapId)->asInt();
    
    auto vm_ExploreMaps = &UserData::getInstance()->GetUserData(UserDataEnum::ExploreMaps)->asValueMap();
    if (UserData::getInstance()->Get_MVI_Value(vm_ExploreMaps, m_MapCfgId) == Value::Null)
    {
        UserData::getInstance()->Update_VMI_Fun(vm_ExploreMaps, m_MapCfgId, 0, false , true , true , true);
    }
    
    //m_TMXTiledMap = TMXTiledMap::create("TiledMap.tmx");
    m_TMXTiledMap = cocos2d::experimental::TMXTiledMap::create("TiledMap.tmx");
    if (m_TMXTiledMap != nullptr)
    {
        result =  true;
        m_RootLayer = rootLayer;
        //-1 层次
        rootLayer->addChild(m_TMXTiledMap, -2);
        
        MistTiledMapUI::getInstance()->Load(rootLayer);
        
        //顺序不要变动
        InitLayerData(rootLayer);
		InitTiledData();
        InitToucheEvent();
        
        // 记录：玩家在地图
        UserData::getInstance()->Update_Value(UserDataEnum::IsPlayerInMap, Value(true));
    }
    
    return result;
}

void TiledMapUI::UnLoad()
{
//    //地图里的瓦片位置
//    delete [] m_PlayTiles;
//    m_PlayTiles = nullptr;
//    delete [] m_FlagTiles;
//    m_FlagTiles = nullptr;
//    delete [] m_BuildTiles;
//    m_BuildTiles = nullptr;
//    delete [] m_BackgroundTiles;
//    m_BackgroundTiles = nullptr;
    
    m_MapBuildStatus->clear();
    
    if (m_TMXTiledMap != nullptr && m_TMXTiledMap->getReferenceCount() > 0)
    {
        m_TMXTiledMap->getEventDispatcher()->removeAllEventListeners();
        
        m_TMXTiledMap->unscheduleAllCallbacks();
        //会自动删除 各层 Tiles
        if (m_TMXTiledMap->getParent()) m_TMXTiledMap->removeFromParent();
        m_ListenerTouchAllAtOnce = nullptr;
        m_TMXTiledMap = nullptr;
    }
    MistTiledMapUI::getInstance()->UnLoad();
    
    //记录：玩家不在地图
    UserData::getInstance()->Update_Value(UserDataEnum::IsPlayerInMap, Value(false));
}


void TiledMapUI::SetTouchEnabled(bool enabled)
{
    if (enabled)
    {
        if (m_ListenerTouchAllAtOnce == nullptr)
        {
            InitToucheEvent();
        }
    }else
    {
        if (m_ListenerTouchAllAtOnce != nullptr)
        {
            m_TMXTiledMap->getEventDispatcher()->removeEventListener(m_ListenerTouchAllAtOnce);
            m_ListenerTouchAllAtOnce = nullptr;
        }
    }
}

//根据瓦片Id 获取得 buildId
uint32_t TiledMapUI::GetBuildId(uint32_t tiledIdx)
{
    uint32_t buildId = 0;
    auto iter = m_MapBuildStatus->find(tiledIdx);
    if (iter == m_MapBuildStatus->end())
    {
        CCLOGERROR("MapId:%d Error tiledIdx:%d", m_MapCfgId, tiledIdx);
    }else
    {
        buildId = iter->second.first;
    }
    return buildId;
}

void TiledMapUI::InitToucheEvent()
{
    if (m_TMXTiledMap == nullptr) return;
    
    m_ListenerTouchAllAtOnce = EventListenerTouchAllAtOnce::create();
    m_ListenerTouchAllAtOnce->onTouchesMoved = [this](const std::vector<Touch*>& touches, Event *event)
    {
        if (m_MoveMapNum > 0)
        {
            return ; //点击移动完才能再次移动
        }
        
        auto moveSize = touches[0]->getDelta();
        auto mapPos = m_TMXTiledMap->getPosition();
        auto move_TargetPos = mapPos + moveSize;

        IsMoveToMapBorder( move_TargetPos);
        m_TMXTiledMap->setPosition(move_TargetPos);
        MistTiledMapUI::getInstance()->MovePosition(move_TargetPos);
    };
    
    m_ListenerTouchAllAtOnce->onTouchesEnded = [this](const std::vector<Touch*>& touches, Event * event)
    {
        auto touch = touches[0];
        //判断是移动还是点击
//        if (touch->getLocation() != touch->getStartLocation()) return ;
        auto touchMoveLength = touch->getLocation().distance(touch->getStartLocation());
        if (touchMoveLength >= 30) {
            return;
        }
        
        if (m_MoveMapNum > 0)
        {
            return ; //移动完才能再次移动
            //m_TMXTiledMap->unschedule("MoveTileMap");
        }
        
        auto location = touch->getLocation();
        auto mapPos = m_TMXTiledMap->getPosition();
        auto tiledSize = m_TMXTiledMap->getTileSize();
        auto playSprite = m_PlayLayer->getTileAt(m_PlayTiledVec2);
        auto playSpritePos = m_PlayLayer->convertToWorldSpace(playSprite->getPosition());
        playSpritePos = CCDirector::getInstance()->convertToUI(playSpritePos);
        // 地图方向箭头显示强制适配
        playSpritePos.x += 30;
        playSpritePos.y += 90;
        
        auto layoutDirection = MapUI::getInstance()->GetLayoutDirection();
        layoutDirection->setPosition(playSpritePos);
        
        auto xAxisData = UserData::getInstance()->GetUserData(UserDataEnum::PlayerInMapXAxis);
        auto yAxisData = UserData::getInstance()->GetUserData(UserDataEnum::PlayerInMapYAxis);
        if (xAxisData != nullptr && yAxisData != nullptr)
        {
            Vec2 vec2(xAxisData->asFloat(), yAxisData->asFloat());
            if (mapPos.x != vec2.x || mapPos.y != vec2.y)
            {
                m_TMXTiledMap->setPosition(vec2);
                MistTiledMapUI::getInstance()->MovePosition(vec2);
                return ;
            }
        }

        //计算移动方向
        auto offset_location = location - m_VisibleSize / 2;

        auto offsetPos = Size::ZERO;
        
        if (abs(offset_location.x) > abs(offset_location.y))
        {
            if (offset_location.x < 0)
            {
                offsetPos.width = tiledSize.width;
            }
            
            if (offset_location.x > 0)
            {
                offsetPos.width = -tiledSize.width;
            }
        }else
        {
            if (offset_location.y < 0)
            {
                offsetPos.height = tiledSize.height;
            }
            
            if (offset_location.y > 0)
            {
                offsetPos.height = -tiledSize.height;
            }
        }
        
        CCLOGERROR("offset width:%f height:%f",offsetPos.width, offsetPos.height);
        
        auto move_TargetPos = mapPos + offsetPos;
        
        IsMoveToMapBorder(move_TargetPos);
        
        offsetPos = move_TargetPos - mapPos;
        
        if (abs(offsetPos.width) < 2 && abs(offsetPos.height) < 2) return ;
        
        auto newPlayTiledVec2 = m_PlayTiledVec2;
        MapUI::MoveDirection moveDirection;
        if (offsetPos.width < 0)
        {
            newPlayTiledVec2.x += 1;
            moveDirection = MapUI::MoveDirection::Right;
            
        }else if( offsetPos.width > 0)
        {
            newPlayTiledVec2.x -= 1;
            moveDirection = MapUI::MoveDirection::Left;
        }
        
        if (offsetPos.height < 0)
        {
            newPlayTiledVec2.y -= 1;
            moveDirection = MapUI::MoveDirection::Up;
            
        }else if( offsetPos.height > 0)
        {
            newPlayTiledVec2.y += 1;
            moveDirection = MapUI::MoveDirection::Down;
        }
        
        CCLOGERROR("tiled width:%f height:%f",newPlayTiledVec2.x, newPlayTiledVec2.y);
        
        if (newPlayTiledVec2.x >= 0 && newPlayTiledVec2.x < m_TMXTiledMap->getMapSize().width
            && newPlayTiledVec2.y >= 0 && newPlayTiledVec2.y < m_TMXTiledMap->getMapSize().height)
        {
            //移动人物
            m_PlayLayer->removeTileAt(m_PlayTiledVec2);
            m_PlayTiledVec2 = newPlayTiledVec2;
            m_PlayLayer->setTileGID(m_PlayImgId, m_PlayTiledVec2);
            MapUI::getInstance()->ShowMoveDirection(moveDirection);
        }
        
        m_MoveMapNum = 0;
        m_TMXTiledMap->schedule([this, offsetPos, newPlayTiledVec2](float f)
        {
            auto map_location = m_TMXTiledMap->getPosition();
            map_location += offsetPos / 10;
            m_TMXTiledMap->setPosition(map_location);
            
            MistTiledMapUI::getInstance()->MovePosition(map_location);
            m_MoveMapNum++;
            //延时检测障碍 10 帧
            if (m_MoveMapNum >= 10)
            {
                SoundUtils::getInstance()->playMusicSound(WALK_EFFECT, false);
                m_MoveMapNum = 0;
                m_TMXTiledMap->unschedule("MoveTileMap");
                MapUI::getInstance()->HideMoveDirection();
                
                if (newPlayTiledVec2.x >= 0 && newPlayTiledVec2.x < m_TMXTiledMap->getMapSize().width
                    && newPlayTiledVec2.y >= 0 && newPlayTiledVec2.y < m_TMXTiledMap->getMapSize().height)
                {
                    map_location = m_TMXTiledMap->getPosition();
                    
                    auto tiledIdx = GetTiledIdx(m_PlayTiledVec2.x, m_PlayTiledVec2.y);
                    MistTiledMapUI::getInstance()->MovePoint(tiledIdx);
                    if(!CheckTiledObstacle(tiledIdx)) return;
                    
                    UserData::getInstance()->Update_Value(UserDataEnum::PlayerInMapTiledId, Value(tiledIdx), false);
                    UserData::getInstance()->Update_Value(UserDataEnum::PlayerInMapXAxis, Value(map_location.x), false);
                    UserData::getInstance()->Update_Value(UserDataEnum::PlayerInMapYAxis, Value(map_location.y));
                }
            }
        }, 0.03, "MoveTileMap");
    };
    
    m_TMXTiledMap->getEventDispatcher()->addEventListenerWithSceneGraphPriority(m_ListenerTouchAllAtOnce, m_TMXTiledMap);
}

void TiledMapUI::InitLayerData(Layer* rootLayer)
{
    CfgDataRow dataRow(m_MapCfgId);
    //地图的瓦片数量
    int tileNum = dataRow.GetValue(CfgField::MapSize)->asInt();
    float width = tileNum / 100;
    float height = tileNum % 100;
    
    const Size mapTileNum(width, height);
    m_TMXTiledMap->setMapSize(mapTileNum);
    
    auto tileSize = m_TMXTiledMap->getTileSize();
    m_TileMapContentSize = Size(m_TMXTiledMap->getMapSize().width * tileSize.width,
                                m_TMXTiledMap->getMapSize().height * tileSize.height);
    m_TMXTiledMap->setContentSize(m_TileMapContentSize);
    m_TMXTiledMap->setAnchorPoint(Vec2(0.5, 0.5));
    // 地图中心点是 m_TileMapContentSize.width /2  m_TileMapContentSize.height / 2;
    m_TileMapCenterPoint = Size(m_TileMapContentSize.width /2, m_TileMapContentSize.height / 2);
    
    // 640 * 960, 因为之前调用了setDesignResolutionSize(640, 960, FIXED_WIDTH)
    m_VisibleSize = Director::getInstance()->getVisibleSize();
    //地图显示在视屏中心点坐标
    m_TileMapVisibleCenterPoint = Size(m_VisibleSize.width /2 , m_TileMapContentSize.height + m_VisibleSize.height /2);
    auto newTileMapVisibleCenterPoint = m_TileMapVisibleCenterPoint;
    auto mapTiledIdData = UserData::getInstance()->GetUserData(UserDataEnum::PlayerInMapTiledId);
    if (mapTiledIdData != nullptr && mapTiledIdData->asInt() != -1)
    {
        auto xAxisData = UserData::getInstance()->GetUserData(UserDataEnum::PlayerInMapXAxis);
        auto yAxisData = UserData::getInstance()->GetUserData(UserDataEnum::PlayerInMapYAxis);
        if (xAxisData != nullptr && yAxisData != nullptr)
        {
            newTileMapVisibleCenterPoint.width = xAxisData->asFloat();
            newTileMapVisibleCenterPoint.height = yAxisData->asFloat();
        }
    }else
    {
        UserData::getInstance()->Update_Value(UserDataEnum::PlayerInMapXAxis, Value(newTileMapVisibleCenterPoint.width), false);
        UserData::getInstance()->Update_Value(UserDataEnum::PlayerInMapYAxis, Value(newTileMapVisibleCenterPoint.height));
    }
    
    m_TMXTiledMap->setPosition(newTileMapVisibleCenterPoint);
    
    m_PlayLayer = m_TMXTiledMap->getLayer("PlayLayer");
    m_FlagLayer = m_TMXTiledMap->getLayer("FlagLayer");
    m_BuildLayer = m_TMXTiledMap->getLayer("BuildLayer");
    m_BackgroundLayer = m_TMXTiledMap->getLayer("BackgroundLayer");
    
    m_PlayLayer->removeTileAt(Vec2(0,0));
    m_FlagLayer->removeTileAt(Vec2(0,0));
    m_BuildLayer->removeTileAt(Vec2(0,0));
    m_BackgroundLayer->removeTileAt(Vec2(0,0));
    
    m_PlayLayer->setLayerSize(mapTileNum);
    m_FlagLayer->setLayerSize(mapTileNum);
    m_BuildLayer->setLayerSize(mapTileNum);
    m_BackgroundLayer->setLayerSize(mapTileNum);
    
    MistTiledMapUI::getInstance()->InitLayerData(m_MapCfgId, mapTileNum, m_TileMapContentSize, newTileMapVisibleCenterPoint);
}

//跟据建筑数据 初始化瓦片数据
bool TiledMapUI::InitTiledData()
{
    auto mapSize = m_TMXTiledMap->getMapSize();
    auto mapTileNum = (int)(mapSize.width * mapSize.height);
    //玩家层数据
    m_PlayTiles = new uint32_t[mapTileNum]();
    
    //玩家进入地图时人物出现的中心点 初始瓦片坐标
    auto centerX = (int)mapSize.width / 2 ;
    auto centerY = (int)mapSize.height / 2;
    
    //玩家进入地图后的 初始瓦片坐标
    m_PlayTiledVec2 = Vec2(centerX , centerY);
    auto tiledIdx = GetTiledIdx(centerX ,centerY);
    
    auto mapTiledIdData = UserData::getInstance()->GetUserData(UserDataEnum::PlayerInMapTiledId);
    if (mapTiledIdData != nullptr && mapTiledIdData->asInt() != -1)
    {
        tiledIdx = mapTiledIdData->asInt();
        m_PlayTiledVec2 = PosToTiledXY(tiledIdx);
    }
    
    m_PlayTiles[tiledIdx] = m_PlayImgId; //人物图标
    
    m_PlayLayer->setTiles(m_PlayTiles);
    m_PlayLayer->setupTiles();
    
    //建筑位置和标记位置
    m_FlagTiles = new uint32_t[mapTileNum]();
    m_BuildTiles = new uint32_t[mapTileNum]();
    auto buildIcon = 0;
    CommonBuildRandomPoint();//计算 城堡 墓园 传送阵位置
    AllBuildRandomPoint();
    
    for (auto iter = m_MapBuildStatus->begin(); iter != m_MapBuildStatus->end(); iter++)
    {
        CfgDataRow cfgDataRow(iter->second.first);
        
        //显示图标
        if(iter->second.second == (int)BuildStatus::Normal)
        {
            buildIcon = cfgDataRow.GetValue(CfgField::BoBuildingIconID)->asInt();
            m_BuildTiles[iter->first] = buildIcon;
        }else
        {
            auto iconType = cfgDataRow.GetValue(CfgField::IconType)->asInt();
            buildIcon = cfgDataRow.GetValue(CfgField::AoBuildingIconID)->asInt();
            
            if (iconType == 2)
            {
                m_FlagTiles[iter->first] = buildIcon;
                buildIcon = cfgDataRow.GetValue(CfgField::BoBuildingIconID)->asInt();
                m_BuildTiles[iter->first] = buildIcon;
            }else
            {
                m_BuildTiles[iter->first] = buildIcon;
            }
        }
    }
    
    m_FlagLayer->setTiles(m_FlagTiles); m_FlagLayer->setupTiles();
    m_BuildLayer->setTiles(m_BuildTiles); m_BuildLayer->setupTiles();
    
    m_BackgroundTiles = new uint32_t[mapTileNum]();
    
    CfgDataRow dataRow(m_MapCfgId);
    auto defaultImgId = dataRow.GetValue(CfgField::MapIconId)->asInt();
    
    for (auto i = 0; i < mapTileNum; i++)
    {
        m_BackgroundTiles[i] = defaultImgId;
    }
    m_BackgroundLayer->setTiles(m_BackgroundTiles);
    m_BackgroundLayer->setupTiles();
    
    return true;
}

//所有建筑随机点
void TiledMapUI::AllBuildRandomPoint()
{
    CfgDataRow dataRow(m_MapCfgId);
    
    auto mapBuild = dataRow.GetValue(CfgField::MapBuilding_S)->asString();
    auto tableIdList = CfgData::getInstance()->GetCfgTableId(mapBuild);
    
    Vec2 vec2;
    int mapPos;
    int cfgBuildId =0;
    BuildStatus buildStatus;
    const ValueVector* vv_Range;
    
    
    auto vm_MapNotHoldBuild = &UserData::getInstance()->GetUserData(UserDataEnum::MapNotHoldBuild)->asValueMap();
    auto mapData = vm_MapNotHoldBuild->find(StringUtils::format("%d", m_MapCfgId));
    if (mapData == vm_MapNotHoldBuild->end())
    {
        CCLOGERROR("MapNotHoldBuild Not MapId:%d", m_MapCfgId);
    }
    auto map_NotHoldBuild = &mapData->second.asValueVector();
    
    for (auto iter = tableIdList->begin(); iter != tableIdList->end(); iter ++)
    {
        cfgBuildId = *iter;
        CfgDataRow dataRow(cfgBuildId);
        vv_Range = &dataRow.GetValue(CfgField::Range_V)->asValueVector();
        
        //随机次数
        int randomNum = 0;
        //如果随机5次还是被战用不再随机
        while (randomNum < 5)
        {
            randomNum ++;
            vec2 = BuildRandomPoint(vv_Range);
            mapPos = vec2.x + m_TMXTiledMap->getMapSize().width * vec2.y;
            if (mapPos >= m_TMXTiledMap->getMapSize().width * m_TMXTiledMap->getMapSize().height)
            {
                CCLOGERROR("Build CfgId:%d Range_V Error ", cfgBuildId); continue;
            }
            if (m_MapBuildStatus->find(mapPos) != m_MapBuildStatus->end()) continue;
            
            if( find(map_NotHoldBuild->begin(), map_NotHoldBuild->end(), Value(cfgBuildId)) == map_NotHoldBuild->end())
            {
                buildStatus = BuildStatus::Occupy;
                
            }else
            {
                buildStatus = BuildStatus::Normal;
            }
            m_MapBuildStatus->insert(pair<int, pair<int, int>>(mapPos, pair<int, int>(cfgBuildId, (int)buildStatus)));
            break;
        }
        if (randomNum == 5)
        {
            CCLOGERROR("Build CfgId:%d Range_V Error Repeat", cfgBuildId);
        }
    }
    
}


//获取建筑随机点
Vec2 TiledMapUI::BuildRandomPoint(const ValueVector* vv_Range)
{
    Rect cfgRect;
    auto randomX = 0;
    auto randomY = 0;
    if (vv_Range->size() == 1)
    {
        cfgRect = GetCfgRect(vv_Range->at(0).asInt());
        
    }else
    {
        auto rrs = RandomEngine::getInstance()->GetRandomValue(0, (uint32_t)vv_Range->size() - 1);
        cfgRect = GetCfgRect(vv_Range->at(rrs).asInt());
    }
    
    randomX = RandomEngine::getInstance()->GetRandomValue(cfgRect.origin.x, cfgRect.origin.x + cfgRect.size.width);
    randomY = RandomEngine::getInstance()->GetRandomValue(cfgRect.origin.y, cfgRect.origin.y + cfgRect.size.height);
    
    return Vec2(randomX, randomY);
}

//计算 城堡 墓园 传送阵位置
void TiledMapUI::CommonBuildRandomPoint()
{
    auto mapSize = m_TMXTiledMap->getMapSize();
    
    // x >=0 x < mapSize.width
    auto centerX = (int)mapSize.width / 2 ;//+  (int)mapSize.width % 2;
    auto centerY = (int)mapSize.height / 2; //+  (int)mapSize.height % 2;
    
    auto castleBuildId = (int)CfgIdEnum::MapCastleBuildId;
    auto buildImg = CfgData::getInstance()->GetValue(castleBuildId, CfgField::BoBuildingIconID)->asInt();
    
    auto tiledIdx =  GetTiledIdx(centerX, centerY); m_BuildTiles[tiledIdx] = buildImg;
    
    m_MapBuildStatus->insert(pair<int, pair<int, int>>(tiledIdx, pair<int, int>(castleBuildId, (int)BuildStatus::Normal)));
    
    RandomEngine::getInstance()->SetRandomSeed(m_MapCfgId);
    
    Tiled9X9Border((int)mapSize.width , centerX, centerY, (int)CfgIdEnum::MapDeadBuildId);
    Tiled9X9Border((int)mapSize.width , centerX, centerY, (int)CfgIdEnum::MapPortalBuildId);
}

bool TiledMapUI::CheckTiledObstacle(int tiledIdx)
{
    if(!MapUI::getInstance()->UpdateBreadNum()) return false;
    
    auto iter = m_MapBuildStatus->find(tiledIdx);
    if (iter != m_MapBuildStatus->end())
    {
        SetTouchEnabled(false);
        auto builedId = iter->second.first;
        MapUI::getInstance()->MapObstacle(tiledIdx, builedId, true);
        
    }else
    {
        CfgDataRow dataRow(m_MapCfgId);
        auto monsterPro = dataRow.GetValue(CfgField::MonsterPro)->asInt();
        //是否遇到怪
        auto reVal = RandomEngine::getInstance()->GetDefaultRandomValue(1, 1000);
        CCLOGERROR("random monster %d", reVal);
        if (reVal < monsterPro )
        {
            
            string monsterTable = dataRow.GetValue(CfgField::MapMonster_S)->asString();
            auto tableIdList = CfgData::getInstance()->GetCfgTableId(monsterTable);
            
            int cfgId = 0;
            int totalWeights = 0;
            vector<pair<int, int>> vec_pair;
            for (auto iter = tableIdList->begin(); iter != tableIdList->end(); iter++)
            {
                cfgId = *iter;
                auto w = CfgData::getInstance()->GetValue(cfgId, CfgField::Fmeetpro)->asInt();
                if (w <= 0) continue;
                totalWeights += w;
                vec_pair.push_back(pair<int, int>(cfgId, w));
            }
            
            uint32_t randomSeed = RandomEngine::getInstance()->GetDefaultRandomSeed();
            cfgId = RandomEngine::getInstance()->GetRandomWeights(vec_pair, totalWeights, randomSeed);
            
            SetTouchEnabled(false);
            MapUI::getInstance()->MapObstacle(tiledIdx, cfgId, false);
        }
    }
    return true;

}

//把配置的地图坐标转成 vec2
Rect TiledMapUI::GetCfgRect(int cfgRect)
{
    //Rect(float x, float y, float width, float height);
    float x = cfgRect / 1000000;
    float y = cfgRect / 10000 % 100;
    float w = cfgRect / 100 % 100;
    float h = cfgRect % 100;
    return Rect(x, y, w, h);
}

// OpenGL坐标 转成 格子坐标
Vec2 TiledMapUI::GLToTile(const Vec2& gl)
{
    Size mapSize = m_TMXTiledMap->getMapSize();
    Size tileSize = m_TMXTiledMap->getTileSize();
    int x = gl.x / tileSize.width;
    int y = (mapSize.height * tileSize.height - gl.y) / tileSize.height;
    return Vec2(x, y);
}

//getPositionAt 通过瓦片坐标获得像素坐标
// tile坐标 转成 瓦片格子中心的OpenGL坐标
Vec2 TiledMapUI::TileToGL(const Vec2& tile)
{
    Size mapSize = m_TMXTiledMap->getMapSize();
    Size tileSize = m_TMXTiledMap->getTileSize();
    int x = tile.x * tileSize.width + tileSize.width/2;
    int y = (mapSize.height - tile.y) * tileSize.height - tileSize.height/2;
    return Vec2(x, y);
}

//判断移动到的目标点后  是否看到了地图的地缘
bool TiledMapUI::IsMoveToMapBorder(Vec2& position)
{
    bool result = false;
    auto x = 0.0f, y = 0.0f;
    //左边
    x = m_TileMapVisibleCenterPoint.width + m_TileMapCenterPoint.width;// - m_VisibleSize.width /5 ;
    if( position.x >= x)
    {
        result = true;
        position.x = x;
    }
    
    //右边
    x = m_TileMapVisibleCenterPoint.width - m_TileMapCenterPoint.width;// + m_VisibleSize.width/ 5 ;
    if( position.x <= x)
    {
        result = true;
        position.x = x;
    }

    //上边
    y = m_TileMapVisibleCenterPoint.height - m_TileMapCenterPoint.height;// + m_VisibleSize.height / 5;
    if( position.y <= y )
    {
        result = true;
        position.y = y;
    }
    
    //下边
    y = m_TileMapVisibleCenterPoint.height + m_TileMapCenterPoint.height;// - m_VisibleSize.height / 5 ;
    if( position.y >= y)
    {
        result = true;
        position.y = y;
    }
    return result;
}

//计算 9 x 9 边上的瓦片 mapSizeWidth：地图宽的瓦片数量
void TiledMapUI::Tiled9X9Border(int mapSizeWidth, int centerX, int centerY, int cfgId)
{
    //随机9格
    int tiled9 = 0;
    //以中心点 为中心  随机4条边 0:左 1:右 2:上 3:下
    int border4 = 0;
    
    auto wnum = 0;
    auto tiledIdx = 0;
    while (wnum <=32)
    {
        wnum ++;
        tiled9 = RandomEngine::getInstance()->GetRandomValue(0, 6);
        border4 = RandomEngine::getInstance()->GetRandomValue(0, 3);
        switch (border4)
        {
            case 0:
                centerX -= 3;
                centerY += tiled9 - 3;
                break;
            case 1:
                centerX += 3;
                centerY += tiled9 - 3;
                break;
            case 2:
                centerX += tiled9 - 3;
                centerY -= 3;
                break;
            case 3:
                centerX += tiled9 - 3;
                centerY += 3;
                break;
            default:
                break;
        }
        tiledIdx = GetTiledIdx(centerX ,centerY);
        if (m_MapBuildStatus->find(tiledIdx) == m_MapBuildStatus->end())
        {
            m_MapBuildStatus->insert(pair<int, pair<int, int>>(tiledIdx, pair<int, int>(cfgId, (int)BuildStatus::Normal)));
            break;
        }
    }

}


//根据x y 获取瓦片的索引
int GetTiledIdx(int x , int y);

//根据瓦片索引 获取得x y
Vec2 TiledMapUI::PosToTiledXY(int tiledIdx)
{
    auto w = (int)m_TMXTiledMap->getMapSize().width;
    return Vec2(tiledIdx % w, tiledIdx / w);
    
}
//根据x y 获取瓦片的索引
int TiledMapUI::GetTiledIdx(int x , int y)
{
    return x + (int)m_TMXTiledMap->getMapSize().width * y;
}

//删除一个未占领的建筑
void TiledMapUI::DelNotHoldBuild(int buildId)
{
    
    //auto mapId = UserData::getInstance()->GetUserData(UserDataEnum::PlayerInMapId)->asInt();
    auto strMapId = StringUtils::format("%d", m_MapCfgId);
    
    auto vm_MapNotHoldBuild = &UserData::getInstance()->GetUserData(UserDataEnum::MapNotHoldBuild)->asValueMap();
    
    auto iter = vm_MapNotHoldBuild->find(strMapId);
    if (iter == vm_MapNotHoldBuild->end())
    {
        CCLOGERROR("Map:%d MapNotHoldBuild Data Error", m_MapCfgId);
    }else
    {
        auto vv = &iter->second.asValueVector();
        auto buildIter = find(vv->begin(), vv->end(), Value(buildId));
        if (buildIter != vv->end())
        {
            vv->erase(buildIter);
//            UserData::getInstance()->SaveUserData();
            UserData::getInstance()->SaveCorUserData(true);
        }
    }
}

//获取建筑状态
TiledMapUI::BuildStatus TiledMapUI::GetBuildStatus(uint32_t tiledIdx)
{
    BuildStatus status = BuildStatus::Visit;
    auto iter = m_MapBuildStatus->find(tiledIdx);
    if (iter != m_MapBuildStatus->end())
    {
        switch (iter->second.second)
        {
            case (int)BuildStatus::Normal:
                status = BuildStatus::Normal;
                break;
            case (int)BuildStatus::Occupy:
                status = BuildStatus::Occupy;
                break;
            default:
                status = BuildStatus::Visit;
                break;
        }
    }
    return status;
}

void TiledMapUI::UpdateBuildStatus(uint32_t tiledIdx, BuildStatus buildStatus)
{
    //UserData::getInstance()->GetUserData(UserDataEnum::MapNotHoldBuild)->asValueMap()
    auto iter = m_MapBuildStatus->find(tiledIdx);
    if (iter != m_MapBuildStatus->end())
    {
        iter->second.second = (int)buildStatus;
        
        auto buildId = iter->second.first;
        
        CfgDataRow dataRow(buildId);
        
        auto iconType = dataRow.GetValue(CfgField::IconType)->asInt();
        
        auto buildIcon = -1;
        switch (buildStatus)
        {
            case BuildStatus::Occupy :
            {
                DelNotHoldBuild(buildId);
                buildIcon = dataRow.GetValue(CfgField::AoBuildingIconID)->asInt();
                break;
            }
            case BuildStatus::Visit :
            {
                buildIcon = dataRow.GetValue(CfgField::AvBuildingIconID )->asInt();
                break;
            }
            default:
                break;
        }
        
        if (buildIcon != -1)
        {
            if (iconType == 2)
            {
                m_FlagLayer->setTileGID(buildIcon, PosToTiledXY(tiledIdx));
                buildIcon = dataRow.GetValue(CfgField::BoBuildingIconID)->asInt();
                m_BuildLayer->setTileGID(buildIcon, PosToTiledXY(tiledIdx));
                
            }else
            {
                m_BuildLayer->setTileGID(buildIcon, PosToTiledXY(tiledIdx));
            }
        }

    }
    
    
}



