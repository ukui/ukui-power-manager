#ifndef HUAWEI_H
#define HUAWEI_H

#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>

void cpuFreqAdjust(char mode[]);
void gpuFreqAdjust(char mode[]);
bool is990();
void pcieAdjust(char mode[]);

#endif				// HUAWEI_H
