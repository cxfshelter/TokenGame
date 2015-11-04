//
//  ProductionItem.h
//  TokenGame
//
//  Created by token on 15/4/19.
//
//

#ifndef __TokenGame__ProductionItem__
#define __TokenGame__ProductionItem__


#include "Common.h"

class ProductionItem
{
    
private:
    bool Init();
    static ProductionItem* sm_ProductionItem;
    ProductionItem& operator = (const ProductionItem& other){return *this;};
    
private:
    
    Node* m_Node;
    
    //最后生产时间
    float m_LastTime;
    
    //生产一次间隔时间 还剩余的时间
    float m_SurplusIntervalTime;
    
    //最大休眠生产时长(单位秒)
    float m_MaxSleepProductionTime = 60 * 60;
    
    //生产一次间隔时间(单位秒)
    float m_CbIntervalTime = 20;
    
    //调度器名
    const string m_ScheduleName = "ProductionItemSchedule";
    
    
public:
    
    static ProductionItem* getInstance();
    
    //取消调度器
    void UnSchedule();
    
    //循环计算一个长时间段内所有生产数据
    void LoopProductionItem();
    
    //设置生产调度结节
    void SetSchedule(Node* node, Text* text = nullptr);
    
    void ComputeProductionItemList(map<int, int>* outItems);
    //每20秒计算一次生产数据
    void ComputeProductionItemData(ValueMapIntMultiKey* outputItems = nullptr,
                        map<int, int>* expendItems = nullptr, bool isSave = true);
    
    void setMaxSleepProductionTime(float newMaxTime);
    
    void setCBIntervalTime(bool isBuild);
};


#endif /* defined(__TokenGame__ProductionItem__) */
