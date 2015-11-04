/*
 * JniForJava.cpp
 *
 *  Created on: 2015-8-6
 *      Author: best
 */

#include "JniForJava.h"
#include "cocos2d.h"
#include "platform/android/jni/JniHelper.h"
#include <jni.h>
#include "../../../Classes/DiamondBuyData.h"

USING_NS_CC;

extern "C"
{

	void Java_org_cocos2dx_cpp_JniHelper_payReturn(JNIEnv* env, jobject thiz, jstring productId)
	{
		const char* feeCodeChar = env->GetStringUTFChars(productId, NULL);
		int feeCode = atoi(feeCodeChar);
		DiamondBuyData::getInstance()->androidPaySuc(feeCode);
	}


}
