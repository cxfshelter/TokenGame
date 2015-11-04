//
//  SlideLayer.cpp
//  TokenGame
//
//  Created by best on 15/5/7.
//
//

#include "UIUtils.h"
#include "cocostudio/CocoStudio.h"
#include "CfgData.h"
#include "CfgDataRow.h"
#include "UserData.h"
#include "UserEventContainer.h"
#include "MultiLanguage.h"
#include "MsgListNode.h"
#include "TiledMapUI.h"
#include "Network.h"

USING_NS_CC;

#define LAYER_ZORDER        100 // 被添加层的层级
#define MOVE_X_SIZE         150	// 左右滑动判定最小距离（水平方向）
#define MOVE_Y_SIZE         75	// 左右滑动判定最小距离（垂直方向）
#define LONG_BTN            0.8	// 长按判定最低时间

const char * LAYER_NAME = "SlideLayer";
const char * LAYER_BUY_NAME = "BatchesLayer";

UIUtils* UIUtils::m_Instance = nullptr;

UIUtils* UIUtils::getInstance()
{
    if (m_Instance == nullptr) {
        m_Instance = new (std::nothrow) UIUtils();
    }
    return m_Instance;
}

void UIUtils::addSlideEvent(Node * rootNode, std::function<void()> leftEvent, std::function<void()> rightEvent)
{
    if (!rootNode) {
        return;
    }
    
    auto addedLayer = rootNode->getChildByName(LAYER_NAME);
    if (addedLayer == nullptr) {
        addedLayer = new (std::nothrow)SlideLayer();
        addedLayer->autorelease();
        addedLayer->retain();
        rootNode->addChild(addedLayer);
        addedLayer->setContentSize(rootNode->getContentSize());
    }
    m_leftEvent = leftEvent;
    m_rightEvent = rightEvent;
}

void UIUtils::addBuyBatches(cocos2d::ui::Button * btn, UIBase* uiBase, int cfgId)
{
    if (btn == nullptr) {
        return;
    }
    
    btn->addTouchEventListener(
       [=](Ref* pSender, cocos2d::ui::Widget::TouchEventType type)
       {
           Node * node = btn->getChildByName("BatchesBuy");
           if (!node) {
               node = Node::create();
               node->setName("BatchesBuy");
               btn->addChild(node);
           }
           
           switch (type) {
               case cocos2d::ui::Widget::TouchEventType::BEGAN:
                   node->schedule(
                   [=](float dt)
                    {
                        btn->setTag(BTN_IS_LONGBTN);
                        node->unschedule("BATCHES_BUY");
                        fillWithBatchesBuyLayout(uiBase, cfgId);
                    }, LONG_BTN, "BATCHES_BUY");
                   break;
               case cocos2d::ui::Widget::TouchEventType::MOVED:
                   break;
               default:
                   node->unschedule("BATCHES_BUY");
                   break;
           }
           
       });
    
}

void UIUtils::addMakeBatches(cocos2d::ui::Button * btn, UIBase* uiBase, int cfgId)
{
    if (btn == nullptr) {
        return;
    }
    
    CfgDataRow dataRow(cfgId);
    // 非可升级
    if(dataRow.GetValue(CfgField::UpGradable)->asInt() != 1)
    {
        btn->addTouchEventListener(
           [=](Ref* pSender, cocos2d::ui::Widget::TouchEventType type)
           {
               Node * node = btn->getChildByName("BatchesMake");
               if (!node) {
                   node = Node::create();
                   node->setName("BatchesMake");
                   btn->addChild(node);
               }
               
               switch (type) {
                   case cocos2d::ui::Widget::TouchEventType::BEGAN:
                       node->schedule(
                                      [=](float dt)
                                      {
                                          btn->setTag(BTN_IS_LONGBTN);
                                          node->unschedule("BATCHES_MAKE");
                                          fillWithBatchesMakeLayout(uiBase, cfgId);
                                      }, LONG_BTN, "BATCHES_MAKE");
                       break;
                   case cocos2d::ui::Widget::TouchEventType::MOVED:
                       break;
                   default:
                       node->unschedule("BATCHES_MAKE");
                       break;
               }
           });
    }
}

void UIUtils::fillWithBatchesBuyLayout(UIBase* uiBase, int cfgId)
{
    auto rootNode = uiBase->m_RootWidget;
    
    auto addedLayout = rootNode->getChildByName<cocos2d::ui::Layout *>("Layout_Confirm");
    if (!addedLayout) {
        return;
    }
    addedLayout->setVisible(true);
    
    auto btn_buy10 = addedLayout->getChildByName<cocos2d::ui::Button *>("Btn_Buy1");
    auto btn_buy100 = addedLayout->getChildByName<cocos2d::ui::Button *>("Btn_Buy2");
    auto btn_buy1000 = addedLayout->getChildByName<cocos2d::ui::Button *>("Btn_Buy3");
    auto btn_close = addedLayout->getChildByName<cocos2d::ui::Button *>("Btn_Cancel");
    
    btn_buy10->addClickEventListener(
                                     [=](Ref* sender)
                                     {
                                         handleBuy(cfgId, 10, uiBase);
                                         addedLayout->setVisible(false);
                                     });

    btn_buy100->addClickEventListener(
                                     [=](Ref* sender)
                                     {
                                         handleBuy(cfgId, 100, uiBase);
                                         addedLayout->setVisible(false);
                                     });
    
    btn_buy1000->addClickEventListener(
                                     [=](Ref* sender)
                                     {
                                         handleBuy(cfgId, 1000, uiBase);
                                         addedLayout->setVisible(false);
                                     });
    
    btn_close->addClickEventListener(
                                    [=](Ref* sender)
                                     {
                                         addedLayout->setVisible(false);
                                     });
    
}

void UIUtils::fillWithBatchesMakeLayout(UIBase* uiBase, int cfgId)
{
    auto rootNode = uiBase->m_RootWidget;
    
    auto addedLayout = rootNode->getChildByName<cocos2d::ui::Layout *>("Layout_Confirm");
    if (!addedLayout) {
        return;
    }
    addedLayout->setVisible(true);
    
    auto btn_buy10 = addedLayout->getChildByName<cocos2d::ui::Button *>("Btn_Buy1");
    auto btn_buy100 = addedLayout->getChildByName<cocos2d::ui::Button *>("Btn_Buy2");
    auto btn_buy1000 = addedLayout->getChildByName<cocos2d::ui::Button *>("Btn_Buy3");
    auto btn_close = addedLayout->getChildByName<cocos2d::ui::Button *>("Btn_Cancel1");
    
    btn_buy10->addClickEventListener(
                                     [=](Ref* sender)
                                     {
                                         handleMake(cfgId, 10, uiBase);
                                         addedLayout->setVisible(false);
                                     });
    
    btn_buy100->addClickEventListener(
                                      [=](Ref* sender)
                                      {
                                          handleMake(cfgId, 100, uiBase);
                                          addedLayout->setVisible(false);
                                      });
    
    btn_buy1000->addClickEventListener(
                                       [=](Ref* sender)
                                       {
                                           handleMake(cfgId, 1000, uiBase);
                                           addedLayout->setVisible(false);
                                       });
    
    btn_close->addClickEventListener(
                                     [=](Ref* sender)
                                     {
                                         addedLayout->setVisible(false);
                                     });
}

void UIUtils::handleBuy(int cfgId, int num, UIBase* uiBase)
{
    auto rootWidget = uiBase->m_RootWidget;
    
    ValueMapIntMultiKey temp_Vmi_Price;
    auto vmi = CfgData::getInstance()->GetCfgRow(cfgId);
    if (vmi == nullptr) return;
    
    auto vmi_Price = &CfgData::getInstance()->GetValue(vmi, cfgId, CfgField::PriceID_M)->asIntMultiKeyMap();
    for (auto iter_price = vmi_Price->begin(); iter_price != vmi_Price->end(); iter_price++)
    {
        if (iter_price->first == (int)CfgIdEnum::GemId)
        {
            
        }else
        {
            temp_Vmi_Price.insert(pair<int, Value>(iter_price->first, Value(iter_price->second.asInt() * num)));
        }
    }
    vector<pair<int, int> > lackData;
    bool delResutl = UserData::getInstance()->DeleteStorehouseData(&temp_Vmi_Price, &lackData);
    CfgDataRow dataRow(cfgId);
    if (delResutl)
    {
        if (dataRow.GetValue(CfgField::Uniqueness)->asInt() == 1)
        {
            UserData::getInstance()->Add_VI_Fun(UserDataEnum::OnlyItemList, cfgId);
        }
        UserData::getInstance()->Update_VMI_Fun(UserDataEnum::Storehouse, cfgId, num);
        UserEventContainer::getInstance()->Send(UserEventType::GetItemEvent, cfgId);
        auto langId = dataRow.GetValue(CfgField::Schedule)->asInt();
        auto langText = MultiLanguage::getInstance()->GetText(langId);
        langText = StringUtils::format(langText.c_str(), num);
        MsgListNode::getInstance()->NewMsg(langText);
    }else
    {
        // 弹出引导钻石消费界面
        int totalNum = UIUtils::getInstance()->getLackCostDiamond(&lackData);
        string showText = StringUtils::format(MultiLanguage::getInstance()->GetText((int)LanguageIdEnum::Diamond_ToGet).c_str(), totalNum) + MultiLanguage::getInstance()->GetText(dataRow.GetValue(CfgField::Name)->asInt());
        UIUtils::getInstance()->addLeadBuyLayer(uiBase, showText, totalNum,
                                                [=]()
                                                {
                                                    costUpgNeededItem(&temp_Vmi_Price, &lackData);
                                                    CfgDataRow dataRow(cfgId);
                                                    if (dataRow.GetValue(CfgField::Uniqueness)->asInt() == 1)
                                                    {
                                                        UserData::getInstance()->Add_VI_Fun(UserDataEnum::OnlyItemList, cfgId);
                                                    }
                                                    UserData::getInstance()->Update_VMI_Fun(UserDataEnum::Storehouse, cfgId, num);
                                                    UserEventContainer::getInstance()->Send(UserEventType::GetItemEvent, cfgId);
                                                    auto langId = dataRow.GetValue(CfgField::Schedule)->asInt();
                                                    auto langText = MultiLanguage::getInstance()->GetText(langId);
                                                    langText = StringUtils::format(langText.c_str(), num);
                                                    MsgListNode::getInstance()->NewMsg(langText);
                                                });
        
        MoveText::getInstance()->LackResText(rootWidget, &lackData);
    }
}

void UIUtils::handleMake(int cfgId, int num, UIBase* uiBase)
{
    auto rootWidget = uiBase->m_RootWidget;
    
    CfgDataRow dataRow(cfgId);
    auto vmi_Price = dataRow.GetValue(CfgField::MaterialID_M)->asIntMultiKeyMap();
    for (auto iter = vmi_Price.begin(); iter != vmi_Price.end(); ++iter)
    {
        iter->second = Value(iter->second.asInt() * num);
    }
    
    vector<pair<int, int> > lackData;
    bool delResutl = UserData::getInstance()->DeleteStorehouseData(&vmi_Price, &lackData);
    if (delResutl)
    {
        UserData::getInstance()->Update_VMI_Fun(UserDataEnum::Storehouse, cfgId, num);
        UserEventContainer::getInstance()->Send(UserEventType::GetItemEvent, cfgId);
        MsgListNode::getInstance()->NewMsg(cfgId, num, CfgField::Schedule);
    }else
    {
        // 弹出引导钻石消费界面
        int totalNum = UIUtils::getInstance()->getLackCostDiamond(&lackData);
        string showText = StringUtils::format(MultiLanguage::getInstance()->GetText((int)LanguageIdEnum::Diamond_ToGet).c_str(), totalNum) + MultiLanguage::getInstance()->GetText(dataRow.GetValue(CfgField::Name)->asInt());
        UIUtils::getInstance()->addLeadBuyLayer(uiBase, showText, totalNum,
                                                [=]()
                                                {
                                                    costUpgNeededItem(&vmi_Price, &lackData);
                                                    UserData::getInstance()->Update_VMI_Fun(UserDataEnum::Storehouse, cfgId, num);
                                                    UserEventContainer::getInstance()->Send(UserEventType::GetItemEvent, cfgId);
                                                    MsgListNode::getInstance()->NewMsg(cfgId, num, CfgField::Schedule);
                                                });
        
        MoveText::getInstance()->LackResText(rootWidget, &lackData);
    }
}

bool UIUtils::isStuffEnough(int cfgId, int needNum)
{
    bool isEnough = true;

    auto curItemNum = 0;
    auto vm_Shorehouse = &UserData::getInstance()->GetUserData(UserDataEnum::Storehouse)->asValueMap();
    // 遍历得到其仓库数量
    for (auto iter = vm_Shorehouse->begin(); iter != vm_Shorehouse->end(); iter++)
    {
        if (cfgId == atoi(iter->first.c_str())) {
            curItemNum = iter->second.asInt();
            break;
        }
    }
    // 如果数量不足，置灰
    if (curItemNum < needNum) {
        isEnough = false;
    }
    
    return isEnough;
}

/*          PropertyID
            近战 1001
            肉盾	1002
            法师	1003
            远程	1004
            飞行	1005
            显示职业图标
 */
void UIUtils::showProperImg(cocos2d::Node * parentNode, int cfgId, bool isSmall)
{
    if(cfgId / 1000000 != 205) {
        // 非兵种id
        showRealProper(parentNode, 0, isSmall);
        return;
    }
    
    CfgDataRow dataRow(cfgId);
    auto properId = dataRow.GetValue(CfgField::PropertyID)->asInt();
    showRealProper(parentNode, properId, isSmall);
}

void UIUtils::showRealProper(cocos2d::Node * parentNode, int proEnum, bool isSmall)
{
    auto properNode = parentNode->getChildByName("PropertyNode");
    if (!properNode) {
        properNode = CSLoader::createNode("PropertyNode.csb");
        properNode->setName("PropertyNode");
        parentNode->addChild(properNode);
    }
    
    ImageView* img_Pro = nullptr;
    auto layout_proper = properNode->getChildByName("Layout_Proper");
    for (int proId = 1001; proId <= 1005; ++proId) {
        img_Pro = layout_proper->getChildByName<ImageView*>(StringUtils::format("Img_Pro%d", proId));
        if(img_Pro) img_Pro->setVisible(false);
    }
    
    img_Pro = layout_proper->getChildByName<ImageView*>(StringUtils::format("Img_Pro%d", proEnum));
    if(img_Pro) {
        img_Pro->setVisible(true);
        if (isSmall) {
            img_Pro->setContentSize(Size(32, 32));
        }
        else {
            img_Pro->setContentSize(Size(44, 44));
        }
    }
    
}

void UIUtils::showCountryFightTxt(cocos2d::Node * rootNode)
{
    Text * text = Text::create();
    text->setString("遇到怪物！");
    
    
    text->runAction(
                    CCSequence::create(CCScaleTo::create(2, 2),
                                       NULL));
    
}

void UIUtils::mapBuildingDialog(cocos2d::Node * rootNode, uint32_t tiledIdx, uint32_t buildId, const std::function<void()> & comfirmBack, const std::function<void()> & cancelBack, bool isShow)
{
    if (!rootNode) {
        return;
    }
    
    auto buildStatus = TiledMapUI::getInstance()->GetBuildStatus(tiledIdx);
    if (buildStatus == TiledMapUI::BuildStatus::Normal || isShow) {
        // 未占领 或者 个别建筑
        auto layout_BuildDialog = rootNode->getChildByName<Layout*>("Layout_BuildDialog");
        if(!layout_BuildDialog) return;
        layout_BuildDialog->setVisible(true);
        
        CfgDataRow dataRow(buildId);
        
        auto text_BuildName = layout_BuildDialog->getChildByName<Text*>("Text_BuildName");
        auto langId = dataRow.GetValue(CfgField::Name)->asInt();
        auto langText = MultiLanguage::getInstance()->GetText(langId);
        text_BuildName->setString(langText);
        
        auto text_Desc = layout_BuildDialog->getChildByName<Text*>("Text_Desc");
        langId = dataRow.GetValue(CfgField::BodescribeId)->asInt();
        langText = MultiLanguage::getInstance()->GetText(langId);
        text_Desc->setString(langText);
        
        auto btn_Enter = layout_BuildDialog->getChildByName<Button*>("Btn_Enter");
        langId = (int)LanguageIdEnum::EnterLangId;
        langText = MultiLanguage::getInstance()->GetText(langId);
        btn_Enter->setTitleText(langText);
        btn_Enter->addClickEventListener(
                                         [comfirmBack, layout_BuildDialog](Ref* sender)
                                         {
                                             layout_BuildDialog->setVisible(false);
                                             if(comfirmBack) comfirmBack();
                                         });
        
        auto btn_Exit = layout_BuildDialog->getChildByName<Button*>("Btn_ExitBuild");
        langId = (int)LanguageIdEnum::ExitLangId;
        langText = MultiLanguage::getInstance()->GetText(langId);
        btn_Exit->setTitleText(langText);
        btn_Exit->addClickEventListener([cancelBack, layout_BuildDialog](Ref* sender)
                                        {
                                            layout_BuildDialog->setVisible(false);
                                            if(cancelBack) cancelBack();
                                        });
        
    }
    else if(buildStatus == TiledMapUI::BuildStatus::Occupy) {
        // 已占领
        if(comfirmBack) comfirmBack();
    }
    
}

void UIUtils::addBackLister(cocos2d::Layer * layer)
{
    if (!layer) {
        return;
    }
    m_CurLayer = layer;
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return;
#endif
    
    auto keyListener = EventListenerKeyboard::create();
    keyListener->onKeyReleased = [=](EventKeyboard::KeyCode key, Event*)
    {
        // 触发返回键，不同android手机的返回键的码可能不一样
        if (EventKeyboard::KeyCode::KEY_RETURN == key || EventKeyboard::KeyCode::KEY_ESCAPE == key || EventKeyboard::KeyCode::KEY_BACKSPACE == key) {
            
            Node * node = m_CurLayer->getChildByName<Node *>("ScheduleNode");
            if (!node) {
                node = Node::create();
                node->setName("ScheduleNode");
                m_CurLayer->addChild(node);
            }
            
            if (m_isPressFirst) {
                m_isPressFirst = false;
                node->unschedule("ExitGame");
                
                RecordUtils::getInstance()->sdkExitGame();

                UserData::getInstance()->SaveUserData();
                Director::getInstance()->end();
                return;
            }
            else {
                m_isPressFirst = true;
            }
            
            // 2秒内连续按back键退出游戏
            node->schedule(
                            [=](float t)
                            {
                                m_isPressFirst = false;
                                node->unschedule("ExitGame");
                                return;
                            },
                            2, "ExitGame");
            
            MoveText::getInstance()->AddDefaultTextData(m_CurLayer, "再按一次退出游戏");
        }
    };
    m_CurLayer->getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyListener, m_CurLayer);
    
}

void UIUtils::showAchievementMoveText(int achieveId)
{
    if (achieveId == 0 || !m_CurLayer) {
        return;
    }
    
    // 勋章达成时，弹出达成文案语言栏
    auto langText = MultiLanguage::getInstance()->GetText((int)LanguageIdEnum::AchievementText);
    
    CfgDataRow dataRow(achieveId);
    auto langId = dataRow.GetValue(CfgField::Name)->asInt();
    langText = langText + MultiLanguage::getInstance()->GetText(langId);
    
    MoveText::getInstance()->AddDefaultTextData(m_CurLayer, langText);
}

void UIUtils::plistImgInit()
{
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("allnewui.plist");
}

void UIUtils::addLeadBuyLayer(UIBase* uiBase, string showText, int totalDiamonds, std::function<void()> buyEvent, umengEventID eventID)
{
    auto rootWidget = uiBase->m_RootWidget;
    if (totalDiamonds <= 0) return;
    
    if(UserData::getInstance()->GetUserData(UserDataEnum::ShopItem)->asValueMap().empty()) return;
    
    const char * LAYER_NAME = "leadBuyLayer";
    
    auto leadBuyLayer = rootWidget->getChildByName(LAYER_NAME);
    if (!leadBuyLayer) {
        leadBuyLayer = CSLoader::createNode("LeadBuyLayer.csb");
        leadBuyLayer->setName(LAYER_NAME);
        rootWidget->addChild(leadBuyLayer);
    }
    leadBuyLayer->getChildByName("Layout_LeadBuyMap")->setVisible(false);
    leadBuyLayer->setVisible(true);
    
    auto layout_leadBuy = leadBuyLayer->getChildByName("Layout_LeadBuy");
    if (!layout_leadBuy) return;
    layout_leadBuy->setVisible(true);
    
    auto Text_ShowText = layout_leadBuy->getChildByName<Text *>("Text_Desc");
    Text_ShowText->setString(showText);
    
    auto Btn_Cancel = layout_leadBuy->getChildByName<Button *>("Button_Cancel");
    Btn_Cancel->addClickEventListener(
                                      [=](Ref* sender)
                                      {
                                          layout_leadBuy->setVisible(false);
                                          leadBuyLayer->setVisible(false);
                                          if(eventID != 0)
                                          {
                                        	  RecordUtils::getInstance()->sdkRecord((int)eventID, "取消");
                                          }
                                      });
    
    auto Btn_Buy = layout_leadBuy->getChildByName<Button *>("Button_Buy");
    Btn_Buy->setTitleText(StringUtils::format("%d钻石", totalDiamonds));
    Btn_Buy->addClickEventListener(
                                   [=](Ref* sender)
                                   {
                                       layout_leadBuy->setVisible(false);
                                       leadBuyLayer->setVisible(false);
                                       if(DiamondBuyData::getInstance()->costDiamond(totalDiamonds))
                                       {
                                           // 扣除钻石成功，则执行传入buyEvent
                                           buyEvent();
                                       }
                                       else
                                       {
                                           uiBase->UnLoad();
                                           // 扣除钻石失败，跳至充值界面
                                           DiamondBuyData::getInstance()->goToTopUp(uiBase->m_ResName);
                                       }
                                       
                                       if(eventID != 0)
                                       {
                                    	   RecordUtils::getInstance()->sdkRecord((int)eventID, "确定");
                                       }
                                   });
    
}

void UIUtils::addLeadBuyMapLayer(UIBase* uiBase, int curMapID, std::function<void()> buyEvent)
{
    auto rootWidget = uiBase->m_RootWidget;
    const char * LAYER_NAME = "leadBuyMapLayer";
    
    auto leadBuyLayer = rootWidget->getChildByName(LAYER_NAME);
    if (!leadBuyLayer) {
        leadBuyLayer = CSLoader::createNode("LeadBuyLayer.csb");
        leadBuyLayer->setName(LAYER_NAME);
        rootWidget->addChild(leadBuyLayer);
    }
    leadBuyLayer->getChildByName("Layout_LeadBuy")->setVisible(false);
    leadBuyLayer->setVisible(true);
    
    auto layout_leadBuy = leadBuyLayer->getChildByName("Layout_LeadBuyMap");
    if (!layout_leadBuy) return;
    layout_leadBuy->setVisible(true);
    
    int langID = 0;
    int langCostID = 0;
    int buyMapBagID = 0;
    if (curMapID == 909000005) {
        langID = 25131;
        langCostID = 25132;
        buyMapBagID = (int)CfgIdEnum::BuyMap6ID;
    }
    else if (curMapID == 909000007) {
        langID = 25133;
        langCostID = 25134;
        buyMapBagID = (int)CfgIdEnum::BuyMap8ID;
    }
    else if (curMapID == 909000009) {
        langID = 25135;
        langCostID = 25136;
        buyMapBagID = (int)CfgIdEnum::BuyMap10ID;
    }
    auto langStr = MultiLanguage::getInstance()->GetText(langID);
    auto langCostStr = MultiLanguage::getInstance()->GetText(langCostID);
    
    auto Text_ShowText = layout_leadBuy->getChildByName<Text *>("Text_Desc");
    Text_ShowText->setString(langStr);
    
    auto Btn_Cancel = layout_leadBuy->getChildByName<Button *>("Button_Cancel");
    Btn_Cancel->addClickEventListener(
                                      [=](Ref* sender)
                                      {
                                          layout_leadBuy->setVisible(false);
                                          leadBuyLayer->setVisible(false);
                                          TiledMapUI::getInstance()->SetTouchEnabled(true);
                                      });
    
    auto Btn_Buy = layout_leadBuy->getChildByName<Button *>("Button_Buy");
    Btn_Buy->addClickEventListener(
                            [=](Ref* sender)
                            {
                                layout_leadBuy->setVisible(false);
                                leadBuyLayer->setVisible(false);
                                
                                if(DiamondBuyData::getInstance()->BuyEvent(buyMapBagID))
                                {
                                    // 购买成功
                                    buyEvent();
                                }
                                else
                                {
                                    // 购买失败，跳至商城充值界面
                                    uiBase->UnLoad();
                                    // 扣除钻石失败，跳至充值界面
                                    DiamondBuyData::getInstance()->goToTopUp(uiBase->m_ResName);
                                }
                                
                            });
    
    auto Text_BuyNum = Btn_Buy->getChildByName<Text *>("Text_BuyNum");
    Text_BuyNum->setString(langCostStr);
    
}

int UIUtils::getTotalCostDiamond(const ValueMapIntMultiKey* vmi)
{
    float totalDiamondNums = 0;
    for (auto iter = vmi->begin(); iter != vmi->end(); ++iter)
    {
        int cfgId = iter->first;
        int num = iter->second.asInt();
        CfgDataRow dataRow(cfgId);
        auto vm_price = dataRow.GetValue(CfgField::PriceID_M)->asIntMultiKeyMap();
        auto iterPrice = vm_price.find((int)CfgIdEnum::GemId);
        if (iterPrice != vm_price.end()) {
            totalDiamondNums += (iterPrice->second.asInt()) * num / 100;
        }
        else
        {
            // PriceID_M字段为空，则取原材料的总价格
            auto materialMap = dataRow.GetValue(CfgField::MaterialID_M);
            if (!materialMap->isNull())
            {
                auto mt_price = materialMap->asIntMultiKeyMap();
                if (mt_price.size() == 0) return 0;
                else return getTotalCostDiamond(&mt_price);
            }
            else
            {
                return 0;
            }
        }
    }
    
    // 向上取整，且最小是1（暂定）
    if (totalDiamondNums < 1) totalDiamondNums = 1;
    totalDiamondNums = ceil(totalDiamondNums);
    return totalDiamondNums;
}

int UIUtils::getLackCostDiamond(const std::vector<std::pair<int, int>>* lackData)
{
    float totalDiamondNums = 0;
    for (auto iter = lackData->begin(); iter != lackData->end(); ++iter)
    {
        int cfgId = iter->first;
        int num = iter->second;
        CfgDataRow dataRow(cfgId);
        auto vm_price = dataRow.GetValue(CfgField::PriceID_M)   ->asIntMultiKeyMap();
        auto iterPrice = vm_price.find((int)CfgIdEnum::GemId);
        if (iterPrice != vm_price.end()) {
            totalDiamondNums += (iterPrice->second.asInt()) * num / 100;
        }
        else
        {
            // PriceID_M字段为空，则取原材料的总价格
            auto materialMap = dataRow.GetValue(CfgField::MaterialID_M);
            if (!materialMap->isNull())
            {
                auto mt_price = materialMap->asIntMultiKeyMap();
                if (mt_price.size() == 0) return 0;
                else return getTotalCostDiamond(&mt_price);
            }
            else
            {
                return 0;
            }
        }
    }
    
    if (totalDiamondNums < 1) totalDiamondNums = 1;
    totalDiamondNums = ceil(totalDiamondNums);
    return totalDiamondNums;
}

void UIUtils::costUpgNeededItem(const cocos2d::ValueMapIntMultiKey* vmi, const std::vector<std::pair<int, int>>* lackData)
{
    ValueMapIntMultiKey costItemMap = *vmi;
    vector<pair<int, int> > lackDataUnUsed;
    for (auto lackPair : *lackData)
    {
        int lackCfgId = lackPair.first;
        costItemMap.erase(lackCfgId);
    }
    
    UserData::getInstance()->DeleteStorehouseData(&costItemMap, &lackDataUnUsed);
    
}

void UIUtils::syncLeadHandShowUI(std::string showUIName, bool isRight,  bool isAdd)
{
    if (m_leadHandMap == nullptr) m_leadHandMap = new ValueMap();
    
    auto iter = m_leadHandMap->find(showUIName);
    if (iter != m_leadHandMap->end()) {
        if (isAdd) {
            iter->second = Value(isRight);
        }
        else {
            m_leadHandMap->erase(showUIName);
        }
    }
    else {
        if (isAdd) {
            m_leadHandMap->insert(std::pair<std::string, Value>(showUIName, Value(isRight)));
        }
    }
    
}

void UIUtils::checkShowLeadHand(cocos2d::Node * rootWidget)
{
    if (m_leadHandMap == nullptr || rootWidget == nullptr) return;
    
    bool isRight = false;
    
    std::string curUIName = rootWidget->getName();
    auto iter = m_leadHandMap->find(curUIName);
    if (iter == m_leadHandMap->end()) {
        return;
    }
    else {
        isRight = iter->second.asBool();
        syncLeadHandShowUI(curUIName, false, false);
    }
    
    const char * LAYER_NAME = "leadHandLayer";
    auto leadHandLayer = rootWidget->getChildByName(LAYER_NAME);
    if (!leadHandLayer) {
        leadHandLayer = CSLoader::createNode("LeadHandLayer.csb");
        leadHandLayer->setName(LAYER_NAME);
        rootWidget->addChild(leadHandLayer);
    }
    leadHandLayer->setVisible(true);
    
    auto layout_leadHand = leadHandLayer->getChildByName<Layout *>("Layout_LeadHand");
    auto img_Hand = layout_leadHand->getChildByName<ImageView *>("Image_Hand");
    auto img_ArrowToRight = layout_leadHand->getChildByName<ImageView *>("Image_ArrowToRight");
    auto img_ArrowToLeft = layout_leadHand->getChildByName<ImageView *>("Image_ArrowToLeft");
    auto visibleSize = Director::getInstance()->getVisibleSize();
    
    Vec2 oriPos;
    Vec2 desPos;
    ImageView * img_Arrow = img_ArrowToRight;
    if (isRight) {
        // 手势向右
        oriPos = Vec2(visibleSize.width * 0.33, visibleSize.height * 0.65);
        desPos = Vec2(visibleSize.width * 0.5, visibleSize.height * 0.65);
        img_Arrow = img_ArrowToRight;
        img_Hand->setAnchorPoint(Vec2(0, 1));
    }
    else {
        // 手势向左
        oriPos = Vec2(visibleSize.width * 0.67, visibleSize.height * 0.65);
        desPos = Vec2(visibleSize.width * 0.5, visibleSize.height * 0.65);
        img_Arrow = img_ArrowToLeft;
        img_Hand->setAnchorPoint(Vec2(1, 1));
    }
    
    Sequence * runHandSequence = Sequence::create(
                                                MoveTo::create(1, desPos),
                                                FadeOut::create(1),
                                                CallFunc::create([=]()
                                                                 {
                                                                     img_Hand->setOpacity(255);
                                                                     img_Hand->setPosition(oriPos);
                                                                 }),
                                                NULL
                                                  );
    
    Sequence * runArrowSequence = Sequence::create(
                                              DelayTime::create(0.2),
                                              CallFunc::create([=]()
                                                                {
                                                                    img_Arrow->setVisible(true);
                                                                }),
                                              MoveTo::create(0.8, desPos),
                                              FadeOut::create(1),
                                              CallFunc::create([=]()
                                                               {
                                                                   img_Arrow->setVisible(false);
                                                                   img_Arrow->setOpacity(255);
                                                                   img_Arrow->setPosition(oriPos);
                                                               }),
                                              NULL
                                              );
    
    img_Arrow->setVisible(false);
    img_Arrow->setPosition(oriPos);
    img_Arrow->runAction(RepeatForever::create(runArrowSequence));
    
    img_Hand->setPosition(oriPos);
    img_Hand->runAction(RepeatForever::create(runHandSequence));
    
}

////////////////////////////////////////////////////////////////////

SlideLayer::SlideLayer()
{
    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(SlideLayer::onTouchBegan, this);
    listener->onTouchEnded = CC_CALLBACK_2(SlideLayer::onTouchEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    this->setName(LAYER_NAME);
    
    m_touchStartPoint = Vec2::ZERO;
}

SlideLayer::~SlideLayer()
{
    
}

bool SlideLayer::onTouchBegan(Touch* touch, Event  *event)
{
    m_touchStartPoint = touch->getStartLocation();
    return true;
}

void SlideLayer::onTouchEnded(Touch* touch, Event  *event)
{
    auto touchEndPoint = touch->getLocation();
    auto wLength = m_touchStartPoint.x - touchEndPoint.x;
    auto hLength = m_touchStartPoint.y - touchEndPoint.y;
    CCLOG("moveXsize: %f ", wLength);
    CCLOG("moveYsize: %f ", hLength);
    if (std::abs(wLength) > MOVE_X_SIZE && std::abs(hLength) < MOVE_Y_SIZE)
    {
        if(wLength > 0)
        {
            CCLOG("move right...");
            if (UIUtils::getInstance()->m_rightEvent != nullptr) {
                UIUtils::getInstance()->m_rightEvent();
            }
        }
        else if (wLength < 0)
        {
            CCLOG("move left...");
            if (UIUtils::getInstance()->m_leftEvent != nullptr) {
                UIUtils::getInstance()->m_leftEvent();
            }
        }
    }
    
    m_touchStartPoint = Vec2::ZERO;
}
