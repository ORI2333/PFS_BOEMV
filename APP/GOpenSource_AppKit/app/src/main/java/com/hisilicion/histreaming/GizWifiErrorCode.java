package com.hisilicion.histreaming;

import java.io.Serializable;

public enum GizWifiErrorCode implements Serializable {
    GIZ_SDK_FAILED(-1),
    GIZ_SDK_SUCCESS(0),
    GIZ_SDK_PARAM_FORM_INVALID(8001),
    GIZ_SDK_CLIENT_NOT_AUTHEN(8002),
    GIZ_SDK_CLIENT_VERSION_INVALID(8003),
    GIZ_SDK_UDP_PORT_BIND_FAILED(8004),
    GIZ_SDK_DAEMON_EXCEPTION(8005),
    GIZ_SDK_PARAM_INVALID(8006),
    GIZ_SDK_APPID_LENGTH_ERROR(8007),
    GIZ_SDK_LOG_PATH_INVALID(8008),
    GIZ_SDK_LOG_LEVEL_INVALID(8009),
    GIZ_SDK_NO_AVAILABLE_DEVICE(8020),
    GIZ_SDK_DEVICE_CONFIG_SEND_FAILED(8021),
    GIZ_SDK_DEVICE_CONFIG_IS_RUNNING(8022),
    GIZ_SDK_DEVICE_CONFIG_TIMEOUT(8023),
    GIZ_SDK_DEVICE_DID_INVALID(8024),
    GIZ_SDK_DEVICE_MAC_INVALID(8025),
    GIZ_SDK_SUBDEVICE_INVALID(8026),
    GIZ_SDK_DEVICE_PASSCODE_INVALID(8027),
    GIZ_SDK_DEVICE_NOT_CENTERCONTROL(8028),
    GIZ_SDK_DEVICE_NOT_SUBSCRIBED(8029),
    GIZ_SDK_DEVICE_NO_RESPONSE(8030),
    GIZ_SDK_DEVICE_NOT_READY(8031),
    GIZ_SDK_DEVICE_NOT_BINDED(8032),
    GIZ_SDK_DEVICE_CONTROL_WITH_INVALID_COMMAND(8033),
    GIZ_SDK_DEVICE_GET_STATUS_FAILED(8035),
    GIZ_SDK_DEVICE_CONTROLL_FAILED(8034),
    GIZ_SDK_DEVICE_CONTROL_VALUE_TYPE_ERROR(8036),
    GIZ_SDK_DEVICE_CONTROL_VALUE_OUT_OF_RANGE(8037),
    GIZ_SDK_DEVICE_CONTROL_NOT_WRITABLE_COMMAND(8038),
    GIZ_SDK_BIND_DEVICE_FAILED(8039),
    GIZ_SDK_UNBIND_DEVICE_FAILED(8040),
    GIZ_SDK_DNS_FAILED(8041),
    GIZ_SDK_M2M_CONNECTION_SUCCESS(8042),
    GIZ_SDK_SET_SOCKET_NON_BLOCK_FAILED(8043),
    GIZ_SDK_CONNECTION_TIMEOUT(8044),
    GIZ_SDK_CONNECTION_REFUSED(8045),
    GIZ_SDK_CONNECTION_ERROR(8046),
    GIZ_SDK_CONNECTION_CLOSED(8047),
    GIZ_SDK_SSL_HANDSHAKE_FAILED(8048),
    GIZ_SDK_DEVICE_LOGIN_VERIFY_FAILED(8049),
    GIZ_SDK_INTERNET_NOT_REACHABLE(8050),
    GIZ_SDK_M2M_CONNECTION_FAILED(8051),
    GIZ_SDK_HTTP_SERVER_NOT_SUPPORT_API(8095),
    GIZ_SDK_HTTP_ANSWER_FORMAT_ERROR(8096),
    GIZ_SDK_HTTP_ANSWER_PARAM_ERROR(8097),
    GIZ_SDK_HTTP_SERVER_NO_ANSWER(8098),
    GIZ_SDK_HTTP_REQUEST_FAILED(8099),
    GIZ_SDK_THREAD_CREATE_FAILED(8100),
    GIZ_SDK_MEMORY_MALLOC_FAILED(8101),
    GIZ_SDK_OTHERWISE(8102),
    GizWifiSDKErrorCode_OTHERWISE(8103),
    GIZ_SDK_SCHEDULER_CREATE_FAILED(8120),
    GIZ_SDK_SCHEDULER_DELETE_FAILED(8121),
    GIZ_SDK_SCHEDULER_EDIT_FAILED(8122),
    GIZ_SDK_SCHEDULER_LIST_UPDATE_FAILED(8123),
    GIZ_SDK_SCHEDULER_TASK_EDIT_FAILED(8124),
    GIZ_SDK_SCHEDULER_TASK_LIST_UPDATE_FAILED(8125),
    GIZ_SDK_SCHEDULER_ID_INVALID(8126),
    GIZ_SDK_SCHEDULER_ENABLE_DISABLE_FAILED(8127),
    GIZ_SDK_SCHEDULER_STATUS_UPDATE_FAILED(8128),
    GIZ_SDK_SUBDEVICE_ADD_FAILED(8140),
    GIZ_SDK_SUBDEVICE_DELETE_FAILED(8141),
    GIZ_SDK_SUBDEVICE_LIST_UPDATE_FAILED(8142),
    GIZ_SDK_USER_ID_INVALID(8150),
    GIZ_SDK_TOKEN_INVALID(8151),
    GIZ_SDK_GROUP_ID_INVALID(8152),
    GIZ_SDK_GROUPNAME_INVALID(8153),
    GIZ_SDK_GROUP_PRODUCTKEY_INVALID(8154),
    GIZ_SDK_GROUP_CREATE_FAILED(8155),
    GIZ_SDK_GROUP_DELETE_FAILED(8156),
    GIZ_SDK_GROUP_EDIT_FAILED(8157),
    GIZ_SDK_GROUP_LIST_UPDATE_FAILED(8158),
    GIZ_SDK_GROUP_COMMAND_WRITE_FAILED(8159),
    GIZ_SDK_SCENE_CREATE_FAILED(8170),
    GIZ_SDK_SCENE_DELETE_FAILED(8171),
    GIZ_SDK_SCENE_EDIT_FAILED(8172),
    GIZ_SDK_SCENE_LIST_UPDATE_FAILED(8173),
    GIZ_SDK_SCENE_ITEM_LIST_EDIT_FAILED(8174),
    GIZ_SDK_SCENE_ITEM_LIST_UPDATE_FAILED(8175),
    GIZ_SDK_SCENE_ID_INVALID(8176),
    GIZ_SDK_SCENE_RUN_FAILED(8177),
    GIZ_SDK_SCENE_STATUS_UPDATE_FAILED(8178),
    GIZ_SDK_DATAPOINT_NOT_DOWNLOAD(8201),
    GIZ_SDK_DATAPOINT_SERVICE_UNAVAILABLE(8202),
    GIZ_SDK_DATAPOINT_PARSE_FAILED(8203),
    GIZ_SDK_SDK_NOT_INITIALIZED(8300),
    GIZ_SDK_APK_CONTEXT_IS_NULL(8301),
    GIZ_SDK_APK_PERMISSION_NOT_SET(8302),
    GIZ_SDK_CHMOD_DAEMON_REFUSED(8303),
    GIZ_SDK_EXEC_DAEMON_FAILED(8304),
    GIZ_SDK_EXEC_CATCH_EXCEPTION(8305),
    GIZ_SDK_APPID_IS_EMPTY(8306),
    GIZ_SDK_UNSUPPORTED_API(8307),
    GIZ_SDK_REQUEST_TIMEOUT(8308),
    GIZ_SDK_DAEMON_VERSION_INVALID(8309),
    GIZ_SDK_PHONE_NOT_CONNECT_TO_SOFTAP_SSID(8310),
    GIZ_SDK_DEVICE_CONFIG_SSID_NOT_MATCHED(8311),
    GIZ_SDK_NOT_IN_SOFTAPMODE(8312),
    GIZ_SDK_CONFIG_NO_AVAILABLE_WIFI(8313),
    GIZ_SDK_RAW_DATA_TRANSMIT(8314),
    GIZ_SDK_PRODUCT_IS_DOWNLOADING(8315),
    GIZ_SDK_START_SUCCESS(8316),
    GIZ_SDK_NEED_UPDATE_TO_LATEST(8317),
    GIZ_SDK_OTA_FIRMWARE_IS_LATEST(8350),
    GIZ_SDK_OTA_FIRMWARE_DOWNLOAD_OK(8351),
    GIZ_SDK_OTA_FIRMWARE_DOWNLOAD_FAILED(8352),
    GIZ_SDK_OTA_DEVICE_BUSY_IN_UPGRADE(8353),
    GIZ_SDK_OTA_PUSH_FAILED(8354),
    GIZ_SDK_OTA_FIRMWARE_VERSION_TOO_LOW(8355),
    GIZ_SDK_OTA_FIRMWARE_CHECK_FAILED(8356),
    GIZ_SDK_OTA_UPGRADE_FAILED(8357),
    GIZ_SDK_OTA_UPGRADE_SUCCESS(8358),
    GIZ_SITE_PRODUCTKEY_INVALID(10003),
    GIZ_SITE_DATAPOINTS_NOT_DEFINED(10010),
    GIZ_SITE_DATAPOINTS_NOT_MALFORME(10011),
    GIZ_OPENAPI_MAC_ALREADY_REGISTERED(9001),
    GIZ_OPENAPI_PRODUCT_KEY_INVALID(9002),
    GIZ_OPENAPI_APPID_INVALID(9003),
    GIZ_OPENAPI_TOKEN_INVALID(9004),
    GIZ_OPENAPI_USER_NOT_EXIST(9005),
    GIZ_OPENAPI_TOKEN_EXPIRED(9006),
    GIZ_OPENAPI_M2M_ID_INVALID(9007),
    GIZ_OPENAPI_SERVER_ERROR(9008),
    GIZ_OPENAPI_CODE_EXPIRED(9009),
    GIZ_OPENAPI_CODE_INVALID(9010),
    GIZ_OPENAPI_SANDBOX_SCALE_QUOTA_EXHAUSTED(9011),
    GIZ_OPENAPI_PRODUCTION_SCALE_QUOTA_EXHAUSTED(9012),
    GIZ_OPENAPI_PRODUCT_HAS_NO_REQUEST_SCALE(9013),
    GIZ_OPENAPI_DEVICE_NOT_FOUND(9014),
    GIZ_OPENAPI_FORM_INVALID(9015),
    GIZ_OPENAPI_DID_PASSCODE_INVALID(9016),
    GIZ_OPENAPI_DEVICE_NOT_BOUND(9017),
    GIZ_OPENAPI_PHONE_UNAVALIABLE(9018),
    GIZ_OPENAPI_USERNAME_UNAVALIABLE(9019),
    GIZ_OPENAPI_USERNAME_PASSWORD_ERROR(9020),
    GIZ_OPENAPI_SEND_COMMAND_FAILED(9021),
    GIZ_OPENAPI_EMAIL_UNAVALIABLE(9022),
    GIZ_OPENAPI_DEVICE_DISABLED(9023),
    GIZ_OPENAPI_FAILED_NOTIFY_M2M(9024),
    GIZ_OPENAPI_ATTR_INVALID(9025),
    GIZ_OPENAPI_USER_INVALID(9026),
    GIZ_OPENAPI_FIRMWARE_NOT_FOUND(9027),
    GIZ_OPENAPI_JD_PRODUCT_NOT_FOUND(9028),
    GIZ_OPENAPI_DATAPOINT_DATA_NOT_FOUND(9029),
    GIZ_OPENAPI_SCHEDULER_NOT_FOUND(9030),
    GIZ_OPENAPI_QQ_OAUTH_KEY_INVALID(9031),
    GIZ_OPENAPI_OTA_SERVICE_OK_BUT_IN_IDLE(9032),
    GIZ_OPENAPI_BT_FIRMWARE_UNVERIFIED(9033),
    GIZ_OPENAPI_BT_FIRMWARE_NOTHING_TO_UPGRADE(9034),
    GIZ_OPENAPI_SAVE_KAIROSDB_ERROR(9035),
    GIZ_OPENAPI_EVENT_NOT_DEFINED(9036),
    GIZ_OPENAPI_SEND_SMS_FAILED(9037),
    GIZ_OPENAPI_SIGNATURE_INVALID(9038),
    GIZ_OPENAPI_NOT_ALLOWED_CALL_API(9039),
    GIZ_OPENAPI_BAD_QRCODE_CONTENT(9040),
    GIZ_OPENAPI_REQUEST_THROTTLED(9041),
    GIZ_OPENAPI_DEVICE_OFFLINE(9042),
    GIZ_OPENAPI_TIMESTAMP_INVALID(9043),
    GIZ_OPENAPI_X_GIZWITS_SIGNATURE_INVALID(9044),
    GIZ_OPENAPI_DEPRECATED_API(9045),
    GIZ_OPENAPI_REGISTER_IS_BUSY(9046),
    GIZ_OPENAPI_CANNOT_SHARE_TO_SELF(9080),
    GIZ_OPENAPI_ONLY_OWNER_CAN_SHARE(9081),
    GIZ_OPENAPI_NOT_FOUND_GUEST(9082),
    GIZ_OPENAPI_GUEST_ALREADY_BOUND(9083),
    GIZ_OPENAPI_NOT_FOUND_SHARING_INFO(9084),
    GIZ_OPENAPI_NOT_FOUND_THE_MESSAGE(9085),
    GIZ_OPENAPI_SHARING_IS_WAITING_FOR_ACCEPT(9087),
    GIZ_OPENAPI_SHARING_IS_EXPIRED(9088),
    GIZ_OPENAPI_SHARING_IS_COMPLETED(9089),
    GIZ_OPENAPI_INVALID_SHARING_BECAUSE_UNBINDING(9090),
    GIZ_OPENAPI_ONLY_OWNER_CAN_BIND(9092),
    GIZ_OPENAPI_ONLY_OWNER_CAN_OPERATE(9093),
    GIZ_OPENAPI_SHARING_ALREADY_CANCELLED(9094),
    GIZ_OPENAPI_OWNER_CANNOT_UNBIND_SELF(9095),
    GIZ_OPENAPI_ONLY_GUEST_CAN_CHECK_QRCODE(9096),
    GIZ_OPENAPI_MESSAGE_ALREADY_DELETED(9098),
    GIZ_OPENAPI_BINDING_NOTIFY_FAILED(9099),
    GIZ_OPENAPI_ONLY_SELF_CAN_MODIFY_ALIAS(9100),
    GIZ_OPENAPI_ONLY_RECEIVER_CAN_MARK_MESSAGE(9101),
    GIZ_PUSHAPI_BODY_JSON_INVALID(5001),
    GIZ_PUSHAPI_DATA_NOT_EXIST(5300),
    GIZ_PUSHAPI_NO_CLIENT_CONFIG(5501),
    GIZ_PUSHAPI_NO_SERVER_DATA(5502),
    GIZ_PUSHAPI_GIZWITS_APPID_EXIST(5601),
    GIZ_PUSHAPI_PARAM_ERROR(5700),
    GIZ_PUSHAPI_AUTH_KEY_INVALID(5701),
    GIZ_PUSHAPI_APPID_OR_TOKEN_ERROR(5702),
    GIZ_PUSHAPI_TYPE_PARAM_ERROR(5703),
    GIZ_PUSHAPI_ID_PARAM_ERROR(5704),
    GIZ_PUSHAPI_APPKEY_SECRETKEY_INVALID(5705),
    GIZ_PUSHAPI_CHANNELID_ERROR_INVALID(5706),
    GIZ_PUSHAPI_PUSH_ERROR(5900),
    GIZ_OTHER_ERROR(66666),
    GIZ_OPENAPI_RESERVED(9099);

    private int result;

    private GizWifiErrorCode(int result) {
        this.result = result;
    }

    public int getResult() {
        return this.result;
    }

    public static GizWifiErrorCode valueOf(int value) {
        switch(value) {
            case 0:
                return GIZ_SDK_SUCCESS;
            case 1003:
                return GIZ_SITE_PRODUCTKEY_INVALID;
            case 5001:
                return GIZ_PUSHAPI_BODY_JSON_INVALID;
            case 5300:
                return GIZ_PUSHAPI_DATA_NOT_EXIST;
            case 5501:
                return GIZ_PUSHAPI_NO_CLIENT_CONFIG;
            case 5502:
                return GIZ_PUSHAPI_NO_SERVER_DATA;
            case 5601:
                return GIZ_PUSHAPI_GIZWITS_APPID_EXIST;
            case 5700:
                return GIZ_PUSHAPI_PARAM_ERROR;
            case 5701:
                return GIZ_PUSHAPI_AUTH_KEY_INVALID;
            case 5702:
                return GIZ_PUSHAPI_APPID_OR_TOKEN_ERROR;
            case 5703:
                return GIZ_PUSHAPI_TYPE_PARAM_ERROR;
            case 5704:
                return GIZ_PUSHAPI_ID_PARAM_ERROR;
            case 5705:
                return GIZ_PUSHAPI_APPKEY_SECRETKEY_INVALID;
            case 5706:
                return GIZ_PUSHAPI_CHANNELID_ERROR_INVALID;
            case 5900:
                return GIZ_PUSHAPI_PUSH_ERROR;
            case 8001:
                return GIZ_SDK_PARAM_FORM_INVALID;
            case 8002:
                return GIZ_SDK_CLIENT_NOT_AUTHEN;
            case 8003:
                return GIZ_SDK_CLIENT_VERSION_INVALID;
            case 8004:
                return GIZ_SDK_UDP_PORT_BIND_FAILED;
            case 8005:
                return GIZ_SDK_DAEMON_EXCEPTION;
            case 8006:
                return GIZ_SDK_PARAM_INVALID;
            case 8007:
                return GIZ_SDK_APPID_LENGTH_ERROR;
            case 8008:
                return GIZ_SDK_LOG_PATH_INVALID;
            case 8009:
                return GIZ_SDK_LOG_LEVEL_INVALID;
            case 8020:
                return GIZ_SDK_NO_AVAILABLE_DEVICE;
            case 8021:
                return GIZ_SDK_DEVICE_CONFIG_SEND_FAILED;
            case 8022:
                return GIZ_SDK_DEVICE_CONFIG_IS_RUNNING;
            case 8023:
                return GIZ_SDK_DEVICE_CONFIG_TIMEOUT;
            case 8024:
                return GIZ_SDK_DEVICE_DID_INVALID;
            case 8025:
                return GIZ_SDK_DEVICE_MAC_INVALID;
            case 8026:
                return GIZ_SDK_SUBDEVICE_INVALID;
            case 8027:
                return GIZ_SDK_DEVICE_PASSCODE_INVALID;
            case 8028:
                return GIZ_SDK_DEVICE_NOT_CENTERCONTROL;
            case 8029:
                return GIZ_SDK_DEVICE_NOT_SUBSCRIBED;
            case 8030:
                return GIZ_SDK_DEVICE_NO_RESPONSE;
            case 8031:
                return GIZ_SDK_DEVICE_NOT_READY;
            case 8032:
                return GIZ_SDK_DEVICE_NOT_BINDED;
            case 8033:
                return GIZ_SDK_DEVICE_CONTROL_WITH_INVALID_COMMAND;
            case 8034:
                return GIZ_SDK_DEVICE_CONTROLL_FAILED;
            case 8035:
                return GIZ_SDK_DEVICE_GET_STATUS_FAILED;
            case 8036:
                return GIZ_SDK_DEVICE_CONTROL_VALUE_TYPE_ERROR;
            case 8037:
                return GIZ_SDK_DEVICE_CONTROL_VALUE_OUT_OF_RANGE;
            case 8038:
                return GIZ_SDK_DEVICE_CONTROL_NOT_WRITABLE_COMMAND;
            case 8039:
                return GIZ_SDK_BIND_DEVICE_FAILED;
            case 8040:
                return GIZ_SDK_UNBIND_DEVICE_FAILED;
            case 8041:
                return GIZ_SDK_DNS_FAILED;
            case 8042:
                return GIZ_SDK_M2M_CONNECTION_SUCCESS;
            case 8043:
                return GIZ_SDK_SET_SOCKET_NON_BLOCK_FAILED;
            case 8044:
                return GIZ_SDK_CONNECTION_TIMEOUT;
            case 8045:
                return GIZ_SDK_CONNECTION_REFUSED;
            case 8046:
                return GIZ_SDK_CONNECTION_ERROR;
            case 8047:
                return GIZ_SDK_CONNECTION_CLOSED;
            case 8048:
                return GIZ_SDK_SSL_HANDSHAKE_FAILED;
            case 8049:
                return GIZ_SDK_DEVICE_LOGIN_VERIFY_FAILED;
            case 8050:
                return GIZ_SDK_INTERNET_NOT_REACHABLE;
            case 8051:
                return GIZ_SDK_M2M_CONNECTION_FAILED;
            case 8095:
                return GIZ_SDK_HTTP_SERVER_NOT_SUPPORT_API;
            case 8096:
                return GIZ_SDK_HTTP_ANSWER_FORMAT_ERROR;
            case 8097:
                return GIZ_SDK_HTTP_ANSWER_PARAM_ERROR;
            case 8098:
                return GIZ_SDK_HTTP_SERVER_NO_ANSWER;
            case 8099:
                return GIZ_SDK_HTTP_REQUEST_FAILED;
            case 8100:
                return GIZ_SDK_THREAD_CREATE_FAILED;
            case 8101:
                return GIZ_SDK_MEMORY_MALLOC_FAILED;
            case 8102:
                return GIZ_SDK_OTHERWISE;
            case 8103:
                return GizWifiSDKErrorCode_OTHERWISE;
            case 8120:
                return GIZ_SDK_SCHEDULER_CREATE_FAILED;
            case 8121:
                return GIZ_SDK_SCHEDULER_DELETE_FAILED;
            case 8122:
                return GIZ_SDK_SCHEDULER_EDIT_FAILED;
            case 8123:
                return GIZ_SDK_SCHEDULER_LIST_UPDATE_FAILED;
            case 8124:
                return GIZ_SDK_SCHEDULER_TASK_EDIT_FAILED;
            case 8125:
                return GIZ_SDK_SCHEDULER_TASK_LIST_UPDATE_FAILED;
            case 8126:
                return GIZ_SDK_SCHEDULER_ID_INVALID;
            case 8127:
                return GIZ_SDK_SCHEDULER_ENABLE_DISABLE_FAILED;
            case 8128:
                return GIZ_SDK_SCHEDULER_STATUS_UPDATE_FAILED;
            case 8140:
                return GIZ_SDK_SUBDEVICE_ADD_FAILED;
            case 8141:
                return GIZ_SDK_SUBDEVICE_DELETE_FAILED;
            case 8142:
                return GIZ_SDK_SUBDEVICE_LIST_UPDATE_FAILED;
            case 8150:
                return GIZ_SDK_USER_ID_INVALID;
            case 8151:
                return GIZ_OPENAPI_TOKEN_INVALID;
            case 8152:
                return GIZ_SDK_GROUP_ID_INVALID;
            case 8153:
                return GIZ_SDK_GROUPNAME_INVALID;
            case 8154:
                return GIZ_SDK_GROUP_PRODUCTKEY_INVALID;
            case 8155:
                return GIZ_SDK_GROUP_CREATE_FAILED;
            case 8156:
                return GIZ_SDK_GROUP_DELETE_FAILED;
            case 8157:
                return GIZ_SDK_GROUP_EDIT_FAILED;
            case 8158:
                return GIZ_SDK_GROUP_LIST_UPDATE_FAILED;
            case 8159:
                return GIZ_SDK_GROUP_COMMAND_WRITE_FAILED;
            case 8170:
                return GIZ_SDK_SCENE_CREATE_FAILED;
            case 8171:
                return GIZ_SDK_SCENE_DELETE_FAILED;
            case 8172:
                return GIZ_SDK_SCENE_EDIT_FAILED;
            case 8173:
                return GIZ_SDK_SCENE_LIST_UPDATE_FAILED;
            case 8174:
                return GIZ_SDK_SCENE_ITEM_LIST_EDIT_FAILED;
            case 8175:
                return GIZ_SDK_SCENE_ITEM_LIST_UPDATE_FAILED;
            case 8176:
                return GIZ_SDK_SCENE_ID_INVALID;
            case 8177:
                return GIZ_SDK_SCENE_RUN_FAILED;
            case 8178:
                return GIZ_SDK_SCENE_STATUS_UPDATE_FAILED;
            case 8201:
                return GIZ_SDK_DATAPOINT_NOT_DOWNLOAD;
            case 8202:
                return GIZ_SDK_DATAPOINT_SERVICE_UNAVAILABLE;
            case 8203:
                return GIZ_SDK_DATAPOINT_PARSE_FAILED;
            case 8300:
                return GIZ_SDK_SDK_NOT_INITIALIZED;
            case 8301:
                return GIZ_SDK_APK_CONTEXT_IS_NULL;
            case 8302:
                return GIZ_SDK_APK_PERMISSION_NOT_SET;
            case 8303:
                return GIZ_SDK_CHMOD_DAEMON_REFUSED;
            case 8304:
                return GIZ_SDK_EXEC_DAEMON_FAILED;
            case 8305:
                return GIZ_SDK_EXEC_CATCH_EXCEPTION;
            case 8306:
                return GIZ_SDK_APPID_IS_EMPTY;
            case 8307:
                return GIZ_SDK_UNSUPPORTED_API;
            case 8308:
                return GIZ_SDK_REQUEST_TIMEOUT;
            case 8309:
                return GIZ_SDK_DAEMON_VERSION_INVALID;
            case 8310:
                return GIZ_SDK_PHONE_NOT_CONNECT_TO_SOFTAP_SSID;
            case 8311:
                return GIZ_SDK_DEVICE_CONFIG_SSID_NOT_MATCHED;
            case 8312:
                return GIZ_SDK_NOT_IN_SOFTAPMODE;
            case 8313:
                return GIZ_SDK_CONFIG_NO_AVAILABLE_WIFI;
            case 8314:
                return GIZ_SDK_RAW_DATA_TRANSMIT;
            case 8315:
                return GIZ_SDK_PRODUCT_IS_DOWNLOADING;
            case 8316:
                return GIZ_SDK_START_SUCCESS;
            case 8317:
                return GIZ_SDK_NEED_UPDATE_TO_LATEST;
            case 8350:
                return GIZ_SDK_OTA_FIRMWARE_IS_LATEST;
            case 8351:
                return GIZ_SDK_OTA_FIRMWARE_DOWNLOAD_OK;
            case 8352:
                return GIZ_SDK_OTA_FIRMWARE_DOWNLOAD_FAILED;
            case 8353:
                return GIZ_SDK_OTA_DEVICE_BUSY_IN_UPGRADE;
            case 8354:
                return GIZ_SDK_OTA_PUSH_FAILED;
            case 8355:
                return GIZ_SDK_OTA_FIRMWARE_VERSION_TOO_LOW;
            case 8356:
                return GIZ_SDK_OTA_FIRMWARE_CHECK_FAILED;
            case 8357:
                return GIZ_SDK_OTA_UPGRADE_FAILED;
            case 8358:
                return GIZ_SDK_OTA_UPGRADE_SUCCESS;
            case 9001:
                return GIZ_OPENAPI_MAC_ALREADY_REGISTERED;
            case 9002:
                return GIZ_OPENAPI_PRODUCT_KEY_INVALID;
            case 9003:
                return GIZ_OPENAPI_APPID_INVALID;
            case 9004:
                return GIZ_OPENAPI_TOKEN_INVALID;
            case 9005:
                return GIZ_OPENAPI_USER_NOT_EXIST;
            case 9006:
                return GIZ_OPENAPI_TOKEN_EXPIRED;
            case 9007:
                return GIZ_OPENAPI_M2M_ID_INVALID;
            case 9008:
                return GIZ_OPENAPI_SERVER_ERROR;
            case 9009:
                return GIZ_OPENAPI_CODE_EXPIRED;
            case 9010:
                return GIZ_OPENAPI_CODE_INVALID;
            case 9011:
                return GIZ_OPENAPI_SANDBOX_SCALE_QUOTA_EXHAUSTED;
            case 9012:
                return GIZ_OPENAPI_PRODUCTION_SCALE_QUOTA_EXHAUSTED;
            case 9013:
                return GIZ_OPENAPI_PRODUCT_HAS_NO_REQUEST_SCALE;
            case 9014:
                return GIZ_OPENAPI_DEVICE_NOT_FOUND;
            case 9015:
                return GIZ_OPENAPI_FORM_INVALID;
            case 9016:
                return GIZ_OPENAPI_DID_PASSCODE_INVALID;
            case 9017:
                return GIZ_OPENAPI_DEVICE_NOT_BOUND;
            case 9018:
                return GIZ_OPENAPI_PHONE_UNAVALIABLE;
            case 9019:
                return GIZ_OPENAPI_USERNAME_UNAVALIABLE;
            case 9020:
                return GIZ_OPENAPI_USERNAME_PASSWORD_ERROR;
            case 9021:
                return GIZ_OPENAPI_SEND_COMMAND_FAILED;
            case 9022:
                return GIZ_OPENAPI_EMAIL_UNAVALIABLE;
            case 9023:
                return GIZ_OPENAPI_DEVICE_DISABLED;
            case 9024:
                return GIZ_OPENAPI_FAILED_NOTIFY_M2M;
            case 9025:
                return GIZ_OPENAPI_ATTR_INVALID;
            case 9026:
                return GIZ_OPENAPI_USER_INVALID;
            case 9027:
                return GIZ_OPENAPI_FIRMWARE_NOT_FOUND;
            case 9028:
                return GIZ_OPENAPI_JD_PRODUCT_NOT_FOUND;
            case 9029:
                return GIZ_OPENAPI_DATAPOINT_DATA_NOT_FOUND;
            case 9030:
                return GIZ_OPENAPI_SCHEDULER_NOT_FOUND;
            case 9031:
                return GIZ_OPENAPI_QQ_OAUTH_KEY_INVALID;
            case 9032:
                return GIZ_OPENAPI_OTA_SERVICE_OK_BUT_IN_IDLE;
            case 9033:
                return GIZ_OPENAPI_BT_FIRMWARE_UNVERIFIED;
            case 9034:
                return GIZ_OPENAPI_BT_FIRMWARE_NOTHING_TO_UPGRADE;
            case 9035:
                return GIZ_OPENAPI_SAVE_KAIROSDB_ERROR;
            case 9036:
                return GIZ_OPENAPI_EVENT_NOT_DEFINED;
            case 9037:
                return GIZ_OPENAPI_SEND_SMS_FAILED;
            case 9038:
                return GIZ_OPENAPI_SIGNATURE_INVALID;
            case 9039:
                return GIZ_OPENAPI_NOT_ALLOWED_CALL_API;
            case 9040:
                return GIZ_OPENAPI_BAD_QRCODE_CONTENT;
            case 9041:
                return GIZ_OPENAPI_REQUEST_THROTTLED;
            case 9042:
                return GIZ_OPENAPI_DEVICE_OFFLINE;
            case 9043:
                return GIZ_OPENAPI_TIMESTAMP_INVALID;
            case 9044:
                return GIZ_OPENAPI_X_GIZWITS_SIGNATURE_INVALID;
            case 9045:
                return GIZ_OPENAPI_DEPRECATED_API;
            case 9046:
                return GIZ_OPENAPI_REGISTER_IS_BUSY;
            case 9080:
                return GIZ_OPENAPI_CANNOT_SHARE_TO_SELF;
            case 9081:
                return GIZ_OPENAPI_ONLY_OWNER_CAN_SHARE;
            case 9082:
                return GIZ_OPENAPI_NOT_FOUND_GUEST;
            case 9083:
                return GIZ_OPENAPI_GUEST_ALREADY_BOUND;
            case 9084:
                return GIZ_OPENAPI_NOT_FOUND_SHARING_INFO;
            case 9085:
                return GIZ_OPENAPI_NOT_FOUND_THE_MESSAGE;
            case 9087:
                return GIZ_OPENAPI_SHARING_IS_WAITING_FOR_ACCEPT;
            case 9088:
                return GIZ_OPENAPI_SHARING_IS_EXPIRED;
            case 9089:
                return GIZ_OPENAPI_SHARING_IS_COMPLETED;
            case 9090:
                return GIZ_OPENAPI_INVALID_SHARING_BECAUSE_UNBINDING;
            case 9092:
                return GIZ_OPENAPI_ONLY_OWNER_CAN_BIND;
            case 9093:
                return GIZ_OPENAPI_ONLY_OWNER_CAN_OPERATE;
            case 9094:
                return GIZ_OPENAPI_SHARING_ALREADY_CANCELLED;
            case 9095:
                return GIZ_OPENAPI_OWNER_CANNOT_UNBIND_SELF;
            case 9096:
                return GIZ_OPENAPI_ONLY_GUEST_CAN_CHECK_QRCODE;
            case 9098:
                return GIZ_OPENAPI_MESSAGE_ALREADY_DELETED;
            case 9099:
                return GIZ_OPENAPI_BINDING_NOTIFY_FAILED;
            case 9100:
                return GIZ_OPENAPI_ONLY_SELF_CAN_MODIFY_ALIAS;
            case 9101:
                return GIZ_OPENAPI_ONLY_RECEIVER_CAN_MARK_MESSAGE;
            case 9999:
                return GIZ_OPENAPI_RESERVED;
            case 10010:
                return GIZ_SITE_DATAPOINTS_NOT_DEFINED;
            case 10011:
                return GIZ_SITE_DATAPOINTS_NOT_MALFORME;
            default:
                return GIZ_OTHER_ERROR;
        }
    }
}
