## dbus 服务

```
用于提权

使用方法：
mkdir build
make -j8 
sudo make install  //这样会讲service文件安装到系统目录下
调用exitService接口退出 //或者重启，否则修改无法生效（注销也不生效）

```
