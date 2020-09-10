针对Linux 电源管理省电模式做的优化，主要有以下几方面（边开发边增加，主要是对标TLP做的一个C/C++改写）
- CPU频率：选择不同的模式调整相应的CPU频率，其模式大概有五种，已在本人博客中列出，具体可以点击下方链接查看。

- USB Auto Suspend:内核2.6.3之后，默认将power/control文件中的值设为auto，所以说实际上开发人员再来编写自动挂载已经意义不大。

- Misc:添加NMI_Watchdog改变、声卡power save、以及根挂载noatime和脏页写回时间、PCIe ASPM

- IOSchedulers: 添加磁盘的io调度顺序改变

- disable WOL:disable wake on lan

- wifi power save:添加wifi省电

- bluetooth power save:蓝牙空闲时省电

[My Site](https://liyiping.cn)
