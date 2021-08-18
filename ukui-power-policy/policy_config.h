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
int set_drm_mode(int status,int mode);
int set_pcie_aspm_mode(int status,int mode);
int set_audio_mode(int status,int mode);
int set_cpu_scaling_gover_speed_mode(int status,int mode);
int set_sata_alsm_mode(int status,int mode);
int get_drm_type();
int set_amdgpu_drm_mode(int status,int mode);
int set_radeon_drm_mode(int status,int mode);
int get_drm_type();


#endif // POLICY_CONFIG_H
