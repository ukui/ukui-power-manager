//
// Created by kylin on 2020/9/4.
//

#ifndef POWER_SAVE_USB_H
#define POWER_SAVE_USB_H
void recommend_usb_autosuspend(char mode[],char expect[]);
void activate_usb_autosuspend(char expect[]);
void push_line(char *string, int count);
#endif //POWER_SAVE_USB_H
