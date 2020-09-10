#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <dirent.h>
#include "usb.h"

void activate_usb_autosuspend(char expect[])
{
    DIR *dir;
    struct dirent *dirent;
    FILE *file,*file1;
    char filename[PATH_MAX];
    dir = opendir("/sys/bus/usb/devices");
    if (!dir)
        return;
    while ((dirent = readdir(dir))) {
        if (dirent->d_name[0]=='.')
            continue;

        sprintf(filename, "/sys/bus/usb/devices/%s/power/control", dirent->d_name);
        file = fopen(filename, "w");
        if (!file)
            continue;
        fprintf(file, "%s\n",expect);
        printf("%s now in %s mode \n",filename,expect);
	sprintf(filename, "/sys/bus/usb/devices/%s/power/autosuspend_delay_ms", dirent->d_name);
        file1 = fopen(filename, "w");
        if (!file1)
            continue; 
        fprintf(file1, "5000\n");
        fclose(file);
	fclose(file1);
    }

    closedir(dir);
}
void recommend_usb_autosuspend(char mode[], char expect[])
{
    DIR *dir;
    struct dirent *dirent;
    FILE *file;
    char filename[PATH_MAX];
    char line[1024];
    int need_hint = 0;

    dir = opendir("/sys/bus/usb/devices");
    if (!dir)
        return;

    while ((dirent = readdir(dir))) {
        if (dirent->d_name[0]=='.')
            continue;

        sprintf(filename, "/sys/bus/usb/devices/%s/power/control", dirent->d_name);

        file = fopen(filename, "r");
        if (!file)
            continue;
        memset(line, 0, 1024);
        if (fgets(line, 1023,file)==NULL) {
            fclose(file);
            continue;
        }
        if (strstr(line, mode))
            need_hint = 1;

        fclose(file);
    }

    closedir(dir);

    if (need_hint) {
        activate_usb_autosuspend(expect);
    }
}

//int main(){
//    recommend_usb_autosuspend();
//    return 0;
//}

