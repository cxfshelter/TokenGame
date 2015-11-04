//
//  ProductionUI.h
//  TokenGame
//
//  Created by token on 15/3/20.
//
//

#ifndef __TokenGame__ProductionUI__
#define __TokenGame__ProductionUI__

#include "UIBase.h"
#include "Common.h"
#include "ProgressBar.h"

//生产
class ProductionUI : public UIBase
{

private:
    bool Init();
    static ProductionUI* sm_ProductionUI;
    ProductionUI& operator = (const ProductionUI& other){return *this;};
    
private:
    
    //最后一次采集时间
    int m_LastCollectTime;
    
    //采集一次剩余时间
    int m_SurplusCoolectTime;
    
    //采集时间间隔
    const float m_CollectInvertTime = 30;
    
    //采集获取的物品
    void CollectGetItem();
    
    void CollectCallBackEvent();
    
    ProgressBar* m_ProgressBar = nullptr;
    
    const string m_CollectProgressBar = "CollectProgressBar";
    
protected:
    
    virtual void UnLoad();
    virtual void UpdateUI();
    virtual void LoadFinish();
    
    //生产物品
    void ProductionItem();
    //显示生产列表
    void ShowProductionList();
    //显示生产选项
    void ShowProductionOption();
    
    //更新工人数量
    void UpdateWorkerNum();
    
public:

    static ProductionUI* getInstance();
    
    //增加一个工作中的工人
    void AddWorkerNum();
    //减少一个工作中的工人
    void DelWorkerNum();
    
    //根据当前民宅 工人数量
    int GetMaxWorkerNum();
    
    //当前在工作的工人数
    int GetNowWorkerNum();
};

#endif /* defined(__TokenGame__ProductionUI__) */
