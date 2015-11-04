//
//  SoundUtils.h
//  TokenGame
//
//  Created by best on 15/6/16.
//
//

#ifndef __TokenGame__SoundUtils__
#define __TokenGame__SoundUtils__

#include "cocos2d.h"

class SoundUtils
{
public:
    static SoundUtils* getInstance();
    
    /*  播放音乐或音效
        pszFilePath: 文件路径
        isMusic: true - 背景音乐，false - 音效
     */
    bool playMusicSound(const char* pszFilePath, bool isMusic, bool isLoop = false);
    
    
private:
    static SoundUtils * m_Instance;
    
};

#endif /* defined(__TokenGame__SoundUtils__) */
