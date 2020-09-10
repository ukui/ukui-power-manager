//
// Created by kylin on 2020/9/4.
//

#ifndef POWER_SAVE_MISC_H
#define POWER_SAVE_MISC_H
void nmi_watchdog_off(void);
void recommend_nmi_watchdog(void);
void ac97_power_on(void);
void recommend_ac97_powersave(void);
void hda_power_on(void);
void recommend_hda_powersave(void);
void noatime_on(void);
void recommend_noatime(void);
void writeback_long(int time);
void recommend_writeback_time(int time);
void laptop_mode_on(int mode);
void recommend_laptop_mode(int mode);
void pcie_aspm_save(void);
void recommend_pcie_aspm_save(void);
#endif //POWER_SAVE_MISC_H
