//
//  Network.cpp
//  TokenGame
//
//  Created by token on 15/5/4.
//
//


#include "Network.h"

const string urlbase = "http://182.254.225.236:8098/";

Network* Network::sm_Network = nullptr;

Network* Network::getInstance()
{
    if (sm_Network == nullptr)
    {
        sm_Network = new (std::nothrow) Network();
        if (sm_Network->Init()) {}
    }
    return sm_Network;
}

bool Network::Init()
{
    
    return true;
}

void Network::Destroy()
{
    HttpClient::getInstance()->destroyInstance();
}

string Network::GetUserId()
{
    string imeiStr = RecordUtils::getInstance()->getImei();
    return imeiStr;
}

string Network::GetUserName()
{
    string imeiStr = GetUserId();
    if (imeiStr.compare("") == 0) return "";
    
    string url = urlbase + "GetUserName";
    string data = StringUtils::format("imei=%s", imeiStr.c_str());
    HttpPost(url, data.c_str(),
             [=](std::vector<char> * buffer)
             {
                 if (buffer == nullptr || buffer->empty() || buffer->size() == 0) return;
                 
                 string rankName(buffer->begin(), buffer->end()-1);
                 UserData::getInstance()->setRankName(rankName);
             });
    
    return UserData::getInstance()->GetUserData(UserDataEnum::UserName)->asString();
}

/*  返回0:  成功设置名字
       -1: 名字已存在
 */
void Network::SetUserName(string userName)
{
    string imeiStr = GetUserId();
    if (imeiStr.compare("") == 0) return;
    
    //userName 要base64 解决编码问题
    string url = urlbase + "SetUserName";
    string data = StringUtils::format("userName=%s&imei=%s", userName.c_str(), imeiStr.c_str());
    HttpPost(url, data.c_str(), [=](std::vector<char> * buffer)
    {
        if (buffer == nullptr || buffer->empty() || buffer->size() == 1) return;
        
        string result(buffer->begin(), buffer->end()-1);
        if (result.compare("0") == 0)
        {
            UserData::getInstance()->setRankName(userName);
        }
    });
}

void Network::UpdateProductionTime()
{
    string url = urlbase + "UpdateTime";
    string data = StringUtils::format("type=set&userId=%s", GetUserId().c_str());
    HttpPost(url, data.c_str(), [](std::vector<char> * buffer)
      {
          
      });
}

void Network::GetProductionTime(const std::function<void(int timeStamp)>& httpCallback)
{
    string url = urlbase + "UpdateTime?";
    string data = StringUtils::format("type=get&userId=%s", GetUserId().c_str());
    HttpPost(url, data.c_str(), [=](std::vector<char> * buffer)
             {
                 if (buffer == nullptr || buffer->empty() || buffer->size() == 1)
                 {
                     httpCallback(0);
                 }else
                 {
                     string  strtime;
                     strtime.assign(buffer->begin(), buffer->end()-1);
                     int timeStamp = atoi(strtime.c_str());
                     httpCallback(timeStamp);
                 }
             });
    
}

void Network::GetServerTime(const std::function<void(int timeStamp)> & httpCallBack)
{
    string url = urlbase + "GetServerTime";
    string data = "";
    HttpPost(url, data.c_str(), [=](std::vector<char> * buffer)
    {
        if ( buffer != nullptr && buffer->size() > 1)
        {
            string serverTimeSeconds;
            serverTimeSeconds.assign(buffer->begin(), buffer->end()-1);
            int timeStamp = atoi(serverTimeSeconds.c_str());
            httpCallBack(timeStamp);
        }
    });
    
}

//更新竞技场最高层次
void Network::UpdateAreanVal(int val)
{
    if ( !GetUserName().compare("") ) return;
    
    string url = urlbase + "UpdateRanking";
    string data = StringUtils::format("type=Arena&userId=%s&score=%d", GetUserId().c_str(), val);
    HttpPost(url, data.c_str(), [](std::vector<char> * buffer)
             {
                 
             });
}

//更新探索度最大值
void Network::UpdateExploreVal(int val)
{
    if ( !GetUserName().compare("") ) return;
    
    string url = urlbase + "UpdateRanking";
    string data = StringUtils::format("type=Explore&userId=%s&score=%d", GetUserId().c_str(), val);
    HttpPost(url, data.c_str(), [](std::vector<char> * buffer)
             {
                 
             });
}

//
//{
//    "Explore": {
//        "TopRanking": [
//                       {
//                       "Rank": 1,
//                       "Score": 3,
//                       "Name": "n3name"
//                       }
//                      ],
//        "SelfRanking": {
//            "Rank": 3,
//            "Score": 1,
//            "Name": "n1name"
//        }
//    },
//    "Arena": {
//        "TopRanking": [
//                       {
//                       "Rank": 1,
//                       "Score": 3,
//                       "Name": "n3name"
//                       }
//                       ],
//        "SelfRanking": {
//            "Rank": 3,
//            "Score": 1,
//            "Name": "n1name"
//        }
//    }
//}
void Network::GetRanking(const std::function<void(vector<tuple<int,int,string>>* explore,
                         vector<tuple<int,int,string>>* arena)>& httpCallback)
{
    string url = urlbase + "Ranking";
    string data = StringUtils::format("userId=%s", GetUserId().c_str());
    HttpPost(url, data.c_str(), [=](std::vector<char> * buffer)
     {
         if (buffer == nullptr || buffer->empty() || buffer->size() == 1)
         {
             httpCallback(nullptr, nullptr);
         }else
         {
             rapidjson::Document jd;

             string jsonData(buffer->begin(), buffer->end()-1);
             
             jd.Parse<0>(jsonData.c_str());
             
             if(jd.HasParseError()) httpCallback(nullptr, nullptr);
             if(!jd.IsObject()) httpCallback(nullptr, nullptr);
             
             vector<tuple<int,int,string>>* arena = new vector<tuple<int,int,string>>();;
             vector<tuple<int,int,string>>* explore = new vector<tuple<int,int,string>>();
             RankingData(&jd, "Explore", explore);
             RankingData(&jd, "Arena", arena);
             httpCallback(explore, arena);
         }

     });

}

void Network::RankingData(rapidjson::Document* jd, string typeName, vector<tuple<int,int,string>>* data)
{
    //是否有此成员
    if(!jd->HasMember(typeName.c_str())) return;

    const rapidjson::Value &obj = (*jd)[typeName.c_str()];
    if (!obj.IsObject()) return;
    
    // 通过[]取成员值,再根据需要转为array,int,double,string
    const rapidjson::Value &topRanking = obj["TopRanking"];
    //是否是数组
    if(!topRanking.IsArray()) return;
    
    // 通过[]取成员值,再根据需要转为array,int,double,string
    const rapidjson::Value &selfRanking = obj["SelfRanking"];
    //是否是数组
    if(!selfRanking.IsObject()) return;
    
    for (rapidjson::SizeType i = 0; i < topRanking.Size(); i++)
    {
        const rapidjson::Value &obj = topRanking[i];
        if(obj.IsObject())
        {
            int rank = 10000;
            if (obj.HasMember("Rank"))
            {
                rank = obj["Rank"].GetInt();
            }
            int score = 0;
            if (obj.HasMember("Score"))
            {
                score = obj["Score"].GetInt();
            }
            string name = "";
            if (obj.HasMember("Name"))
            {
                name = obj["Name"].GetString();
            }
            data->push_back(make_tuple(rank, score, name));
        }
    }
    int rank = 10000;
    if (selfRanking.HasMember("Rank"))
    {
        rank = selfRanking["Rank"].GetInt();
    }
    int score = 0;
    if (selfRanking.HasMember("Score"))
    {
        score = selfRanking["Score"].GetInt();
    }
    string name = "";
    if (selfRanking.HasMember("Name"))
    {
        name = selfRanking["Name"].GetString();
    }
    data->push_back(make_tuple(rank, score, name));

}

void Network::HttpGet(string url, const std::function<void(std::vector<char> *buffer)>& httpCallback)
{
    
    HttpRequest* request = new (std::nothrow) HttpRequest();
    request->setUrl(url.c_str());
    request->setRequestType(HttpRequest::Type::GET);
    
    request->setResponseCallback([=](HttpClient *sender, HttpResponse *response)
     {
         if (!response)
         {
             httpCallback(nullptr);
             return;
         }
         
         long statusCode = response->getResponseCode();
         
         CCLOG("Http Status Code:%ld", statusCode);
         
         if (!response->isSucceed())
         {
             log("Http error buffer: %s", response->getErrorBuffer());
             httpCallback(nullptr);
             return;
         }
         
         std::vector<char> *buffer = response->getResponseData();
         httpCallback(buffer);
         if (response->getHttpRequest()->getReferenceCount() != 2)
         {
             log("request ref count not 2, is %d", response->getHttpRequest()->getReferenceCount());
         }
         
     });
    
    HttpClient::getInstance()->send(request);
    
    request->release();
    
}


void Network::HttpPost(string url, const char* data, const std::function<void(std::vector<char> *buffer)>& httpCallback)
{

    HttpRequest* request = new (std::nothrow) HttpRequest();
    request->setUrl(url.c_str());
    request->setRequestData(data, strlen(data));
    request->setRequestType(HttpRequest::Type::POST);
    request->setResponseCallback([=](HttpClient *sender, HttpResponse *response)
     {
         if (!response)
         {
             httpCallback(nullptr);
             return ;
         }
         
         long statusCode = response->getResponseCode();

         CCLOG("Http Status Code:%ld", statusCode);
         
         if (!response->isSucceed())
         {
             log("Http error buffer: %s", response->getErrorBuffer());
             httpCallback(nullptr);
             return ;
         }
         
         std::vector<char> *buffer = response->getResponseData();
         
         httpCallback(buffer);
         
         if (response->getHttpRequest()->getReferenceCount() != 2)
         {
             log("request ref count not 2, is %d", response->getHttpRequest()->getReferenceCount());
         }
                                     
     });

    HttpClient::getInstance()->send(request);
    
    request->release();
    
}

