
#include "BinaryUtils.h"
#include "ExcelToCfgData.h"


ExcelToCfgData::ExcelToCfgData()
{
	
}

ExcelToCfgData::~ExcelToCfgData()
{
}

//字段名相同的前缀
void ExcelToCfgData::FindFieldNamePrefix(string fieldNamePrefix, const std::vector<FieldInfo>& fieldVector, std::vector<FieldInfo>& resultFieldVector)
{
	string fieldName;
	resultFieldVector.clear();
	for (uint16_t j = 0; j < fieldVector.size(); j++)
	{
		if (fieldVector.at(j).IsMergeField) continue;
		fieldName = fieldVector.at(j).EnFieldName;
		if (fieldName.size() <= fieldNamePrefix.size()) continue;
		fieldName = fieldName.substr(0, fieldNamePrefix.size());
		if (fieldNamePrefix == fieldName)
		{
			resultFieldVector.push_back(fieldVector.at(j));
		}
	}
}

//获取字段全局索引
uint16_t ExcelToCfgData::GetFieldGlobalIdx(string fieldName)
{
	uint16_t globalIdx;
	auto it = m_FieldGlobalIdxHm.find(fieldName);
	if (it == m_FieldGlobalIdxHm.end())
	{
		globalIdx = m_FieldGlobalIdxHm.size();
		m_FieldGlobalIdxHm[fieldName] = globalIdx;
	}
	else
	{
		globalIdx = m_FieldGlobalIdxHm[fieldName];
	}
	
	return globalIdx;
}

//数据类型是 float string 加 F S
string ExcelToCfgData::Get_S_F_Char(uint16_t dataType)
{
	string dtc="";
	switch (dataType)
	{
	case FLOAT_VAL:
		dtc = "F";  break;
	case STRING_VAL:
		dtc = "S";  break;
	}
	return dtc;
}

//获取ExcelFileNames.txt中的 excelFileName
bool ExcelToCfgData::GetExcelFiles(string excelDir, std::vector<string>& excelfileVec)
{
		char line[256];
		excelfileVec.clear();
		const char* excelFileNames = excelDir.append("ExcelFiles.txt").c_str();
		FILE * filePtr = fopen(excelFileNames, "r"); if (NULL == filePtr) return false;
		
		while (!feof(filePtr))
		{
			if (NULL == fgets(line, 256, filePtr)) continue;

			//空行不处理
			if (line[0] == '\n' || line[0] == 13) continue;

			string fileName = line;
			excelfileVec.push_back(fileName);
		}
		fclose(filePtr);
		return  true;
}

void ExcelToCfgData::WriteBinaryFile(const char* buffer, uint32_t bufferIdx, string fullFilePath)
{
	remove(fullFilePath.c_str());

	FILE* filePtr = fopen(fullFilePath.c_str(), "a");
	if (NULL == filePtr)
	{
		//error
	}
	else
	{
		fwrite(buffer, bufferIdx, 1, filePtr);
		fclose(filePtr);
	}

}


//验证数据类型是否正确
//excel 中填写的数据是 int 读出来的类型可能是 double
bool ExcelToCfgData::CheckDataType(uint8_t fieldDataType, uint8_t cellDataType)
{
	if (fieldDataType == cellDataType) return true;

	if (fieldDataType == INT_VAL && cellDataType == FLOAT_VAL)
	{
		return true;
	}

	if (fieldDataType == FLOAT_VAL && cellDataType == INT_VAL)
	{
		return true;
	}
	// 数据类型不匹配
	return false;
}


//合并 数组 和 keyVal
bool ExcelToCfgData::MergeField(string excelTableName, std::vector<FieldInfo>& fieldVector, string& errorInfo)
{
	bool result = true;
	string FieldNamePrefix;
	int16_t underlinedIdx = 0;
	//生成的字段信息
	std::vector<FieldInfo> newFieldVector;
	std::vector<FieldInfo> tempFieldVector;
	for (uint16_t i = 0; i < fieldVector.size(); i++)
	{
		if (fieldVector.at(i).IsMergeField == true) continue;
		
		FieldInfo newFieldInfo;
		underlinedIdx = fieldVector.at(i).EnFieldName.find('_');
		if (underlinedIdx < 0)
		{
			newFieldInfo = fieldVector.at(i);

			newFieldInfo.GlobalDataType = newFieldInfo.BaseDataType;
            string s_f_char = Get_S_F_Char(newFieldInfo.BaseDataType);
            if(s_f_char != "")
            {
                newFieldInfo.EnFieldName.append("_"+ s_f_char);
            }
			newFieldVector.push_back(newFieldInfo); continue;
		}
		
		FieldNamePrefix = fieldVector.at(i).EnFieldName.substr(0, underlinedIdx + 2);
		char vm = FieldNamePrefix[FieldNamePrefix.size() - 1];
		if (vm != 'V' && vm != 'M')
		{
			//列名配置错误
			result = false;
			FormatErrorString(errorInfo, excelTableName.c_str(), 0, fieldVector.at(i).TableFieldIdx, fieldVector.at(i).EnFieldName + " column name error _(V,M)");
			break;
		}
		
		if (vm == 'V')
		{

			newFieldInfo.GlobalDataType = VECTOR_VAL;
			newFieldInfo.EnFieldName = FieldNamePrefix;
			newFieldInfo.BaseDataType = fieldVector.at(i).BaseDataType;
			
			FindFieldNamePrefix(FieldNamePrefix, fieldVector, tempFieldVector);
			for (uint16_t j = 0; j < tempFieldVector.size(); j++)
			{
				if (tempFieldVector.at(j).BaseDataType != newFieldInfo.BaseDataType)
				{
					//数据类型不一至
					result = false; 
					FormatErrorString(errorInfo, excelTableName.c_str(), 2, fieldVector.at(j).TableFieldIdx, tempFieldVector.at(j).EnFieldName + " data type Not equal to " + tempFieldVector.at(0).EnFieldName);
					break;
				}
				newFieldInfo.VectorIdx.push_back(tempFieldVector.at(j).TableFieldIdx);
				fieldVector.at(tempFieldVector.at(j).TableFieldIdx).IsMergeField = true; //表示已合并
			}

			newFieldInfo.EnFieldName.append(Get_S_F_Char(newFieldInfo.BaseDataType));
			newFieldVector.push_back(newFieldInfo); continue;
		}

		if (vm == 'M')
		{
			newFieldInfo.GlobalDataType = HASHMAP_VAL;
			newFieldInfo.EnFieldName = FieldNamePrefix;
			FindFieldNamePrefix(FieldNamePrefix, fieldVector, tempFieldVector);

			uint16_t kvCount = tempFieldVector.size();

			if (kvCount % 2 != 0)
			{
				//配置错误  k v 不对应
				result = false; 
				FormatErrorString(errorInfo, excelTableName.c_str(), 2, fieldVector.at(i).TableFieldIdx,
                                  newFieldInfo.EnFieldName +" key / val Do not match");
				break;
			}

			//排序 Test_MK1  Test_MV1 Test_MK2  Test_MV2 -> Test_MK1 Test_MK2 Test_MV1 Test_MV2
			std::sort(tempFieldVector.begin(), tempFieldVector.end(), [&](const FieldInfo field1, const FieldInfo field2)
			{
				return field1.EnFieldName < field2.EnFieldName;
			});

			IntKeyVal kv;
			uint8_t kDT, vDT;
			for (uint16_t j = 0; j < kvCount /2 ; j++)
			{

				kDT = tempFieldVector.at(j).BaseDataType;
				//key的类型必须是 int
				if (kDT != INT_VAL)
				{

					//错误 当前kv对应的数据类型不是 int
					result = false;
					FormatErrorString(errorInfo, excelTableName.c_str(), 2, tempFieldVector.at(j).TableFieldIdx, tempFieldVector.at(j).EnFieldName + " keyval dataType  key not int");
					break;
				}
				vDT = tempFieldVector.at(kvCount / 2 + j).BaseDataType;
				if (j == 0)
				{
					newFieldInfo.BaseDataType = vDT;
				}
				else
				{
					if (vDT != newFieldInfo.BaseDataType)
					{
						//错误 当前kv对应该的数据类型与 第一对kv对应的数据类型不一至
						result = false; 
						FormatErrorString(errorInfo, excelTableName.c_str(), 2, tempFieldVector.at(kvCount / 2 + j).TableFieldIdx, tempFieldVector.at(kvCount / 2 + j).EnFieldName + " KeyVal Val data type  Do not match");
						break;
					}
				}

				kv.Key = tempFieldVector.at(j).TableFieldIdx;
				kv.Val = tempFieldVector.at(kvCount / 2 + j).TableFieldIdx;
				newFieldInfo.VectorKVIdx.push_back(kv);
				fieldVector.at(kv.Key).IsMergeField = true; //表示已合并
				fieldVector.at(kv.Val).IsMergeField = true; //表示已合并
			}
			string s_f_char = Get_S_F_Char(newFieldInfo.BaseDataType);
			newFieldInfo.EnFieldName.append(s_f_char);	newFieldVector.push_back(newFieldInfo);
		}
		
	}
	fieldVector.clear();
	fieldVector = newFieldVector;
	return result;
}

bool ExcelToCfgData::GetExcelFieldInfo(BasicExcelWorksheet* sheet, std::vector<FieldInfo>& fieldInfoVector, string& errorInfo)
{
	string enFieldName, typeFieldName;
	int16_t maxCols = sheet->GetTotalCols();

	bool result = true;
	FieldInfo fieldInfo;
	BasicExcelCell* cell;
	for (auto fieldIdx = 0; fieldIdx < maxCols; fieldIdx++)
	{
		//获取英文字段名
		cell = sheet->Cell(0, fieldIdx);
		switch (cell->Type())
		{
			case BasicExcelCell::STRING:
				enFieldName = cell->GetString();
				break;
			default: enFieldName = ""; break;
		}

		
		//获取数据类型
		cell = sheet->Cell(2, fieldIdx);
		switch (cell->Type())
		{
		case BasicExcelCell::STRING:
			typeFieldName = cell->GetString(); break;
			
		default: typeFieldName = "";	 break;
		}

		if (fieldIdx == 0 && enFieldName != "Id")
		{
			//表格第一列名必须是Id
			result = false;
			FormatErrorString(errorInfo, sheet->GetAnsiSheetName(), 1, 0, "The first column name not Id");
			break;
		}

		if (enFieldName == "")
        {
            //没用的列
            fieldInfo.IsMergeField = true;
            fieldInfoVector.push_back(fieldInfo);
            continue;
        }
		
		fieldInfo.EnFieldName = enFieldName;

		if(typeFieldName == "int")
		{
			fieldInfo.BaseDataType = INT_VAL;
		}
		else if(typeFieldName == "float")
		{
			fieldInfo.BaseDataType = FLOAT_VAL;
		}
		else if(typeFieldName == "string")
		{
			fieldInfo.BaseDataType = STRING_VAL;
		}

		if (fieldIdx == 0 && fieldInfo.BaseDataType != INT_VAL)
		{
			//表格第列名必须是Id 数据类型必须是 int
			result = false;  break;
		}

		if (fieldInfo.BaseDataType == 0)
        {
            //没用的列
            fieldInfo.IsMergeField = true;
            fieldInfoVector.push_back(fieldInfo);
            continue;
        }
        fieldInfo.IsMergeField = false;
		fieldInfo.TableFieldIdx = fieldIdx;
		fieldInfoVector.push_back(fieldInfo);
	}
	if (result)
	{
		string excelTableName = sheet->GetAnsiSheetName();
		result = MergeField(excelTableName, fieldInfoVector, errorInfo);

		if (result)
		{
			for (uint16_t i = 0; i < fieldInfoVector.size(); i++)
			{
				enFieldName = fieldInfoVector.at(i).EnFieldName;
				fieldInfoVector.at(i).FieldGlobalIdx = GetFieldGlobalIdx(enFieldName);
			}
		}
	}
	return result;
}


void ExcelToCfgData::WriteFieldEnumClass()
{
	
	string str;
	str.append("#ifndef  _CFG_FIELD_H_\n");
	str.append("#define  _CFG_FIELD_H_\n");

	str.append("\n");

	str.append("enum class CfgField\n");
	str.append("{\n");

	
	vector<pair<string, int>> pairVect;
	for (unordered_map<string, int>::iterator map_iter = m_FieldGlobalIdxHm.begin();
		map_iter != m_FieldGlobalIdxHm.end(); map_iter ++)
	{
		pairVect.push_back(make_pair(map_iter->first, map_iter->second));
	}

	std::sort(pairVect.begin(), pairVect.end(), [&](const pair<string, int> pair1, const pair<string, int> pair2)
	{
		return pair1.second < pair2.second;
	});

	
	char fieldIdxs[16];
	for (vector<pair<string, int>>::iterator pair_iter = pairVect.begin();
		pair_iter != pairVect.end(); pair_iter++)
	{
		str.append("	" + pair_iter->first + " = ");
		sprintf(fieldIdxs, "%d", pair_iter->second); str.append(fieldIdxs); str.append(",\n");
	}

	str.append("};\n");

	str.append("\n");
	str.append("#endif\n");

	string filePath = m_CfgFieldFileDir + "CfgField.h";

	//remove(filePath.c_str());
    
    char timeBuffer[256];
    auto nowTime = time(nullptr);
    auto tmPtr = localtime((time_t*)&nowTime);
    
    sprintf(timeBuffer,"%s_%2d-%2d-%2d-%2d-%2d", filePath.c_str(),tmPtr->tm_mon, tmPtr->tm_mday, tmPtr->tm_hour, tmPtr->tm_min, tmPtr->tm_sec);
    
    rename(filePath.c_str(), timeBuffer);
    
	FILE* filePtr = fopen(filePath.c_str(), "w");
	if (NULL == filePtr)
	{
		//error
	}
	else
	{
		char buffer[1024 * 512];
		sprintf(buffer, "%s", str.c_str());
		fwrite(buffer, str.size(), 1, filePtr);
		fclose(filePtr);
	}
}


void ExcelToCfgData::RunExcelToCfgData()
{
	string fileName;
	char* sheetName;
	uint16_t maxRows, maxCols;
	
	std::vector<string> excelFileVec;
	GetExcelFiles(m_ExcelFileDir, excelFileVec);

	BasicExcel e;
	BasicExcelWorksheet* sheet;
	for (uint16_t i = 0; i < excelFileVec.size(); i++)
	{
		fileName = excelFileVec.at(i);
		if (fileName[fileName.size() - 1] =='\n')
		{
			fileName = fileName.erase(fileName.size() - 1);
		}
        e.Load((m_ExcelFileDir +fileName).c_str());

		uint16_t maxSheets = e.GetTotalWorkSheets();
		if (maxSheets == 0) continue;
		for (uint16_t si = 0; si < maxSheets; si++)
		{
			sheetName = e.GetAnsiSheetName(si);
            if (nullptr == sheetName) continue;
            
			sheet = e.GetWorksheet(sheetName);
			if (nullptr == sheet) continue;
			
			maxRows = sheet->GetTotalRows();
			maxCols = sheet->GetTotalCols();
			if (maxRows <= 3 || maxCols < 1)
			{
				continue;
			}
			else
			{
				ReadExcelData(sheet);
			}
		}
	}

	WriteFieldEnumClass();
	
}




void ExcelToCfgData::WriteBuffer(BasicExcelCell* cell, uint8_t dataType, char* buffer, uint32_t& bufferIdx)
{
	switch (cell->Type())
	{
	case BasicExcelCell::UNDEFINED:
		//填写默认值
		switch (dataType)
		{
		case INT_VAL:
			BinaryUtils::WriteInt32(0, buffer, bufferIdx);
			break;
		case FLOAT_VAL:
			BinaryUtils::WriteFloat(0.0f, buffer, bufferIdx);
			break;
		case STRING_VAL:
			BinaryUtils::WriteString("", buffer, bufferIdx);
			break;
		}
		break;
	case BasicExcelCell::INT:
		if (dataType == FLOAT_VAL)
		{
			BinaryUtils::WriteFloat((float)cell->GetInteger(), buffer, bufferIdx);
		}
		else
		{
			BinaryUtils::WriteInt32(cell->GetInteger(), buffer, bufferIdx);
		}
		break;

	case BasicExcelCell::DOUBLE:
		if (dataType == INT_VAL)
		{
			BinaryUtils::WriteInt32((int32_t)cell->GetDouble(), buffer, bufferIdx);
		}
		else
		{
			BinaryUtils::WriteFloat((float)cell->GetDouble(), buffer, bufferIdx);
		}
		break;

	case BasicExcelCell::STRING:
		string strVal = cell->GetString();
		BinaryUtils::WriteString(strVal, buffer, bufferIdx);
		break;
	}
}


void ExcelToCfgData::FormatErrorString(string& froamtStr, const char* sheetName, uint16_t row, uint16_t cols, string reason)
{
	char n[16];
	froamtStr = "ExcelSheet:";
	froamtStr.append(sheetName); 
	froamtStr.append(" Row:");
	sprintf(n, "%d", row +1);
	froamtStr.append(n);
	froamtStr.append(" Cols:");
	sprintf(n, "%d", cols + 1);
	froamtStr.append(n);
	froamtStr.append(" " +reason);
}


void ExcelToCfgData::ReadExcelData(BasicExcelWorksheet* sheet)
{
	string errorInfo;
	bool result = true;

	FieldInfo fieldInfo;

	uint32_t bufferIdx = 0;
	char buffer[BINARYFILE_MAXSIZE];

	std::vector<FieldInfo> fieldInfoVector;
	result = GetExcelFieldInfo(sheet, fieldInfoVector, errorInfo);
	if (result)
	{
        
        auto totalRows = 0;
        //计算 主要是去掉空行的行数
        for (size_t row = 3; row < sheet->GetTotalRows(); row++)
        {
            auto cell = sheet->Cell(row, 0);
            if (cell->Type() == BasicExcelCell::UNDEFINED) break;
            
            totalRows++;
        }
        
		//把 行 列 字段 类型写到 buffer
		BinaryUtils::WriteInt16(totalRows, buffer, bufferIdx);

		BinaryUtils::WriteInt16(fieldInfoVector.size(), buffer, bufferIdx);

		for (size_t i = 0; i < fieldInfoVector.size(); i++)
		{

			//写FieldGlobalIdx
			BinaryUtils::WriteInt16(fieldInfoVector.at(i).FieldGlobalIdx, buffer, bufferIdx);
			//写GlobalDataType
			BinaryUtils::WriteInt16(fieldInfoVector.at(i).GlobalDataType, buffer, bufferIdx);
			//写BaseDataType
			BinaryUtils::WriteInt16(fieldInfoVector.at(i).BaseDataType, buffer, bufferIdx);

		}

		//把数据写到buffer中
		for (size_t row = 3; row < totalRows + 3; row++)
		{
			for (size_t c = 0; c < fieldInfoVector.size(); c++)
			{
				fieldInfo = fieldInfoVector.at(c);
				if (fieldInfo.GlobalDataType == VECTOR_VAL)
				{

					int notNumCell = 0;//非空的数据的个数
					BasicExcelCell* cell;
					uint16_t tableFieldIdx;
					for (uint16_t vidx = 0; vidx < fieldInfo.VectorIdx.size(); vidx++)
					{
						tableFieldIdx = fieldInfo.VectorIdx.at(vidx);
						cell = sheet->Cell(row, tableFieldIdx);
						if (cell->Type() == BasicExcelCell::UNDEFINED)
						{
							continue;  //没填写的直接去掉
						}
						if (!CheckDataType(fieldInfo.BaseDataType, (uint8_t)cell->Type()))
						{
							//错误 数据类型不对
							result = false;
							FormatErrorString(errorInfo, sheet->GetAnsiSheetName(), row, tableFieldIdx, "Error: data type");
							break;
						}
						notNumCell++;
					}

					BinaryUtils::WriteInt16(notNumCell, buffer, bufferIdx);

					if (notNumCell == 0) continue;

					for (size_t i = 0; i < fieldInfo.VectorIdx.size(); i++)
					{
						cell = sheet->Cell(row, fieldInfo.VectorIdx.at(i));

						if (cell->Type() == BasicExcelCell::UNDEFINED)
						{
							continue;  //没填写的直接去掉
						}
						WriteBuffer(cell, fieldInfo.BaseDataType, buffer, bufferIdx);
					}

				}
				else if (fieldInfo.GlobalDataType == HASHMAP_VAL)
				{
					int notNumCell = 0;//非空的数据的个数
					BasicExcelCell* kCell;
					BasicExcelCell* vCell;
					for (size_t i = 0; i < fieldInfo.VectorKVIdx.size(); i++)
					{

						kCell = sheet->Cell(row, fieldInfo.VectorKVIdx.at(i).Key);
						vCell = sheet->Cell(row, fieldInfo.VectorKVIdx.at(i).Val);

						if (kCell->Type() == BasicExcelCell::UNDEFINED || vCell->Type() == BasicExcelCell::UNDEFINED)
						{
							continue;
						}
						//检查 key 数据类型
						if (!CheckDataType(INT_VAL, (uint8_t)kCell->Type()))
						{
							//错误 数据类型不对
							result = false;
							FormatErrorString(errorInfo, sheet->GetAnsiSheetName(), row, fieldInfo.VectorKVIdx.at(i).Key, "key data type not int");
							break;
						}

						if (!CheckDataType(fieldInfo.BaseDataType, (uint8_t)vCell->Type()))
						{
							//错误 数据类型不对
							result = false;
							FormatErrorString(errorInfo, sheet->GetAnsiSheetName(), row, fieldInfo.VectorKVIdx.at(i).Val, "val data type error");
							break;
						}
						notNumCell++;
					}

					BinaryUtils::WriteInt16(notNumCell, buffer, bufferIdx);

					if (notNumCell == 0) continue;
					for (size_t i = 0; i < fieldInfo.VectorKVIdx.size(); i++)
					{

						kCell = sheet->Cell(row, fieldInfo.VectorKVIdx.at(i).Key);
						vCell = sheet->Cell(row, fieldInfo.VectorKVIdx.at(i).Val);
						if (kCell->Type() == BasicExcelCell::UNDEFINED || vCell->Type() == BasicExcelCell::UNDEFINED)
						{
							continue;
						}

						WriteBuffer(kCell, INT_VAL, buffer, bufferIdx);
						WriteBuffer(vCell, fieldInfo.BaseDataType, buffer, bufferIdx);
					}
				}
				else
				{
					BasicExcelCell* cell = sheet->Cell(row, fieldInfo.TableFieldIdx);
					if (fieldInfo.TableFieldIdx == 0)
					{
						int32_t id = 0;

						// excel 中填写的数据是 int 读出来的类型可能是 double
						if (!CheckDataType(INT_VAL, (uint8_t)cell->Type()))
						{
                            if (cell->Type() == BasicExcelCell::UNDEFINED)
                            {
                                break;
                            }
							//错误 Id号数据类型不对
							result = false;
							FormatErrorString(errorInfo, sheet->GetAnsiSheetName(), row, fieldInfo.TableFieldIdx, "Id Error: data type not int");
							break;
						}
						else
						{
							if (cell->Type() == INT_VAL)
							{
								id = cell->GetInteger();
							}
							else
							{
								id = cell->GetDouble();
							}

							if (id < 100000000 || id > 999999999)
							{
								// Id号 取值范围有误
								result = false;
								FormatErrorString(errorInfo, sheet->GetAnsiSheetName(), row,
                                                  fieldInfo.TableFieldIdx, "Id value Error ");
								break;
							}
                            
							auto it = std::find(m_IdVector.begin(), m_IdVector.end(), id);
							if (it != m_IdVector.end())
							{
								// Id号 重复
								result = false;
                                char idchar[16];
                                sprintf(idchar, "%d", id);
                                string errorStr = " Id value Repeat ";
                                errorStr.append(idchar);
								FormatErrorString(errorInfo, sheet->GetAnsiSheetName(), row, fieldInfo.TableFieldIdx, errorStr);
								break;
							}
							else
							{
								m_IdVector.push_back(id);
							}
						}
						//写Id
						BinaryUtils::WriteInt32(id, buffer, bufferIdx);
						continue;
					}

					if (cell->Type() != BasicExcelCell::UNDEFINED)
					{
						if (!CheckDataType(fieldInfo.BaseDataType, (uint8_t)cell->Type()))
						{
							// 数据类型不匹配
							result = false;
							FormatErrorString(errorInfo, sheet->GetAnsiSheetName(), row, fieldInfo.TableFieldIdx, "data type error");
							break;
						}
					}
					WriteBuffer(cell, fieldInfo.BaseDataType, buffer, bufferIdx);
				}
			}

			if (result == false) break;
		}
	}
	string fullFilePath = m_BinaryFileDir + "tk/"+ sheet->GetAnsiSheetName();
	if (result)
	{
		WriteBinaryFile(buffer, bufferIdx, fullFilePath + ".tk");
	}
	else
	{
		rename((fullFilePath + ".tk").c_str(), (fullFilePath + "_old.tk").c_str());
		WriteBinaryFile(errorInfo.c_str(), (uint32_t)errorInfo.size(), fullFilePath + "_error.tk");
	}

}

