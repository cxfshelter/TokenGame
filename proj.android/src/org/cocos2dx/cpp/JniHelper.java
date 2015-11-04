package org.cocos2dx.cpp;

import java.util.HashMap;

import com.umeng.analytics.game.UMGameAgent;

import android.app.Activity;
import android.content.Context;
import android.os.RemoteException;
import android.telephony.TelephonyManager;
import android.util.Log;
import fly.fish.aidl.OutFace;

/* 	for sdk from zse & umeng
 * 	供c++层调用，如：sdkPay
 */
public class JniHelper {
	
	private static Activity atyThis = null;
	
	private static boolean IS_INIT = false;
	
	private static void showTipDialog(final String title, final String text)
	{
		Log.v(title, text);
	}
	
	public static void init(Activity aty)
	{
		atyThis = aty;
	}
	
	/* 参数详情以JniUtils注释为准
	 * 		param1:	付费点id
 	 * 		param2: 商品id
 	 * 		param3: 商品数量
 	 * 		param4: 购买价格
	 */
	private static void sdkPay(final String param1, final String param2, final String param3, final String param4)
	{
		if(null == atyThis)
		{
			return;
		}
		
		try {
			// zse sdk
			OutFace.getInstance(atyThis).pay(
					atyThis, System.currentTimeMillis() + "best", param1, param4, param2);
			
			// umeng sdk
			int buyRMB 			= Integer.parseInt(param4);
			int diamondNum		= Integer.parseInt(param3);
			
				// (rmb, 支付渠道, 钻石)
			UMGameAgent.pay(buyRMB, 6, diamondNum);
			
		} catch (RemoteException e) {
			e.printStackTrace();
		}		
	}
	
	/*
	 *  计数事件（目前均用此事件类型）
	 *  参数详情：
	 *  	param1: 统计的事件ID
	 *  	param2: 事件的属性ID
	 *  例如：param1: 空间雷达, param2: 确定 - 则后台展示点击空间雷达购买并确定
	 */
	private static void ymOnEvent(final String param1, final String param2)
	{
		if(null == atyThis)
		{
			return;
		}
		
		if(param2.equals(""))
		{
			UMGameAgent.onEvent(atyThis, param1);
		}
		else
		{
			HashMap<String,String> map = new HashMap<String,String>();
			map.put("type", param2);
			UMGameAgent.onEvent(atyThis, param1, map);
		}
	}
	
	/*
	 *  游戏退出前调用，用于保存、统计数据用
	 */
	private static void sdkExitGame()
	{
		if(null == atyThis)
		{
			return;
		}
		
		UMGameAgent.onKillProcess(atyThis);
	}
	
	/*
	 *  获取imei码
	 *  	用于排行榜设置名字
	 */
	private static String androidGetImei()
	{
		if(null == atyThis)
		{
			return null;
		}
		
		TelephonyManager tm = (TelephonyManager) atyThis.getSystemService(Context.TELEPHONY_SERVICE);
		String imeiStr = tm.getDeviceId();
		return imeiStr;
	}
	
	// -----------  上面c++调用java，下面则是java调用c++  -----------
	
	/*
	 *  购买返回，参数是商品ID（只有成功才调用此函数）
	 */
	public static native void payReturn(String productId);
	
}
