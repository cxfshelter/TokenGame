//
//  MultiLanguage.cpp
//  TokenGame
//
//  Created by token on 15/3/19.
//
//

#include "MultiLanguage.h"


MultiLanguage*  MultiLanguage::sm_MultiLanguage = nullptr;

MultiLanguage* MultiLanguage::getInstance()
{
    if (sm_MultiLanguage == nullptr)
    {
        sm_MultiLanguage = new (std::nothrow) MultiLanguage();
        if(sm_MultiLanguage->Init() == false)
        {
            
        }
        
    }
    return sm_MultiLanguage;
}

bool MultiLanguage::Init()
{
    m_LanguageName = new string("Chinese");
    m_MultiLanguageMap = new (std::nothrow) ValueMap();
    if (m_LanguageName == nullptr || m_MultiLanguageMap == nullptr)
    {
        return false;
    }else
    {
        return true;
    }
        
}

//加载语言配置文件 文件编码必须是 utf_8
bool MultiLanguage::SetLanguageName(const char* languageName)
{
    //*m_LanguageName = languageName;
    return LoadLanguageFile();
}

//加载语言文件
bool MultiLanguage::LoadLanguageFile()
{
    
    string languageFileName = StringUtils::format("%s.xml", m_LanguageName->c_str());

    *m_MultiLanguageMap = FileUtils::getInstance()->getValueMapFromFile(languageFileName);
    if (m_MultiLanguageMap->empty())
    {
        languageFileName =  "Chinese";
        languageFileName = StringUtils::format("%s.xml", m_LanguageName->c_str());
        *m_MultiLanguageMap = FileUtils::getInstance()->getValueMapFromFile(languageFileName);
    }
    
    if (m_MultiLanguageMap->empty())
    {
        return false;
    }else
    {
        return true;
    }
    
}

//获取语言包 key 对应内容
const string MultiLanguage::GetText(uint32_t key)
{
    string strKey = StringUtils::format("%u", key);
    auto iter = m_MultiLanguageMap->find(strKey);
    if (iter == m_MultiLanguageMap->end())
    {
        m_MultiLanguageMap->insert(pair<string, Value>(strKey, Value(strKey)));
        iter = m_MultiLanguageMap->find(strKey);
    }
    return iter->second.asString();
}

const string* MultiLanguage::GetLanguageName()
{
    switch (Application::getInstance()->getCurrentLanguage())
    {
        case LanguageType::ENGLISH:
            *m_LanguageName = "English";
            break;
        case LanguageType::CHINESE:
            *m_LanguageName = "Chinese";
            break;
        case LanguageType::FRENCH:
            *m_LanguageName = "French";
            break;
        case LanguageType::GERMAN:
            *m_LanguageName = "German";
            break;
        case LanguageType::ITALIAN:
            *m_LanguageName = "Italian";
            break;
        case LanguageType::RUSSIAN:
            *m_LanguageName = "Russian";
            break;
        case LanguageType::SPANISH:
            *m_LanguageName = "Spanish";
            break;
        case LanguageType::KOREAN:
            *m_LanguageName = "Korean";
            break;
        case LanguageType::JAPANESE:
            *m_LanguageName = "Japanese";
            break;
        case LanguageType::HUNGARIAN:
            *m_LanguageName = "Hungarian";
            break;
        case LanguageType::PORTUGUESE:
            *m_LanguageName = "Portuguese";
            break;
        case LanguageType::ARABIC:
            *m_LanguageName = "Arabic";
            break;
        case LanguageType::NORWEGIAN:
            *m_LanguageName = "Norwegian";
            break;
        case LanguageType::POLISH:
            *m_LanguageName = "Polish";
            break;
        case LanguageType::DUTCH:
            *m_LanguageName = "Dutch";
            break;
        default:
            *m_LanguageName = "Chinese";
            break;
    }
    return m_LanguageName;
}

