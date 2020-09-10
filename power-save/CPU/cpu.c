#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <dirent.h>
#include "../powersave.h"
#include "cpu.h"
extern struct device_node *ondemand;

extern struct device_node *getnode()
{
	struct device_node *p;
	p=(char *)malloc(sizeof(struct device_node));
	return p;
}

static void activate_ondemand(void)
{
	DIR *dir;
	struct dirent *dirent;
	FILE *file;
	char filename[PATH_MAX];
	struct device_node *node1;
	node1=ondemand->next;
	system("/sbin/modprobe cpufreq_ondemand > /dev/null 2>&1");


	dir = opendir("/sys/devices/system/cpu");
	if (!dir)
		return;

	while ((dirent = readdir(dir))) {
		if (dirent->d_name[0]=='.')
			continue;
		sprintf(filename, "/sys/devices/system/cpu/%s/cpufreq/scaling_governor", dirent->d_name);
		file = fopen(filename, "w");
		if (!file)
			continue;
		sprintf(node1->devinfo.activity,"GOVERNOR = ONDEMAND");
		sprintf(node1->devinfo.remark,"CPU FREQUENCY ACCORDING TO THE WORKLOAD");
		node1=node1->next;
		int flag = fprintf(file, "powersave\n");
		if(!flag){
			fprintf(file, "ondemand\n");
		}
		printf("%s has set powersave\n",filename);

		fclose(file);
	}

	closedir(dir);
}

void recommend_ondemand_governor(void)
{
	DIR *dir;
	struct dirent *dirent;
	FILE *file;
	char filename[PATH_MAX];
	char line[1024];
	struct device_node *node1,*node2;
	char gov[1024];
	int ret = 0;
	node1=getnode();
	ondemand=node1;
	node1->next=NULL;

	gov[0] = 0;


	dir = opendir("/sys/devices/system/cpu");
	if (!dir)
		return;

	while ((dirent = readdir(dir))) {
		if (dirent->d_name[0]=='.')
			continue;
		sprintf(filename, "/sys/devices/system/cpu/%s/cpufreq/scaling_governor", dirent->d_name);
		file = fopen(filename, "r");
		if (!file)
			continue;
		node2=getnode();
		node1->next=node2;
		strcpy(node2->devinfo.devname,dirent->d_name);
		node2->next=NULL;
		node1=node2;
		memset(line, 0, 1024);
		if (fgets(line, 1023,file)==NULL) {
			fclose(file);
			continue;
		}
		if (strlen(gov)==0)
			strcpy(gov, line);
		else
			/* if the governors are inconsistent, warn */
			if (strcmp(gov, line))
				ret = 1;
		fclose(file);
	}

	closedir(dir);

	/* if the governor is set to userspace, also warn */
	if (strstr(gov, "userspace"))
		ret = 1;

	if (strstr(gov, "performance"))
		ret = 1;


	if (ret) {
			activate_ondemand();
	}
}

//int main(){
//	recommend_ondemand_governor();
//	return 0;
//
//}
