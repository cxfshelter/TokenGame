/*
 * JniUtils.cpp
 *
 *  Created on: 2015-8-5
 *      Author: best
 */

#include "JniUtils.h"
#include "cocos2d.h"
#include "platform/android/jni/JniHelper.h"
#include <jni.h>

using namespace cocos2d;

#define CLASS_NAME 			"org/cocos2dx/cpp/JniHelper"
#define MATHOD_PAY_NAME		"sdkPay"
#define MATHOD_RECORD_NAME	"ymOnEvent"
#define MATHOD_EXIT_NAME	"sdkExitGame"
#define MATHOD_GET_IMEI		"androidGetImei"

JniUtils * JniUtils::m_Instance;

JniUtils * JniUtils::getInstance()
{
	return m_Instance;
}

/* 	param1:	付费点id
 * 	param2: 商品id
 * 	param3: 商品数量
 * 	param4: 购买价格
 */
void JniUtils::startToPay(std::string param1, std::string param2, std::string param3, std::string param4)
{
#if(CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID)
	return;
#endif

	JniMethodInfo t;
	if(JniHelper::getStaticMethodInfo(t, CLASS_NAME, MATHOD_PAY_NAME, "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V"))
	{
		// 下面为执行函数的参数
		jstring param11	 = t.env->NewStringUTF(param1.c_str());
		jstring param22	 = t.env->NewStringUTF(param2.c_str());
		jstring param33  = t.env->NewStringUTF(param3.c_str());
		jstring param44  = t.env->NewStringUTF(param4.c_str());

		t.env->CallStaticVoidMethod(t.classID, t.methodID, param11, param22, param33, param44);

		t.env->DeleteLocalRef(param11);
		t.env->DeleteLocalRef(param22);
		t.env->DeleteLocalRef(param33);
		t.env->DeleteLocalRef(param44);
	}

}

/*
 *  param1: 统计的事件ID
 *  param2: 事件的属性ID
 */
void JniUtils::recordDataInYM(std::string param1, std::string param2)
{
#if(CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID)
	return;
#endif

	JniMethodInfo t;
	if(JniHelper::getStaticMethodInfo(t, CLASS_NAME, MATHOD_RECORD_NAME, "(Ljava/lang/String;Ljava/lang/String;)V"))
	{
		// 下面为执行函数的参数
		jstring param11	 = t.env->NewStringUTF(param1.c_str());
		jstring param22	 = t.env->NewStringUTF(param2.c_str());

		t.env->CallStaticVoidMethod(t.classID, t.methodID, param11, param22);

		t.env->DeleteLocalRef(param11);
		t.env->DeleteLocalRef(param22);
	}
}

/*
 *  游戏退出前调用，用于保存、统计数据用
 */
void JniUtils::exitGame()
{
#if(CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID)
	return;
#endif

	JniMethodInfo t;
	if(JniHelper::getStaticMethodInfo(t, CLASS_NAME, MATHOD_EXIT_NAME, "()V"))
	{
		// 下面为执行函数的参数
		t.env->CallStaticVoidMethod(t.classID, t.methodID);
	}
}

/*
 *  android得到imei码
 */
std::string JniUtils::getImei()
{
#if(CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID)
	return;
#endif

	JniMethodInfo t;
	if(JniHelper::getStaticMethodInfo(t, CLASS_NAME, MATHOD_GET_IMEI, "()Ljava/lang/String;"))
	{
		// 下面为执行函数的参数
		jstring imeiStr = (jstring)t.env->CallStaticObjectMethod(t.classID, t.methodID);
		return JniHelper::jstring2string(imeiStr);
	}

	return "";
}



