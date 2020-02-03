#!/bin/bash

power_status=$1
power_mode=$2

if [ $# -ne 2 ];
then
	echo "args error"
	exit
fi

#echo ${power_status}
#echo ${power_mode}

hosts=`ls /sys/class/scsi_host/`
policys=`ls /sys/devices/system/cpu/cpufreq/`
cards=`ls /sys/class/drm/`

function cpu_scaling_gover_speed()
{
	scaling_governor="performance"

	if [ ${power_status} == "AC" ];
	then
		scaling_governor="performance"
	fi

	if [ ${power_status} == "BAT" -a ${power_mode} == "POWERSAVE" ];
	then
		scaling_governor="ondemand"

	fi

	for policy in ${policys};
	do
		if [ -f "/sys/devices/system/cpu/cpufreq/${policy}/scaling_governor" ];
		then
			echo ${scaling_governor} > /sys/devices/system/cpu/cpufreq/${policy}/scaling_governor
		fi
	done
}

function drm_set_mode()
{
	state="performance"
	level="auto"
	if [ ${power_status} == "AC" ];
	then
		state="performance"
	fi

	if [ ${power_status} == "BAT" -a ${power_mode} == "POWERSAVE" ];
	then
		state="boot"
		level="low"
	fi
	
	if [ -f "/sys/class/drm/card0/device/power_dpm_state" ];
	then
		echo ${state} > /sys/class/drm/card0/device/power_dpm_state
	fi

	if [ -f "/sys/class/drm/card0/device/power_dpm_force_performance_level" ];
	then
		echo ${level} > /sys/class/drm/card0/device/power_dpm_force_performance_level
	fi
}

function pcie_aspm_set_mode()
{
	pcie_policy="performance"
	if [ ${power_status} == "AC" ];
	then
		pcie_policy="performance"
	fi
	
	if [ ${power_status} == "BAT" -a ${power_mode} == "POWERSAVE" ];
	then
		pcie_policy="powersave"
	fi
	if [ -f "/sys/module/pcie_aspm/parameters/policy" ];
	then
		echo ${pcie_policy} > /sys/module/pcie_aspm/parameters/policy
	fi	
}

function sata_alsm_set()
{
        alsm_policy="max_performance"
        if [ ${power_status} == "AC" ];
        then
                alsm_policy="max_performance"
        fi

        if [ ${power_status} == "BAT" -a ${power_mode} == "POWERSAVE" ];
        then
                alsm_policy="min_power"
        fi

        for host in ${hosts};
        do
                if [ -f "/sys/class/scsi_host/${host}/link_power_management_policy" ];
                then
                        echo ${alsm_policy} > /sys/class/scsi_host/${host}/link_power_management_policy
                fi
        done
}

function audio_set_mode()
{
	audio_policy="on"
	if [ ${power_status} == "AC" ];
        then
                audio_policy="on"
        fi

        if [ ${power_status} == "BAT" -a ${power_mode} == "POWERSAVE" ];
        then
                audio_policy="auto"
        fi

	if [ -f "/sys/devices/platform/FTHD0001:00/hdaudioC1D0/power/control" ];
	then
		echo ${audio_policy} > /sys/devices/platform/FTHD0001:00/hdaudioC1D0/power/control
#		cat /sys/devices/platform/FTHD0001:00/hdaudioC1D0/power/control
	fi
}

cpu_scaling_gover_speed
drm_set_mode
pcie_aspm_set_mode
sata_alsm_set
audio_set_mode
