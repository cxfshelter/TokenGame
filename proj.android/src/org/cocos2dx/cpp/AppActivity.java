/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2013-2014 Chukong Technologies Inc.
 
http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/
package org.cocos2dx.cpp;

import org.cocos2dx.lib.Cocos2dxActivity;
import android.os.Bundle;
import android.os.RemoteException;
import android.widget.Toast;

import org.cocos2dx.cpp.JniHelper;

import com.umeng.analytics.AnalyticsConfig;
import com.umeng.analytics.game.UMGameAgent;
import fly.fish.aidl.OutFace;
import fly.fish.aidl.OutFace.FlyFishSDK;

public class AppActivity extends Cocos2dxActivity {
	// 值尚sdk 信息
	private static String GAME_ID 	= "20047";
	private static String GAME_KEY	= "0ff00ee3e88d3263";
	private static String GAME_NAME = "xjzm";
	private static boolean isZseInitSuc = false;
	
	// 友盟sdk 信息
	private static String YM_KEY 	= "55caa2dfe0f55a762200886b";
	private static String YM_CHANNEL= "zse";
	
	public FlyFishSDK callback = new FlyFishSDK() {
		@Override
		public void initback(String status) throws RemoteException {
			if("0".equals(status))
			{
				isZseInitSuc = true;
			}
			else
			{
				isZseInitSuc = false;
				OutFace.getInstance(AppActivity.this).init(GAME_ID, GAME_KEY, GAME_NAME);
			}
		}
		
		@Override
		public void payback(String reqmoney, String status, String feecode,
				String chargetype, String order, String custominfo) throws RemoteException {
			if(!isZseInitSuc)
			{
				return;
			}
			
			if("0".equals(status))
			{
				// 返回计费点id
				JniHelper.payReturn(feecode);
				Toast.makeText(AppActivity.this, "购买成功", Toast.LENGTH_SHORT).show();
			}
			else
			{
				Toast.makeText(AppActivity.this, "购买失败", Toast.LENGTH_SHORT).show();
			}
		}
	};
	
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		JniHelper.init(this);
		
		// init zse's sdk
		try {
			OutFace.getInstance(this).callBack(callback, GAME_KEY);
			OutFace.getInstance(this).init(GAME_ID, GAME_KEY, GAME_NAME);
		} catch(RemoteException e) {
			e.printStackTrace();
		}
		
		// init 友盟's sdk
		AnalyticsConfig.setAppkey(YM_KEY);
		AnalyticsConfig.setChannel(YM_CHANNEL);
		UMGameAgent.init(this);
	}
	
	protected void onResume() {
		super.onResume();
		UMGameAgent.onResume(this);
	}
	
	protected void onPause() {
		super.onPause();
		UMGameAgent.onPause(this);
	}
	
	protected void onStop()
	{
		super.onStop();
	}
	
	protected void onDestroy()
	{
		super.onDestroy();
	}

}
