//
// Created by kylin on 2020/9/9.
//
#include <stdio.h>

#include "powersave.h"
#include "usbAutoSuspend/usb.h"
#include "misc/misc.h"

struct device_info usb;
struct device_info alsa;
struct device_info ahci;
struct device_info ac97;
struct device_info hda;
struct device_info writeback;
struct device_info noatime;

int main(){

    recommend_usb_autosuspend("auto","on");
    laptop_mode_on(0);
    recommend_writeback_time(1500);

    return 0;
}