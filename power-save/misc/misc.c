/*
 * pcie_aspm一共有四种模式可供选择，分别是：default,performance,powersave,powersupersave
 * */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <dirent.h>
#include "../powersave.h"
#include "misc.h"

extern struct device_info ac97;
extern struct device_info hda;
extern struct device_info noatime;
extern struct device_info watchdog;
extern struct device_info writeback;

void nmi_watchdog_off(void)
{
	FILE *file;
	file = fopen("/proc/sys/kernel/nmi_watchdog", "w");
	if (!file)
		return;
	fprintf(file,"0\n");
	printf("nmi_watchdog off \n");
	fclose(file);
}
void recommend_nmi_watchdog(void)
{
	FILE *file;
	int i;
	char buffer[1024];
	file = fopen("/proc/sys/kernel/nmi_watchdog", "r");
	if (!file)
		return;
	memset(buffer, 0, 1024);
	if (!fgets(buffer, 1023, file)) {
		fclose(file);
		return;
	}
	i = strtoul(buffer, NULL, 10);
	if (i!=0) {
		nmi_watchdog_off();
	}
	fclose(file);
}

void ac97_power_on(void)
{
	FILE *file;

	file = fopen("/sys/module/snd_ac97_codec/parameters/power_save", "w");
	if (!file)
	{
		ac97.detected=0;
		
		return;
	}
	ac97.detected=1;
	sprintf(ac97.remark,"AC97 IN POWER SAVING MODE");
	sprintf(ac97.activity,"power_save");	
	fprintf(file,"1");
	printf("ac97 power save on\n");
	fclose(file);
	if (access("/dev/dsp", F_OK))
		return;
	/* kick power mgmt update to the driver */
	file = fopen("/dev/dsp", "w");
	if (file) {
		fprintf(file,"1");
		fclose(file);
	}
}

void recommend_ac97_powersave(void)
{
	FILE *file;
	char buffer[1024];
	file = fopen("/sys/module/snd_ac97_codec/parameters/power_save", "r");
	if (!file)
		return;
	memset(buffer, 0, 1024);
	if (!fgets(buffer, 1023, file)) {
		fclose(file);
		return;
	}
	if (buffer[0]=='N') {
		ac97_power_on();
	}
	fclose(file);
}

void hda_power_on(void)
{
	FILE *file;

	file = fopen("/sys/module/snd_hda_intel/parameters/power_save", "w");
	if (!file)
	{
		hda.detected=0;		
		return;
	}	
	hda.detected=1;
	sprintf(hda.activity,"power_save");
	sprintf(hda.remark,"HIGH DEFINITION AUDIO PUT IN POWER SAVING MODE");
	fprintf(file,"1");
	printf("hda power save on\n");
	fclose(file);
	if (access("/dev/dsp", F_OK))
		return;
	/* kick power mgmt update to the driver */
	file = fopen("/dev/dsp", "w");
	if (file) {
		fprintf(file,"1");
		fclose(file);
	}
}

void recommend_hda_powersave(void)
{
	FILE *file;
	char buffer[1024];

	file = fopen("/sys/module/snd_hda_intel/parameters/power_save", "r");
	if (!file)
		return;
	memset(buffer, 0, 1024);
	if (!fgets(buffer, 1023, file)) {
		fclose(file);
		return;
	}
	if (buffer[0]=='0') {
		hda_power_on();
	}
	fclose(file);
}

void noatime_on(void)
{
	system("sudo mount -o remount,noatime,nodiratime /");
	sprintf(noatime.activity,"REMOUNTED FS WITH NOATIME");
	strcat(noatime.remark," & NEEDLESS WRITEs FOR EACH READ PREVENTED");
	printf("noatime on\n");
}

void recommend_noatime(void)
{
	FILE *file;
	char *which;
	char buffer[1024];
	int recommend = 0;
	file = fopen("/proc/mounts","r");
	if (!file)
		return;
	while (!feof(file)) {
		memset(buffer, 0, 1024);
		if (!fgets(buffer, 1023, file))
			break;

		if (strstr(buffer, " / ext") && !strstr(buffer, "noatime") && strstr(buffer, "relatime"))
		{
			which=strstr(buffer, " / ext");
			sprintf(noatime.remark,"EXT%c FILESYSTEM DETECTED",*(which+6));
			recommend = 1;
		}
	}
	if (recommend) {
		noatime_on();
	}
	fclose(file);
}

void writeback_long(int time)
{
	FILE *file;
	file = fopen("/proc/sys/vm/dirty_writeback_centisecs", "w");
	if (!file)
		return;
	
	sprintf(writeback.activity,"WRITEBACK TIME INCREASED FROM 5 SECONDS TO 15 SECONDS");
	sprintf(writeback.remark,"MORE POWER SAVING OPPORTUNITIES");
	fprintf(file,"%d\n",time);
	printf("writeback %d on\n",time);
	fclose(file);
}

void recommend_writeback_time(int time)
{
	FILE *file;
	char buffer[1024];
	int i;
	file = fopen("/proc/sys/vm/dirty_writeback_centisecs", "r");
	if (!file)
		return;
	memset(buffer, 0, 1024);
	if (!fgets(buffer, 1023, file)) {
		fclose(file);
		return;
	}
	i = strtoull(buffer, NULL, 10);
	if (i != 6000) {
        writeback_long(time);
    }
	fclose(file);
}

void laptop_mode_on(int mode){
    FILE *file;
    file = fopen("/proc/sys/vm/laptop_mode", "w");
    if (!file)
        return;
    fprintf(file,"%d \n",mode);
    printf("laptop mode on %d\n",mode);
    fclose(file);
}

void recommend_laptop_mode(int mode){
    FILE *file;
    char buffer[1024];
    int i;
    file = fopen("/proc/sys/vm/laptop_mode", "r");
    if (!file)
        return;
    memset(buffer, 0, 1024);
    if (!fgets(buffer, 1023, file)) {
        fclose(file);
        return;
    }

        laptop_mode_on(mode);

    fclose(file);
}

void pcie_aspm_save(void)
{
        FILE *file;
        file = fopen("/sys/module/pcie_aspm/parameters/policy", "w");
        if (!file)
                return;
        fprintf(file,"powersave\n");
        printf("pcie_aspm power save on \n");
        fclose(file);
}
void recommend_pcie_aspm_save(void)
{
        FILE *file;
        int i;
        char buffer[1024];
        char *mode = "[powersave]";
        file = fopen("/sys/module/pcie_aspm/parameters/policy", "r");
        if (!file)
                return;
        memset(buffer, 0, 1024);
        if (!fgets(buffer, 1023, file)) {
                fclose(file);
                return;
        }
        if (!strstr(buffer, mode)) {
                pcie_aspm_save();
        }
        fclose(file);
}


//int main(){
//	recommend_writeback_time();
//	recommend_noatime();
//	recommend_hda_powersave();
//	recommend_nmi_watchdog();
//	recommend_ac97_powersave();
//	recommend_laptop_mode();
//	recommend_pcie_aspm_save();
//	return 0;
//}
