//
//  Network.h
//  TokenGame
//
//  Created by token on 15/5/4.
//
//

#ifndef __TokenGame__Network__
#define __TokenGame__Network__

#include "Common.h"


//#include "extensions/xxtea.h"
//#include "extensions/cocos-ext.h"
#include "network/HttpClient.h"

//USING_NS_CC_EXT;
using namespace cocos2d::network;

class Network
{
    
private:
    bool Init();
    static Network* sm_Network;
    Network& operator = (const Network& other){return *this;};
    
    void RankingData(rapidjson::Document* jd, string typeName, vector<tuple<int,int,string>>* data);
    
public:
    
    void Destroy();
    
    /*  返回移动设备的imei码（每个设备唯一的标识码）
     */
    string GetUserId();
    
    string GetUserName();
    
    static Network* getInstance();
    
    void UpdateProductionTime();
    
    //更新竞技场最高层次
    void UpdateAreanVal(int val);
    //更新探索度最大值
    void UpdateExploreVal(int val);
    
    void SetUserName(string userName);
    
    void GetProductionTime(const std::function<void(int timeStamp)>& httpCallback);
    void GetRanking(const std::function<void(vector<tuple<int,int,string>>* explore,
                               vector<tuple<int,int,string>>* arena)>& httpCallback);
    
    /*  获得服务器当前时间戳
        以服务器时间为准，写入userData里
     */
    void GetServerTime(const std::function<void(int timeStamp)> & httpCallBack);
    
    void HttpGet(string url, const std::function<void(std::vector<char> *buffer)>& httpCallback);
    
    void HttpPost(string url, const char* data, const std::function<void(std::vector<char> *buffer)>& httpCallback);
    
};

#endif /* defined(__TokenGame__Network__) */
