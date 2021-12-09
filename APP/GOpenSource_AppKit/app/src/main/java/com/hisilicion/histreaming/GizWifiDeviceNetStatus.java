package com.hisilicion.histreaming;

import java.io.Serializable;

public enum GizWifiDeviceNetStatus implements Serializable {
    GizDeviceOffline,
    GizDeviceOnline,
    GizDeviceControlled,
    GizDeviceUnavailable;

    private GizWifiDeviceNetStatus() {
    }
}