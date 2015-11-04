//
//  MoveText.h
//  TokenGame
//
//  Created by token on 15/4/10.
//
//

#ifndef __TokenGame__MoveText__
#define __TokenGame__MoveText__


#include "Common.h"

class MoveText
{
    
public:
    
    enum class Direction
    {
        Up,
        Down,
    };
    
    typedef struct TextData
    {
        
        Color3B Color;
        
        float FontSize;
        float PositionX;
        float PositionY;
        
        Node* ParenNode;
        Text* TextWidget;
        
        string TextInfo;
        string ScheduleName;
        MoveText::Direction Direction;
        
        bool isBattleText;
        
    }TextData;

    
private:
    

    bool Init();
    static MoveText* sm_MoveText;
    MoveText& operator = (const MoveText& other){return *this;};
    
private:
    
    Text* m_LastUpTextWidget = nullptr;
    Text* m_LastDownTextWidget = nullptr;
    
    const string m_TTFFontName = "FZZYJW.TTF";
    
public:

    static MoveText* getInstance();
    
    //清除文本数据
    void ClearTextData();
    
    void DelTextData(int Id);
    
    //增加一个文本数据
    void AddTextData(TextData textData);
    
    //增加一个默认显示方式的文本
    void AddDefaultTextData(Node* node, string text);
    
    //获取了资源 移动信息
    void GetResText(Node* node, int itemId, int itemNum);
    
    //缺少资源 移动信息
    void LackResText(Node* node, vector<pair<int, int>>* lackData);
    
    //
    string getLackText(vector<pair<int, int>>* lackData);
};

#endif /* defined(__TokenGame__MoveText__) */
