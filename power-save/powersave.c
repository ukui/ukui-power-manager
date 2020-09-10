//
// Created by kylin on 2020/9/4.降低 CPU 频率和屏幕亮度 是最有效的节电
//
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>
#include "bluetooth/bluetooth.h"
#include "IOscheduler/disk.h"
#include "misc/misc.h"
#include "usbAutoSuspend/usb.h"
#include "wifi/wifi.h"
#include "WOL/wol.h"
#include "CPU/cpu.h"
#include "SataALPM/alpm.h"
#include "urb/urb.h"
#include "powersave.h"

struct device_info bluetooth;
struct device_info wifi;
struct device_info usb;
struct device_info alsa;
struct device_info ahci;
struct device_info ac97;
struct device_info hda;
struct device_info writeback;
struct device_node *ondemand;
struct device_info noatime;
struct device_info watchdog;

//bluetooth
int bluetoothCount=20, bluetoothInteraction=0,bluetoothFlag=0;

void *bluetooth_care_taker(void)
{
    printf("\nbluetooth is being taken care of....\n");
    sprintf(bluetooth.devname,"BLUETOOTH");
        recommend_bluetooth_off();
    printf("thread for bluetooth care taker is exiting....\n");
}

//wifi
int wifiCount=0, wifiInteraction=0, wifiFlag;

void *wifi_care_taker(void)
{
    sprintf(wifi.devname,"WIFI");
        recommend_wifi_new_powersave();
}


void *other_care_taker()
{
    count_usb_urbs();
    count_usb_urbs();

        count_usb_urbs();
        recommend_ac97_powersave();
        recommend_hda_powersave();
        recommend_noatime();
        recommend_sata_alpm();
        recommend_writeback_time(6000);
        recommend_usb_autosuspend("on","auto");
        recommend_ondemand_governor();
        recommend_pcie_aspm_save();
        recommend_laptop_mode(5);
        usb_activity_hint();
}

int main()
{
    pthread_t blueThread, blueTimeThread;
    pthread_t wifiThread, wifiTimeThread;
    pthread_t others;

    pthread_create(&blueThread, NULL, bluetooth_care_taker,"blueThread");
    pthread_create(&wifiThread, NULL, wifi_care_taker, "wifiThread");
    pthread_create(&others, NULL, other_care_taker, "otherdevices");

    pthread_join(wifiThread, NULL);
    pthread_join(others, NULL);

    return 0;
}
