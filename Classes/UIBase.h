//
//  UI.h
//  TokenGame
//
//  Created by token on 15/3/20.
//
//

#ifndef __TokenGame__UI__
#define __TokenGame__UI__

#include "Common.h"
#include "UserEventContainer.h"
#include "UIUtils.h"

class UIBase
{

    
private:
    
    EventListenerTouchAllAtOnce* m_ListenerLeftRightTouchAllAtOnce = nullptr;
    
public:
    
    string m_ResName;
    
    //每次显示都调用 用于更UI
    virtual void UpdateUI() = 0;
    
    //加载成功后运行一次
    virtual void LoadFinish() = 0;
    
    virtual void UnLoad()
    {
        if(m_RootLayer == nullptr) return;
        
        if(m_RootLayer->getChildByName(m_ResName) != nullptr)
        {
            //m_ListenerLeftRightTouchAllAtOnce = nullptr;
            //m_RootWidget->getEventDispatcher()->removeAllEventListeners();
            m_RootWidget->unscheduleAllCallbacks();
            m_RootLayer->removeChildByName(m_ResName);
            
        }
        m_RootWidget = nullptr;
        //m_RootLayer = nullptr;
    }

    
    Value m_LoadPram;
    Layer* m_RootLayer;
    Node* m_RootWidget = nullptr;
    
    bool Load(Layer* layer, string resName, Value pram = Value::Null)
    {
        m_LoadPram = pram;
        bool result = true;
        m_ResName = resName;
        m_RootLayer = layer;
        if (m_RootWidget == nullptr)
        {
            auto rootNode = CSLoader::createNode(m_ResName);
            if (rootNode == nullptr)
            {
                result = false;
            }else
            {
                //----------开始 重新调整控件位置 以支持多分辨率--耗性能----------
                auto size= Director::getInstance()->getVisibleSize();
                rootNode->setContentSize(size);
                ui::Helper::doLayout(rootNode);
                //----------结束 重新调整控件位置 以支持多分辨率--耗性能----------
                
                m_RootWidget = rootNode;
                layer->addChild(rootNode);
                rootNode->setName(resName);
                rootNode->setVisible(false);
                
                UserEventContainer::getInstance()->Send(UserEventType::LoadFinishUI, 0, Value(resName));
                
                LoadFinish();
                
            }
        }
        // 监听back，退出游戏
        UIUtils::getInstance()->addBackLister(layer);
        
        return result;
    }
    
    
    void Show()
    {
        if (m_RootWidget != nullptr)
        {
            UpdateUI(); 
            if (m_RootWidget != nullptr)
            {
                m_RootWidget->setVisible(true);
            }
        }
    }
    
    
    void Hidden()
    {
        if (m_RootWidget != nullptr)
        {
            m_RootWidget->setVisible(false);
        }
    }
    
    bool IsVisible()
    {
        bool result = false;
        if (m_RootWidget != nullptr)
        {
            result = m_RootWidget->isVisible();
        }
        return result;
    }
    
};


#endif /* defined(__TokenGame__UI__) */
