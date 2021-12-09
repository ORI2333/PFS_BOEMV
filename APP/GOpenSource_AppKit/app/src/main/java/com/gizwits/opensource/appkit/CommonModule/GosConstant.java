package com.gizwits.opensource.appkit.CommonModule;

import java.util.ArrayList;
import java.util.List;



import android.net.wifi.ScanResult;

import com.hisilicion.histreaming.GizUserInfo;

public class GosConstant {

	public static List<ScanResult> ssidList = new ArrayList<ScanResult>();

	public static int nowPager = -1;

	public static List<GizUserInfo> mybindUsers = new ArrayList<GizUserInfo>();
//
//	public static List<GizDeviceSharingInfo> mydeviceSharingInfos = new ArrayList<GizDeviceSharingInfo>();
//
//	public static List<GizDeviceSharingInfo> newmydeviceSharingInfos = new ArrayList<GizDeviceSharingInfo>();

	public static boolean isEdit = false;

	public static String mozu = "https://item.taobao.com/item.htm?spm=686.1000925.0.0.nPcYfD&id=542479181481";

	// 设备热点默认密码
	public static final String SoftAP_PSW = "123456789";
	
	
	// 设备热点默认前缀
	public static final String SoftAP_Start = "XPG-GAgent";

}
