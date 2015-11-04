//
//  DiamondBuyUI.cpp
//  TokenGame
//
//  Created by best on 15/6/24.
//
//

#include "DiamondBuyUI.h"
#include "MainScene.h"
#include "MapScene.h"

DiamondBuyUI * DiamondBuyUI::sm_Instance = nullptr;

DiamondBuyUI* DiamondBuyUI::getInstance()
{
    if (sm_Instance == nullptr) {
        sm_Instance = new (std::nothrow) DiamondBuyUI();
        if (sm_Instance->Init()) {}
    }
    return sm_Instance;
}

bool DiamondBuyUI::Init()
{
    return true;
}

void DiamondBuyUI::saveDiamondData(int cfgID, Value val)
{
    if (val.isNull()) {
        return;
    }
    
    Value* oriVal = DiamondBuyData::getInstance()->GetDiamondData(cfgID);
    *oriVal = val;
    
    DiamondBuyData::getInstance()->SaveDiamondData();
}

void DiamondBuyUI::UnLoad()
{
    UIBase::UnLoad();
}

void DiamondBuyUI::UpdateUI()
{
    auto layout_template = m_RootWidget->getChildByName<Layout *>("Layout_Template");
    auto layout_templateItem = layout_template->getChildByName<Layout *>("Layout_Item");
    
    auto listView_btnBtn = m_RootWidget->getChildByName<ListView *>("ListView_BuyBtn");
    
    auto diamondTableList = CfgData::getInstance()->GetCfgTableId("DiamondItem");
    int cfgId = 0, langId = 0, costNum;
    string langText = "";
    
    listView_btnBtn->removeAllChildren();
    for (auto iter = diamondTableList->begin(); iter != diamondTableList->end();)
    {
        auto layout_item = layout_templateItem->clone();
        listView_btnBtn->addChild(layout_item);
        
        for (int i = 1; i <= 2; ++iter) {
            auto btn_Name = layout_item->getChildByName<Button *>(StringUtils::format("Btn_Buy%d", i));
            auto img_icon = btn_Name->getChildByName<ImageView *>("Image_Icon");
            auto text_name = btn_Name->getChildByName<Text *>("Text_Name");
            auto text_price = btn_Name->getChildByName<Text *>("Text_Price");
            
            if (iter == diamondTableList->end()) {
                if (i == 1) listView_btnBtn->removeLastItem();
                else        btn_Name->setVisible(false);
                break;
            }
            
            cfgId = *iter;
            // 生产类型只同时存在一个
            if (cfgId == 202000006) {
                cfgId = DiamondBuyData::getInstance()->GetProduceShowID();
                iter += 4;
                if (cfgId == -1) {
                    continue;
                }
            }
            
            CfgDataRow dataRow(cfgId);
            // 通用的是否已购买
            if (dataRow.GetValue(CfgField::GenericID)->asInt() == 1) {
                Value * oriVal = DiamondBuyData::getInstance()->GetDiamondData(cfgId);
                if (oriVal->asInt() == 1) {
                    continue;
                }
            }
            
            btn_Name->setVisible(true);
            langId = dataRow.GetValue(CfgField::Name)->asInt();
            langText = MultiLanguage::getInstance()->GetText(langId);
            text_name->setString(langText);
            btn_Name->addClickEventListener(
                                            [=](Ref* sender)
                                            {
                                                // 弹出确定界面，扣除钻石数量，成功 - 执行下面，失败 - 弹出充值界面
                                                showBuyLayout(cfgId);
                                                
                                            });
            
//            auto img_frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(dataRow.GetValue(CfgField::DiamondIconType_S)->asString() + ".png")->getTexture();
            img_icon->loadTexture(dataRow.GetValue(CfgField::DiamondIconType_S)->asString() + ".png", TextureResType::PLIST);
            img_icon->setSize(Size(97, 100));
            
            costNum = dataRow.GetValue(CfgField::PriceID_M)->asIntMultiKeyMap().begin()->second.asInt();
            text_price->setString(StringUtils::format("%d", costNum));
            
            ++i;
        }
    }
    
    auto layout_title = m_RootWidget->getChildByName<Layout *>("Layout_Title");
    auto btn_topUp = layout_title->getChildByName<Button *>("Button_TopUp");
    auto text_diamondNum = btn_topUp->getChildByName<Text *>("Text_DiamondNum");
    auto diamondValue = UserData::getInstance()->Get_MVI_Value(UserDataEnum::Storehouse, (int)CfgIdEnum::GemId);
    auto diamondNum = 0;
    if (!diamondValue.isNull()) diamondNum = diamondValue.asInt();
    text_diamondNum->setString(StringUtils::format("%d", diamondNum));
    
    if (DiamondBuyData::getInstance()->m_isShowTopUp)
    {
        showTopUpLayout();
        DiamondBuyData::getInstance()->m_isShowTopUp = false;
    }
    
}

void DiamondBuyUI::showBuyLayout(int cfgId)
{
    auto layout_buy = m_RootWidget->getChildByName<Layout *>("Layout_BuyTips");
    if (layout_buy) layout_buy->setVisible(true);
    
    CfgDataRow dataRow(cfgId);
    auto langIdV = dataRow.GetValue(CfgField::Details_V)->asValueVector();
    auto textStr = MultiLanguage::getInstance()->GetText(langIdV[0].asInt());
    auto text_desc = layout_buy->getChildByName<Text *>("Text_Desc");
    text_desc->setString(textStr);
    
    textStr = MultiLanguage::getInstance()->GetText(langIdV[1].asInt());
    auto text_desc2 = layout_buy->getChildByName<Text *>("Text_Desc2");
    text_desc2->setString(textStr);
    
    auto langId = dataRow.GetValue(CfgField::Name)->asInt();
    auto btn_buy = layout_buy->getChildByName<Button *>("Button_Buy");
    btn_buy->addClickEventListener(
                                   [=](Ref* sender)
                                   {
                                       layout_buy->setVisible(false);
                                       // 写入文件记录并使其生效
                                       if(DiamondBuyData::getInstance()->BuyEvent(cfgId))
                                       {// 购买成功，弹出提示
                                           MoveText::getInstance()->AddDefaultTextData(m_RootWidget, "已获得" + MultiLanguage::getInstance()->GetText(langId));
                                       }
                                       else
                                       {// 购买失败，弹出提示，跳至钻石充值界面
                                           MoveText::getInstance()->AddDefaultTextData(m_RootWidget, "钻石不足，请充值");
                                           showTopUpLayout();
                                       }
                                       UpdateUI();
                                       RecordUtils::getInstance()->sdkRecord(cfgId, "确定");
                                   });
    
    auto btn_cancel = layout_buy->getChildByName<Button *>("Button_Cancel");
    btn_cancel->addClickEventListener(
                                   [=](Ref* sender)
                                   {
                                       layout_buy->setVisible(false);
                                       UpdateUI();
                                       RecordUtils::getInstance()->sdkRecord(cfgId, "取消");
                                   });

    auto costNum = dataRow.GetValue(CfgField::PriceID_M)->asIntMultiKeyMap().begin()->second.asInt();
    auto text_costNum = btn_buy->getChildByName<Text *>("Text_BuyNum");
    text_costNum->setString(StringUtils::format("%d", costNum));
}

void DiamondBuyUI::showTopUpLayout()
{
    if(!m_RootWidget) return;
    
    auto layout_topUp = m_RootWidget->getChildByName<Layout *>("Layout_TopUp");
    if (layout_topUp) layout_topUp->setVisible(true);
    else              return;
    
    // 扣6元+100
    auto btn_buy1 = layout_topUp->getChildByName<Button *>("Button_Buy1");
    btn_buy1->addClickEventListener(
                                    [](Ref* sender)
                                    {
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
                                        IOSiAP_Bridge::getInstance()->requestProducts(Purchase_Diamond::Small_Num);
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
                                        AndroidJni_Bridge::getInstance()->androidPay((int)BuyProductID::BuyID_First, (int)CfgIdEnum::GemId, 100, 6);
#endif
                                    });
    
    // 扣18元+300
    auto btn_buy2 = layout_topUp->getChildByName<Button *>("Button_Buy2");
    btn_buy2->addClickEventListener(
                                    [](Ref* sender)
                                    {
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
                                        IOSiAP_Bridge::getInstance()->requestProducts(Purchase_Diamond::Mid_Num);
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
                                        AndroidJni_Bridge::getInstance()->androidPay((int)BuyProductID::BuyID_Second, (int)CfgIdEnum::GemId, 300, 18);
#endif
                                    });
    
    // ios扣60元+1000，android扣30元+500
    auto btn_buy3 = layout_topUp->getChildByName<Button *>("Button_Buy3");
    btn_buy3->addClickEventListener(
                                    [](Ref* sender)
                                    {
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
                                        IOSiAP_Bridge::getInstance()->requestProducts(Purchase_Diamond::Large_Num);
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
                                        AndroidJni_Bridge::getInstance()->androidPay((int)BuyProductID::BuyID_Third, (int)CfgIdEnum::GemId, 500, 30);
#endif
                                    });
    
    auto btn_buy4 = layout_topUp->getChildByName<Button *>("Button_Buy4");
    btn_buy4->addClickEventListener(
                                    [](Ref* sender)
                                    {
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
                                        IOSiAP_Bridge::getInstance()->requestProducts(Purchase_Diamond::Large_Num);
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
                                        AndroidJni_Bridge::getInstance()->androidPay((int)BuyProductID::BuyID_Fourth, (int)CfgIdEnum::GemId, 5, 0.1);
#endif
                                    });

    auto btn_close = layout_topUp->getChildByName<Button *>("Button_Close");
    btn_close->addClickEventListener(
                                     [=](Ref* sender)
                                     {
                                         layout_topUp->setVisible(false);
                                         if (DiamondBuyData::getInstance()->m_oriUIName.compare("MapUI.csb") == 0) {
                                             this->UnLoad();
                                             auto scene = MapScene::createScene();
                                             Director::getInstance()->replaceScene(CCTransitionFade::create(0.8f, scene, Color3B::BLACK));
                                             DiamondBuyData::getInstance()->m_oriUIName = "";
                                         }
                                         else {
                                             UpdateUI();
                                         }
                                     }
                                     );
    
    // 是否显示首充
    for (int i=1; i<=3; ++i) {
        auto parentName = StringUtils::format("Button_Buy%d", i);
        auto imgFrist = layout_topUp->getChildByName(parentName)->getChildByName("Image_First");
        if (std::atoi(UserData::getInstance()->Get_MVI_Value(UserDataEnum::FirstTopUp, i, true).asString().c_str()) == 0) {
            // 首充
            imgFrist->setVisible(true);
        }
        else {
            imgFrist->setVisible(false);
        }
        
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
        imgFrist->setVisible(false);
#endif
    }
    

    // 超值礼包（只能购买一次，购买之后隐藏）
    // ios平台下面永久设为隐藏（因为ios最低支付为6RMB）
    // android平台的短信付费最高是30元
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    btn_buy4->setVisible(false);
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    // 该短接sdk没有0.1的计费代码，暂时隐藏
    btn_buy4->setVisible(false);
    
    // 超值礼包显示
//    if (std::atoi(UserData::getInstance()->Get_MVI_Value(UserDataEnum::FirstTopUp, 4, true).asString().c_str()) == 0) {
//        btn_buy4->setVisible(true);
//    }
//    else {
//        btn_buy4->setVisible(false);
//    }

    // 大量钻石价格显示调整
    auto text_rmb     = btn_buy3->getChildByName<Text *>("Text_Num");
    auto text_diamond = btn_buy3->getChildByName<Text *>("Text_addNum");
    auto text_name = btn_buy3->getChildByName<Text *>("Text_Name");
    text_rmb->setString(StringUtils::format("%d元", 30));
    text_diamond->setString(StringUtils::format("+%d", 550));
    text_name->setString(StringUtils::format("%d钻石", 550));
    
#endif
    
}

void DiamondBuyUI::LoadFinish()
{
    auto layout_title = m_RootWidget->getChildByName<Layout *>("Layout_Title");
    
    auto btn_left = layout_title->getChildByName<Button *>("Btn_Left");
    btn_left->addClickEventListener(
                                    [this](Ref* sender)
                                    {
                                        this->UnLoad();
                                        auto scene = MainScene::createScene();
                                        Director::getInstance()->replaceScene(CCTransitionMoveInL::create(0.18f, scene));
                                    });
    
    // 充值
    auto btn_topUp = layout_title->getChildByName<Button *>("Button_TopUp");
    btn_topUp->addClickEventListener(
                                     [=](Ref* sender)
                                     {
                                         showTopUpLayout();
                                         RecordUtils::getInstance()->sdkRecord((int)umengEventID::Open_TopUp, "点击进入");
                                     });
    
    // 兑换码
    auto btn_convert = layout_title->getChildByName<Button *>("Button_Convert");
    btn_convert->addClickEventListener(
                                     [](Ref* sender)
                                     {
                                         
                                     });

    UIUtils::getInstance()->addSlideEvent(m_RootWidget,
                                          [this]()
                                          {
                                              this->UnLoad();
                                              auto scene = MainScene::createScene();
                                              Director::getInstance()->replaceScene(CCTransitionMoveInL::create(0.18f, scene));
                                              
                                          }, nullptr);

    
}










