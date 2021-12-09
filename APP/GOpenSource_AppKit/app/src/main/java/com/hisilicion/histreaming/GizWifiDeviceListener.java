package com.hisilicion.histreaming;

import java.util.concurrent.ConcurrentHashMap;

public class GizWifiDeviceListener {
    public GizWifiDeviceListener() {
    }

    public void didSetSubscribe(GizWifiErrorCode result, GizWifiDevice device, boolean isSubscribed) {
    }

    public void didGetHardwareInfo(GizWifiErrorCode result, GizWifiDevice device, ConcurrentHashMap<String, String> hardwareInfo) {
    }

    /** @deprecated */
    public void didQueryHardwareInfo(GizWifiDevice device, int result, ConcurrentHashMap<String, String> hardwareInfo) {
    }

    /** @deprecated */
    public void didDisconnected(GizWifiDevice device, int result) {
    }

    /** @deprecated */
    public void didLogin(GizWifiDevice device, int result) {
    }

    /** @deprecated */
    public void didDeviceOnline(GizWifiDevice device, boolean isOnline) {
    }

    public void didExitProductionTesting(GizWifiErrorCode result, GizWifiDevice device) {
    }

    public void didReceiveData(GizWifiErrorCode result, GizWifiDevice device, ConcurrentHashMap<String, Object> dataMap, int sn) {
    }

    /** @deprecated */
    public void didReceiveData(GizWifiDevice device, ConcurrentHashMap<String, Object> dataMap, int result) {
    }

    public void didUpdateNetStatus(GizWifiDevice device, GizWifiDeviceNetStatus netStatus) {
    }

    public void didSetCustomInfo(GizWifiErrorCode result, GizWifiDevice device) {
    }

    public void didUpdateProduct(GizWifiErrorCode result, GizWifiDevice device, String productUI) {
    }
}

