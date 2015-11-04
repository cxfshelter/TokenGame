//
//  MultiLanguage.h
//  TokenGame
//
//  Created by token on 15/3/19.
//
//

#ifndef __MultiLanguage_H_
#define __MultiLanguage_H_

#include <string>
#include <unordered_map>

#include "cocos2d.h"

USING_NS_CC;
using namespace std;

class MultiLanguage
{
    
private:
    
    static MultiLanguage* sm_MultiLanguage;
    
    //语言名
    string* m_LanguageName;
    
    //存语言 key val
    ValueMap* m_MultiLanguageMap;
    
private:
    
    bool Init();
    
    //加载语言文件
    bool LoadLanguageFile();
    
public:
    
    static MultiLanguage* getInstance();

    //获取语言名
    const string* GetLanguageName();
    
    
    //获取语言包 key 对应内容
    const string GetText(uint32_t key);

    //设置语言
    bool SetLanguageName(const char* langName = "Chinese");

};

#endif /* defined(__MultiLanguage_H_) */
