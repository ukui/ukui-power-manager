//
// Created by kylin on 2020/9/4.
//

#ifndef POWER_SAVE_POWERSAVE_H
#define POWER_SAVE_POWERSAVE_H
#include <libintl.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
struct device_info
{
    char devname[20];
    int detected;
    int remtime;
    char remark[100];
    char activity[100];

};

struct device_node
{
    struct device_info devinfo;
    struct device_node *next;
};


int kbhit(void);

#endif //POWER_SAVE_POWERSAVE_H
