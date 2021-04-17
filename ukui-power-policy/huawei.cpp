#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <dirent.h>
#include "huawei.h"
struct device_info {
    char devname[20];
    int detected;
    int remtime;
    char remark[100];
    char activity[100];

};
struct device_node {
    struct device_info devinfo;
    struct device_node *next;
};

struct device_node *ondemand;

struct device_node *getnode()
{
    struct device_node *p;
    p = (struct device_node *) malloc(sizeof(struct device_node));
    return p;
}

static int *getCpuFreq(char path[])
{
    int *freq;
    freq = (int *) malloc(1024);
    char filename[PATH_MAX];
    FILE *file;
    //read min freq
    sprintf(filename, "%s%s", path, "cpuinfo_min_freq");

    file = fopen(filename, "r");
    if (!file)
	return freq;

    if (fscanf(file, "%d", &freq[0]) == NULL) {
	fclose(file);
	return freq;
    }
    fclose(file);
    //read max freq
    sprintf(filename, "%s%s", path, "cpuinfo_max_freq");
    file = fopen(filename, "r");
    if (!file)
	return freq;

    if (fscanf(file, "%d", &freq[1]) == NULL) {
	fclose(file);
	return freq;
    }
    fclose(file);
    return freq;

}

static void cpuOndemand(char mode[])
{
    DIR *dir;
    struct dirent *dirent;
    FILE *file, *file1;
    char filename[PATH_MAX];
    char mode_options[PATH_MAX];
    struct device_node *node1;
    node1 = ondemand->next;

    dir = opendir("/sys/devices/system/cpu");
    if (!dir)
	return;

    while ((dirent = readdir(dir))) {
	if (dirent->d_name[0] == '.')
	    continue;
	sprintf(filename,
		"/sys/devices/system/cpu/%s/cpufreq/scaling_governor",
		dirent->d_name);
	file = fopen(filename, "w");
	if (!file)
	    continue;
	sprintf(node1->devinfo.activity, "GOVERNOR = ONDEMAND");
	sprintf(node1->devinfo.remark,
		"CPU FREQUENCY ACCORDING TO THE WORKLOAD");
	node1 = node1->next;
	sprintf(filename,
		"/sys/devices/system/cpu/%s/cpufreq/scaling_available_governors",
		dirent->d_name);
	file1 = fopen(filename, "r");
	if (!file1) {
	    return;
	}
	fscanf(file1, "%s", mode_options);

	fclose(file);
	sprintf(filename, "/sys/devices/system/cpu/%s/cpufreq/",
		dirent->d_name);
	int *cpu_freq = getCpuFreq(filename);

	if (strcmp(mode, "powersave") == 0) {
	    sprintf(filename, "/sys/devices/system/cpu/%s/cpufreq/",
		    dirent->d_name);
	    if (cpu_freq[1] == 0 || cpu_freq[0] == 0) {
		continue;
	    } else {
		sprintf(filename,
			"/sys/devices/system/cpu/%s/cpufreq/scaling_min_freq",
			dirent->d_name);
		file = fopen(filename, "w");
		if (!file) {
		    return;
		}
		fprintf(file, "%d\n", cpu_freq[0]);
		fclose(file);

		sprintf(filename,
			"/sys/devices/system/cpu/%s/cpufreq/scaling_max_freq",
			dirent->d_name);
		file = fopen(filename, "w");
		if (!file) {
		    return;
		}
		fprintf(file, "%d\n", cpu_freq[0]);
		fclose(file);
	    }
	} else if (strcmp(mode, "performance") == 0) {

	    sprintf(filename, "/sys/devices/system/cpu/%s/cpufreq/",
		    dirent->d_name);
	    if (cpu_freq[1] == 0 || cpu_freq[0] == 0) {
		continue;
	    } else {
		sprintf(filename,
			"/sys/devices/system/cpu/%s/cpufreq/scaling_max_freq",
			dirent->d_name);
		file = fopen(filename, "w");
		if (!file) {
		    return;
		}
		fprintf(file, "%d\n", cpu_freq[1]);
		fclose(file);

		sprintf(filename,
			"/sys/devices/system/cpu/%s/cpufreq/scaling_min_freq",
			dirent->d_name);
		file = fopen(filename, "w");
		if (!file) {
		    return;
		}
		fprintf(file, "%d\n", cpu_freq[1]);
		fclose(file);
	    }
	} else if (strcmp(mode, "normal") == 0) {
	    if (cpu_freq[1] == 0 || cpu_freq[0] == 0) {
		continue;
	    } else {
		sprintf(filename,
			"/sys/devices/system/cpu/%s/cpufreq/scaling_max_freq",
			dirent->d_name);
		file = fopen(filename, "w");
		if (!file) {
		    return;
		}
		fprintf(file, "%d",
			(int) (cpu_freq[0] +
			       (cpu_freq[1] - cpu_freq[0]) / 2));
		fclose(file);

		sprintf(filename,
			"/sys/devices/system/cpu/%s/cpufreq/scaling_min_freq",
			dirent->d_name);
		file = fopen(filename, "w");
		if (!file) {
		    return;
		}
		fprintf(file, "%d",
			(int) (cpu_freq[0] +
			       (cpu_freq[1] - cpu_freq[0]) / 2));
		fclose(file);
	    }
	} else {
	    sprintf(filename,
		    "/sys/devices/system/cpu/%s/cpufreq/scaling_governor",
		    dirent->d_name);
	    file = fopen(filename, "w");
	    if (!file) {
		return;
	    }
	    fprintf(file, "ondemand\n");
	    fclose(file);
	}


    }
    closedir(dir);
}

void cpuFreqAdjust(char mode[])
{
    DIR *dir;
    struct dirent *dirent;
    FILE *file;
    char filename[PATH_MAX];
    char line[1024];
    struct device_node *node1, *node2;
    char gov[1024];
    int ret = 0;
    node1 = getnode();
    ondemand = node1;
    node1->next = NULL;

    gov[0] = 0;


    dir = opendir("/sys/devices/system/cpu");
    if (!dir)
	return;

    while ((dirent = readdir(dir))) {
	if (dirent->d_name[0] == '.')
	    continue;
	sprintf(filename,
		"/sys/devices/system/cpu/%s/cpufreq/scaling_governor",
		dirent->d_name);
	file = fopen(filename, "r");
	if (!file)
	    continue;
	node2 = getnode();
	node1->next = node2;
	strcpy(node2->devinfo.devname, dirent->d_name);
	node2->next = NULL;
	node1 = node2;
	memset(line, 0, 1024);
	if (fgets(line, 1023, file) == NULL) {
	    fclose(file);
	    continue;
	}
	if (strlen(gov) == 0)
	    strcpy(gov, line);
	else
	    /* if the governors are inconsistent, warn */
	if (strcmp(gov, line))
	    ret = 1;
	fclose(file);
    }

    closedir(dir);
    if (strcmp(gov, mode) == 0) {
	return;
    } else {
	cpuOndemand(mode);
    }
    /* if the governor is set to userspace, also warn */
//      if (strstr(gov, "userspace"))
//              ret = 1;

//      if (strstr(gov, "performance"))
//              ret = 1;

//      if (ret) {
//                      cpuOndemand();
//      }
}

static int *getGpuFreq(char path[])
{
    int *freq;
    freq = (int *) malloc(1024);
    char filename[PATH_MAX];
    FILE *file;
    //read min freq
    sprintf(filename, "/sys/class/devfreq/gpufreq/available_frequencies");
    file = fopen(filename, "r");
    if (!file)
	return freq;

    int gggg[30] = { 0 };
    fseek(file, 0L, SEEK_SET);
    for (int i = 0; i < 30; i++) {
	fscanf(file, "%d", &gggg[i]);
	if (gggg[i] > gggg[i - 1])
	    freq[1] = gggg[i];

    }
    freq[0] = gggg[0];
    fclose(file);
    return freq;

}


void gpuFreqAdjust(char mode[])
{
    DIR *dir;
    struct dirent *dirent;
    FILE *file, *file1;
    char filename[PATH_MAX];
    char mode_options[PATH_MAX];

    dir = opendir("/sys/class/devfreq/gpufreq");
    if (!dir)
	return;

    while ((dirent = readdir(dir))) {
	if (dirent->d_name[0] == '.')
	    continue;

	sprintf(filename, "/sys/class/devfreq/gpufreq/");
	int *gpu_freq = getGpuFreq(filename);

	if (strcmp(mode, "powersave") == 0) {
	    if (gpu_freq[1] == 0 || gpu_freq[0] == 0) {
		continue;
	    } else {
		sprintf(filename, "/sys/class/devfreq/gpufreq/min_freq");
		file = fopen(filename, "w");
		if (!file) {
		    return;
		}
		fprintf(file, "%d\n", gpu_freq[0]);
		fclose(file);

		sprintf(filename, "/sys/class/devfreq/gpufreq/max_freq");
		file = fopen(filename, "w");
		if (!file) {
		    return;
		}
		fprintf(file, "%d\n", gpu_freq[0]);
		fclose(file);
	    }
	} else if (strcmp(mode, "performance") == 0) {

	    if (gpu_freq[1] == 0 || gpu_freq[0] == 0) {
		continue;
	    } else {
		printf("======%d\n", gpu_freq[1]);
		sprintf(filename, "/sys/class/devfreq/gpufreq/max_freq");
		file = fopen(filename, "w");
		if (!file) {
		    return;
		}
		fprintf(file, "%d\n", gpu_freq[1]);
		fclose(file);

		sprintf(filename, "/sys/class/devfreq/gpufreq/min_freq");
		file = fopen(filename, "w");
		if (!file) {
		    return;
		}
		fprintf(file, "%d\n", gpu_freq[1]);
		fclose(file);
	    }
	}
	break;
    }
    closedir(dir);
}

void pcieAdjust(char mode[])
{
    FILE *file;
    char filename[PATH_MAX];
    sprintf(filename, "/sys/module/pcie_aspm/parameters/policy");
    file = fopen(filename, "w");
    if (!file) {
	return;
    }
    printf("%s", mode);
    int ret = fprintf(file, "%s", mode);
    fclose(file);
}


bool is990()
{
    FILE *file;
    char filename[PATH_MAX];
    sprintf(filename, "/proc/cpuinfo");
    file = fopen(filename, "r");
    char str[1024] = { 0 };
    if (!file) {
	return false;
    }
    fseek(file, 0L, SEEK_SET);
    //for (int i=0;i<3000;i++){
    while (!feof(file)) {
	fscanf(file, "%s", str);
	if (strstr("HUAWEI", str)) {
	    return true;
	}
    }
    return false;
}


/*
int main(){
        
        //cpuFreqAdjust("performance");
        //gpuFreqAdjust("powersave");
	is990();
	//pcieAdjust("powersupersave");
	return 0;

}
*/
