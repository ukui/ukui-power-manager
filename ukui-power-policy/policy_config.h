#ifndef POLICY_CONFIG_H
#define POLICY_CONFIG_H

#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>

#define MAX_PATH_LEN 128
int set_drm_mode(int status, int mode);
int set_pcie_aspm_mode(int status, int mode);
int set_audio_mode(int status, int mode);
int set_cpu_scaling_gover_speed_mode(int status, int mode);
int set_sata_alsm_mode(int status, int mode);
int get_drm_type();
int set_amdgpu_drm_mode(int status, int mode);
int set_radeon_drm_mode(int status, int mode);
int get_drm_type();
void cpuFreqAdjust(char mode[]);
void gpuFreqAdjust(char mode[]);
bool is990();
void pcieAdjust(char mode[]);

#endif				// POLICY_CONFIG_H
