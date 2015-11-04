//
//  DataEncryptUtils.h
//  TokenGame
//
//  Created by best on 15/7/31.
//
//

#ifndef __TokenGame__DataEncryptUtils__
#define __TokenGame__DataEncryptUtils__

#include "Common.h"

class DataEncryptUtils
{
    
private:
    static DataEncryptUtils * m_Instance;
    
    
    
public:
    static DataEncryptUtils * getInstance();
    
    
    // 只对int进行加密解密，int 加密-> string, string 解密-> int
    // 加密
    std::string encode(int valNum);
    
    std::string encode(std::string valStr);
    
    // 解密
    int decode(std::string valNum);
    
    std::string bestSaveData(unsigned char const* , unsigned int len);
    std::string bestParseData(std::string const& s);
    
};

#endif /* defined(__TokenGame__DataEncryptUtils__) */
