#ifndef  _BINARYUTILS_H_
#define  _BINARYUTILS_H_

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>


#define SWAP64(a) (((uint64_t)SWAP32(a) << 32) | SWAP32((uint64_t)(a)>>32))

#define SWAP16(i) \
	((uint16_t)((((uint32_t)(i) >> 8) & 0xFF) | (((uint32_t)(i) << 8) & 0xFF00)))

#define SWAP32(i) \
	((((i)& 0x000000FF) << 24) | (((i)& 0x0000FF00) << 8) | (((i)& 0x00FF0000) >> 8) | (((i)& 0xFF000000) >> 24))


class BinaryUtils
{

public:

	typedef union{
		     float f;
		     char c[4];
	}float_union;

	//判断是大端还是小端 
	//true,little-endian false,big-endian 
	static bool IsLittleEndian;

public:

	//buffer to int8_t
	static int8_t BufferToInt8(const char* buffer);
	//int8_t to buffer
	static void Int8ToBuffer(int8_t val, char* buffer);

	//buffer to uint8_t
	static uint8_t BufferToUInt8(const char* buffer);

	//uint8_t to buffer
	static void UInt8ToBuffer(uint8_t val, char* buffer);

	//unsigned char [2] len>=2 -> unsigned short int 
	static uint16_t BufferToUInt16(const char* buffer);
	//short int ->  unsigned char bytes[2] len>=2
	static void UInt16ToBuffer(uint16_t val, char* buffer);

	//unsigned char [2] len>=2 -> unsigned short int 
	static int16_t BufferToInt16(const char* buffer);
	//short int ->  unsigned char bytes[2] len>=2
	static void Int16ToBuffer(int16_t val, char* buffer);


	//unsigned char [4] len>=4 -> unsigned int
	static uint32_t BufferToUInt32(const char* buffer);
	//unsigned int -> unsigned char[4] len >=4
	static void UInt32ToBuffer(uint32_t val, char* buffer);


	//unsigned char [4] len>=4 -> unsigned int
	static int32_t BufferToInt32(const char* buffer);
	//unsigned int -> unsigned char[4] len >=4
	static void Int32ToBuffer(int32_t val, char* buffer);


	//unsigned char [8] len>=8 -> unsigned long long int
	static uint64_t BufferToUInt64(const char* buffer);
	//unsigned long long int -> unsigned char [8] len >=8
	static void UInt64ToBuffer(uint64_t val, char* buffer);

	//unsigned char [8] len>=8 -> unsigned long long int
	static int64_t BufferToInt64(const char* buffer);
	//unsigned long long int -> unsigned char [8] len >=8
	static void Int64ToBuffer(int64_t val, char* buffer);

	//char to float
	static float BufferToFloat(const char* buffer);
	//float to char
	static void FloatToBuffer(float val, char* buffer);

	//little endian float data to big endian 
	static float FloatLToB(float data);
	//big endian float data to little endian 
	static float FloatBToL(float data);

	//把str中的全部内容转到 buffer中
	static void WriteString(const std::string& str, char* buffer, uint32_t& idx);

	//把 buffer中 size个字节转到 str中
	static void ReadString(std::string& str, const char* buffer, uint32_t& idx);


	static int8_t ReadInt8(const char* buffer, uint32_t& idx);

	static void WriteInt8(const char val, char* buffer, uint32_t& idx);

	static uint8_t ReadUInt8(const char* buffer, uint32_t& idx);

	static void WriteUInt8(const uint8_t val, char* buffer, uint32_t& idx);

	static int16_t ReadInt16(const char* buffer, uint32_t& idx);
	static void WriteInt16(int16_t val, char* buffer, uint32_t& idx);
	

	static uint16_t ReadUInt16(const char* buffer, uint32_t& idx);
	static void WriteUInt16(uint16_t val, char* buffer, uint32_t& idx);
	

	static int32_t ReadInt32(const char* buffer, uint32_t& idx);
	static void WriteInt32(int32_t val, char* buffer, uint32_t& idx);
	
	static uint32_t ReadUInt32(const char* buffer, uint32_t& idx);
	static void WriteUInt32(uint32_t val, char* buffer, uint32_t& idx);
	
	static int64_t ReadInt64(const char* buffer, uint32_t& idx);
	static void WriteInt64(int64_t val, char* buffer, uint32_t& idx);

	static uint64_t ReadUInt64(const char* buffer, uint32_t& idx);
	static void WriteUInt64(uint64_t val, char* buffer, uint32_t& idx);
	
	static float ReadFloat(const char* buffer, uint32_t& idx);
	static void WriteFloat(float val, char* buffer, uint32_t& idx);

private:

};


#endif