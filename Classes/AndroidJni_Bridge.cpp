//
//  AndroidJni_Bridge.cpp
//  TokenGame
//
//  Created by best on 15/8/13.
//
//

#include "platform/CCPlatformConfig.h"
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID

#include "AndroidJni_Bridge.h"

AndroidJni_Bridge * AndroidJni_Bridge::m_Instance = nullptr;
AndroidJni_Bridge * AndroidJni_Bridge::getInstance()
{
    if (m_Instance == nullptr) {
        m_Instance = new AndroidJni_Bridge();
    }
    return m_Instance;
}

void AndroidJni_Bridge::androidPay(int param1, int param2, int param3, int param4)
{
//	return;
    std::string param11 = StringUtils::format("%d", param1);
    std::string param22 = StringUtils::format("%d", param2);
    std::string param33 = StringUtils::format("%d", param3);
    std::string param44 = StringUtils::format("%d", param4);
    JniUtils::getInstance()->startToPay(param11, param22, param33, param44);
}

#endif
