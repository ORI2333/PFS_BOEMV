package com.hisilicion.histreaming;

import android.nfc.Tag;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.os.Message;
import android.os.Parcel;
import android.os.Parcelable;
import android.text.TextUtils;
import android.util.Log;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.File;
import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

import static com.hisilicion.histreaming.GizWifiDeviceNetStatus.GizDeviceOnline;

public class GizWifiDevice implements Parcelable {
    public static final Creator<GizWifiDevice> CREATOR = new Creator<GizWifiDevice>() {
        public GizWifiDevice createFromParcel(Parcel source) {
            GizWifiDevice device = new GizWifiDevice();
            device.ipAddress = source.readString();
            device.alias = source.readString();
            device.m_index = source.readInt();
            return device;
        }

        public GizWifiDevice[] newArray(int size) {
            return null;
        }
    };
    private static final int MSG_RECV = 5;
    /**
     * @deprecated
     */
    public static String XPGWifiDeviceHardwareWifiHardVerKey = "wifiHardVer";
    /**
     * @deprecated
     */
    public static String XPGWifiDeviceHardwareWifiSoftVerKey = "wifiSoftVer";
    /**
     * @deprecated
     */
    public static String XPGWifiDeviceHardwareMCUHardVerKey = "mcuHardVer";
    /**
     * @deprecated
     */
    public static String XPGWifiDeviceHardwareMCUSoftVerKey = "mcuSoftVer";
    /**
     * @deprecated
     */
    public static String XPGWifiDeviceHardwareFirmwareIdKey = "firmwareId";
    /**
     * @deprecated
     */
    public static String XPGWifiDeviceHardwareFirmwareVerKey = "firmwareVer";
    /**
     * @deprecated
     */
    public static String XPGWifiDeviceHardwareProductKey = "productKey";
    public GizWifiDeviceNetStatus m_status;
    public boolean m_isBind = false;
    public boolean m_isLAN = false;
    public GizWifiDeviceListener mListener;
    public boolean isLAN;
    public String macAddress;
    public String did;
    public String ipAddress;
    public GizWifiDeviceNetStatus netStatus;
    public String alias;
    private String productKey;
    private String productName;
    private boolean subscribed;
    private boolean isBind;
    private boolean hasProductDefine;
    private boolean isDisabled;
    private String remark;
    private String productUI;
    private boolean oldIsOnline;
    private boolean oldIsConnected;
    private boolean loginning;
    private int logintimeout = 0;
    private int notify_status_sn = 0;
    private String subdid;
    public int m_index;

    public String toString() {
        return "Light";
    }

    public String getSubdid() {
        return "SN123456";
    }

    public void setAddress(String addr) {
        ipAddress = addr;
    }

    public void setType(String type) {
        alias = type;
    }

    public void setIndex(int idx) {
        m_index = idx;
    }

    public void setOnline() {
        m_isBind = true;
        m_isLAN = true;
        m_status = GizDeviceOnline;
    }

//      public GizDeviceSharingUserRole getSharingRole() {
//        return 0;
//    }


    public void setListener(GizWifiDeviceListener Listener) {
        this.mListener = Listener;
    }

//    public GizWifiDeviceListener getListener() {
//        return null;
//    }
//
//    /** @deprecated */
//    public void setSubscribe(boolean subsribe) {
//    }

    public void setSubscribe(String productSecret, boolean subscribed) {
        //this.mListener.didSetSubscribe(GizWifiErrorCode.GIZ_SDK_SUCCESS, this, true);
    }

    public void getHardwareInfo() {
    }

    public void exitProductionTesting() {
    }

    /**
     * @deprecated
     */
    public void getDeviceStatus() {

    }

    public void getDeviceStatus(List<String> attrs) {

    }

    public void write(ConcurrentHashMap<String, Object> data, int sn) {

    }

    /**
     * @deprecated
     */
    public void write(String jsonData) {

    }

    public void setCustomInfo(String remark, String alias) {

    }

    /**
     * @deprecated
     */
    public void disconnect() {

    }

    /**
     * @deprecated
     */
    public void login(String uid, String token) {
    }

    /**
     * @deprecated
     */
//    public boolean isBind(String uid) {
//        return this.isBind();
//    }
    public String getMacAddress() {
        return ipAddress;
    }

    //
//    protected void setMacAddress(String macAddress) {
//        this.macAddress = macAddress;
//    }
//
    public String getDid() {
        return did;
    }

    //
//    protected void setDid(String did) {
//        this.did = did;
//    }
//
//    public String getIPAddress() {
//        return this.ipAddress;
//    }
//
//    protected void setIpAddress(String ipAddress) {
//        this.ipAddress = ipAddress;
//    }
//
    public String getProductKey() {
        return "xxxxxxxxx";
    }

    //
//    protected void setProductKey(String productKey) {
//        this.productKey = productKey;
//    }
//
    public String getProductName() {
        return getAlias();
    }

    //
//    protected void setProductName(String productName) {
//        this.productName = productName;
//    }
//
    public boolean isLAN() {
        return m_isLAN;
    }

    //
//    protected void setLAN(boolean isLAN) {
//        this.isLAN = isLAN;
//    }
//
//    public boolean isDisabled() {
//        return this.isDisabled;
//    }
//
//    protected void setDisabled(boolean isDisabled) {
//        this.isDisabled = isDisabled;
//    }
//
//    public boolean isSubscribed() {
//        return this.subscribed;
//    }
//
//    protected void setSubscribed(boolean subscribed) {
//        this.subscribed = subscribed;
//    }
//
    public boolean isBind() {
        return m_isBind;
    }

    //
//    protected void setIsBind(boolean isBind) {
//        this.isBind = isBind;
//    }
//
//    public boolean isProductDefined() {
//        return this.hasProductDefine;
//    }
//
//    protected void setHasProductDefine(boolean hasProductDefine) {
//        this.hasProductDefine = hasProductDefine;
//    }
//
//    public GizWifiDeviceType getProductType() {
//        return this.productType;
//    }
//
//    /** @deprecated */
//    public XPGWifiDeviceType type() {
//    }
//
//
    public GizWifiDeviceNetStatus getNetStatus() {
        return m_status;
    }


//    /** @deprecated */
//    public boolean isOnline() {
//     }
//
//
//    /** @deprecated */
//    public boolean isConnected() {
//     }


    public String getRemark() {
        return alias;
    }

    public String getAlias() {
        return alias;
    }

    public String getProductUI() {
        return null;
    }


    /**
     * @deprecated
     */
    public String getPasscode() {
        return "";
    }

    public int describeContents() {
        return 0;
    }

    public void writeToParcel(Parcel dest, int flags) {
        dest.writeString(ipAddress);
        dest.writeString(alias);
        dest.writeInt(m_index);
    }

    public void updateProduct() {
    }
}

