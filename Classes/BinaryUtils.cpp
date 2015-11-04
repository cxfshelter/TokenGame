#include "BinaryUtils.h"

using namespace std;


//判断是大端还是小端 true,little-endian false,big-endian 
//负数没有 大端小端 区别
bool BinaryUtils::IsLittleEndian = 0x1234 == 0x12 ? false : true;


//buffer to int8_t
int8_t BinaryUtils::BufferToInt8(const char* buffer)
{
	return *(int8_t*)buffer;
}

//buffer to uint8_t
uint8_t BinaryUtils::BufferToUInt8(const char* buffer)
{
	return *(uint8_t*)buffer;
}


//int8_t to buffer
void BinaryUtils::Int8ToBuffer(int8_t val, char* buffer)
{
	memcpy(buffer, &val, sizeof(int8_t));
}


//uint8_t to buffer
void BinaryUtils::UInt8ToBuffer(uint8_t val, char* buffer)
{
	memcpy(buffer, &val, sizeof(uint8_t));
}

//unsigned char [4] len>=4 -> unsigned int
uint32_t BinaryUtils::BufferToUInt32(const char* buffer)
{
	uint32_t val = *(uint32_t*)buffer;
	if (BinaryUtils::IsLittleEndian)
	{
		return val;
	}
	else
	{
		return SWAP32(val);
	}
}

//unsigned char [4] len>=4 -> unsigned int
int32_t BinaryUtils::BufferToInt32(const char* buffer)
{
	int32_t val = *(int32_t*)buffer;
	if (BinaryUtils::IsLittleEndian)
	{
		return val;
	}
	else
	{
		return SWAP32(val);
	}
}


//unsigned int -> unsigned char[4] len >=4
void  BinaryUtils::UInt32ToBuffer(uint32_t val, char* buffer)
{
	if (!BinaryUtils::IsLittleEndian)
	{
		val = SWAP32(val);
	}
	*(uint32_t*)buffer = val;
}

//unsigned int -> unsigned char[4] len >=4
void  BinaryUtils::Int32ToBuffer(int32_t val, char* buffer)
{
	if (!BinaryUtils::IsLittleEndian)
	{
		val = SWAP32(val);
	}
	*(int32_t*)buffer = val;
}


//unsigned char [8] len>=8 -> unsigned long long int
uint64_t  BinaryUtils::BufferToUInt64(const char* buffer)
{
	uint64_t val = *(uint64_t*)buffer;
	if (BinaryUtils::IsLittleEndian)
	{
		return val;
	}
	else
	{
		return SWAP64(val);
	}
	
}
//unsigned long long int -> unsigned char [8] len >=8
void  BinaryUtils::UInt64ToBuffer(uint64_t val, char* buffer)
{
	if (!BinaryUtils::IsLittleEndian)
    {
		val = SWAP64(val);
    }
	*(uint64_t*)buffer = val;
}

// char [8] len>=8 ->  long long int
int64_t  BinaryUtils::BufferToInt64(const char* buffer)
{
	int64_t val = *(int64_t*)buffer;
	if (BinaryUtils::IsLittleEndian)
	{
		return val;
	}
	else
	{
		return SWAP64(val);
	}

}
// long long int ->  char [8] len >=8
void  BinaryUtils::Int64ToBuffer(int64_t val, char* buffer)
{
	if (!BinaryUtils::IsLittleEndian)
	{
		val = SWAP64(val);
	}
	*(int64_t*)buffer = val;
}


//unsigned char [2] len>=2 -> unsigned short int 
uint16_t BinaryUtils::BufferToUInt16(const char* buffer)
{
	uint16_t val = *(uint16_t*)buffer;
	if (BinaryUtils::IsLittleEndian)
	{
		return val;
	}
	else
	{
		return SWAP16(val);
	}
}
//short int ->  unsigned char bytes[2] len>=2
void  BinaryUtils::UInt16ToBuffer(uint16_t val, char* buffer)
{
	if (!BinaryUtils::IsLittleEndian)
	{
		val = SWAP16(val);
	}
	*(uint16_t*)buffer = val;
}

//unsigned char [2] len>=2 -> unsigned short int 
int16_t BinaryUtils::BufferToInt16(const char* buffer)
{
	int16_t val = *(int16_t*)buffer;
	if (BinaryUtils::IsLittleEndian)
	{
		return val;
	}
	else
	{
		return SWAP16(val);
	}
}
//short int ->  unsigned char bytes[2] len>=2
void  BinaryUtils::Int16ToBuffer(int16_t val, char* buffer)
{
	if (!BinaryUtils::IsLittleEndian)
	{
		val = SWAP16(val);
	}
	*(int16_t*)buffer = val;
}


//little endian float data to big endian 
float BinaryUtils::FloatLToB(float data)
{

	float_union d1, d2;

	d1.f = data;

	d2.c[0] = d1.c[3];

	d2.c[1] = d1.c[2];

	d2.c[2] = d1.c[1];

	d2.c[3] = d1.c[0];

	return d2.f;
}

//big endian float data to little endian
float BinaryUtils::FloatBToL(float data)
{
	float_union d1, d2;

	d2.f = data;

	d1.c[0] = d2.c[3];

	d1.c[1] = d2.c[2];

	d1.c[2] = d2.c[1];

	d1.c[3] = d2.c[0];

	return d1.f;
}

//char to float
float BinaryUtils::BufferToFloat(const char* buffer)
{
	float* val = (float*)malloc(sizeof(float));
	memcpy(val, (const char*)buffer, sizeof(float));
	if (BinaryUtils::IsLittleEndian)
	{
		return *val;
	}
	else
	{
		return FloatLToB(*val);
	}
}

//float to char
void BinaryUtils::FloatToBuffer(float val, char* buffer)
{
	if (!BinaryUtils::IsLittleEndian)
	{
		val = FloatBToL(val);
	}
	memcpy(buffer, &val, sizeof(float));
}


//把str中的全部内容转到 buffer中
void BinaryUtils::WriteString(const std::string& str, char* buffer, uint32_t& idx)
{
	BinaryUtils::WriteUInt16(str.size(), buffer, idx);
	memcpy(buffer + idx, str.c_str(), str.size()); idx += str.size();
}


//把 buffer中 先读2个字节长度 再读数据
void BinaryUtils::ReadString(std::string& str, const char* buffer, uint32_t& idx)
{
	uint16_t strLen = BinaryUtils::ReadUInt16(buffer, idx);
	str.clear();	str.append((buffer + idx), strLen); idx += strLen;
}

int8_t BinaryUtils::ReadInt8(const char* buffer, uint32_t& idx)
{
	int8_t val = BinaryUtils::BufferToInt8(buffer + idx);
	idx += sizeof(char); return val;
}

void BinaryUtils::WriteInt8(const char val, char* buffer, uint32_t& idx)
{
	BinaryUtils::Int8ToBuffer(val, buffer + idx); idx += sizeof(int8_t);
}

uint8_t BinaryUtils::ReadUInt8(const char* buffer, uint32_t& idx)
{
	uint8_t val = BinaryUtils::BufferToUInt8(buffer + idx);
	idx += sizeof(char); return val;
}

void BinaryUtils::WriteUInt8(const uint8_t val, char* buffer, uint32_t& idx)
{
	BinaryUtils::Int8ToBuffer(val, buffer + idx); idx += sizeof(uint8_t);
}

int16_t BinaryUtils::ReadInt16(const char* buffer, uint32_t& idx)
{
	int16_t val = BinaryUtils::BufferToInt16(buffer + idx); 
	idx += sizeof(int16_t); return val;
}

void BinaryUtils::WriteInt16(int16_t val, char* buffer, uint32_t& idx)
{
	BinaryUtils::Int16ToBuffer(val, buffer + idx); idx += sizeof(int16_t);
}

uint16_t BinaryUtils::ReadUInt16(const char* buffer, uint32_t& idx)
{
	uint16_t val = BinaryUtils::BufferToUInt16(buffer + idx);
	idx += sizeof(uint16_t); return val;
}

void BinaryUtils::WriteUInt16(uint16_t val, char* buffer, uint32_t& idx)
{
	BinaryUtils::UInt16ToBuffer(val, buffer + idx); idx += sizeof(uint16_t);
}


int32_t BinaryUtils::ReadInt32(const char* buffer, uint32_t& idx)
{
	int32_t val = BinaryUtils::BufferToInt32(buffer + idx);
	idx += sizeof(int32_t); return val;
}

void BinaryUtils::WriteInt32(int32_t val, char* buffer, uint32_t& idx)
{
	BinaryUtils::Int32ToBuffer(val, buffer + idx); idx += sizeof(int32_t);
}

uint32_t BinaryUtils::ReadUInt32(const char* buffer, uint32_t& idx)
{
	uint32_t val = BinaryUtils::BufferToUInt32(buffer + idx); 
	idx += sizeof(uint32_t); return val;
}

void BinaryUtils::WriteUInt32(uint32_t val, char* buffer, uint32_t& idx)
{
	BinaryUtils::UInt32ToBuffer(val, buffer + idx); idx += sizeof(uint32_t);
}

int64_t BinaryUtils::ReadInt64(const char* buffer, uint32_t& idx)
{
	int64_t val = BinaryUtils::BufferToInt64(buffer + idx);
	idx += sizeof(int64_t); return val;
}

void BinaryUtils::WriteInt64(int64_t val, char* buffer, uint32_t& idx)
{
	BinaryUtils::Int64ToBuffer(val, buffer + idx);  idx += sizeof(int64_t);
}

uint64_t BinaryUtils::ReadUInt64(const char* buffer, uint32_t& idx)
{
	uint64_t val = BinaryUtils::BufferToUInt64(buffer + idx);
	idx += sizeof(uint64_t); return val;
}

void BinaryUtils::WriteUInt64(uint64_t val, char* buffer, uint32_t& idx)
{
	BinaryUtils::UInt64ToBuffer(val, buffer + idx);  idx += sizeof(uint64_t);
}

void BinaryUtils::WriteFloat(float val, char* buffer, uint32_t& idx)
{
	BinaryUtils::FloatToBuffer(val, buffer + idx);
	idx += sizeof(float);
}

float BinaryUtils::ReadFloat(const char* buffer, uint32_t& idx)
{
	float val = BinaryUtils::BufferToFloat(buffer + idx);
	idx += sizeof(float); return val; 
}

