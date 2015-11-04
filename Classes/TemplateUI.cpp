//
//  TemplateUI.cpp
//  TokenGame
//
//  Created by token on 15/3/20.
//
//

#include "TemplateUI.h"

TemplateUI* TemplateUI::sm_TemplateUI = nullptr;


TemplateUI* TemplateUI::getInstance()
{
    if (sm_TemplateUI == nullptr)
    {
        sm_TemplateUI = new (std::nothrow) TemplateUI();
        if (sm_TemplateUI->Init()) {}
    }
    return sm_TemplateUI;
}

bool TemplateUI::Init()
{
    return true;
}

void TemplateUI::UpdateUI()
{
    
}

void TemplateUI::LoadFinish()
{
   
}


