#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <dirent.h>
#include <linux/types.h>
#include <linux/sockios.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <linux/wireless.h>
#include "../powersave.h"
#include "wifi.h"

extern struct device_info wifi;

extern int wifiCount, wifiInteraction;

static int wext_sock = -1;
static char wireless_nic[IFNAMSIZ + 1] = {0};
static bool ps_not_working = false;

static bool check_wireless_unused(void)
{
	struct iwreq wrq;
	char ssid[34] = {0};

	memset(&wrq, 0, sizeof(wrq));
	strncpy(wrq.ifr_name, wireless_nic, sizeof(wrq.ifr_name));

	/* get mode -- PS only available in STA mode */
	if (ioctl(wext_sock, SIOCGIWMODE, &wrq) < 0)
		return false;
	if (wrq.u.mode != IW_MODE_INFRA)
		return false;

	wrq.u.essid.pointer = ssid;
	wrq.u.essid.length = sizeof(ssid);
	if (ioctl(wext_sock, SIOCGIWESSID, &wrq) < 0)
		return false;

	/* no SSID, so can't be connected */
	if (!wrq.u.essid.flags)
		return true;

	if (ioctl(wext_sock, SIOCGIWAP, &wrq) < 0)
		return false;

	/* no AP -- not connected */
	if (memcmp(wrq.u.ap_addr.sa_data, "\0\0\0\0\0\0", 6) == 0)
		return true;

	return false;
}

static void activate_down_recommendion(void)
{
	struct ifreq ifr;
	int ret;

	strncpy(ifr.ifr_name, wireless_nic, sizeof(ifr.ifr_name));

	/* get flags */
	ret = ioctl(wext_sock, SIOCGIFFLAGS, &ifr);
	if (ret < 0)
		return;
	ifr.ifr_flags &= ~IFF_UP;

	ioctl(wext_sock, SIOCSIFFLAGS, &ifr);
}

void dummy_activate_down_recommendion(void)
{
	sprintf(wifi.remark,"WIFI DEVICE IS TURNED OFF");
	sprintf(wifi.activity,"OFF");
	activate_down_recommendion();
}

static int check_wireless_powersave(void)
{
	struct iwreq wrq;

	if (ps_not_working)
		return false;

	memset(&wrq, 0, sizeof(wrq));
	strncpy(wrq.ifr_name, wireless_nic, sizeof(wrq.ifr_name));

	/* get powersave -- if supported */
	if (ioctl(wext_sock, SIOCGIWPOWER, &wrq) < 0) {
		ps_not_working = 1;
		return false;
	}
	return wrq.u.param.disabled;
}

static void activate_wireless_recommendion(void)
{
	struct iwreq wrq;

	memset(&wrq, 0, sizeof(wrq));
	strncpy(wrq.ifr_name, wireless_nic, sizeof(wrq.ifr_name));

	wrq.u.param.disabled = false;
	wrq.u.param.flags = IW_POWER_ON | IW_POWER_TIMEOUT;
	wrq.u.param.value = 500*1000; /* 500 ms */
 again:
	/* set powersave -- if supported */
	if (ioctl(wext_sock, SIOCSIWPOWER, &wrq) == 0){
		return ;
	}
		

	/* maybe timeout is not supported? */
	if (wrq.u.param.flags & IW_POWER_TIMEOUT) {
		wrq.u.param.flags = IW_POWER_ON;
		goto again;
	}

	ps_not_working = true;
}

static void find_wireless_nic(void)
{
	DIR *net;
	struct dirent *ent;
	struct ifreq ifr;
	struct iwreq wrq;
	int ret;

	memset(&ifr, 0, sizeof(struct ifreq));
	memset(&wrq, 0, sizeof(wrq));

	net = opendir("/sys/class/net/");
	if (!net)
		return;

	while ((ent = readdir(net))) {
		strncpy(ifr.ifr_name, ent->d_name, sizeof(ifr.ifr_name));
		strncpy(wrq.ifr_name, ent->d_name, sizeof(wrq.ifr_name));

		/* Check if the interface is up */
		ret = ioctl(wext_sock, SIOCGIFFLAGS, &ifr);

		
		if (ret < 0)
			continue;
		if (!(ifr.ifr_flags & (IFF_UP | IFF_RUNNING)))
			continue;

		/* and a wireless interface (that we handle) */

		ret = ioctl(wext_sock, SIOCGIWNAME, &wrq);
		if (ret < 0)
			continue;
		strcpy(wireless_nic, ifr.ifr_name);
		break;
	}

	closedir(net);
}

void recommend_wifi_new_powersave(void)
{
	char sug[1024];

	if (wext_sock == -1) {
		wext_sock = socket(AF_INET, SOCK_DGRAM, 0);
		if (wext_sock < 0)
			return;
	}

	if (strlen(wireless_nic) == 0)
		find_wireless_nic();

	if (strlen(wireless_nic) == 0)
		return;

static int first=1;

	if (check_wireless_unused())
	{
		if(!wifiCount && first)
		{
			wifiCount=10;
			first=0;
		}
	} 
	else 
	{	first=1;
		wifiInteraction=1;
		sprintf(wifi.activity,"ON");

		if (check_wireless_powersave()) 
		{
			sprintf(wifi.remark,"WIFI PUT TO POWER SAVE MODE");
			sprintf(wifi.activity,"POWER SAVE");
			activate_wireless_recommendion();
		}
	}
}


//int  main(int argc, char const *argv[])
//{
//	/* code */
//	recommend_wifi_new_powersave();
//	return 0;
//}
