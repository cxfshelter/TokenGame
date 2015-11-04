//
//  IOSiAP_Bridge.cpp
//  LongChengDaRen
//
//  Created by 白白 on 14-11-11.
//
//

#include "platform/CCPlatformConfig.h"
#if(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)

#include "IOSiAP_Bridge.h"
#include "UserData.h"
#include "DiamondBuyUI.h"

#include "cocos2d.h"
USING_NS_CC;

IOSiAP_Bridge* IOSiAP_Bridge::m_Instance = nullptr;

IOSiAP_Bridge::IOSiAP_Bridge()
{
    iap = new IOSiAP();
    iap->delegate = this;
}

IOSiAP_Bridge::~IOSiAP_Bridge()
{
    delete iap;
}

IOSiAP_Bridge* IOSiAP_Bridge::getInstance()
{
    if (m_Instance == nullptr) {
        m_Instance = new IOSiAP_Bridge();
    }
    return m_Instance;
}

void IOSiAP_Bridge::requestProducts(Purchase_Diamond id)
{
    productID = id;
    std::vector<std::string> product;
    product.push_back(SMALL_NUM_ID);
    product.push_back(MID_NUM_ID);
    product.push_back(LARGE_NUM_ID);
	//把需要付费的道具的所有product id都放到容器里面传进去
    iap->requestProducts(product);
}

void IOSiAP_Bridge::onRequestProductsFinish(void)
{
    std::string identifier = "";
    switch (productID) {
        case Purchase_Diamond::Small_Num:
            identifier = SMALL_NUM_ID;
            break;
        case Purchase_Diamond::Mid_Num:
            identifier = MID_NUM_ID;
            break;
        case Purchase_Diamond::Large_Num:
            identifier = LARGE_NUM_ID;
            break;
        default:
            break;
    }
    
    //必须在onRequestProductsFinish后才能去请求iAP产品数据。
    IOSProduct *product = iap->iOSProductByIdentifier(identifier);
    // 然后可以发起付款请求,第一个参数是由iOSProductByIdentifier获取的IOSProduct实例，第二个参数是购买数量
    iap->paymentWithProduct(product, 1);
}

void IOSiAP_Bridge::onRequestProductsError(int code)
{
    //这里requestProducts出错了，不能进行后面的所有操作。
    log("付款失败");
}

void IOSiAP_Bridge::onPaymentEvent(std::string &identifier, IOSiAPPaymentEvent event, int quantity)
{
    if (event == IOSIAP_PAYMENT_PURCHAED) {
        int goldNum = 0;
        //付款成功了，可以吧金币发给玩家了。
		//根据传入的参数就能知道购买的是哪种类型的金币
        switch (productID) {
            case Purchase_Diamond::Small_Num:
                if (std::atoi(UserData::getInstance()->Get_MVI_Value(UserDataEnum::FirstTopUp, 1, true).asString().c_str()) >= 1) {
                    goldNum = 100;
                }
                else {
                    // 首次充值
                    goldNum = 100 * 2;
                    UserData::getInstance()->Update_VMV_Encode(UserDataEnum::FirstTopUp, 1, 1);
                }
                break;
            case Purchase_Diamond::Mid_Num:
                if (std::atoi(UserData::getInstance()->Get_MVI_Value(UserDataEnum::FirstTopUp, 2, true).asString().c_str()) >= 1) {
                    goldNum = 300;
                }
                else {
                    // 首次充值
                    goldNum = 300 * 2;
                    UserData::getInstance()->Update_VMV_Encode(UserDataEnum::FirstTopUp, 2, 1);

                }
                break;
            case Purchase_Diamond::Large_Num:
                if (std::atoi(UserData::getInstance()->Get_MVI_Value(UserDataEnum::FirstTopUp, 3, true).asString().c_str()) >= 1) {
                    goldNum = 1000;
                }
                else {
                    // 首次充值
                    goldNum = 1000 * 2;
                    UserData::getInstance()->Update_VMV_Encode(UserDataEnum::FirstTopUp, 3, 1);
                }
                break;
            default:
                break;
        }
        log("付款成功");
        UserData::getInstance()->Update_VMI_Fun(UserDataEnum::Storehouse, (int)CfgIdEnum::GemId, goldNum);
        DiamondBuyUI::getInstance()->showTopUpLayout();
    }
    //其他状态依情况处理掉。
}

#endif
