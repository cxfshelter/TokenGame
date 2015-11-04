#ifndef  _EXCELTOCFGDATA_H_
#define  _EXCELTOCFGDATA_H_



#include <vector>
#include <unordered_map>
#include <BasicExcel.hpp>


#define INT_VAL 1
#define FLOAT_VAL 2
#define STRING_VAL 3
#define VECTOR_VAL 4
#define HASHMAP_VAL 5


#define FLOAT_CHAR "F"
#define STRING_CHAR "S"


//最大10M
#define BINARYFILE_MAXSIZE  1024 * 512

using namespace std;
using namespace BExcel;

class ExcelToCfgData
{

public:
	ExcelToCfgData();
	~ExcelToCfgData();

public:


	
	typedef enum
	{
		EnFieldRowIdx  = 0,
		CnFieldRowIdx  = 1,
		DataTypeRowIdx = 2,
	}HeadRowInfo;

	

	struct IntKeyVal
	{
		uint16_t Key;
		uint16_t Val;
	};

	
	struct StrKeyVal
	{
		string Key;
		string Val;
	};
	

	struct FieldInfo
	{
		//int float string
		uint8_t BaseDataType;

		//int float string vector Map(KeyVal)
		uint8_t GlobalDataType;

		/*
		//Map Key 数据类型
		uint8_t M_KeyDataType;

		//Map Val 数据类型
		uint8_t M_ValDataType;

		*/

		//英文名
		string EnFieldName;

		//当前表索引
		uint16_t TableFieldIdx;
		
		//全局列名索引
		uint16_t FieldGlobalIdx;
		
		//当前表列索引的数组
		vector<uint16_t> VectorIdx;

		//当前表V,M类型的索引
		vector<IntKeyVal> VectorKVIdx;

		//该字段是否已合并的字段
		bool IsMergeField = false;

	};

	
public:

	string m_ExcelFileDir;
	string m_BinaryFileDir;
    string m_CfgFieldFileDir;
    
	std::vector<int> m_IdVector;//配置表所有Id 用于判断是否重复
	std::unordered_map<string, int> m_FieldGlobalIdxHm; //配置表所有 英文列名与对应位置

public:
	
	void RunExcelToCfgData();

	void WriteFieldEnumClass();
    
    string Get_S_F_Char(uint16_t dataType);
    
	uint16_t GetFieldGlobalIdx(string fieldName);
	
	void ReadExcelData(BasicExcelWorksheet* sheet);
	
	bool CheckDataType(uint8_t fieldDataType, uint8_t cellDataType);

	bool GetExcelFiles(string excelDir, vector<string>& excelfileVec);

	void WriteBinaryFile(const char* buffer, uint32_t bufferIdx, string file);

	void WriteBuffer(BasicExcelCell* cell, uint8_t dataType, char* buffer, uint32_t& bufferIdx);

	bool MergeField(string excelTableName, std::vector<FieldInfo>& fieldVector, string& errorInfo);

	bool GetExcelFieldInfo(BasicExcelWorksheet* sheet, std::vector<FieldInfo>& fieldVector, string& errorInfo);

	void FormatErrorString(string& froamtStr, const char* sheetName, uint16_t row, uint16_t cols, string reason);

	void FindFieldNamePrefix(string fieldNamePrefix, const std::vector<FieldInfo>& fieldVector, std::vector<FieldInfo>& mapFieldVector);
	

};

/*
excel文件格式：(不支持中文）
第一行英文名
第二行中文名
第三行数据类型
如果没填写(英文名,中文名,数据类型)中其中一项目不转表

列名规则
1,长度固定为9位数,所有的配置表Id不能重复 按3位数分段 100001002 第一段值1000 第二段值1 第三段2
2,每个表必须要有Id列 Id列必须放到第一列,列名第一字母必须用大写
3,如果表中有 多列意义相同只是数据不同时(ItemId_V1,ItemId_V2,ItemId_V3)必须使用 _V重复数 格式  物品1Id,物品2Id
4,如果表中有 多列与其它列数据有关联时(Item_Mk1,Item_Mv1)必须使用 _MK重复数-->MV重复数 相对应 列如 物品1Id 物品1数据，物品2Id 物品2数据
5,多表中意义相同的用一个英文和中文名
6,Item_Mk1的数据类型必须是int,Item_Mv1（int float string）


转换后的文件格式：
1,行数(2个字节),列数(2个字节),列索引(2个字节),全局类型(2个字节),数据类型(2个字节)
2,int(4个字节),float(4个字节),string(2个字节长度),v(2个字节),m(2个字节)
转换后的文件格式 列如:
Id   Name   Point  Item_V1  Item_V2  Item_MK1 Item_MV1   Item_MK2 Item_MV2
int  string float  int      int      int      float        int      float

//文件头数据结构：
行数(2个字节)列数(2个字节)
Id列索引(2个字节)全局类型(2个字节)数据类型(2个字节)
Name列索引(2个字节)全局类型(2个字节)数据类型(2个字节)
Point列索引(2个字节)全局类型(2个字节)数据类型(2个字节)
Item_V列索引(2个字节)全局类型(2个字节)数据类型(2个字节)
Item_M列索引(2个字节)全局类型(2个字节)Item_MK数据类型(2个字节)+Item_MV数据类型(2个字节)

文件数据结构：
Id值(4个字节)+string值(2个字节长度+string)+Point值(4个字节)+Item_V数量(2个字节)+N个Item值+Item_M数量(2个字节)+N个(Item_MK值+Item_MV值)

Item_VI(I表示类型)  Item_MF(F 表示类型Val类型)             


*/



/*
void Wchar_tToString(std::string& szDst, const wchar_t *wchar)
{
	const wchar_t * wText = wchar;
	DWORD dwNum = WideCharToMultiByte(CP_OEMCP, NULL, wText, -1, NULL, 0, NULL, FALSE);// WideCharToMultiByte的运用
	char *psText;  // psText为char*的临时数组，作为赋值给std::string的中间变量
	psText = new char[dwNum];
	WideCharToMultiByte(CP_OEMCP, NULL, wText, -1, psText, dwNum, NULL, FALSE);// WideCharToMultiByte的再次运用
	szDst = psText;// std::string赋值
	delete[]psText;// psText的清除
}
*/





#endif