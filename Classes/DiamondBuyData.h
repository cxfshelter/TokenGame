//
//  DiamondBuyData.h
//  TokenGame
//
//  Created by best on 15/6/29.
//
//

#ifndef __TokenGame__DiamondBuyData__
#define __TokenGame__DiamondBuyData__

#include "cocos2d.h"
#include <unordered_map>
#include "EnunClass.h"

USING_NS_CC;
using namespace std;

class DiamondBuyData
{
public:
    static DiamondBuyData * getInstance();
    
    /*  加载商店里面的数据
     */
    bool LoadDiamondData();
    
    /*  保存商店数据
     */
    bool SaveDiamondData();
    
    /*
     * return: 进度通用返回int, 进度非通用返回map
     */
    Value* GetDiamondData(int itemID);
    
    /*
        return: -1表示没找到能显示的ID（可能是全部生产时间均购买）
     */
    int GetProduceShowID();
    
    /*  处理点击事件：写入文件记录
     */
    bool BuyEvent(int cfgID);
    
    /*  扣除钻石（商场）
     costNum 扣除钻石数量
     true，扣除成功，false，钻石不足
     */
    bool costDiamond(int costNum);
    
    /*  钻石不足，跳至商城的钻石充值界面
     */
    void goToTopUp(string oriUIName = "");

    /*
     *  安卓平台支付生效
     *  param1: 计费点id
     */
    void androidPaySuc(int param1);

    // 标识: 跳至商城是否显示钻石充值界面
    bool m_isShowTopUp = false;
    
    // 跳转之前的UI名字
    string m_oriUIName = "";


private:
    void init();
    
    /*  是否为生产ID
     */
    bool isProduceID(int itemID);
    
    /*  实现购买物品的功能
     */
    void MakeEffect(int itemID);
    
    /*  使已购买进度通用生效
     */
    void MakeComEffectx();
    
private:
    static DiamondBuyData * sm_Instance;
    
    string m_DiamondDataDir;
    
    string m_DiamondDataPath;
    
    // 设置基本数据（单独存在，否则切换存档会丢失）
    ValueMap * m_DiamondDataVm = nullptr;
    
    /*  目前只有生产ID是唯一显示的（即该类型的ID只能同时存在一个）
     */
    const vector<int> m_EnumProduceID =
    {
        202000006,
        202000007,
        202000008,
        202000009,
        202000010,
    };


};

#endif /* defined(__TokenGame__DiamondBuyData__) */
