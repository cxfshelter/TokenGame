//
//  RecordUtils.cpp
//  TokenGame
//
//  Created by best on 15/8/12.
//
//

#include "RecordUtils.h"

RecordUtils* RecordUtils::m_Instance = nullptr;

RecordUtils* RecordUtils::getInstance()
{
    if (m_Instance == nullptr) {
        m_Instance = new (std::nothrow) RecordUtils();
    }
    return m_Instance;
}

void RecordUtils::sdkRecord(int param1, std::string param2)
{
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    std::string param11 = StringUtils::format("%d", param1);
    JniUtils::getInstance()->recordDataInYM(param11, param2);
#endif
	
}

void RecordUtils::sdkExitGame()
{
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	JniUtils::getInstance()->exitGame();
#endif
	
}

std::string RecordUtils::getImei()
{
    std::string imeiStr = "";
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    imeiStr = JniUtils::getInstance()->getImei();
#endif
    
    return imeiStr;
}
