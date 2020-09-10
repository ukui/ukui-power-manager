#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdbool.h>
#include "disk.h"
static char *hddIOscheduler0 = "cfq";
static char *hddIOscheduler1 = "mq-deadline";


static char *ssdIOscheduler0 = "noop";
static char *ssdIOscheduler1 = "none";

void io_scheduler_on(char* pos, int hdd)
{
	FILE *file;
	char filename[PATH_MAX];
	bool flag = true;
	sprintf(filename, "/sys/block/%s/queue/scheduler", pos);

	file = fopen(filename, "w");
	if (!file){
		fclose(file);
		return ;
	}

	if(hdd){
		if(!fprintf(file, "%s\n", hddIOscheduler1)){
			fprintf(file, "%s\n", hddIOscheduler0);
			flag = false;
			printf("/sys/block/%s/queue/scheduler has modify: %s \n",pos, hddIOscheduler0);
		}
		if(flag)
			printf("/sys/block/%s/queue/scheduler has modify: %s \n",pos, hddIOscheduler1);
	}else{
		if(!fprintf(file, "%s\n",ssdIOscheduler1)){
			fprintf(file, "%s\n",ssdIOscheduler0);
			flag = false;
			printf("/sys/block/%s/queue/scheduler has modify: %s \n",pos, ssdIOscheduler0);
		}
		if(flag)
			printf("/sys/block/%s/queue/scheduler has modify: %s \n",pos, ssdIOscheduler1);
	}

	fclose(file);

}

void recommend_io_scheduler(void)
{
	DIR *dir;
	struct dirent *dirent;
	FILE *file;
	char filename[PATH_MAX];
	char line[1024];
	int hdd = 0;

	dir = opendir("/sys/block");
	if (!dir)
		return;

	while ((dirent = readdir(dir))) {
			if (dirent->d_name[0]=='.' || dirent->d_name[0]=='l' || dirent->d_name[1]=='r'){//排除loop和./ ../
				continue;
			}
			
			sprintf(filename, "/sys/block/%s/queue/rotational", dirent->d_name);

			file = fopen(filename, "r");
			if (!file)
				continue;
			memset(line, 0, 1024);
			if (fgets(line, 1023,file)==NULL) {
				fclose(file);
				continue;
			}
			if (strstr(line, "1")){//说明是HDD
				io_scheduler_on(dirent->d_name,1);
			}else{
				io_scheduler_on(dirent->d_name,0);
			}
	
			fclose(file);
	}

	closedir(dir);

}

//int main(){
//	recommend_io_scheduler();
//}