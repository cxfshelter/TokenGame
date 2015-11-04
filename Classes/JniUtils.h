/*
 * JniUtils.h
 *
 *  Created on: 2015-8-5
 *      Author: best
 */

#ifndef __JNI_UTILS_H__
#define __JNI_UTILS_H__

#include <string>
#include "cocos2d.h"

using namespace std;
USING_NS_CC;

class JniUtils
{
private:
	static JniUtils * m_Instance;

public:
	static JniUtils * getInstance();

	/* 	param1:	付费点id
	 * 	param2: 商品id
	 * 	param3: 商品数量
	 * 	param4: 购买价格
	 */
	void startToPay(std::string param1, std::string param2, std::string param3, std::string param4);

	/*
	 *  计数事件（目前均用此事件类型）
	 *  参数详情：
	 *  	param1: 统计的事件ID
	 *  	param2: 事件的属性ID
	 *  例如：param1: 空间雷达, param2: 确定 - 则后台展示点击空间雷达购买并确定
	 */
	void recordDataInYM(std::string param1, std::string param2);

	/*
	 *  游戏退出前调用，用于保存、统计数据用
	 */
	void exitGame();

	/*
	 *  android得到imei码
	 */
	std::string getImei();

};


#endif	// __JNI_UTILS_H__

