//
//  RecordUtils.h
//  TokenGame
//
//  Created by best on 15/8/12.
//
//

#ifndef __TokenGame__RecordUtils__
#define __TokenGame__RecordUtils__

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
#include "JniUtils.h"
#endif

#include "cocos2d.h"
using namespace std;

class RecordUtils
{
public:
    static RecordUtils * getInstance();
    
    /*
     *  平台统计数据
     *  param1: 事件id
     *  param2: 事件属性（确定、取消）
     */
    void sdkRecord(int param1, std::string param2 = "");

    /*
     * 	平台统计数据
     */
    void sdkExitGame();
    
    /*  得到设备的imei码
     */
    std::string getImei();

private:
    static RecordUtils * m_Instance;
    
};

#endif /* defined(__TokenGame__RecordUtils__) */
