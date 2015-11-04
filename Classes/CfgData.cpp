
#include "CfgData.h"


CfgData* CfgData::sm_CfgData = nullptr;

CfgData* CfgData::getInstance()
{
    if (sm_CfgData == nullptr)
    {
        sm_CfgData = new (std::nothrow) CfgData();
        if(sm_CfgData->Init() == false)
        {
            
        }
        
    }
    return sm_CfgData;
}

bool CfgData::Init()
{
    //存放每个表的所有Id
    sm_TableIdMap = new (std::nothrow) unordered_map<string, vector<uint32_t>>();
    
    //存放所有Id对应每行记录
    sm_GlobalIdMap = new (std::nothrow) unordered_map<uint32_t, ValueMapIntKey>();
    
    return  true;
}

//删除配置Id
void CfgData::DeleteCfgId(uint32_t cfgId)
{
    sm_GlobalIdMap->erase(cfgId);
}

//加载配置文件
bool CfgData::LoadCfgFile(const string& cfgFile)
{
    
    auto fileData = FileUtils::getInstance()->getDataFromFile(cfgFile.c_str());
    if (fileData.isNull())
    {
        return false;
    }
    if (fileData.getSize() <10) return false;
    
    auto buffer = (char*)fileData.getBytes();
    uint32_t idx = 0;
    FieldInfo fieldInfo;
    vector<FieldInfo> fieldInfoVector;
    uint16_t rows = BinaryUtils::ReadInt16(buffer, idx);
    uint16_t cols = BinaryUtils::ReadInt16(buffer, idx);
    
    for (size_t i = 0; i < cols; i++)
    {
        fieldInfo.FieldGlobalIdx = BinaryUtils::ReadInt16(buffer, idx);
        fieldInfo.GlobalDataType = BinaryUtils::ReadInt16(buffer, idx);
        fieldInfo.BaseDataType = BinaryUtils::ReadInt16(buffer, idx);
        fieldInfoVector.push_back(fieldInfo);
    }
    
    uint16_t num;
    Value tempVal;
    
    ValueVector vv;
    uint32_t cfgId;
    string strData;
    uint32_t tempKey;
    //Vector<pair<int, Value>>
    ValueMapIntMultiKey vmim;
    
    ValueMapIntKey vmRowData;
    vmRowData.reserve(cols);
    
    vector<uint32_t> vTableId;
    vTableId.reserve(rows);
    
    for (uint16_t row = 0; row < rows; row++)
    {
        //每行第一列是 cfgId
        cfgId = BinaryUtils::ReadUInt32(buffer, idx);
        
        vTableId.push_back(cfgId);
        vmRowData.clear();
        for (size_t i = 1; i < fieldInfoVector.size(); i++)
        {
            fieldInfo = fieldInfoVector.at(i);
            switch (fieldInfo.GlobalDataType)
            {
                case VECTOR_VAL:
                    num = BinaryUtils::ReadInt16(buffer, idx);
                    vv.clear();
                    vv.reserve(num);
                    for (uint16_t i = 0; i < num; i++)
                    {
                        switch (fieldInfo.BaseDataType)
                        {
                            case INT_VAL:
                                tempVal = BinaryUtils::ReadInt32(buffer, idx);
                                break;
                            case FLOAT_VAL:
                                tempVal = BinaryUtils::ReadFloat(buffer, idx);
                                break;
                            default:
                                BinaryUtils::ReadString(strData, buffer, idx);
                                tempVal = strData;
                                break;
                        }
                        vv.push_back(tempVal);
                    }
                    vmRowData[fieldInfo.FieldGlobalIdx] = vv;
                    break;
                case HASHMAP_VAL:
                    num = BinaryUtils::ReadInt16(buffer, idx);
                    vmim.clear(); //vmim.re reserve(num);
                    for (uint16_t i = 0; i < num; i++)
                    {
                        //规则key 类型为int
                        tempKey = BinaryUtils::ReadInt32(buffer, idx);
                        switch (fieldInfo.BaseDataType)
                        {
                            case INT_VAL:
                                tempVal = BinaryUtils::ReadInt32(buffer, idx);
                                break;
                            case FLOAT_VAL:
                                tempVal = BinaryUtils::ReadFloat(buffer, idx);
                                break;
                            default:
                                BinaryUtils::ReadString(strData, buffer, idx);
                                tempVal = strData;
                                break;
                        }
                        vmim.insert(pair<int, Value>(tempKey, tempVal));
                        //vm[tempKey] = tempVal;
                    }
                    vmRowData[fieldInfo.FieldGlobalIdx] = vmim;
                    break;
                    
                default:
                    switch (fieldInfo.BaseDataType)
                    {
                        case INT_VAL:
                            vmRowData[fieldInfo.FieldGlobalIdx] = BinaryUtils::ReadInt32(buffer, idx);
                            break;
                        case FLOAT_VAL:
                            vmRowData[fieldInfo.FieldGlobalIdx] = BinaryUtils::ReadFloat(buffer, idx);
                            break;
                        default:
                            BinaryUtils::ReadString(strData, buffer, idx);
                            vmRowData[fieldInfo.FieldGlobalIdx] = strData;
                            break;
                    }
                    break;
            }
        }
        sm_GlobalIdMap->insert(pair<uint32_t, ValueMapIntKey>(cfgId, vmRowData));
        //CCLOGINFO("CfgId:%d, cols:%lu", cfgId, vmRowData.size());
    }
    string tableName = cfgFile.substr(0, cfgFile.size() - 3);
    sm_TableIdMap->insert(pair<string, vector<uint32_t>>(tableName, vTableId));
    CCLOGINFO("CfgTable:%s, rows:%lu", tableName.c_str(), vTableId.size());
    fileData.clear();
    return true;
}


///// <summary>
///// 获取Id的最后3位值
///// </summary>
///// <param name="cfgId">配置表Id</param>
///// <returns></returns>
//uint16_t CfgData::TailCfgId(uint32_t cfgId)
//{
//    return cfgId % 10000;
//}

///// <summary>
///// 获取Id的前3位值
///// </summary>
///// <param name="cfgId">配置表Id</param>
///// <returns></returns>
//uint16_t CfgData::HeadCfgId(uint32_t cfgId)
//{
//    return cfgId / 1000000;
//}
//
///// <summary>
///// 获取Id中间4~6位值
///// </summary>
///// <param name="cfgId">配置表Id</param>
///// <returns></returns>
//uint16_t CfgData::MiddleCfgId(uint32_t cfgId)
//{
//    return cfgId / 10000 % 10000;
//}

const ValueMapIntKey* CfgData::GetCfgRow(uint32_t cfgId)
{
    ValueMapIntKey* vm = nullptr;
    
    auto iter = sm_GlobalIdMap->find(cfgId);
    if (iter == sm_GlobalIdMap->end())
    {
        CCLOGERROR("Not cfgId:%d", cfgId);
    }
    else
    {
        vm = &iter->second;
    }
    
    return vm;
}

//获取指定表中所有Id
const vector<uint32_t>* CfgData::GetCfgTableId(string cfgTable)
{
    vector<uint32_t>* v = nullptr;
    
    auto iter = sm_TableIdMap->find(cfgTable);
    if (iter == sm_TableIdMap->end())
    {
        //Id不存在
        CCLOGERROR("Not CfgName:%s", cfgTable.c_str());
    }
    else
    {
        v = &iter->second;
    }
    
    return v;
}

//把 vmi 给 vii 并排序
void CfgData::VMIMKToVIV_Sort(const ValueMapIntMultiKey* vmim, vector<pair<int, Value>>* viv)
{
    auto iter = vmim->begin();
    while (iter != vmim->end())
    {
        viv->push_back(make_pair(iter->first, iter->second));
        iter++;
    }
    
    std::sort(viv->begin(), viv->end(), [](const pair<int, Value> pair1, const pair<int, Value> pair2)
              {
                  return pair1.first < pair2.first;
              });
}


//根据Id 和 field 获取 对应的 value
const Value* CfgData::GetValue(uint32_t cfgId, CfgField field)
{
    return GetValue(GetCfgRow(cfgId), cfgId, field);
}

//根据Id 和 field 获取 对应的 value
const Value* CfgData::GetValue(const ValueMapIntKey* vm, uint32_t cfgId, CfgField field)
{
    const Value* val = nullptr;
    auto iter = vm->find((int)field);
    if (iter == vm->end())
    {
        //Id不存在
        CCLOGERROR("not id:%d field:%d", cfgId, (int)field);
        //val = &Value::Null;
    }
    else
    {
        val = &(iter->second);
    }
    
    return val;
}
