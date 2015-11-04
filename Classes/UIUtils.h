//
//  SlideLayer.h
//  TokenGame
//
//  Created by best on 15/5/7.
//
//

#ifndef __TokenGame__UIUtils__
#define __TokenGame__UIUtils__

#include "ui/CocosGui.h"
#include "RecordUtils.h"
#include "EnunClass.h"

class UIBase;

class UIUtils
{
public:
    static UIUtils* getInstance();
    
    /*   添加监听左右滑动事件的layer以及相应事件
     */
    void addSlideEvent(cocos2d::Node * rootNode, std::function<void()> leftEvent, std::function<void()> rightEvent);
    
    /*   添加长按处理批量购买的layer
     */
    void addBuyBatches(cocos2d::ui::Button * btn, UIBase* uiBase, int cfgId);
    
    /*   添加长按处理批量购买的layer
     */
    void addMakeBatches(cocos2d::ui::Button * btn, UIBase* uiBase, int cfgId);
    
    /*   处理批量购买
     */
    void handleBuy(int cfgId, int num, UIBase* uiBase);
    
    /*   处理批量制造
     *   （无需调用界面刷新，因为可以批量制造的都是不可升级的）
     */
    void handleMake(int cfgId, int num, UIBase* uiBase);
    
    /*   判断物品所需数量是否足够
     */
    bool isStuffEnough(int cfgId, int needNum);
    
    /*   显示人物、怪物职业图标
     *   isSmall，true: 32x32，false: 44x44
     */
    void showProperImg(cocos2d::Node * properNode, int cfgId, bool isSmall = true);
    
    /*   真正显示职业图标
     */
    void showRealProper(cocos2d::Node * properNode, int proEnum, bool isSmall);
    
    /*   野外遇怪提示
     */
    void showCountryFightTxt(cocos2d::Node * rootNode);
    
    /*   建筑占领前描述
     *   isShow为true：与建筑状态无关，每次进去前都要显示占领前描述
     */
    void mapBuildingDialog(cocos2d::Node * rootNode, uint32_t tiledIdx, uint32_t buildId, const std::function<void()> & comfirmBack, const std::function<void()> & cancelBack, bool isShow);
    
    /*   添加android手机的back监听键
     */
    void addBackLister(cocos2d::Layer * layerNode);
    
    /*   勋章达成时，弹出文案
     */
    void showAchievementMoveText(int achieveId);
    
    /*   加载plist，先将游戏界面所有图片一次性加载（1024 * 2048 * 4 = 8MB）
     */
    void plistImgInit();
    
    ///////////////////////////
    /*   加入购买提示层
         cfgIds为提示缺少的物品cfgId
         buyEvent		扣除钻石成功执行事件
         eventID 		umeng sdk自定义事件id
     */
    void addLeadBuyLayer(UIBase* uiBase, std::string showText, int totalDiamonds, std::function<void()> buyEvent, umengEventID eventID = (umengEventID)0);
    
    void addLeadBuyMapLayer(UIBase* uiBase, int curMapID, std::function<void()> buyEvent);
    
    /*   获取所有升级的物品所需的钻石数量
     */
    int  getTotalCostDiamond(const cocos2d::ValueMapIntMultiKey* vmi);
    
    /*   获取缺少升级的物品所需的钻石数量
     */
    int  getLackCostDiamond(const std::vector<std::pair<int, int>>* lackData);
    
    /*   消耗非钻石购买的升级所需物品（钻石升级）
     */
    void costUpgNeededItem(const cocos2d::ValueMapIntMultiKey* vmi, const std::vector<std::pair<int, int>>* lackData);
    
    /*   添加手势滑动的界面及移动手势方向
         showUIName: "MainUI.csd"
         isRight - true:向右, false:向左
     */
    void syncLeadHandShowUI(std::string showUIName, bool isRight, bool isAdd = true);
    
    void checkShowLeadHand(cocos2d::Node * rootWidget);
    
public:
    std::function<void()> m_leftEvent;
    std::function<void()> m_rightEvent;
    
private:
    // 填充批量购买层
    void fillWithBatchesBuyLayout(UIBase* uiBase, int cfgId);
    
    // 填充批量制造层
    void fillWithBatchesMakeLayout(UIBase* uiBase, int cfgId);
    
private:
    static UIUtils* m_Instance;
    
    bool m_isPressFirst = false;
    
    cocos2d::Layer * m_CurLayer = nullptr;
    
    // <显示的界面cdb文件名, 是否向右>，etc: <"MainUI.csd", true>
    cocos2d::ValueMap* m_leadHandMap = nullptr;
    
};

// 左右滑动层
class SlideLayer : public cocos2d::Layer
{
public:
    SlideLayer();
    ~SlideLayer();

private:
    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event  *event);
    void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event  *event);

private:
    cocos2d::Vec2 m_touchStartPoint;
    
};

#endif /* defined(__TokenGame__UIUtils__) */
