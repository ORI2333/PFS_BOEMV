/*
 * Copyright (c) 2020 HiHope Community.
 * Description: wifi softap
 * Author: HiSpark Product Team.
 * Create: 2019-11-30
 */

#include "hi_wifi_api.h"
#include "wifi_softap.h"
#include "lwip/ip_addr.h"
#include "lwip/netifapi.h"
#include <hi_types_base.h>
#include <app_demo_get_mac_addr.h>

#define APP_INIT_VAP_NUM    2
#define APP_INIT_USR_NUM    2

static struct netif *g_lwip_netif = NULL;
extern hi_uchar hispark_ssid[64];

/* clear netif's ip, gateway and netmask */
void hi_softap_reset_addr(struct netif *pst_lwip_netif)
{
    ip4_addr_t st_gw;
    ip4_addr_t st_ipaddr;
    ip4_addr_t st_netmask;

    if (pst_lwip_netif == NULL) {
        printf("hisi_reset_addr::Null param of netdev\r\n");
        return;
    }

    IP4_ADDR(&st_ipaddr, 0, 0, 0, 0);
    IP4_ADDR(&st_gw, 0, 0, 0, 0);
    IP4_ADDR(&st_netmask, 0, 0, 0, 0);

    netifapi_netif_set_addr(pst_lwip_netif, &st_ipaddr, &st_netmask, &st_gw);
}

int hi_wifi_start_softap(void)
{
    int ret;
    errno_t rc;
    char ifname[WIFI_IFNAME_MAX_SIZE + 1] = {0};
    int len = sizeof(ifname);
    hi_wifi_softap_config hapd_conf = {0};
    const unsigned char wifi_vap_res_num = APP_INIT_VAP_NUM;
    const unsigned char wifi_user_res_num = APP_INIT_USR_NUM;
    ip4_addr_t st_gw;
    ip4_addr_t st_ipaddr;
    ip4_addr_t st_netmask;

    ret = hi_wifi_init(wifi_vap_res_num, wifi_user_res_num);
    if (ret != HISI_OK) {
        printf("hi_wifi_init\n");
        return -1;
    }
    /*get Hi3861 mac addr*/
    hi3816_get_mac_addr();

    rc = memcpy_s(hapd_conf.ssid, HI_WIFI_MAX_SSID_LEN + 1, hispark_ssid, 20); /* 20:ssid length */
    if (rc != EOK) {
        return -1;
    }

    hapd_conf.authmode = HI_WIFI_SECURITY_OPEN;
    hapd_conf.channel_num = 1;

    ret = hi_wifi_softap_start(&hapd_conf, ifname, &len);
    if (ret != HISI_OK) {
        printf("hi_wifi_softap_start\n");
        return -1;
    }

    /* acquire netif for IP operation */
    g_lwip_netif = netifapi_netif_find(ifname);
    if (g_lwip_netif == NULL) {
        printf("%s: get netif failed\n", __FUNCTION__);
        return -1;
    }

    IP4_ADDR(&st_gw, 192, 168, 1, 1);          /* input your IP for example: 192.168.1.1 */
    IP4_ADDR(&st_ipaddr, 192, 168, 1, 1);      /* input your netmask for example: 192.168.1.1 */
    IP4_ADDR(&st_netmask, 255, 255, 255, 0);     /* input your gateway for example: 255.255.255.0 */
    netifapi_netif_set_addr(g_lwip_netif, &st_ipaddr, &st_netmask, &st_gw);

    netifapi_dhcps_start(g_lwip_netif, 0, 0);

    return 0;
}

void hi_wifi_stop_softap(void)
{
    int ret;

    netifapi_dhcps_stop(g_lwip_netif);
    hi_softap_reset_addr(g_lwip_netif);

    ret = hi_wifi_softap_stop();
    if (ret != HISI_OK) {
        printf("failed to stop softap\n");
    }

    ret = hi_wifi_deinit();
    if (ret != HISI_OK) {
        printf("failed to deinit wifi\n");
    }

    g_lwip_netif = NULL;
}

