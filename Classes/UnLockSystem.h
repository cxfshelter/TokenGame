//
//  UnLockSystem.h
//  TokenGame
//
//  Created by token on 15/3/27.
//
//

#ifndef __TokenGame__UnLockSystem__
#define __TokenGame__UnLockSystem__

#include "cocos2d.h"
#include "UserEventEnum.h"

#include <vector>
#include <unordered_map>


using namespace std;

class UnLockSystem
{
private:
    
    bool Init();
    

    static UnLockSystem* sm_UnLockSystem;
    
    //把达成条件的结果保存到相应用户数据中
    void UnLockData(ValueMapIntMultiKey* vmi);
    
    //是否已有触发条件Id
    bool IsExistTriggerId(int triggerid, int num, int cfgId);
    
    vector<vector< tuple<uint32_t, ValueMapIntMultiKey, ValueMapIntMultiKey> > >* sm_UnLockData;
    
    
public:
    
    static UnLockSystem* getInstance();
    
    // 清空相关变量
    void releaseData();
    
    //加载要解锁的数据
    void LoadUnLockData();
    
    //检查是否达成解锁条件
    bool CheckUnLockEvent(UserEventType userEventType, int32_t triggerId, int num = 1);
};

#endif /* defined(__TokenGame__UnLockSystem__) */
