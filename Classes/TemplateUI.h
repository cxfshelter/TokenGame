//
//  TemplateUI.h
//  TokenGame
//
//  Created by token on 15/3/20.
//
//

#ifndef __TokenGame__TemplateUI__
#define __TokenGame__TemplateUI__

#include "UIBase.h"
#include "Common.h"

class TemplateUI : public UIBase
{

private:
    bool Init();
    static TemplateUI* sm_TemplateUI;
    //TemplateUI(const TemplateUI& other){};
    TemplateUI& operator = (const TemplateUI& other){return *this;};
protected:
    
    virtual void UpdateUI();
    virtual void LoadFinish();
    
public:
    
    static TemplateUI* getInstance();
    
};

#endif /* defined(__TokenGame__TemplateUI__) */
