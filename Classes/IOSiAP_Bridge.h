//
//  IOSiAP_Bridge.h
//  LongChengDaRen
//
//  Created by 白白 on 14-11-11.
//
//

#include "platform/CCPlatformConfig.h"
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS

#ifndef __LongChengDaRen__IOSiAP_Bridge__
#define __LongChengDaRen__IOSiAP_Bridge__

#import "IOSiAP.h"

// 在iTunes上面对应的各个付费点的product ID
#define SMALL_NUM_ID        "com.zseToken.buyDiamonds"
#define MID_NUM_ID          "com.zseToken.midNum"
#define LARGE_NUM_ID        "com.zseToken.largeNum"

enum class Purchase_Diamond
{
    Small_Num = 0,          // 一小袋钻石
    Mid_Num = 1,            //
    Large_Num = 2,          //
    
};

class IOSiAP_Bridge : public IOSiAPDelegate
{
public:
    IOSiAP_Bridge();
    ~IOSiAP_Bridge();
    
    static IOSiAP_Bridge* m_Instance;
    static IOSiAP_Bridge * getInstance();

    IOSiAP * iap;
    Purchase_Diamond productID;

    void requestProducts(Purchase_Diamond);
    virtual void onRequestProductsFinish(void);
    virtual void onRequestProductsError(int code);
    virtual void onPaymentEvent(std::string &identifier, IOSiAPPaymentEvent event, int quantity);
};

#endif
#endif /* defined(__LongChengDaRen__IOSiAP_Bridge__) */
