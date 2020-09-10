#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <dirent.h>
#include <linux/types.h>
#include <net/if.h>
#include <linux/sockios.h>
#include <sys/ioctl.h>
#include <errno.h>
#include "wol.h"
#include <linux/ethtool.h>

void activate_WOL_recommendion(char *filename)
{
	char command[1024];
	strcpy(command,"ethtool -s ");
	strcat(command,filename);
	strcat(command," wol d");
	printf("%s\n",command);
	int ret = system(command);
printf("%d\n",ret);
	if(ret == -1){
		printf("%s disable wake on lan fail!\n",filename);

	}else if(ret == 0){
		printf("%s disable wake on lan succesfully!\n",filename);
	}
	
}

void recommend_WOL_off(void)
{
	DIR *dir;
	struct dirent *dirent;
	FILE *file;
	char filename[PATH_MAX];

	dir = opendir("/sys/class/net");
	if (!dir)
		return;

	while ((dirent = readdir(dir))) {
			if (dirent->d_name[0]=='.')
				continue;

			sprintf(filename, "/sys/class/net/%s/device/class", dirent->d_name);
			file = fopen(filename, "r");
			if (!file)
				continue;
			
			fclose(file);

			activate_WOL_recommendion(dirent->d_name);
	}

	closedir(dir);
}


//int main(){
//
//	recommend_WOL_off();
//}
