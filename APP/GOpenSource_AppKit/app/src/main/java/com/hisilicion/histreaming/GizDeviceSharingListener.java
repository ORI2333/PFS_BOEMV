package com.hisilicion.histreaming;

import android.graphics.Bitmap;

import java.util.List;

public class GizDeviceSharingListener {
    public GizDeviceSharingListener() {
    }

    public void didGetBindingUsers(GizWifiErrorCode result, String deviceID, List<GizUserInfo> bindUsers) {
    }

    public void didUnbindUser(GizWifiErrorCode result, String deviceID, String guestUID) {
    }

//    public void didGetDeviceSharingInfos(GizWifiErrorCode result, String deviceID, List<GizDeviceSharingInfo> deviceSharingInfos) {
//    }

    public void didSharingDevice(GizWifiErrorCode result, String deviceID, int sharingID, Bitmap QRCodeImage) {
    }

    public void didRevokeDeviceSharing(GizWifiErrorCode result, int sharingID) {
    }

    public void didAcceptDeviceSharing(GizWifiErrorCode result, int sharingID) {
    }

    public void didCheckDeviceSharingInfoByQRCode(GizWifiErrorCode result, String userName, String productName, String deviceAlias, String expiredAt) {
    }

    public void didAcceptDeviceSharingByQRCode(GizWifiErrorCode result) {
    }

    public void didModifySharingInfo(GizWifiErrorCode result, int sharingID) {
    }

    public void didQueryMessageList(GizWifiErrorCode result, List<GizMessage> messageList) {
    }

    public void didMarkMessageStatus(GizWifiErrorCode result, String messageID) {
    }
}
