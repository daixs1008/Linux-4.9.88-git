=>printenv
baudrate=115200
board_name=EVK
board_rev=14X14
boot_fdt=try

bootcmd=run updateset;
run findfdt;
run findtee;
mmc dev ${mmcdev};
mmc dev ${mmcdev}; 
if mmc rescan; then 
	if run loadbootscript; then run bootscript; 
		else if run loadimage; then run mmcboot; 
		else run netboot; 
			fi; 
	fi; 
else run netboot; 
fi

bootcmd_mfg=
run mfgtool_args; 
if test ${tee} = yes; then bootm ${tee_addr} ${initrd_addr} ${fdt_addr}; 
else bootz ${loadaddr} ${initrd_addr} ${fdt_addr};
 fi;
bootdelay=3
bootdir=/boot
bootscript=echo Running bootscript from mmc ...; source
console=ttymxc0
eth1addr=00:01:3f:2d:3e:4d
ethact=ethernet@020b4000
ethaddr=00:01:1f:2d:3e:4d
ethprime=eth1
fdt_addr=0x83000000
fdt_file=100ask_imx6ull-14x14.dtb
fdt_high=0xffffffff
fdtcontroladdr=9ef40478
findfdt=
if test $fdt_file = undefined; then 
	if test $board_name = EVK && test $board_rev = 9X9; then setenv fdt_file imx6ull-9x9-evk.dtb; 
	fi; 
	if test $board_name = EVK && test $board_rev = 14X14; then setenv fdt_file imx6ull-14x14-evk.dtb; 
	fi; 
	if test $fdt_file = undefined; then setenv fdt_file imx6ull-14x14-alpha.dtb; 
	fi; 
fi;
image=zImage
initrd_addr=0x83800000
initrd_high=0xffffffff
ip_dyn=yes
loadaddr=0x80800000
loadbootscript=fatload mmc ${mmcdev}:${mmcpart} ${loadaddr} ${script};
//loadbootscript=fatload mmc 1:2 0x80800000 boot.scr
loadfdt=ext2load mmc ${mmcdev}:${mmcpart} ${fdt_addr} ${bootdir}/${fdt_file}
//loadfdt=ext2load mmc 1:2 0x83000000 /boot/100ask_imx6ull-14x14.dtb
loadimage=ext2load mmc ${mmcdev}:${mmcpart} ${loadaddr} ${bootdir}/${image}
//loadimage=ext2load mmc  1:2 0x80800000 /boot/zImage
loadtee=fatload mmc ${mmcdev}:${mmcpart} ${tee_addr} ${tee_file}
//loadtee=fatload mmc  1:2  0x84000000  uTee-6ullevk

mfgtool_args=setenv bootargs console=${console},${baudrate} 
//mfgtool_args=setenv bootargs console=ttymxc0,115200

rdinit=/linuxrc g_mass_storage.stall=0 
g_mass_storage.removable=1 
g_mass_storage.file=/fat g_mass_storage.ro=1 
g_mass_storage.idVendor=0x066F 
g_mass_storage.idProduct=0x37FF 
g_mass_storage.iSerialNumber="" 
clk_ignore_unused

mmcargs=setenv bootargs console=${console},${baudrate} root=${mmcroot}
//mmcargs=setenv bootargs console==ttymxc0,115200  root=/dev/mmcblk1p2 rootwait rw
mmcautodetect=no

mmcboot=echo Booting from mmc ...; run mmcargs; 
if test ${tee} = yes; then run loadfdt; run loadtee; bootm ${tee_addr} - ${fdt_addr}; 
else if test ${boot_fdt} = yes || test ${boot_fdt} = try;
	 then if run loadfdt; then bootz ${loadaddr} - ${fdt_addr}; 
		else if test ${boot_fdt} = try; then bootz; 
		else echo WARN: Cannot load the DT;
			fi; 
		fi; 
else bootz; 
	fi;
 fi;
 
 mmcboot=echo Booting from mmc ...; run setenv bootargs console==ttymxc0,115200  root=/dev/mmcblk1p2 rootwait rw;run loadfdt; then bootz 0x80800000 -  0x83000000; 
 -->  

/*
mmcboot=echo Booting from mmc ...; run mmcargs; 
if test no = yes; then run loadfdt; run loadtee; bootm ${tee_addr} - ${fdt_addr}; 
else if test try = yes || test try= try;
	 then if run loadfdt; then bootz 0x80800000 -  0x83000000; 
		else if test try = try; then bootz; 
		else echo WARN: Cannot load the DT;
			fi; 
		fi; 
else bootz; 
	fi;
 fi;
*/

mmcdev=1
mmcpart=2
mmcroot=/dev/mmcblk1p2 rootwait rw
netargs=setenv bootargs console=${console},${baudrate} root=/dev/nfs ip=dhcp nfsroot=${serverip}:${nfsroot},v3,tcp
netboot=echo Booting from net ...; run netargs; setenv get_cmd tftp; ${get_cmd} ${image}; ${get_cmd} ${fdt_addr} ${fdt_file};  bootz ${loadaddr} - ${fdt_addr};
//netboot=echo Booting from net ...; run netargs; setenv get_cmd tftp; tftp  zImage; tftp 0x83000000 100ask_imx6ull-14x14.dtb; bootz  0x80800000 -  0x83000000
panel=TFT7016
script=boot.scr
tee=no
tee_addr=0x84000000
tee_file=uTee-6ullevk
update=yes
updateset=if test $update = undefined; then setenv update yes; saveenv; fi;

Environment size: 2765/8188 bytes



分析启动参数：
使用网络启动时执行的命令解析：setenv serverip 192.168.2.69;setenv ipaddr 192.168.2.232;setenv nfsroot /home/book/nfs_rootfs;run netboot
前面三个  setenv  设置三个环境变量
最后一个  run netboot  为  run  echo Booting from net ...; run netargs; setenv get_cmd tftp; ${get_cmd} ${image}; ${get_cmd} ${fdt_addr} ${fdt_file};  bootz ${loadaddr} - ${fdt_addr};  
echo Booting from net ...; //直接输出内容
run netargs;    /* run   setenv bootargs console=ttymxc0,115200 root=/dev/nfs ip=dhcp nfsroot=192.168.2.69:/home/book/nfs_rootfs,v3,tcp*/
setenv get_cmd tftp;   // get_cmd <- tftp 
${get_cmd} ${image};  // tftp zImage  //这里为什么不是  ${get_cmd}  ${loadaddr}  ${image}-->  tftp 0x80800000  zImage  
${get_cmd} ${fdt_addr} ${fdt_file};  // tftp  0x83000000  100ask_imx6ull-14x14.dtb
 bootz ${loadaddr} - ${fdt_addr}; // bootz  0x80800000 -  0x83000000   最后启动命令 传给内核俩个地址： 
																		//内核地址：0x80800000    设备树地址   0x83000000



Welcome to imx6ull buildroot system !

* SourceCode:     https://gitee.com/weidongshan
* Support:        https://support@100ask.net
* Shop：          https://100ask.taobao.com/
* Login:          name: root  Passwd:
100ask login: root

[root@100ask:~]# reboot
[root@100ask:~]# killall: mxapp2: no process killed
Stopping swupdate: Trying to connect to SWUpdate...
Connected to SWUpdate via /tmp/swupdateprog
Connection closing..
Trying to connect to SWUpdate...
Connection closing..
Trying to connect to SWUpdate...
OK
Stopping telnetd: OK
Stopping sshd: OK
Stopping pulseaudio: E: [pulseaudio] main.c: Failed to kill daemon: No such process
OK
Stopping mosquitto: OK
Stopping ntpd: OK
Stopping NetworkManager ... done.
Stopping ModemManager: OK
Stopping network: OK
Stopping system message bus: done
Saving random seed: OK
[  127.260646] ds18b20_drv_exit OK.
[  127.277800] dht11_drv_exit OK.
[  127.304531] irda_drv_exit OK.
[  127.319949] ------------[ cut here ]------------
[  127.325137] WARNING: CPU: 0 PID: 405 at drivers/base/driver.c:191 driver_unregister+0x54/0x58
[  127.336197] ---[ end trace fd0f46b52c69bd93 ]---
Stopping lvgl: OK
Stopping klogd: OK
Stopping syslogd: OK
umount: tmpfs busy - remounted read-only
umount: devtmpfs busy - remounted read-only
[  127.875132] EXT4-fs (mmcblk1p2): re-mounted. Opts: (null)
The system is going down NOW!
logout
Sent SIGTERM to all processes
Sent SIGKILL to all processes
Requesting system reboot
[  129.934986] goodix-ts 1-005d: Regulator vcc_i2c disabled
[  129.940389] goodix-ts 1-005d: Regulator vdd_ana disabled
[  129.946788] ci_hdrc ci_hdrc.1: remove, state 1
[  129.951381] usb usb1: USB disconnect, device number 1
[  129.956849] usb 1-1: USB disconnect, device number 2
[  129.961894] usb 1-1.1: USB disconnect, device number 3
[  130.273033] ci_hdrc ci_hdrc.1: USB bus 1 deregistered
[  130.280813] reboot: Restarting system


//系统启动
U-Boot 2017.03 (May 11 2021 - 09:15:14 +0000)

CPU:   Freescale i.MX6ULL rev1.1 696 MHz (running at 396 MHz)
CPU:   Industrial temperature grade (-40C to 105C) at 47C
Reset cause: WDOG
Model: Freescale i.MX6 ULL 14x14 EVK Board
Board: MX6ULL 14x14 EVK
DRAM:  512 MiB
MMC:   FSL_SDHC: 0, FSL_SDHC: 1
Display: TFT7016 (1024x600)
Video: 1024x600x24
In:    serial
Out:   serial
Err:   serial
Net:   eth1: ethernet@020b4000 [PRIME], eth0: ethernet@02188000
Normal Boot
Hit any key to stop autoboot:  0
=> setenv serverip 192.168.2.69;setenv ipaddr 192.168.2.232;setenv nfsroot /home/book/nfs_rootfs;run netboot


//echo Booting from net ...;  运行结果

Booting from net ...

//tftp zImage   //运行结果 

ethernet@020b4000 Waiting for PHY auto negotiation to complete.... done
Using ethernet@020b4000 device
TFTP from server 192.168.2.69; our IP address is 192.168.2.232
Filename 'zImage'.
Load address: 0x80800000
Loading: #################################################################
         #################################################################
         #################################################################
         #################################################################
         #################################################################
         #################################################################
         #################################################################
         #################################################################
         #################################################################
         ##################
         996.1 KiB/s
done
Bytes transferred = 8846608 (86fd10 hex)
Using ethernet@020b4000 device

//tftp  0x83000000  100ask_imx6ull-14x14.dtb    //运行结果

TFTP from server 192.168.2.69; our IP address is 192.168.2.232
Filename '100ask_imx6ull-14x14.dtb'.
Load address: 0x83000000
Loading: ###
         1000 KiB/s
done
Bytes transferred = 38928 (9810 hex)
Kernel image @ 0x80800000 [ 0x000000 - 0x86fd10 ]
## Flattened Device Tree blob at 83000000
   Booting using the fdt blob at 0x83000000
   Using Device Tree in place at 83000000, end 8300c80f
Modify /soc/aips-bus@02200000/epdc@0228c000:status disabled
ft_system_setup for mx6


//bootz  0x80800000 -  0x83000000  运行后。启动内核

Starting kernel ...











