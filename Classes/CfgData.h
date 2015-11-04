#ifndef  _CFG_DATA_H_
#define  _CFG_DATA_H_

#include <string>
#include <vector>
#include <unordered_map>

#include "cocos2d.h"

#include "CfgField.h"
#include "BinaryUtils.h"


#define INT_VAL 1
#define FLOAT_VAL 2
#define STRING_VAL 3
#define VECTOR_VAL 4
#define HASHMAP_VAL 5


USING_NS_CC;
using namespace std;

class CfgData
{

private:

	struct FieldInfo
	{
		//int float string
		uint8_t BaseDataType;

		//int float string vector Map(KeyVal)
		uint8_t GlobalDataType;

		//全局列名索引
		uint16_t FieldGlobalIdx;

	};

    static CfgData* sm_CfgData;

	//存放每个表的所有Id
    unordered_map<string, vector<uint32_t>>* sm_TableIdMap;

	//存放所有Id对应每行记录
    unordered_map<uint32_t, ValueMapIntKey>* sm_GlobalIdMap;

	

public:

    static CfgData* getInstance();
    
    bool Init();
    
    //删除配置Id
    void DeleteCfgId(uint32_t cfgId);

	//加载配置文件
	bool LoadCfgFile(const string& cfgFile);

//	/// <summary>
//	/// 获取Id的最后3位值
//	/// </summary>
//	/// <param name="cfgId">配置表Id</param>
//	/// <returns></returns>
//	uint16_t TailCfgId(uint32_t cfgId);
//
//	/// <summary>
//	/// 获取Id的前3位值
//	/// </summary>
//	/// <param name="cfgId">配置表Id</param>
//	/// <returns></returns>
//	uint16_t HeadCfgId(uint32_t cfgId);
//
//	/// <summary>
//	/// 获取Id中间4~6位值
//	/// </summary>
//	/// <param name="cfgId">配置表Id</param>
//	/// <returns></returns>
//	uint16_t MiddleCfgId(uint32_t cfgId);
    
	//获取指定cfgId中的一行数据
	const ValueMapIntKey* GetCfgRow(uint32_t cfgId);
    
	//获取指定表中所有Id
	const vector<uint32_t>* GetCfgTableId(string cfgTable);
    
    //根据Id 和 field 获取 对应的 value
    const Value* GetValue(uint32_t cfgId, CfgField field);
    
    const Value* GetValue(const ValueMapIntKey* vm, uint32_t cfgId, CfgField field);
    
    //把 vmim 给 vii 并排序
    void VMIMKToVIV_Sort(const ValueMapIntMultiKey* vmim, vector<pair<int, Value>>* viv);
    
};




#endif