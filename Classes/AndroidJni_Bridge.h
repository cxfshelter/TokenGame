//
//  AndroidJni_Bridge.h
//  TokenGame
//
//  Created by best on 15/8/13.
//
//

#include "platform/CCPlatformConfig.h"
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID

#ifndef __TokenGame__AndroidJni_Bridge__
#define __TokenGame__AndroidJni_Bridge__

#include "JniUtils.h"

class AndroidJni_Bridge
{
public:
    static AndroidJni_Bridge * getInstance();
    
    // CC_PLATFORM_ANDROID
    /*
     * 	安卓平台支付
     */
    void androidPay(int param1, int param2, int param3, int param4);
    
private:
    static AndroidJni_Bridge * m_Instance;
};



#endif
#endif /* defined(__TokenGame__AndroidJni_Bridge__) */
