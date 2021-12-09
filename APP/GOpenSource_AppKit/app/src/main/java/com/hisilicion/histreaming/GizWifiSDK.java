package com.hisilicion.histreaming;

import android.content.Context;
import android.util.Log;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.concurrent.ConcurrentHashMap;

import static com.hisilicion.histreaming.GizWifiDeviceNetStatus.GizDeviceOffline;
import static com.hisilicion.histreaming.GizWifiDeviceNetStatus.GizDeviceOnline;

public class GizWifiSDK {
    private static final GizWifiSDK mInstance = new GizWifiSDK();
    private final String Tag = "link";
    private GizWifiSDKListener m_wifiSdkListener = null;
    private GizWifiSDKListener m_deviesListener = null;
    private ArrayList<GizWifiDevice> m_devices = new ArrayList<GizWifiDevice>();
    private LinkPlatform m_link = new LinkPlatform();
    private QueryResult m_queryResult = null;
    private Thread m_deviesTask = null;
    private boolean m_deviesTaskFlag = false;
    private Runnable m_deviesTaskRunnale = new Runnable() {

        private void buildDevicesList() {
            m_devices.clear();
            for (int i = 0; i < m_queryResult.count(); i++) {
                LinkServiceAgent agent = m_queryResult.at(i);
                GizWifiDevice dev = new GizWifiDevice();
                dev.setAddress(agent.address());
                dev.setIndex(i);
                dev.setOnline();
                dev.setType(agent.type());
                m_devices.add(dev);
            }

            if (m_deviesListener != null) {
                Log.d(Tag, "devies hav: " + Integer.toString(m_devices.size()));
                m_deviesListener.updateDeviceList( m_devices);
            }
        }

        @Override
        public void run() {
            if (m_link == null) {
                Log.d(Tag, "link is null");
                return;
            }

            if (m_link.open() != 0) {
                Log.d(Tag, "link open failure");
                return;
            }

            m_deviesTaskFlag = true;
            while (m_deviesTaskFlag == true) {
                try {
                    Thread.sleep(5000);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }

                QueryResult qResult = m_link.query("ALL");
                if (qResult == null) {
                    continue;
                }

                if (m_queryResult != null) {
                    m_queryResult.free();
                }

                m_queryResult = qResult;
                buildDevicesList();
            }

            m_deviesTaskFlag = false;

            if (m_queryResult != null) {
                m_queryResult.free();
                m_queryResult = null;
            }

            m_link.close();
            m_link = null;
        }
    };

    private GizWifiSDK() {
    }

    public static synchronized GizWifiSDK sharedInstance() {
        return mInstance;
    }

    protected void finalize() {
        close();
    }

    public int post(int index, String prop, String v) {
        int count =   m_queryResult.count();
        if (index > count || index < 0) {
            return -1;
        }

        LinkServiceAgent agent = m_queryResult.at(index);
        if (agent != null) {
            return agent.modify(prop, v);
        }

        return -1;
    }

    public void setDiscoverListener(GizWifiSDKListener observer)
    {
        m_deviesListener = observer;
    }

    public void open() {
        m_deviesTask = new Thread(m_deviesTaskRunnale);
        m_deviesTask.start();
    }

    public void close() {
        m_deviesTaskFlag = false;
        m_deviesTask.interrupt();
    }

    public void setListener(GizWifiSDKListener listener) {
        m_wifiSdkListener = listener;
    }

    public void discover() {
        if (m_link != null) {
            String queryString = "ALL";
            m_link.discover(queryString);
        }
    }

    /**
     * @deprecated
     */
    public void startWithAppID(Context context, String appID, List<String> specialProductKeys, ConcurrentHashMap<String, String> cloudServiceInfo) {
    }

    /**
     * @deprecated
     */
    public void startWithAppID(Context context, String appID, List<String> specialProductKeys, ConcurrentHashMap<String, String> cloudServiceInfo, boolean autoSetDeviceDomain) {
    }

    public void startWithAppID(Context context, String appID, String appSecret, List<String> specialProductKeys, ConcurrentHashMap<String, String> cloudServiceInfo, boolean autoSetDeviceDomain) {
    }

    /**
     * @deprecated
     */
    public void startWithAppID(Context context, String appID) {
    }

    public void disableLAN(boolean disabled) {

    }

    public String getVersion() {
        return null;
    }

    private String getDaemonVersion() {
        return null;
    }

    public void setLogLevel(GizLogPrintLevel logLevel) {

    }

    /**
     * @deprecated
     */
    public void setLogLevel(GizLogPrintLevel logLevel, boolean writeSDCard) {

    }

    public void setDeviceOnboarding(String ssid, String key, GizWifiConfigureMode mode, String softAPSSIDPrefix, int timeout, List<GizWifiGAgentType> types) {
    }

    /**
     * @deprecated
     */
    public void setDeviceWifi(String ssid, String key, XPGWifiConfigureMode mode, String softAPSSIDPrefix, int timeout, List<XPGWifiGAgentType> types) {
    }

    public void getSSIDList() {

    }

    public void getBoundDevices(String uid, String token, List<String> specialProductKey) {

    }

    /**
     * @deprecated
     */
    public void updateDeviceFromServer(String productKey) {

    }

    /**
     * @deprecated
     */
    public void bindDevice(String uid, String token, String did, String passCode, String remark) {

    }

    public void bindRemoteDevice(String uid, String token, String mac, String productKey, String productSecret) {
    }

    /**
     * @deprecated
     */
    public void unbindDevice(String uid, String token, String did, String passCode) {
    }

    public void unbindDevice(String uid, String token, String did) {
    }

    public void bindDeviceByQRCode(String uid, String token, String QRContent) {
    }

    public void getCaptchaCode(String appSecret) {

    }

    public void requestSendPhoneSMSCode(String token, String captchaId, String captchaCode, String phone) {
    }

    public void requestSendPhoneSMSCode(String appSecret, String phone) {
    }

    public void verifyPhoneSMSCode(String token, String phoneCode, String phone) {
    }

    public void registerUser(String username, String password, String code, GizUserAccountType accountType) {
    }

    /**
     * @deprecated
     */
    public void registerUser(String userName, String password) {
    }

    /**
     * @deprecated
     */
    public void registerUserByPhoneAndCode(String phone, String password, String code) {
    }

    /**
     * @deprecated
     */
    public void registerUserByEmail(String email, String password) {
    }

    public void userLoginAnonymous() {
    }

    public void userLogin(String username, String password) {
        if (m_wifiSdkListener != null) {
            final String uuid = "dummy";
            final String token = "dummy";

            m_wifiSdkListener.didUserLogin(GizWifiErrorCode.GIZ_SDK_SUCCESS, uuid, token);
        }
    }

    /**
     * @deprecated
     */
    public void userLoginWithUserName(String userName, String password) {
    }

    public void loginWithThirdAccount(GizThirdAccountType thirdAccountType, String uid, String token) {
    }

    /**
     * @deprecated
     */
    public void loginWithThirdAccountType(XPGWifiThirdAccountType thirdAccountType, String uid, String token) {
    }

    public void changeUserPassword(String token, String oldPassword, String newPassword) {
    }

    public void resetPassword(String username, String code, String newPassword, GizUserAccountType accountType) {
    }

    /**
     * @deprecated
     */
    public void changeUserPasswordByCode(String phone, String code, String newPassword) {
    }

    /**
     * @deprecated
     */
    public void changeUserPasswordByEmail(String email) {
    }

    public void changeUserInfo(String token, String username, String code, GizUserAccountType accountType, GizUserInfo additionalInfo) {
    }

    /**
     * @deprecated
     */
    public void changeUserInfo(String token, String username, String code, XPGUserAccountType accountType, GizUserInfo additionalInfo) {
    }

    public void transAnonymousUser(String token, String username, String password, String code, GizUserAccountType accountType) {
    }

    /**
     * @deprecated
     */
    public void transAnonymousUserToNormalUser(String token, String userName, String password) {
    }

    /**
     * @deprecated
     */
    public void transAnonymousUserToPhoneUser(String token, String userName, String password, String code) {
    }

    /**
     * @deprecated
     */
    public void getGroups(String uid, String token, List<String> specialProductKeys) {
    }

    /**
     * @deprecated
     */
    public void addGroup(String uid, String token, String productKey, String groupName, List<ConcurrentHashMap<String, String>> specialDevices) {
    }

    /**
     * @deprecated
     */
    public void removeGroup(String uid, String token, String gid) {
    }

    /**
     * @deprecated
     */
    public void editGroup(String uid, String token, String gid, String groupName, List<ConcurrentHashMap<String, String>> specialDevices) {
    }

    /**
     * @deprecated
     */
    public void setCloudService(String openAPIDomain, int openAPIPort, String siteDomain, int sitePort) {
    }

    /**
     * @deprecated
     */
    public void setCloudService(ConcurrentHashMap<String, String> cloudServiceInfo) {
    }

    public void getCurrentCloudService() {
    }

    public void getUserInfo(String token) {

    }

    public List<GizWifiDevice> getDeviceList() {
        return m_devices;
    }

    public List<GizWifiGroup> getGroupList() {
        return null;
    }

    /**
     * @deprecated
     */
    public void userLogout(String uid) {
    }

    public void channelIDBind(String token, String channelID, GizPushType pushType) {
    }

    public void channelIDBind(String token, String channelID, String alias, GizPushType pushType) {

    }

    public void channelIDUnBind(String token, String channelID) {

    }

    /**
     * @deprecated
     */
    public void createScheduler(String token, GizDeviceScheduler schedulerInfo) {

    }

    /**
     * @deprecated
     */
    public void getSchedulers(String token) {

    }

    /**
     * @deprecated
     */
    public void deleteScheduler(String token, String sid) {

    }

    /**
     * @deprecated
     */
    public void getSchedulerStatus(String token, String sid) {

    }

    public void getDevicesToSetServerInfo() {

    }

    public void setDeviceServerInfo(String domain, String mac) {
    }
}

