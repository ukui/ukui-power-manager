
/*
min_power, max_performance, medium_power,med_power_with_dipm(4.15later main aim to intel, others arch can't use)
*/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <dirent.h>
#include "alpm.h"
int alpm_activated;

void activate_alpm(void)
{
	DIR *dir;
	struct dirent *dirent;
	FILE *file;
	char filename[PATH_MAX];

	alpm_activated = 1;					//flag that signifies alpm is activated

	dir = opendir("/sys/class/scsi_host");			//directory containing info of the scsi's i.e. links of hostX
	if (!dir)						//if directory not present
		return;

	while ((dirent = readdir(dir)))				//point to hostX;
	{
		if (dirent->d_name[0]=='.')			//if name starts with '.' i.e. hidden go for next entry
			continue;

		sprintf(filename, "/sys/class/scsi_host/%s/link_power_management_policy", dirent->d_name);
		file = fopen(filename, "w");			//open link_power_management_policy file

		if (!file)					//if file doesn't exist go to next entry
			continue;

		fprintf(file, "min_power\n");			//write min_power to l_p_m_p
		fclose(file);
	}

	closedir(dir);
}

void recommend_sata_alpm(void)
{
	DIR *dir;
	struct dirent *dirent;
	FILE *file;
	char filename[PATH_MAX];
	char line[1024];
	int need_hint  = 0;

	if (alpm_activated)
		return;


	dir = opendir("/sys/class/scsi_host/");
	if (!dir)
		return;

	while ((dirent = readdir(dir)))
	{
		if (dirent->d_name[0]=='.')
			continue;

		sprintf(filename, "/sys/class/scsi_host/%s/link_power_management_policy", dirent->d_name);
		file = fopen(filename, "r");

		if (!file)
			continue;

		memset(line, 0, 1024);
		if (fgets(line, 1023,file)==NULL)
		{
			fclose(file);
			continue;
		}

		if (!strstr(line, "min_power\n"))
			need_hint = 1;

		fclose(file);
	}

	closedir(dir);

	if (need_hint) {
		
		activate_alpm();
	}
	
}

//int main(){
//	recommend_sata_alpm();
//	return 0;
//
//}
