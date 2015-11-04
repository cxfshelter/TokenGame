#ifndef __TiledFourCell_H_
#define __TiledFourCell_H_

class TiledFourCell
{
    
public:
    
    void setiLeftTop(int _iLeftTop);
    
    int getiLeftTop();
    
    void setiLeftBottom(int _iLeftBottom);
    
    int getiLeftBottom();
    
    void setiRightTop(int _iRightTop);
    
    int getiRightTop();
    
    void setiRightBottom(int _iRightBottom);
    
    int getiRightBottom();
    
    /* 获取顶点值的总和 */
    
    int getiTotalNum();
    
private:
    
    int iLeftTop = 0; // 左上角
    
    int iLeftBottom = 0;// 左下角
    
    int iRightTop = 0; // 右上角
    
    int iRightBottom = 0; // 右下角

};


#endif