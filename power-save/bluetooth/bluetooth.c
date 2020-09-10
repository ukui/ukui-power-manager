#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <errno.h>
#include<signal.h>
#include "../powersave.h"
#include "bluetooth.h"
extern struct device_info bluetooth;
extern int bluetoothCount, bluetoothInteraction;

#define HCIGETDEVINFO   _IOR('H', 211, int)
#define BTPROTO_HCI     1

#define __u16 uint16_t
#define __u8 uint8_t
#define __u32 uint32_t

static int recommended = 0;

typedef struct {
        __u8 b[6];
} __attribute__((packed)) bdaddr_t;

struct hci_dev_stats {
        __u32 err_rx;
        __u32 err_tx;
        __u32 cmd_tx;
        __u32 evt_rx;
        __u32 acl_tx;
        __u32 acl_rx;
        __u32 sco_tx;
        __u32 sco_rx;
        __u32 byte_rx;
        __u32 byte_tx;
};


struct hci_dev_info {
	__u16 dev_id;
	char  name[8];

	bdaddr_t bdaddr;

	__u32 flags;
	__u8  type;

	__u8  features[8];

	__u32 pkt_type;
	__u32 link_policy;
	__u32 link_mode;

	__u16 acl_mtu;
	__u16 acl_pkts;
	__u16 sco_mtu;
	__u16 sco_pkts;

	struct hci_dev_stats stat;
};

static int previous_bytes = -1;

void turn_bluetooth_off(void)
{
	system("hciconfig hci0 down &> /dev/null");
	sprintf(bluetooth.activity,"OFF");
	sprintf(bluetooth.remark,"BLUETOOTH IS TURNED OFF");
}

void recommend_bluetooth_off(void)
{
	struct hci_dev_info devinfo;
	FILE *file;
	int fd;
	int ret;
	int thisbytes = 0;
	if (access("/sys/module/bluetooth",F_OK))    //check whether bluetooth is present for the system
		return;

	fd = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI);
	if (fd < 0)//can't hci socket
		return;

	memset(&devinfo, 0, sizeof(devinfo));
	strcpy(devinfo.name, "hci0");
	ret = ioctl(fd, HCIGETDEVINFO, (void *) &devinfo);
	if (ret < 0)
		goto out;

	if ( (devinfo.flags & 1) == 0 &&
		access("/sys/module/hci_usb",F_OK)) /* interface down already */
				goto out;

	thisbytes += devinfo.stat.byte_rx;
	thisbytes += devinfo.stat.byte_tx;

	if (thisbytes != previous_bytes)
	{
		bluetoothInteraction=1;
		sprintf(bluetooth.activity,"ON");
		sprintf(bluetooth.remark,"DATA IS BEING EXCHANGED");
		goto out;
	}
	/* now, also check for active connections */
	file = popen("/usr/bin/hcitool con 2> /dev/null", "r");
	if (file) {
		char line[2048];
		/* first line is standard header: "Connections:" */
		fgets(line,2048,file);
		memset(line, 0, 2048);
		fgets(line, 2047, file);
		pclose(file);
		if (strlen(line)>0)
			goto out;
	}
	printf("bluetooth %d\n",bluetoothCount);
	if(!bluetoothCount)
	{
		bluetoothCount=20;
	}
	out:

	previous_bytes = thisbytes;
	close(fd);
	return;
}
