/*
 * Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/&gt;.
 *
 */
#include "policy_config.h"
#include <drm.h>
#include <xf86drm.h>
#include <unistd.h>
#include <QByteArray>
#include <QString>

//status：1电源2电池
//mode：1性能2省电
#define UNDEFINE 0
#define RADEON 1
#define AMDGPU 2
#define JM     3

enum MODE{INIT_MODE,PERF_MODE,SAVE_MODE};

bool get_drm_type_jm()
{
    int fd;
    fd = open("/dev/mwv206_0", O_RDWR, 0777);
    if( fd <= -1 ) {
        return false;
    }
    close(fd);

    return true;
}

/**
 * get_drm_type:
 * Return value: The type of drm. RADEON 1 AMDGPU 2 UNDEFINE 0.
 **/
int get_drm_type()
{
    int ret = UNDEFINE;
    drmVersion *ver = NULL;
    char *drm_file = "/dev/dri/card0";
    int fd;
    if((fd=open(drm_file,O_RDONLY)) >= 0)
    {
        printf("open drm_file ok!\n");
        ver = drmGetVersion(fd);
        close(fd);
        if(!ver)
            return UNDEFINE;
        printf("drmversion.version_major= %d\n",ver->version_major);
        printf("drmversion.version_minor = %d\n",ver->version_minor);
        printf("drmversion.name = %s\n",ver->name);
        if(!strcmp("amdgpu",ver->name))
        {
            printf("amdgpu\n");
            ret = AMDGPU;
        }
        else if(!strcmp("radeon",ver->name))
        {
            printf("radeon\n");
            ret = RADEON;
        }
        else {
            ret = UNDEFINE;
        }
    }
    if(ver!=NULL)
    {
        free(ver);
        ver = NULL;
    }

    if (ret == UNDEFINE)
    {
        if(get_drm_type_jm())
        {
            printf("JM DRM [DECTECTED]\n");
            ret = JM;
        }
    }
    return ret;
}

/**
 * set_jmgpu_mode:
 * @status: system is powered by battery or adapter.
 * @mode: the mode to selection. for example, 1 mean performance, 2 mean powersave.
 * Return value: the status of function execution.
 **/
int set_jmgpu_mode(int status,int mode)
{
	int fd;
	fd = open("/sys/class/mwv206_0/mwv206_0/power_mode", O_RDWR, 0777);

	if( fd <= -1 ) {
		printf("JJW open power_mode fail!\n");
		/* 打开power_mode  失败 */

		return -1;
	}

	if( mode == SAVE_MODE) {
		write(fd, "0", 2);		/* 省电模式 */
	} 
	else if ( mode == PERF_MODE) { 
		write(fd, "1", 2);		/* 性能模式 */
	}

	close(fd);

    	return 0;
}

/**
 * set_drm_mode:
 * @status: system is powered by battery or adapter.
 * @mode: the mode to selection. for example, 1 mean performance, 2 mean powersave.
 * Return value: the status of function execution.
 **/
int set_drm_mode(int status, int mode)
{
    static int drm_type = -1;
    if(drm_type == -1)
    {
        drm_type = get_drm_type();
    }
    if(drm_type == UNDEFINE)
        return -1;
    if(drm_type == RADEON)
        set_radeon_drm_mode(status,mode);
    else if(drm_type == AMDGPU)
        set_amdgpu_drm_mode(status,mode);
    else if(drm_type == JM)
        set_jmgpu_mode(status,mode);

    return 0;
}


/**
 * set_radeon_drm_mode:
 * @status: system is powered by battery or adapter.
 * @mode: the mode to selection. for example, 1 mean performance, 2 mean powersave.
 * Return value: the status of function execution.
 **/
int set_radeon_drm_mode(int status,int mode)
{
    printf("set_radeon_drm_mode---------\n");
    enum STATE{BOOT,PERFORMANCE};
    enum LEVEL{LOW,AUTO};

    char *level_array[]={
        "low",
        "auto"
    };
    char *state_array[]={
        "boot",
        "performance"
    };
    FILE *fp;

    char *power_dpm_force_performance_level = "/sys/class/drm/card0/device/power_dpm_force_performance_level";
    char *power_dpm_state = "/sys/class/drm/card0/device/power_dpm_state";

    //使用电源
    if(status == 1)
    {
        //性能模式
        if(mode == 1)
        {
            if(access(power_dpm_state,0) != -1)
            {
                if((fp=fopen(power_dpm_state,"w+")) != NULL)
                {
                    printf("open power_dpm_state ok!%s\n",state_array[PERFORMANCE]);
                    fprintf(fp,"%s",state_array[PERFORMANCE]);
                    fclose(fp);
                    fp = NULL;
                }
            }

            if(access(power_dpm_force_performance_level,0) != -1)
            {
                if((fp=fopen(power_dpm_force_performance_level,"w+")) != NULL)
                {
                    printf("open power_dpm_force_performance_level ok!%s\n",level_array[AUTO]);
                    fprintf(fp,"%s",level_array[AUTO]);
                    fclose(fp);
                    fp = NULL;
                }
            }
        }
        //省电模式
        else if(mode == 2)
        {
            if(access(power_dpm_state,0) != -1)
            {
                if((fp=fopen(power_dpm_state,"w+")) != NULL)
                {
                    printf("open power_dpm_state ok!%s\n",state_array[BOOT]);
                    fprintf(fp,"%s",state_array[BOOT]);
                    fclose(fp);
                    fp = NULL;
                }
            }

            if(access(power_dpm_force_performance_level,0) != -1)
            {
                if((fp=fopen(power_dpm_force_performance_level,"w+")) != NULL)
                {
                    printf("open power_dpm_force_performance_level ok!%s\n",level_array[LOW]);
                    fprintf(fp,"%s",level_array[LOW]);
                    fclose(fp);
                    fp = NULL;
                }
            }
        }
    }
    //使用电池
    else if(status == 2)
    {
        //性能模式
        if(mode == 1)
        {
            if(access(power_dpm_state,0) != -1)
            {
                if((fp=fopen(power_dpm_state,"w+")) != NULL)
                {
                    printf("open power_dpm_state ok!%s\n",state_array[PERFORMANCE]);
                    fprintf(fp,"%s",state_array[PERFORMANCE]);
                    fclose(fp);
                    fp = NULL;
                }
            }

            if(access(power_dpm_force_performance_level,0) != -1)
            {
                if((fp=fopen(power_dpm_force_performance_level,"w+")) != NULL)
                {
                    printf("open power_dpm_force_performance_level ok!%s\n",level_array[AUTO]);
                    fprintf(fp,"%s",level_array[AUTO]);
                    fclose(fp);
                    fp = NULL;
                }
            }
        }
        //省电模式
        else if(mode == 2)
        {
            if(access(power_dpm_state,0) != -1)
            {
                if((fp=fopen(power_dpm_state,"w+")) != NULL)
                {
                    printf("open power_dpm_state ok!%s\n",state_array[BOOT]);
                    fprintf(fp,"%s",state_array[BOOT]);
                    fclose(fp);
                    fp = NULL;
                }
            }

            if(access(power_dpm_force_performance_level,0) != -1)
            {
                if((fp=fopen(power_dpm_force_performance_level,"w+")) != NULL)
                {
                    printf("open power_dpm_force_performance_level ok!%s\n",level_array[LOW]);
                    fprintf(fp,"%s",level_array[LOW]);
                    fclose(fp);
                    fp = NULL;
                }
            }
        }
    }

    return 0;
}

/**
 * set_amdgpu_drm_mode:
 * @status: system is powered by battery or adapter.
 * @mode: the mode to selection. for example, 1 mean performance, 2 mean powersave.
 * Return value: the status of function execution.
 **/
int set_amdgpu_drm_mode(int status,int mode)
{
    printf("set_amdgpu_drm_mode---------\n");
    char mclk[MAX_PATH_LEN];
    char sclk[MAX_PATH_LEN];
    char level[MAX_PATH_LEN];
    char *level_array[]={
        "auto",
        "manual"
    };
    char *clk[]={
        "0",
        "1",
        "2",
        "3",
        "4",
        "5",
        "6",
        "7",
        "8",
        "9"
    };
    FILE *fp;

    memset(mclk,0,sizeof(mclk));
    sprintf(mclk,"%s","2");
    memset(sclk,0,sizeof(sclk));
    sprintf(sclk,"%s","7");

    memset(level,0,sizeof(level));
    sprintf(level,"%s","auto");

    char *pp_dpm_mclk = "/sys/class/drm/card0/device/pp_dpm_mclk";
    char *pp_dpm_sclk = "/sys/class/drm/card0/device/pp_dpm_sclk";
    char *power_dpm_force_performance_level = "/sys/class/drm/card0/device/power_dpm_force_performance_level";

//    if(status == 1)
//    {
//        memset(level,0,sizeof(level));
//        sprintf(level,"%s","auto");
//    }
//    if(status==2)
//    {

//        if(mode == 1)
//        {
//            memset(level,0,sizeof(level));
//            sprintf(level,"%s","high");

//        }
//        if(mode == 2)
//        {
//            memset(level,0,sizeof(level));
//            sprintf(level,"%s","manual");
//            memset(mclk,0,sizeof(mclk));
//            sprintf(mclk,"%s","0");//main place to set notice!!
//            memset(sclk,0,sizeof(sclk));
//            sprintf(sclk,"%s","0");//main place to set notice!!

//        }
//    }
    //不区分电池和电源，性能用auto，省电用一样配置
    if(mode == 1)
    {
        memset(level,0,sizeof(level));
        sprintf(level,"%s","auto");

    }
    if(mode == 2)
    {
        memset(level,0,sizeof(level));
        sprintf(level,"%s","manual");
        memset(mclk,0,sizeof(mclk));
        sprintf(mclk,"%s","0");//main place to set notice!!
        memset(sclk,0,sizeof(sclk));
        sprintf(sclk,"%s","0");//main place to set notice!!

    }

    if(access(power_dpm_force_performance_level,0) != -1)
    {
        if((fp=fopen(power_dpm_force_performance_level,"w+")) != NULL)
        {
            printf("open power_dpm_force_performance_level ok!%s\n",level);
            fprintf(fp,"%s",level);
            fclose(fp);
            fp = NULL;
        }
    }

    if(mode == 2)
    {
        if(access(pp_dpm_mclk,0) != -1)
        {
            if((fp=fopen(pp_dpm_mclk,"w+")) != NULL)
            {
                printf("open pp_dpm_mclk ok!%s\n",mclk);
                fprintf(fp,"%s",mclk);
                fclose(fp);
                fp = NULL;
            }
        }

        if(access(pp_dpm_sclk,0) != -1)
        {
            if((fp=fopen(pp_dpm_sclk,"w+")) != NULL)
            {
                printf("open pp_dpm_sclk ok!%s\n",sclk);
                fprintf(fp,"%s",sclk);
                fclose(fp);
                fp = NULL;
            }
        }
    }
    return 0;
}

/**
 * set_pcie_aspm_mode:
 * @status: system is powered by battery or adapter.
 * @mode: the mode to selection. for example, 1 mean performance, 2 mean powersave.
 * Return value: the status of function execution.
 **/
int set_pcie_aspm_mode(int status,int mode)
{
    char pcie_policy[MAX_PATH_LEN];
    FILE *fp;

    memset(pcie_policy,0,sizeof(pcie_policy));
    sprintf(pcie_policy,"%s","performance");

    char *pcie_aspm = "/sys/module/pcie_aspm/parameters/policy";

//    if(status == 1)
//    {
//        memset(pcie_policy,0,sizeof(pcie_policy));
//        sprintf(pcie_policy,"%s","performance");
//    }
//    if(mode == 2 && status==2)
//    {
//        memset(pcie_policy,0,sizeof(pcie_policy));
//        sprintf(pcie_policy,"%s","powersave");
//    }

    if(mode == 1)
    {
        memset(pcie_policy,0,sizeof(pcie_policy));
        sprintf(pcie_policy,"%s","performance");
    }
    if(mode == 2)
    {
        memset(pcie_policy,0,sizeof(pcie_policy));
        sprintf(pcie_policy,"%s","powersave");
    }

    if(access(pcie_aspm,0) != -1)
    {
        if((fp=fopen(pcie_aspm,"w+")) != NULL)
        {
            printf("open pcie aspm ok!%s\n",pcie_policy);
            fprintf(fp,"%s",pcie_policy);
            fclose(fp);
            fp = NULL;
        }

    }

    return 0;
}

/**
 * set_audio_mode:
 * @status: system is powered by battery or adapter.
 * @mode: the mode to selection. for example, 1 mean performance, 2 mean powersave.
 * Return value: the status of function execution.
 **/
int set_audio_mode(int status,int mode)
{
    char audio_policy[MAX_PATH_LEN];
    FILE *fp;

    memset(audio_policy,0,sizeof(audio_policy));
    sprintf(audio_policy,"%s","on");

    char *audio = "/sys/devices/platform/FTHD0001:00/hdaudioC1D0/power/control";

//    if(status == 1)
//    {
//        memset(audio_policy,0,sizeof(audio_policy));
//        sprintf(audio_policy,"%s","on");
//    }
//    if(mode == 2 && status==2)
//    {
//        memset(audio_policy,0,sizeof(audio_policy));
//        sprintf(audio_policy,"%s","auto");
//    }
    if(mode == 1)
    {
        memset(audio_policy,0,sizeof(audio_policy));
        sprintf(audio_policy,"%s","on");
    }
    if(mode == 2)
    {
        memset(audio_policy,0,sizeof(audio_policy));
        sprintf(audio_policy,"%s","auto");
    }

    if(access(audio,0) != -1)
    {
        if((fp=fopen(audio,"w+")) != NULL)
        {
            printf("open pcie aspm ok!%s\n",audio_policy);
            fprintf(fp,"%s",audio_policy);
            fclose(fp);
            fp = NULL;
        }

    }
    return 0;
}

/**
 * list_all:
 * @dirname: the file folder name.
 **/
void list_all(char *dirname)
{
 assert(dirname != NULL);

 char path[512];
 struct dirent *filename;//readdir 的返回类型
 DIR *dir;//

 dir = opendir(dirname);
 if(dir == NULL)
 {
  printf("open dir %s error!\n",dirname);
  exit(1);
 }

 while((filename = readdir(dir)) != NULL)
 {
  if(!strcmp(filename->d_name,".")||!strcmp(filename->d_name,".."))
   continue;

  sprintf(path,"%s/%s",dirname,filename->d_name);

//  printf("%d. %s\n",++count,filename->d_name);
  printf("%s\n",path);

 }
 closedir(dir);
}

/**
 * set_cpu_scaling_gover_speed_mode:
 * @status: system is powered by battery or adapter.
 * @mode: the mode to selection. for example, 1 mean performance, 2 mean powersave.
 * Return value: the status of function execution.
 **/
int set_cpu_scaling_gover_speed_mode(int status,int mode)
{
    char scaling_governor[MAX_PATH_LEN];
    FILE *fp;

    memset(scaling_governor,0,sizeof(scaling_governor));
    sprintf(scaling_governor,"%s","performance");

    char *scaling_dir = "/sys/devices/system/cpu/cpufreq";
/**注释掉，允许ac选择两种模式**/
//    if(status == 1)
//    {
//        memset(scaling_governor,0,sizeof(scaling_governor));
//        sprintf(scaling_governor,"%s","performance");
//        printf("using AC--------\n");
//    }
//    if(mode == 2 && status==2)
//    {
//        printf("using BATTERY--------POWERSAVE\n");
//        memset(scaling_governor,0,sizeof(scaling_governor));
//        sprintf(scaling_governor,"%s","ondemand");
//    }
//    else if(mode == 1 && status==2)
//    {
//        printf("using BATTERY--------PERFORMANCE\n");
//    }
    if(mode == 1)
    {
        memset(scaling_governor,0,sizeof(scaling_governor));
        sprintf(scaling_governor,"%s","performance");
        printf("using --------performance\n");
    }
    if(mode == 2)
    {
        printf("using --------POWERSAVE\n");
        memset(scaling_governor,0,sizeof(scaling_governor));
//        sprintf(scaling_governor,"%s","ondemand");
        sprintf(scaling_governor,"%s","powersave");
    }

    char path[512];
    struct dirent *filename;//readdir 的返回类型
    DIR *dir;

    dir = opendir(scaling_dir);
    if(dir == NULL)
    {
     printf("open dir %s error!\n",scaling_dir);
     return -1;
    }

    while((filename = readdir(dir)) != NULL)
    {
     if(!strcmp(filename->d_name,".")||!strcmp(filename->d_name,".."))
      continue;

     sprintf(path,"%s/%s/%s",scaling_dir,filename->d_name,"scaling_governor");

   //  printf("%d. %s\n",++count,filename->d_name);
//     printf("%s\n",path);
     if((fp=fopen(path,"w+")) != NULL)
     {
         printf("open cpu_scaling ok!%s\n",scaling_governor);
         fprintf(fp,"%s",scaling_governor);
         fclose(fp);
         fp = NULL;
     }
    }
    closedir(dir);
    return 0;
}

/**
 * set_sata_alsm_mode:
 * @status: system is powered by battery or adapter.
 * @mode: the mode to selection. for example, 1 mean performance, 2 mean powersave.
 * Return value: the status of function execution.
 **/
int set_sata_alsm_mode(int status,int mode)
{
    char alsm_policy[MAX_PATH_LEN];
    FILE *fp;

    memset(alsm_policy,0,sizeof(alsm_policy));
    sprintf(alsm_policy,"%s","max_performance");

    char *sata_dir = "/sys/class/scsi_host";

//    if(status == 1)
//    {
//        memset(alsm_policy,0,sizeof(alsm_policy));
//        sprintf(alsm_policy,"%s","max_performance");
//    }
//    if(mode == 2 && status==2)
//    {
//        memset(alsm_policy,0,sizeof(alsm_policy));
//        sprintf(alsm_policy,"%s","min_power");
//    }
    if(mode == 1)
    {
        memset(alsm_policy,0,sizeof(alsm_policy));
        sprintf(alsm_policy,"%s","max_performance");
    }
    if(mode == 2)
    {
        memset(alsm_policy,0,sizeof(alsm_policy));
        sprintf(alsm_policy,"%s","min_power");
    }

    char path[512];
    struct dirent *filename;//readdir 的返回类型
    DIR *dir;

    dir = opendir(sata_dir);
    if(dir == NULL)
    {
     printf("open dir %s error!\n",sata_dir);
     return -1;
    }

    while((filename = readdir(dir)) != NULL)
    {
     if(!strcmp(filename->d_name,".")||!strcmp(filename->d_name,".."))
      continue;

     sprintf(path,"%s/%s/%s",sata_dir,filename->d_name,"link_power_management_policy");

     if((fp=fopen(path,"w+")) != NULL)
     {
         printf("open cpu_scaling ok!%s\n",alsm_policy);
         fprintf(fp,"%s",alsm_policy);
         fclose(fp);
         fp = NULL;
     }
    }
    closedir(dir);
    return 0;
}

