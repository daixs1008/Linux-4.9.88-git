

1.-U-Boot 2017.03 (May 11 2021 - 09:15:14 +0000)

2.-CPU:   Freescale i.MX6ULL rev1.1 696 MHz (running at 396 MHz)
3.-CPU:   Industrial temperature grade (-40C to 105C) at 37C
Reset cause: POR
Model: Freescale i.MX6 ULL 14x14 EVK Board
Board: MX6ULL 14x14 EVK
DRAM:  512 MiB
MMC:   FSL_SDHC: 0, FSL_SDHC: 1
Display: TFT7016 (1024x600)
Video: 1024x600x24
In:    serial
Out:   serial
Err:   serial
Net:   No ethernet found.
Normal Boot
Hit any key to stop autoboot:  0
## Error: "findtee" not defined
switch to partitions #0, OK
mmc1(part 0) is current device
switch to partitions #0, OK
mmc1(part 0) is current device
** Unrecognized filesystem type **
7924928 bytes read in 408 ms (18.5 MiB/s)
Booting from mmc ...
38370 bytes read in 51 ms (734.4 KiB/s)
Kernel image @ 0x80800000 [ 0x000000 - 0x78ecc0 ]
## Flattened Device Tree blob at 83000000
   Booting using the fdt blob at 0x83000000
   Using Device Tree in place at 83000000, end 8300c5e1
Modify /soc/aips-bus@02200000/epdc@0228c000:status disabled
ft_system_setup for mx6

Starting kernel ...

[    0.000000] 1--Booting Linux on physical CPU 0x0
[    0.000000] 2--Linux version 4.9.88 (book@virtual-machine) (gcc version 7.5.0 (Buildroot 2020.02-g6a1d083-dirty) ) #1 SMP PREEMPT Tue May 11 05:22:35 UTC 2021
[    0.000000] 3--CPU: ARMv7 Processor [410fc075] revision 5 (ARMv7), cr=10c53c7d
[    0.000000] 4--CPU: div instructions available: patching division code
[    0.000000] 5--CPU: PIPT / VIPT nonaliasing data cache, VIPT aliasing instruction cache
[    0.000000] 6--OF: fdt:Machine model: Freescale i.MX6 ULL 14x14 EVK Board
[    0.000000] 7--Reserved memory: created CMA memory pool at 0x8c000000, size 320 MiB
[    0.000000] 8--OF: reserved mem: initialized node linux,cma, compatible id shared-dma-pool
[    0.000000] 9--Memory policy: Data cache writealloc
[    0.000000] 10--percpu: Embedded 15 pages/cpu @8bb30000 s30156 r8192 d23092 u61440
[    0.000000] 11--Built 1 zonelists in Zone order, mobility grouping on.  Total pages: 130048
[    0.000000] 12--Kernel command line: console=ttymxc0,115200 root=/dev/mmcblk1p2 rootwait rw
[    0.000000] 13--PID hash table entries: 2048 (order: 1, 8192 bytes)
[    0.000000] 14--Dentry cache hash table entries: 65536 (order: 6, 262144 bytes)
[    0.000000] 15--Inode-cache hash table entries: 32768 (order: 5, 131072 bytes)
[    0.000000] 16--Memory: 172244K/524288K available (11264K kernel code, 1103K rwdata, 4040K rodata, 1024K init, 477K bss, 24364K reserved, 327680K cma-reserved, 0K highmem)
[    0.000000] 17--Virtual kernel memory layout:
[    0.000000]     vector  : 0xffff0000 - 0xffff1000   (   4 kB)
[    0.000000]     fixmap  : 0xffc00000 - 0xfff00000   (3072 kB)
[    0.000000]     vmalloc : 0xa0800000 - 0xff800000   (1520 MB)
[    0.000000]     lowmem  : 0x80000000 - 0xa0000000   ( 512 MB)
[    0.000000]     pkmap   : 0x7fe00000 - 0x80000000   (   2 MB)
[    0.000000]     modules : 0x7f000000 - 0x7fe00000   (  14 MB)
[    0.000000]       .text : 0x80008000 - 0x80c00000   (12256 kB)
[    0.000000]       .init : 0x81100000 - 0x81200000   (1024 kB)
[    0.000000]       .data : 0x81200000 - 0x81313c70   (1104 kB)
[    0.000000]        .bss : 0x81315000 - 0x8138c6d8   ( 478 kB)
[    0.000000] 18--SLUB: HWalign=64, Order=0-3, MinObjects=0, CPUs=1, Nodes=1
[    0.000000] 19--Preemptible hierarchical RCU implementation.
[    0.000000] 20--Build-time adjustment of leaf fanout to 32.
[    0.000000] 21--RCU restricting CPUs from NR_CPUS=4 to nr_cpu_ids=1.
[    0.000000] 22--RCU: Adjusting geometry for rcu_fanout_leaf=32, nr_cpu_ids=1
[    0.000000] 23--NR_IRQS:16 nr_irqs:16 16
[    0.000000] 24--Switching to timer-based delay loop, resolution 333ns
[    0.000018] 25--sched_clock: 32 bits at 3000kHz, resolution 333ns, wraps every 715827882841ns
[    0.000067] 26--clocksource: mxc_timer1: mask: 0xffffffff max_cycles: 0xffffffff, max_idle_ns: 637086815595 ns
[    0.004272] 27--Console: colour dummy device 80x30
[    0.004327] 28--Calibrating delay loop (skipped), value calculated using timer frequency.. 6.00 BogoMIPS (lpj=30000)
[    0.004370] 29--pid_max: default: 32768 minimum: 301
[    0.004622] 30--Mount-cache hash table entries: 1024 (order: 0, 4096 bytes)
[    0.004656] 31--Mountpoint-cache hash table entries: 1024 (order: 0, 4096 bytes)
[    0.006340] 32--CPU: Testing write buffer coherency: ok
[    0.006444] 33--ftrace: allocating 35227 entries in 104 pages
[    0.140374] 34--/cpus/cpu@0 missing clock-frequency property
[    0.140427] 35--CPU0: thread -1, cpu 0, socket 0, mpidr 80000000
[    0.140501] 36--Setting up static identity map for 0x80100000 - 0x80100058
[    0.229177] 37--Brought up 1 CPUs
[    0.229217] 38--SMP: Total of 1 processors activated (6.00 BogoMIPS).
[    0.229241] 39--CPU: All CPU(s) started in SVC mode.
[    0.230822] 40--devtmpfs: initialized
[    0.248827] 41--OF: Duplicate name in lcdif@021c8000, renamed to "display#1"
[    0.255061] 42--VFP support v0.3: implementor 41 architecture 2 part 30 variant 7 rev 5
[    0.255918] 43--clocksource: jiffies: mask: 0xffffffff max_cycles: 0xffffffff, max_idle_ns: 19112604462750000 ns
[    0.255977] 44--futex hash table entries: 256 (order: 2, 16384 bytes)
[    0.282408] 45--pinctrl core: initialized pinctrl subsystem
[    0.285095] 46--NET: Registered protocol family 16
[    0.305051] 47--DMA: preallocated 256 KiB pool for atomic coherent allocations
[    0.331276] 48--cpuidle: using governor menu
[    0.390404] 49--hw-breakpoint: found 5 (+1 reserved) breakpoint and 4 watchpoint registers.
[    0.390444] 50--hw-breakpoint: maximum watchpoint size is 8 bytes.
[    0.394923] 51--imx6ul-pinctrl 20e0000.iomuxc: Invalid fsl,pins property in node /soc/aips-bus@02000000/iomuxc@020e0000/imx6ul-evk/enet1grp
[    0.395735] 52--imx6ul-pinctrl 20e0000.iomuxc: initialized IMX pinctrl driver
[    0.396617] 53--imx6ul-pinctrl 2290000.iomuxc-snvs: initialized IMX pinctrl driver
[    0.535396] 54--mxs-dma 1804000.dma-apbh: initialized
[    0.543515] 55--SCSI subsystem initialized
[    0.554184] 56--usbcore: registered new interface driver usbfs
[    0.554404] 57--usbcore: registered new interface driver hub
[    0.554682] 58--usbcore: registered new device driver usb
[    0.557264] 59--i2c i2c-0: IMX I2C adapter registered
[    0.557322] 60--i2c i2c-0: can't use DMA, using PIO instead.
[    0.559722] 61--i2c i2c-1: IMX I2C adapter registered
[    0.559775] 62--i2c i2c-1: can't use DMA, using PIO instead.
[    0.561023] 63--Linux video capture interface: v2.00
[    0.561251] 64--pps_core: LinuxPPS API ver. 1 registered
[    0.561279] 65--pps_core: Software ver. 5.3.6 - Copyright 2005-2007 Rodolfo Giometti <giometti@linux.it>
[    0.561352] 66--PTP clock support registered
[    0.564578] 67--MIPI CSI2 driver module loaded
[    0.565819] 68--imx rpmsg driver is registered.
[    0.566340] 69--Advanced Linux Sound Architecture Driver Initialized.
[    0.568595] 70--Bluetooth: Core ver 2.22
[    0.568732] 71--NET: Registered protocol family 31
[    0.568756] 72--Bluetooth: HCI device and connection manager initialized
[    0.568801] 73--Bluetooth: HCI socket layer initialized
[    0.568844] 74--Bluetooth: L2CAP socket layer initialized
[    0.568941] 75--Bluetooth: SCO socket layer initialized
[    0.584056] 76--clocksource: Switched to clocksource mxc_timer1
[    0.773016] 77--VFS: Disk quotas dquot_6.6.0
[    0.773201] 78--VFS: Dquot-cache hash table entries: 1024 (order 0, 4096 bytes)
[    0.802949] 79--NET: Registered protocol family 2
[    0.804706] 80--TCP established hash table entries: 4096 (order: 2, 16384 bytes)
[    0.804840] 81--TCP bind hash table entries: 4096 (order: 3, 32768 bytes)
[    0.805000] 82--TCP: Hash tables configured (established 4096 bind 4096)
[    0.805145] 83--UDP hash table entries: 256 (order: 1, 8192 bytes)
[    0.805206] 84--UDP-Lite hash table entries: 256 (order: 1, 8192 bytes)
[    0.805624] 85--NET: Registered protocol family 1
[    0.826972] 86--RPC: Registered named UNIX socket transport module.
[    0.827009] 87--RPC: Registered udp transport module.
[    0.827030] 88--RPC: Registered tcp transport module.
[    0.827051] 89--RPC: Registered tcp NFSv4.1 backchannel transport module.
[    0.833521] 90--Bus freq driver module loaded
[    0.848689] 91--workingset: timestamp_bits=30 max_order=17 bucket_order=0
[    0.879592] 92--NFS: Registering the id_resolver key type
[    0.879684] 93--Key type id_resolver registered
[    0.879707] 94--Key type id_legacy registered
[    0.879834] 95--jffs2: version 2.2. (NAND) ? 2001-2006 Red Hat, Inc.
[    0.881783] 96--fuse init (API version 7.26)
[    0.901136] 97--io scheduler noop registered
[    0.901174] 98--io scheduler deadline registered
[    0.901684] 99--io scheduler cfq registered (default)
[    0.903058] 100--imx-weim 21b8000.weim: Driver registered.
[    0.912375] 101--backlight supply power not found, using dummy regulator
[    0.921844] 102--sii902x 1-0039: No reset pin found
[    0.925780] 103--21c8000.lcdif supply lcd not found, using dummy regulator
[    1.062768] 104--Console: switching to colour frame buffer device 128x37
[    1.194190] 105--mxsfb 21c8000.lcdif: Success seset LCDIF
[    1.194231] 106--mxsfb 21c8000.lcdif: initialized
[    1.197854] 107--imx-sdma 20ec000.sdma: loaded firmware 3.3
[    1.209390] 108--2020000.serial: ttymxc0 at MMIO 0x2020000 (irq = 20, base_baud = 5000000) is a IMX
[    1.913401] 109--console [ttymxc0] enabled
[    1.919501] 110--21ec000.serial: ttymxc2 at MMIO 0x21ec000 (irq = 73, base_baud = 5000000) is a IMX
[    1.930037] 111--21fc000.serial: ttymxc5 at MMIO 0x21fc000 (irq = 74, base_baud = 5000000) is a IMX
[    1.977363] 112--imx-rng 2284000.rngb: iMX RNG Registered.
[    1.983290] 113--imx sema4 driver is registered.
[    1.987817] 114--[drm] Initialized
[    1.992215] 115--[drm] Initialized vivante 1.0.0 20120216 on minor 0
[    2.195772] 116--brd: module loaded
[    2.278180] 117--loop: module loaded
[    2.291825] 118--spi_gpio spi4: gpio-miso property not found, switching to no-rx mode
[    2.305384] 119--spi_imx 2008000.ecspi: probed
[    2.312272] 120--spi_imx 2010000.ecspi: probed
[    2.319719] 121--libphy: Fixed MDIO Bus: probed
[    2.325220] 122--CAN device driver interface
[    2.332039] 123--flexcan 2090000.can: device registered (reg_base=a0a00000, irq=29)
[    2.343201] 124--20b4000.ethernet supply phy not found, using dummy regulator
[    2.395394] 125--pps pps0: new PPS source ptp0
[    2.402178] 126--libphy: fec_enet_mii_bus: probed
[    2.420187] 127--fec 20b4000.ethernet eth0: registered PHC device 0
[    2.427695] 128--2188000.ethernet supply phy not found, using dummy regulator
[    2.485255] 129--pps pps1: new PPS source ptp1
[    2.493319] 130--fec 2188000.ethernet eth1: registered PHC device 1
[    2.501554] 131--PPP generic driver version 2.4.2
[    2.506718] 132--PPP BSD Compression module registered
[    2.511478] 133--PPP Deflate Compression module registered
[    2.516712] 134--PPP MPPE Compression module registered
[    2.521554] 135--NET: Registered protocol family 24
[    2.526166] 136--SLIP: version 0.8.4-NET3.019-NEWTTY (dynamic channels, max=256) (6 bit encapsulation enabled).
[    2.536042] 137--CSLIP: code copyright 1989 Regents of the University of California.
[    2.543393] 138--SLIP linefill/keepalive option.
[    2.547933] 139--usbcore: registered new interface driver asix
[    2.553545] 140--usbcore: registered new interface driver ax88179_178a
[    2.559926] 141--usbcore: registered new interface driver cdc_ether
[    2.566058] 142--usbcore: registered new interface driver cdc_eem
[    2.571913] 143--usbcore: registered new interface driver net1080
[    2.577838] 144--usbcore: registered new interface driver cdc_subset
[    2.583928] 145--usbcore: registered new interface driver zaurus
[    2.589825] 146--usbcore: registered new interface driver cdc_ncm
[    2.595600] 147--ehci_hcd: USB 2.0 'Enhanced' Host Controller (EHCI) Driver
[    2.602210] 148--ehci-platform: EHCI generic platform driver
[    2.608548] 149--ehci-mxc: Freescale On-Chip EHCI Host driver
[    2.614115] 150--random: fast init done
[    2.617811] 151--ohci_hcd: USB 1.1 'Open' Host Controller (OHCI) Driver
[    2.624188] 152--ohci-platform: OHCI generic platform driver
[    2.630303] 153--usbcore: registered new interface driver cdc_acm
[    2.636124] 154--cdc_acm: USB Abstract Control Model driver for USB modems and ISDN adapters
[    2.644475] 155--usbcore: registered new interface driver usblp
[    2.650152] 156--usbcore: registered new interface driver cdc_wdm
[    2.656140] 157--usbcore: registered new interface driver usb-storage
[    2.662467] 158--usbcore: registered new interface driver usbserial
[    2.668564] 159--usbcore: registered new interface driver usbserial_generic
[    2.675329] 160--usbserial: USB Serial support registered for generic
[    2.681498] 161--usbcore: registered new interface driver option
[    2.687289] 162--usbserial: USB Serial support registered for GSM modem (1-port)
[    2.694527] 163--usbcore: registered new interface driver usb_ehset_test
[    2.703980] 164--2184800.usbmisc supply vbus-wakeup not found, using dummy regulator
[    2.713957] 165--2184000.usb supply vbus not found, using dummy regulator
[    2.738012] 166--2184200.usb supply vbus not found, using dummy regulator
[    2.748751] 167--ci_hdrc ci_hdrc.1: EHCI Host Controller
[    2.753763] 168--ci_hdrc ci_hdrc.1: new USB bus registered, assigned bus number 1
[    2.784209] 169--ci_hdrc ci_hdrc.1: USB 2.0 started, EHCI 1.00
[    2.792366] 170--hub 1-0:1.0: USB hub found
[    2.796517] 171--hub 1-0:1.0: 1 port detected
[    2.803939] 172--mousedev: PS/2 mouse device common for all mice
[    2.812216] 173--input: 20cc000.snvs:snvs-powerkey as /devices/soc0/soc/2000000.aips-bus/20cc000.snvs/20cc000.snvs:snvs-powerkey/input/input0
[    2.827262] 174--imx6ul-pinctrl 20e0000.iomuxc: pin MX6UL_PAD_GPIO1_IO01 already requested by 2010000.ecspi; cannot claim for 2040000.tsc
[    2.839501] 175--imx6ul-pinctrl 20e0000.iomuxc: pin-24 (2040000.tsc) status -22
[    2.846507] 176--imx6ul-pinctrl 20e0000.iomuxc: could not request pin 24 (MX6UL_PAD_GPIO1_IO01) from group tscgrp  on device 20e0000.iomuxc
[    2.858683] 177--imx6ul-tsc 2040000.tsc: Error applying setting, reverse things back
[    2.866125] 178--imx6ul-tsc: probe of 2040000.tsc failed with error -22
[    2.873622] 179--usbcore: registered new interface driver usbtouchscreen
[    2.880671] 180--Gt9xx driver installing..
[    2.884966] 181--goodix-ts 1-005d: GTP Driver Version: V2.8.0.2<2017/12/14>
[    2.891551] 182--goodix-ts 1-005d: GTP I2C Address: 0x5d
[    2.896623] 183--goodix-ts 1-005d: touch input parameters is [id x y w p]<5 800 480 1024 1024>
[    2.905390] 184--1-005d supply vdd_ana not found, using dummy regulator
[    2.911797] 185--1-005d supply vcc_i2c not found, using dummy regulator
[    2.918260] 186--goodix-ts 1-005d: Failed get pinctrl state:output_high
[    2.924686] 187--goodix-ts 1-005d: Success request irq-gpio
[    2.929881] 188--goodix-ts 1-005d: Success request rst-gpio
[    2.935125] 189--goodix-ts 1-005d: try to reset and read Guitar 0
[    2.940823] 190--goodix-ts 1-005d: Guitar reset
[    2.976344] 191--goodix-ts 1-005d: I2C Addr is 5d
[    2.982290] 192--goodix-ts 1-005d: IC Version: 911_1060
[    2.989097] 193--goodix-ts 1-005d: Driver set not send config
[    3.025520] 194--goodix-ts 1-005d: Use slot report protocol
[    3.031373] 195--input: goodix-ts as /devices/virtual/input/input1
[    3.037826] 196--goodix-ts 1-005d: INT num 84, trigger type:2
[    3.043803] 197--goodix-ts 1-005d: create proc entry gt9xx_config success
[    3.050416] 198--goodix-ts 1-005d: Alloc memory size:1534.
[    3.055583] 199--goodix-ts 1-005d: I2C function: without pre and end cmd!
[    3.061992] 200--goodix-ts 1-005d: Create proc entry success!
[    3.071637] 201--snvs_rtc 20cc000.snvs:snvs-rtc-lp: rtc core: registered 20cc000.snvs:snvs-r as rtc0
[    3.081742] 202--i2c /dev entries driver
[    3.087102] 203--IR NEC protocol handler initialized
[    3.091686] 204--IR RC5(x/sz) protocol handler initialized
[    3.096898] 205--IR RC6 protocol handler initialized
[    3.101466] 206--IR JVC protocol handler initialized
[    3.106087] 207--IR Sony protocol handler initialized
[    3.110736] 208--IR SANYO protocol handler initialized
[    3.115520] 209--IR Sharp protocol handler initialized
[    3.120254] 210--IR MCE Keyboard/mouse protocol handler initialized
[    3.126161] 211--IR XMP protocol handler initialized
[    3.134368] 212--pxp-v4l2 pxp_v4l2: initialized
[    3.144166] 213--imx2-wdt 20bc000.wdog: timeout 60 sec (nowayout=0)
[    3.152161] 214--Bluetooth: HCI UART driver ver 2.3
[    3.156966] 215--usb 1-1: new high-speed USB device number 2 using ci_hdrc
[    3.164000] 216--Bluetooth: HCI UART protocol H4 registered
[    3.169500] 217--Bluetooth: HCI UART protocol BCSP registered
[    3.175134] 218--Bluetooth: HCI UART protocol ATH3K registered
[    3.181173] 219--usbcore: registered new interface driver bcm203x
[    3.187525] 220--usbcore: registered new interface driver btusb
[    3.197431] 221--sdhci: Secure Digital Host Controller Interface driver
[    3.203819] 222--sdhci: Copyright(c) Pierre Ossman
[    3.208411] 223--sdhci-pltfm: SDHCI platform and OF driver helper
[    3.218469] 224--sdhci-esdhc-imx 2190000.usdhc: could not get ultra high speed state, work on normal mode
[    3.228486] 225--sdhci-esdhc-imx 2190000.usdhc: Got CD GPIO
[    3.296039] 226-mmc0: SDHCI controller on 2190000.usdhc [2190000.usdhc] using ADMA
[    3.304896] 227--sdhci-esdhc-imx 2194000.usdhc: could not get ultra high speed state, work on normal mode
[    3.346837] 228--hub 1-1:1.0: USB hub found
[    3.350918] 229--hub 1-1:1.0: 4 ports detected
[    3.374169] 230--mmc1: SDHCI controller on 2194000.usdhc [2194000.usdhc] using ADMA
[    3.393520] 231--ledtrig-cpu: registered to indicate activity on CPUs
[    3.417047] 232--usbcore: registered new interface driver usbhid
[    3.422680] 233--usbhid: USB HID core driver
[    3.470346] 234--wm8960 1-001a: No platform data supplied
[    3.489429] 235--mmc1: new DDR MMC card at address 0001
[    3.506280] 236--mmcblk1: mmc1:0001 Q2J54A 3.64 GiB
[    3.521780] 237--mmcblk1boot0: mmc1:0001 Q2J54A partition 1 2.00 MiB
[    3.538801] 238--mmcblk1boot1: mmc1:0001 Q2J54A partition 2 2.00 MiB
[    3.555934] 239--mmcblk1rpmb: mmc1:0001 Q2J54A partition 3 512 KiB
[    3.571929] 240-- mmcblk1: p1 p2 p3
[    3.611342] 241--imx-wm8960 sound: wm8960-hifi <-> 202c000.sai mapping ok
[    3.620557] 242--imx-wm8960 sound: snd-soc-dummy-dai <-> 2034000.asrc mapping ok
[    3.628291] 245--imx-wm8960 sound: wm8960-hifi <-> 202c000.sai mapping ok
[    3.674152] 246--usb 1-1.1: new high-speed USB device number 3 using ci_hdrc
[    3.768274] 247--NET: Registered protocol family 26
[    3.775213] 248--NET: Registered protocol family 10
[    3.792786] 249--sit: IPv6, IPv4 and MPLS over IPv4 tunneling driver
[    3.800939] 250--NET: Registered protocol family 17
[    3.805670] 251--can: controller area network core (rev 20120528 abi 9)
[    3.812075] 252--NET: Registered protocol family 29
[    3.816691] 253--can: raw protocol (rev 20120528)
[    3.821006] 254--can: broadcast manager protocol (rev 20161123 t)
[    3.826951] 255--can: netlink gateway (rev 20130117) max_hops=1
[    3.834591] 256--Bluetooth: RFCOMM TTY layer initialized
[    3.842900] 257--Bluetooth: RFCOMM socket layer initialized
[    3.848441] 258--Bluetooth: RFCOMM ver 1.11
[    3.852260] 259--Bluetooth: BNEP (Ethernet Emulation) ver 1.3
[    3.857741] 260--Bluetooth: BNEP filters: protocol multicast
[    3.863377] 261--Bluetooth: BNEP socket layer initialized
[    3.868604] 262--Bluetooth: HIDP (Human Interface Emulation) ver 1.2
[    3.874661] 263--Bluetooth: HIDP socket layer initialized
[    3.880139] 264--8021q: 802.1Q VLAN Support v1.8
[    3.884635] 265--lib80211: common routines for IEEE802.11 drivers
[    3.890445] 266--Key type dns_resolver registered
[    3.904779] 267--cpu cpu0: Registered imx6q-cpufreq
[    3.922429] 268--Bluetooth: hci0: rtl: examining hci_ver=06 hci_rev=000b lmp_ver=06 lmp_subver=8723
[    3.931242] 269--Bluetooth: hci0: rtl: loading rtl_bt/rtl8723b_config.bin
[    3.937851] 270--bluetooth hci0: Falling back to user helper
[    4.003979] 271--imx_thermal 2000000.aips-bus:tempmon: Industrial CPU temperature grade - max:105C critical:100C passive:95C
[    4.016946] 272--input: gpio-keys as /devices/soc0/gpio-keys/input/input2
[    4.024009] 273--snvs_rtc 20cc000.snvs:snvs-rtc-lp: setting system clock to 1970-01-01 00:00:00 UTC (0)
[    4.033728] 274--ltemodule-pwr: disabling
[    4.037426] 275--wifi-pwr: disabling
[    4.040587] 276--ALSA device list:
[    4.043588] 277-- #0: wm8960-audio
[    4.056139] 278--EXT4-fs (mmcblk1p2): couldn't mount as ext3 due to feature incompatibilities
[    4.198707] 279--EXT4-fs (mmcblk1p2): recovery complete
[    4.207865] 280--EXT4-fs (mmcblk1p2): mounted filesystem with ordered data mode. Opts: (null)
[    4.216203] 281--VFS: Mounted root (ext4 filesystem) on device 179:2.
[    4.223862] 282--devtmpfs: mounted
[    4.230126] 283--Freeing unused kernel memory: 1024K
[    4.478450] 284--EXT4-fs (mmcblk1p2): re-mounted. Opts: data=ordered
Starting syslogd: OK
Starting klogd: OK
Running sysctl: OK
Starting 100ask lvgl: OK
[    5.019793] 285--ds18b20_drv_init OK.
[    5.057062] 286--dht11_drv_init OK.
[    5.107282] 287--irda_drv_init OK.
Populating /dev using udev: [    5.249334] udevd[178]: starting version 3.2.9
[    5.428728] 288--udevd[179]: starting eudev-3.2.9
[    6.561805] 289--inv-mpu6000-spi spi2.0: mounting matrix not found: using identity...
[    7.104311] 290--8723bu: loading out-of-tree module taints kernel.
[    7.770153] 291--usbcore: registered new interface driver rtl8723bu
done
Initializing random number generator: OK
Saving random seed: OK
Starting system message bus: done
Starting network: [    8.728963] 292--Generic PHY 20b4000.ethernet-1:01: attached PHY driver [Generic PHY] (mii_bus:phy_addr=20b4000.ethernet-1:01, irq=-1)
[    8.741361] 293--IPv6: ADDRCONF(NETDEV_UP): eth0: link is not ready
udhcpc: started, v1.31.1
udhcpc: sending discover
[   10.855233] 294--fec 20b4000.ethernet eth0: Link is Up - 100Mbps/Full - flow control rx/tx
[   10.863227] 295--IPv6: ADDRCONF(NETDEV_CHANGE): eth0: link becomes ready
udhcpc: sending discover
udhcpc: sending select for 192.168.2.230
udhcpc: lease of 192.168.2.230 obtained, lease time 1800
deleting routers
adding dns 192.168.2.1
adding dns 192.168.2.1
OK
Starting ModemManager: OK
Starting NetworkManager ... done.
Starting ntpd: OK
Starting mosquitto: OK
Starting pulseaudio: W: [pulseaudio] main.c: This program is not intended to be run as root (unless --system is specified).
[   13.542987] IPv6: ADDRCONF(NETDEV_UP): eth1: link is not ready
[   13.665434] IPv6: ADDRCONF(NETDEV_UP): wlan0: link is not ready
[   15.181962] IPv6: ADDRCONF(NETDEV_UP): wlan0: link is not ready
[   15.319405] IPv6: ADDRCONF(NETDEV_UP): wlan0: link is not ready
[
