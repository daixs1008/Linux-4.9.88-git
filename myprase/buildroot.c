Chapter1 .About Buildroot
Buildroot is a tool that simplifies and automates the process of building a complete Linux system for an 
embeddded system,using cross-compilation.
In order to achieve this,Buildroot is able ti generate a cross-compilation toolchain,a root filesystem,a Linux
kernel image and a bootloader for your target.Buildroot can be used for any combination of these options,
independently (you can for example use an existing cross-compilation toolchain,and builds only your root filesystem 
with buildroot).
Buildroot is useful mainly for people working with embedded systems.Embedded systems often use processors 
that are not the regular x86 processors everyone is used to having in his PC.They can be PowerPC processors,
MIPS processors,ARM processors.etc.
Buildroot supports numerous processors and their variant;it also comes with default configurations for several
boards available off-the-shelf.Besides this,a number of third-party projects are based on,or develop their BSP
or SDK on top of Buildroot.
BSP :Board Support Package 
SDK :Software Development Kit
Chapter 2. System requirements
Buildroot is designsd to run on Linux systems.
While Buildroot itself will build most host package it needs for the compilation,certain standard Linux utilities
are expected to be already installed on the host system.Below you will find an overview of mandatory and 
optional packages(note that package names may vary between distribution).
2.1. Mandatory packages
Build tools:
	which
	sed 
	make (version 3.81 or any later)
	binutils
	build-essential(only for Debian based systems)
	gcc(version 2.95 or any later)
	g++(version 2.95 or any later)
	bash
	patch
	gzip
	bzip2
	perl(version 5.8.7 or any later)
	tar 
	cpio 
	python(version 2.6 or any later)
	unzip
	rsync
	file(must be in /usr/bin/file)
	bc
Souece fetching tolls:
	wget
2.2 Optional package 
configuration interface dependencies
 For these libraries,you need to install both runtime and development data,which in many distributions are 
 package 
 separetely.The development packages typically have a -dev or devel suffix
	ncurses5 to use the menuconfig interface 
	qt4 to use the xconfig interface 
	glib2,gtk2,and glade2 to use gconfig interface 
 Source fetching tools:
	In the official tree,most of the package sources are retrieved using wget from ftp,http or https locations.A 
	few packages are only available through a version control systen.Moerover,Buildroot is capable of 
	downloading sources via other tolls,like reync or scp (refer to Chapter 19 Download infrastrusture for more
	details).if you enable packages using any of these methods,you will need to install the corresponding
	tool on the host system:
		bazaar
		cvs
		git
		mercurial
		rsync
		scp
		subversion 
	Java-ralated packages,if the Java Classpath needs to be built for the target system:
		The javac compiler
		The jar tool
	Documentatiion generation tolls:
		asciidoc,version 8.6.3 or higher
		w3m
		python with the argparse module(automatically present in 2.7+ and 3.2+)
		dblatex(required for the pdf manual only)
	Graph generation tools:
		graphhviz to use graph-depends and <pkg>-graph-depends
		python-matplotlib to use graph-build
Chapter 3. Getting Buildroot 
	Buildroot release are made every 3 months,in February,May,August and November.Release numbers are in 
	the format YYYY.MM,so for example 2013.02,2014.08.
	Release tarballs are available at http://buildroot.org/downloads/.
	for your convenience,a Vagrantfile is available in support/misc/vagrantfile in the Buildroot source tree to 
	quickly set up a virtual machine with the needed dependencies to get started.
	if you want to setup an isolated buildroot environment on Linux or Mac Os X,paste this line onto your terminal:
	curl -O https://buildroot .otg/downloads/vagrantfile;vagrant up
	if you are on windows,paste this into you powershell:
	(new-object System.Net,WebClicnt).DowaloadFile(
	"https://buildroot.org/downloads/Vagranfile","Vagrantfile");
	vagrant up 
		
	if you want to follow development,you are use the daily anapshots or make a clone of the Git repository.Refer to 
	the Dowaload page of the Buildroot website  for more delials.
	
	Chapter 4. Buildroot quick start 
	Important:you can and should build everything as a normal user.There is no need to be root to configure 
	and use Buildroot.By running all commands as a regular user,you protect your system against packages 
	behaving badly during compilation and installation .
	The first step when using Buildroot is to create a configuration.Buildroot has a nice configuration tool similar to 
	the one you can  find in the Liunux kernel or in BusyBox,
	From the buildroot directory,run 
		$ make menuconfig 
		for the original curses-based configurator,or 
		$ make nconfig
		for the new curses-based configurator,or 
		$ make xconfig 
		for the Qt-based configurator,or 
		$ make gconfig 
		for the GTK-based configurator.
		All of these "make " commands will need to build a configuration utility(including the interface),so you may need 
		to install "development " packages for relevant libraries used by the configuration utility.Refer to Chapter 2,
		System requirement for more details,specifically the optional requirements Section 2.2"Optional packages"to 
		get the dependencies of your favorite interface.
		For each menu rntry in the configuration tool,you can find associated help that describes the purpose of the 
		entry.Refer to Chapter 6,Buildroot configuration for details on some specific configuration aspects.
		once everythig is configured,the configuration tool generates a .config file that contains the entire configuration.
		This file will be read by the top-level Makefile.
		To start the build process,simply run :
		$make 
		You should never use make -jN with Buildroot :top-level parallel make is currently not supported.Instead,use 
		the BR2_JLEVEL option to tell Buildroot to run the compilation of each individual package with make -jN.
		mThe make command will generally perform the following steps:
		dowanload source files (as required);
		configure,build install the cross-compilation tollchain,or simply import an external toolchain;
		configure,build and install selected target packages;
		build a bootloader image, if selected;
		build a bootloader image,if selected;
		create a root filesystem in selected formats.
		Buildroot output is stored in a single directory,output/.This dircetory contains several subdirectories:
			image/ where all the images (kernel image, bootloader and root filesystem images)are stored.These are the 
			files you need to put on your target system.
			
			build/ where all the components are built(this includes tools needed by buildroot on the host and packages
			compiled for the target ).This directory contains one subdirectory for each of these components.
			
			staging/ which contains a hierarchy similar to a root filesystem hierarchy.this directory contains the 
			headers and libraries of the cross-compilation toolchain and all the userspace packages selected for the 
			target.However,this directory is not intended to be the root filesystem for target:it contains a lot of 
			development files,unstripped binaries and libraried that make it far too big for an embedded system.These 
			development files are used to compile libraries and applications for the target that depend on other libraries.
			
			target/ which contains almost the complete root filesystem for the target:everything needed is present 
			execpt the device files in /dev/(Buildroot can't create them because Buildroot doesn't run as root and doesn't
			want to run as root).Also,it doesn't have the correct permissions(e.g.setuid for the busybox binary).
			Therefor,this directory should not bo used on your terget.Instead,you should use one oif the images 
			built in the images/ directory.if you need an extracted image of the root filesystem for booting over NFS,
			then use the tarball image generated in images/ and extract it as root.Compared to staging/,target/
			contains only the files and libraries needed to run the selected target applications:the development files 
			(headers,etc.)are not present,the binaries are stripped.
			host/ contains the installation of tolls compiled for the host that are needed for the proper execution of 
			Builroot,including the cross-compilation tollchain.
		These commands, make menuconfig|nconfig|gconfig|xconfig and make ,are the basic ones that allow to easily
		and quickly generate image fitting your needs,with all the features and applications you enabled.
		More details about the "make" command usage are given in Section 8.1 "make tips".
		
		Chapter 5. Community resources 
		Like any open source project,Buildroot has different ways to share information in its communoty and outside.
		Each of those ways interest you if you are looking for some help,want to understand Buildroot or contribute 
		to the project.
		Mailing List 
			Buildroot has a makiling list for discussion and development.It is the main method of interaction for 
			Buildroot users and developers .
			Only subscribers to the Buildroot mailing list are also available in the mailing list archives and via Gamane,at 
			gmane.comp.libuclibc.buildroot.Please search the mailing list archives before asking questions,since 
			there a good chance someone else has asked the same question before.
		IRC 
			The Buildroot IRC channel #buildroot is hosted on freenode.It is a useful place to ask quick question or 
			discuss on certain topics.
			When asking for IRC for help on IRC,share relevant logs or pieces of cide using a code sharing website,such as 
			http://cide.bulix.org
			Note that for certain questions,posting to the mainling list may be better as it will reach more people ,both
			developers and users.
		Bug tracjer
			Bugs in Buildroot can be reported via the mailing list or alternatively via the Buildroot bugtracker.please
			refer to Section 21.6, "Reporting issues/bugs or getting help"before creatig a bug report.
		Wiki
			The Buildroot wiki page is hosted on the elinux wiki.It contains some useful links an overview of past and 
			upcoming events,and a TODO list.
		Patchwork 
			Patchwork is a web-based patch tracking system designed to facilitate the contribution and management of 
			contributions to an open-source project.Patches that have sent to a mailing list are 'caught' by the 
			system,and appear on a web page,Any comments posted that reference the patch are appended to the 
			patch page too.For more information on Patvhwor see http://jk.ozlabs.org/projects/patchwork/.
			
			Buildroot's Patchwork website is mainly for use by Buildroot's makintainer to ensure patches missed.
			It is also used by Buildroot patch reviewers(see alse Section 21.3.1 "Applying Patches form Patchwork").
			However,since the website exposes patches and their corresponding review comments in a clean and concise web
			interface,it can be useful for all Builroot developers.
			The buildroot patch management interface is available at http://patchwork.buildroot.org.
		Part II. User guide
	All the configuration options in make *config have a help text providing details about option.
	The make *config commands also offer a search tool.Read the help message in the different frontend menus to 
	know how to use it .
		in menuconfig ,the search tool os called by pressing /;
		in xconfig ,the search tool is called by pressing Ctrl+f.
		The result of search shows the help message of the matching items.In menuconfig ,numbers in the left 
		column provide a shortcut to the corresponding entry.Just type this number to directly hump to the entry ,or to 
		the containing menu in case the entry is not selectable due to a missing dependency.
		Although the menu structure and the help text of the entries should be suffciently self-explanatory,a number of 
		topics require additional explanation that cannot easily be covered in the help text and therefore covered in 
		the following sections.
		6.1. Cross=copilation toolchain 
		A compilation toolchain is the set os tools that allows you to compile code for you system.It consists of a 
		compiler (in our case,gcc),binary utils like assemblerand linker(in our case,bintils)and a C standard library
		(for example GNU Libc,uClibc-ng),
		The system installed on your development station certainly already has a compilation toolchain that you can use 
		to compile an application that runs in your system.If you're using a PC,your compilation toolchain runs in an x86 
		processaor and generates code for an x86 processor.Ubder most Linux systems,the compilation toolchain 
		uses the GUN libc(glibc)as the C standard library.This compilation toolchain is called the "host compilation 
		toolchain".The machine on which it is runing ,and on which you're workong,is called the "host system".
		The compilation toolchain is provided by your distribution,and Buildroot has nothing to with it (other than 
		using it to build a cross-compilation toolchain and other tools that are run on the development host).
		As said above,the compilation toolchain that comes with your system rins on and generates code for the 
		processor in your host system.As your embedded system has a different processor,you need a cross-compilation 
		toolchain - a compilation toolchain that runs on your host system but generates code for your target sysrem(and 
		target processor).For example,if your host system users x86 and your target system uses ARM,the regular 
		compilation toolchain in you host runs on x86 and generates code for x86,while the cross-compilationtoolchain 
		runs on x86 and generates code for ARM,
		Buildroot provides two solutions for the cross-compilation toolchain:
			The internal toolchain backend,called buildroot toolchain in the configuration interface.
			The external toolchain backend,called Extarnal toolchain in the configuration interface.
		The choice between these two suolutions is done using the Toolchain Type option in the Toolchsin menu.Once
		one solution has been chosen,a number of configuration option appear,they are detailed in the following 
		setions,
		6.1.1. internal toolchain backend 
		The internal toolchain backend is the backend whwrw Buildroot builds by itself a croee-compilation toolchain,
		before building the userspace application and libraries for your target embedded system.
		This backend supports several C libraries:uClibc-ng and musl.
		once you have selacted this backend,a number of option appear.The most important ones allow to:
			Change the version of the Linux kernel headers used to build the toolchion.This deserves s few 
			explanations.In the process of buioding a cross-compilation toolchain,the C library is being built.This library
			provides the interface between userspace applications and the Linux kernel.In order to know how to "talk" to 
			the Linux kernal,the C library needs to have sccess to the Linux kernel headers(i.e. yhe .h files from the kernal),
			which define the interface between userspace and the kernel(system calls,data structures,etc.).
			Since this interface is backward compatible,the version of the Linux kernel headers used to build your 
			toolchain do not need to match eaactly the version of the Linux kernel you intend to run on your embedded
			system.They only need to have a version equal or to the version of the Linux kernel you intend to run.If 
			you use kernel headers that are more recent than the Linux kernel you run on you embedded system,then 
			the C library might be using interfaces that are not provided by your Linux kernel.
			Change the versiom of the GCC compiler ,binutils and the C library.
			Select a number of toolchain options(uClibc only):whether the toolchain should have RPC support(used 
			mainly for NFS),wide-char support,locale support(for internationnalization),C++ suport or thead support.
			Depending on which options you choose,the number of userspace applications libraries visible in
			Buildroot menus wil change:many applications and libraries require certain toolchain options to enabled.
			Most packages show a comment when a ceertain toolchain option is required to be anable those
			packages.If needed,you can further refine the uClibc configuration by runing make uclubc-menuconfig.
			Note however that all packages in Buildroot are tested against the default uClibc,configration bundled in 
			Buildroot:if you deviate from this configration by removing features from uClibc,some packages may no 
			longer build.
			It is worth noting that whenever one of those options is modified,then the entire toolchain and system must be 
			rebuilt.See Section 8.2 "Understanding when a full rebuild is necessary".
			Advantages of this backend:
				Well intergrated with BuildrootFast,only huilds what's necessary
			Drabacks of this backend:
				Rebuilding the toolchain the needed when doing make clean,ehich takes time.If you're trying to reduce your 
				build time,consider using the External toolchain backend.
			6.1.2 External toolchain backend 
			The external tollchain backend allows to use existing pre-built cross-compilation toolchain.Buildroot knows
			about a number of well-know cross-compilation toolchains(from Linaro for ARM,Sourcery cideBench for 
			RM,x86-64,PowerPC,and MIPS,and is capable of downloading them automatically,or can be pointed to a 
			custom toolchain,either available for download or installed locally,)
			Then,you have three solutions to use an eaternal toolchain:
				use a predefined eaternal toolchain profile,and let Buildroot download,eatract and install the toolchain.
				Buildroot already knows about a few Codesourcery and Linaro toolchain.Just select the tollchain profile in 
				Toolchain from the available ones.This is definitely the easiest solution.
				use a predefined external toolchain profile, and instead of haning Buildroot download extract the 
				toolchain, you can tell Buildroot where your toolchain is already installed on you system. Just select the 
				toolchain,profile in Toolchain through the available ones,unselect Download toolchain automatically,and
				fill the Tollchain path text entry with with the path to your cross-cmpiling toolchain.
				use a completely custom external toolchain.This is particularly useful for toolchains generated using 
				crosstool-NG or with Buildroot itself.To do this,select the Custom toolchain solution in Toolchain list.
				You nend to fill the the Tollchain path ,Tollchain prefix and External toolchain C library optionsThen,
				you have to tell Buildroot what your eaternal toolchain supports.If your external toolchain uses the glibc
				library,you only have to tell whether your toolchain supports C++ or not and whether it has built-in RPC
				support.If your external toolchain uses the uClibc library,then you have to tell Buildroot if it supports RPC,
				wide-char,locale,program invocation,threads and C++.At the beginning of the execution,Buildroot will tell 
				you if rhe selected options do not match the toolchain configuration.
			our external toolchain support has been tested with toolchains from CideDourcery and Linaro,toolchainsgenerated by 
			crosstool-NG, and toolchains generated by Buildroot itself.In general,all toolchains that support the sysroot 
			feature should work.If not,do not hesitate to contact the developers.
			We do not support toolchains or SDK generated by openEmbedded or Yocto, bucause these toolchains are not 
			pure toolcains(i.e just the compiler,binutils,the C and C++ libraries).Instead these toolchains come with a 
			very large set of pre-compiled libraries and programs,Therefore,Buildroot cannot import the systoot of the toolchain,
			as it would conchain hundreds of megabytes of pre-compiled libraries that normally built bu 
			Buildroot.
			We also do not support using distribution (i.e. the gcc/binutils/C library installed by yourdistribution )
			as the toolchain to build software for the target.This is bucause your distribution toolchain is not a 
			"pure"toolchain (i.e. only with the C/C++ library), so we cannot import it properly into the Buildroot build 
			environment.So even if you are building a system for a x86 or x86_64 target,you have to gengrate a cross-compilation 
			toolchain with Buildroot or ctosstool-NG.
			If you want to generate a custom toolchain for your projcet, that can be used as an external toolchain in 
			Buildroot, out recommendation is definitely to build it with crosstool-NG.We recommend to build the toolchain 
			separetely from Buildroot, and then import it in Buildroot using the external toolchain backend.
		Advantages of this backend:
			Sllows to use well-hnow and well-tested cross-compilation toolchains.
			Avoids the build time of the cross-compilation toolchain,which is often very significant in overall build 
			time of an embedded Linux system.
		Drawbacks of this backend:
			If your pre-built eaternal toolchain has a bug,may be hard to get a fix from the toolchain vendor,unless you 
			build your external toolchain by yourself using Crsstool-NG.
		External toolchain wrapper
		When using an external toolchain ,Buildroot generates a wrapper program ,that transparently passes the 
		appropriate option (according to the configuration )to the extenal toolchain programs.In case you need to 
		debug this wrapper to check exactly what arguments are passed,you can set environment variable
		BR2_DEBUG_WRAPPER to either one of:
			0,empty or not set :no debug 
			2:trace all arguments on a single line
			2:trace one argument per line 
		6.2 /dev management
		On a Linux system, the /dev directory contains special files,called device files,that allow userspace applications
		to access the hardware devices managed by the Linux kernal.Eithout these device files,your userspace 
		applications would not be able to use the hardware devices,even if they are properly recognized by the Linux 
		kernel.
		Under System configuration ,/dev management ,Buildroot offers four different solutions to handle the /dev
		directotry:
			The first solution is Static using device table .This is old classical way of handling device files in Linux.
			With this methed,the device files are persistently stored in the root filesystem(i.e. they persist across
			reboots),and there is nothing that will automatically create and remove those device files when hardware 
			devices are added or removed from the system .Buildroot therefore creates a standard set of device files using 
			a device table ,the default one being stored in system/device table dev.txt in the Buildroot source code.
			This file is processed when Buildroot generates the final root filesystem image,and device files are 
			therefore not visible in the output/target directory .The BR2_ROOTFS_STATIC_DEVICE_TABLE option allows to 
			change the default device table used by Buildroot ,or to add an additional device table ,so that sdditional
			device files are creared by Buildroot during the build.So,if you use this method,and a device file is missing 
			BR2_ROOTFS_STATIC_DEVICE_TABLE to system/device_table_dev.txt
			board/<yourcompany>/device_table_dev.txt.For more details about the format of the device 
			table file,see Chapter 23 Makedev syntax documentation 
			The second solution is Dynamic using devtmpfs only .devtmpfs is a virtual filesystem inside the Linux 
			kernel that has been introduced in kernel 2.5.32(if you use older kernel ,it is not possible to use this 
			option ).When mounted on /dev ,this virtual filesystem will automatically make device files appear and 
			disappear as hardware devices are added and removed from the system .This filesystem is not persistent 
			across reboots:it is filled dynamically by the kernel .Using devtmpfs requires the following kernel
			configuration option to be enabled :CONFIG_DEVTMPFS and CONFIG_DEVTMPFS_MOUNT.When Buildroot is in 
			charge of building the Linux kernel for your embedded device ,it makes sure that those two option are 
			enabled.However,if you build your Linux kernel outside of Buildroot ,then it is your reponsibility to enable
			those two options (if you fail to do so ,your Buildroot system will not boot).
			The third solution is Dynamic using devtmpfs + mdev .This method also relies on the devtmpfs virtual 
			filesyetem details above (so the requirement to have CONFIG_DEVTMPFS and CONFIG_DEVTMPFS_MOUNT enabled
			in the kernel configuration still apply),but adds the mdev suerspace utility on top of it.medv is program part 
			of BusyBox that the kernel will call every time a device is added or removed.Thanks to the /etc/mdev.config
			configuration file,mdev can be configured to for example,set specific permissions or owership on a device
			file,call a script or application whenever a device appears or disappear,etc.Basically,it allowa userspace to 
			react on device addition and removal events.mdev can for example be used to automatically load kernel
			modules when devices appear on the system.medv is slao important if you have devices that require a 
			firmware,as it will be responsible for pushing the firmware contents to the kernel .mdev is lightweigh
			implementation (with fewer features)of udev.Fir more details about mdev and the syntax of its configuration 
			file,see http://git.busybox.net/busybox/tree/docs/mdev.txt.
			The fourth solution is Dynamic using devtmpfs+eudev.This method also relies on the devtmpfs virtual 
			filesystem details above,but adds the eudev userspace daemon on top of it .eudev is daemon that runs in 
			the background,get called by the kernel when a device gets adds or removed from the system.It is a 
			more heavyweight solution than medv,but provides higher flexibility.eudev is a standalone version of udev,
			the original userspace daemon used in most desktop Linux distributions,which is now part of Systemd.For
			more details,see http://en.wikipedia.org/wiki/Udev.
		The Buildroot developers recommendation is to start wit the Dynamic using devtmpfs only solution ,until
		you have the need for userspace to notified when devices are added/removed,or if firmwares are needed,in 
		which case Dynamic using devtmpfs+mdev is usually a good solusion.
		Note that if systemd is chosen as init system,/dev managment will be performed by the udev program provided 
		by systemd.
		6.3 init system
		The init program is the first userspace program started by the kernel (it carries the PID number 1),and is 
		reponsible for starting the userspace services and programs(for example:web server,graphical applications,
		other network servers,etc.).
		Buildroot allows to use three different types of init systems,which can be chosen from System configuration,
		init system:
			The first solution is Busybox.Amongst many programs,BusyBox has an implementation of a basic init 
			program,which is sufficient for most embedded systems.Enabling the BR2_INIT_BUSYBOXwill ensure
			BusyBox will build and install its init program,This is default solution in Builroot.The BusyBox init 
			program wil read the /etc/inittab file at boot to know what to do.The syntax of this file can be found in 
			http:/git.busybox.net/busybox/tree/examples/inittan(note that BusyBox inittab syntax is special:do not
			use a random inittab documentation from the Internet to learn about BusyBox inittab).The default 
			inittab in Buildroot is stored in system/skeleton/etc/inittab.Apart from mounting a few important
			filesystems,the main job the default inittab does is start the /etc/init.d/rcS shell script,and start a getty
			program(which provides a login prompt).
			The second solution is systemV.This solution uses the old traditional sysvinit program,packed in Buildroot 
			in package/sysvinit.This was the solution used in most destop Linux distributions,until they swithed to 
			more recent alternatives such as Upstart or Systemd.sysvinit also works with an inittab file (which has a 
			slightly different synrax than the one from BusyBox).The default inittab installed with this init solution is 
			located in package/ayavinit/inittab.
			The third solution is systemd.systemd is new generation init system for Linux.It does far more than 
			traditional init programs:aggressive parallelization capabilityes,uses socket and D-Bus activation for starting
			services,offers on-demand starting of daemons,keeps track of processes using Linux control groups,
			supports snapshotting and restoring of the system state,etc.systemd will be useful on relatively complex
			embedded systems,for example the ones requiring D-Bus and services communicating between each other.It
			is wirth noting that systemd brings a big number of large dependencies:dbus,udev and more.For more
			details about systemd,see http://www.freedesktop.org/wiki/Software/systemd.
			The solution recommended by Buildroot developers is to use BusyBox init as it is sufficient for most 
			embedded systems.systemd can be used for more complex situations.
		This terminology differs from what is used by GUN configure,where the host is the machine on which the 
		application will run (which is usually the same as target )
		Chapter 7. Configuration of other components
		Before attempting to modify any of the components below,make sure you have alreagy configured Buildroot 
		itself,and have enabled the corresponding package.
		BusyBox
			If you already have a BusyBox configuration file,you can directly specify this file in the Buildroot 
			configuration,using CR2_PACKAGE_BUSYBOX_CONFIG.Otherwise,Buildroot will start from a default BusyBox
			configuration file.
			To make subsequent changes to the configuration,use make busybox-menuconfig to open the BusyBox
			configuration editor.
			It is also possible to specify a BusyBox configuration file through an environment variable,althrough
			this is not recommended.Refer to Section 8.6 "Environment variables"for more details.
		uClibc
			Configuration of uClibc is done in the same way as for BusyBox.The configuration variable to specify an 
			existing configuration file is BR2_UCLIBC_CONFIG.The command to make subsequent change is make 
			uclibc-menuconfig.
		Linux kernel 
			If you already have a kernel configuration file,you can directly specify this file in the Buildroot 
			configuration,using BR2_LINUX_LERNEL_USE_CUSTOM_CONFIG.
			if you do not yet have a kernel configuration file,you can either start by soecifying a defconfig in the 
			Buildroot configuration,using BR2_LINUX_KERNEL_USE_DEFCONFIG.or start by creating an enmoty file and 
			specifying is as custom configuration file,using BR2_LINUX_KERNEL_USR_CUSTOM_CONFIG.
			To make subsequent changes to configuration,use make linux-menuconfig to open the linux 
			configuration editor.
		Barebox 
		Configuration of Barebox is done in the same way as for the Linux kernel.The corresponding configuration
		variables are BR2_TARGET_BAREBOX_USE_CUSTOM_CONFIG and BR2_TARGET_NAREBOX_USE_DEFCONFIG.To open 
		the configuration editor,use make barebox-menuconfig.
		U-Boot
			Configuration of U-Boot(version 2015.04 or newer)is done in the same way aas for the Linux kernel.The 
			corresponding configuration varianles are BR2_TARGET_UBOOT_USE_CUSTOM_CONFIG and 
			BR2_TARGET_UBOOT_USE_DEFCONFIG.To open the configuration editor,use make uboot-menuconfig.
		Chapter 8 General Buildroot usage 
		8.1 make tips
		This is a collection of tips that help you make the most if Buildroot.
		Display all commands executed by make :
			make V=1 <target>
		Display the list of boards with a defconfig:
			make list-defconfigs
		Display all available targets:
			make help
		Not all targets are always available,some setting in the .config file may hide some targets:
			busybos-menuconfig only works when busybos is enabled;
			linux-menuconfig and linux-savedefconfig only work linux is enabled;
			uclibc-menuconfig is only available when the the uClibc C library is selected in the internal toolchain baclend;
			barebox-menuconfig and barebox-savedefconfig only work when the barebox bootloader is enabled.
			uboot-menuconfig and uboor-savedefconfig only work when the U-boot bootloader is anabled.
		Cleaning:Explicit clraning is required when any of the architecture or toolchain configuration oprions are 
		changed.
		To delete all huild products (including build directories,host,staging,and target trees,the images and the 
		toolchain):
			make clean
		Generating the manual:The present manual sources are located int the docs/manual dirctory.To generate 
		the manual:
			make manual-clean 
			make manual
		The manual outputs will be generated in output/docs/manual.
		
	Notes
		a few tools are required to build the documentation (see:Esection 2.2"Optional packages").
		Resetting Buildroot for a new target : To delete all build products as well as the configuration:
		make disclean 
	Notes If cache is enabled,running make clean or disclean does not empty the compiler cache used by 
	Buildroot.To delete it ,refer to Section 8.12.3 "Using aacche in Buildroot".
	Dumping the internal make variables:One can dump all the cariables known to make ,along with their
	values:
		make -s printvars
		VARIABLE=value_of_variable
	It is possoble to tweak the output using some variables:
		VARS will limit the listing to variables which names match the specified make-pattern
		QUOTED_VARS,if set to YES,will single-quote the value_of_variable
		RAW_VARS,if set to YES,will print the unexpanded value
		For example:
			make -s peintvars VARS=BUSYBOX_%DEFENDENCIES
		BUSYBOX_DEPENDENCIES=skeleton toolchain 
		BUSYBOX_FINAL_ALL_DEPENDENCIES=skeleton toolchain
		BUSYBOX_FINAL_DEPENDENCIES=skeleton toolchain
		BUSYBOX_FINAL_PATCH_DEPENDENCIES=
		BUSYBOX_RDEPENDENCIES=ncurses util-linux
		
			make  -s printvars VARS=BUSYBOX_%DEPENDENCIES QUOTED_VARS=YES
		BUSYBOX_DEPENDENCIES='SKELETON TOOLCHAIN'
		BUSYBOX_FINAL_ALL_DEPENDENCIES='skeleton toolchain'
		BUSYBOX_FINAL_DEPENDENCIES='SKELETON TOOLCHAIN'
		BUSYBOX_FINAL_PATCH_DEPENDECIES=''
		BUSYBOX_RDEPENDECIES="NCURSES UTIL-LINUX"
			make -s printvars VARS=BUSYBOX_%DEPENDENCIES RAW_VARS=YES
		BUSYBOX_DEPENDENCIES=skeleton toolchain
		BUSYBOX_FINAL_DEPENENCIES=$(sort $(BUSYBOX_FONAL_DEPENDENCIES) $(BUSYBOX_FINAL_PATCH_DEPENDECIES)
		BUSYBOX_FINAL_PATCH_DEPENDENCIES=$(SORT $(BUSYBOX_DEPENDENCIEA)))
		BUSYBOX_FINAL_PATCH_DEPENDENCIES=$(sort $(BUSYBOX_PATCH_DEPENDENCIES))
		BUSYBOX_RDEPENDENCIES=ncurses util-linux
	The output of quoted variables can be reused in shell scripts,for example:
	$ eval $(make -s printvars VARS=BUSYBOX_DEPENDENCIES QUOTED_VARS=YES)
	$ echo $BUSYBOX_DEPENDENCIES
	skeleton toolchain
	8.2 Undestanding when a full rebuild is nacessary 
	Buildroot does not attempt to detect what parts of the system should be rebuilt when the system configuration is 
	changed through make menuconfig,make xconfig or one of the other configuration tools .In some cases .
	Buildroot should rebuild the entre system, in some cases ,only a specific subset of packages.But detecting this in 
	a completely reliable manner is vary difficult,and therefore the Buildroot developers have decided to simply not
	attempt to do this .
	Instead,it is the responsibility of the user to know when a full rebuildroot is necessary.As a hint,here are a few rules 
	of thumb that can help you understand how to work with Buildroot:
	
	When the target architecture configuration is changed,a complete rebuild is needed.Changing the 
	architecture variant,the binary format or the floating point strategy for example has an inmact on the entire system.
	When the toolchain configuration is changed,a complete rebuild gengrally is needed.Changing the toolchain
	configuration often involves changing the compiler version,type of C library or its configuration,or some
	other fundamental configuration item,and these changes have an onpact on the entire system.
	When an additional package is added to the configuration ,a full rebuild is not necessarily needed.Buildroot 
	will detect that this package has never been built,and will bild it.However,if this package is a library that 
	can optionally br used by packages that have alread been built,Buildroot will not automatically rebuild
	those.Either you know which packages should be rebuilt and you can rebuild them manually,or you should 
	do a full rebuild,For example,let't suppose you have built a system with the ctorrent package, but without
	openssl.Your system works,but you realize you would like to have SSl support in ctorrent,so you enable
	the openssl package in Buildroot configuration and restart the build.Buildroot will detect that openssl
	sdhould be add OpenSSL support.You will either have to do a full rebuild,or rebuild ctorrent itself.
	When a package is removed from the configuration,Buildroot does not do anything special.It does not 
	remove the files installed by this package from the target root filasystem or from the toolchain sysroot.A full 
	rebuild is needed to get rid of thia package.However,generally you don't necessarily need this package to be 
	removed right now:you can wait for the next lunch break to restart the build from scratch.
	When the sub-options of a package are changed,the package is not automatically rebuilt.After making such
	changes,rebuilding only this package is often sufficient,unless enabling the package sub-option adds some
	features to the package that are useful for another package which has alread been built,Again,Buildroot
	does not track when a package should be rebuilt:once a package has been built,it is never rebuilt unless
	explicitly told to do so.
	When a change to the root filesystem skeleton is made,a full rebuild is needed.However,when changes to 
	the root filesyetem overlay,a post-build script or a post-image script are made,there is no need for a full
	rebuild :a simple make invocation will take the changes into account.
Generally speaking,when you're facting a build error and you're unsure of potential consequences of the 
configuration changed you've made,do a full rebuild.If you get the same build error,then you are sure that the 
error is not related to partial rebuilds of packages,and if this error occurs with packages from the official 
Buildroot,do not hesitate to report the problem!As your experience with Buildroot progresses,you will 
progressively learn when a full rebuild is really nacessary,and you will save more and more time .
For reference,a full rebuild id achieved by runing :
	make clean all 
8.3 Understanding how to rebuild packages
one of the most common questions asked by Buildroot users is how to rebuild a given package or how to remove
a package without rebuilding everything from scratch.
Removing a package is unsupported by Buildroot without rebuilding from scratch.This is because Buildroot 
does't keep track of ehich package installs what filsa in the output/staging and output/target directories,or
which package would be compiled differently depending on the availability of another package.
The easoest way to rebuild a single package from scratch is to remove its build directory in output/build.
Buildroot will then re-extract,re-configure,re-compile and re-install this package grom scratch.You can ask 
buildroot to do this with the make <package>-dirclean
On the orher hand,if you pmly want to restart the build process of a package from its compilation step,you can 
run make <package>-rebuild,folled by make or make <package>.It will restart the compilation and installation 
of the package,but not from acratch:it basically re-executes make and nmake install inside the package,so it wil 
only rebuild files that changed.
If you want to rastart the build process of a package from its configuration atep,you can run make 
<apckage>-reconfigure,followed by make or <package>.It will restart the configuration,compilation and 
installation of the package .
internally,Buildroot creates so-called stamp files to keep of which huild steps have been compileted for each
package.They are stored in the package build directory,output/build/<package>-version/ and are 
named.sramp_<stem-name>.The commands detailed above simply manipulate these stamp files to force
Buildroot to restart a specific set of steps of a package build process.
Further details about package special make targets are explained in Secion 8.12.5,"Package-specific make targets"
8.4 Offline builds
If you intend to do an offline build and just want to download all sources that you previously selected in the 
configurator (menuconfig,nconfig,xconfig or gconfig),then issue:
	$ make source 
	you can now disconnect or copy the content of your dl directory to the build-host
	8.5.Building out-of-tree
	As defoult,everything built by Buildroot is stored in the directory output in the Bildroot tree.
	Buildroot also supports building out of tree with a syntax similar to the Linux kernel.To use it,add 
	o=<directory> to the make command line :
		$ make O=/tmp/build 
		Or 
		$ cd /tmp/build;make O=$pwd -C path/to buildroot 
		All the output files will be located under /tmp/build.If the O path does not exist,Buildroot will create it .
		Note:the O path can be either an absolute or a relative path,but if it`s passed as a relative path,it is important to 
	note that it is interpreted relative to the main Buildroot source directory,not the current working directory.
	When using out-of-tree builds,the Buildroot .config and temporary files are also atored in the output directory.
	This means that you can safely run multiple builds in parallel using the same source tree as long as they use 
	unique output directory.
	For ease of use,Buildroot generates a Makefiles wrapper in the output directory - so after the first run,you no 
	longer need to pass O=<...> and -C <...>,simply run (in the output directory):
		$ make <tartet>
	8.6 Environment variables
	Buildroot also honors some environment variables,when they are passed to make or set in the environment:
	HOSTCXX,the host C++ compiler to use 
	HOETCC,the host C compiler to use 
	UCLIBC_CONFIG_FILE=<path/to/.config>,path to the uClibc configuration file,used to compile uClibc,if an 
	internal toolchain is being built.Note that the uClibc configuration file can also be from the configuration 
	interface,so through the Buildroot .config file;this is the recommended way of setting it.
	BUSYBOX_CONFIG_FILE=<path/to/.config>,path to the BusyBox configuration file.Note that BusyBox 
	configuration file can also be set from the configuration interface,so through the Buildroot .config file; this 
	is the recommended way of setting it.
	BR2_CCACHE_DIR to override the directory where Buildroot stores the cached files when using ccache.
	BR2_DL_DIR to override the directory in which Buildroot stores/retrieves downloaded files Note that the 
	Buildroot downlosd directory can also be set from the configuration interface,so through the 
	Buildroot .config file.See Section 8.12.4, "Location of downloaded packages" for more details on how you 
	can set the download directory 
	BR2_GRAPH_ALT,if set and non-empty,to use an alternate color-scheme in build-time graphs
	BR2_GRAPH_OUT to set the filetype of generated graphs,either pdf(the default ),or png.
	BR2_GRAPH_DEPS_OPTS to pass extra options to the dependency graph;see Section 8.8, "Graphing the 
	dependencies between packages" for the accpted option
	BR2_GRAPH_DOT_OPTS is passed verbatim as options to the dot utility to draw the dependency graph.
	
	An example that uses config files located in the toplevel directory and in your $HOME
		$ make UCLIBC_CONFIG_FILE=uClibc.config BUSYBOS_CONFIG_FILE=&HOME/bb.config
	If you want to use a compiler other than default gcc or g++ for building helper-binaries on your host,then do 
		$ make HOSTCXX=g++-4.3-HEAD HOSTCC=gcc-4.3-head 
	8.7 Dealing efficiently with filesystem images
	Filesystem images can get pretty big,depending on the filesystem you choose,the number of packages,whether
	you provisioned free space... Yet,some location in the filesystems images may just be empty (e.g. a long run of 
	zeroes);such a file is called a sparse file.
	Most tools can handle sparse files efficiently,and will only store or write those parts of a sparse file that are not
	empty.
	For example :
		tar accepts -s option to tell it to only store non-zero blocks sparse files:
			tar cf archive.tar -S [files...] will efficiently store sparse files in a tarball
			tar xf archive.tar -S will efficiently store sparse files extracted from a tarball 
		cp accepts the --sparse=WHEN option (WHEN is one of auto ,never or always):
			cp --sparse -always source.file dest.file wiil make dest.file a sparse file if source.file has long 
			runs of zeroses
	Other tools may have similar option.Please consult their respective man pages.
	You can use aparse files if you need to store the filesystem i,ages(e.g. to transfer from one machine to another),
	or if you need to send them (e.g. to the Q&A team ).
	Note bowever that flashing a filesystem image to a device while using the sparse mode of dd may result in a 
	broken filesystem (e.g. the block bitmap of an ext2 filesystem may be corrupted;or,if you have sparse files in 
	your filesystem,those parts may not be all-zeroes when read back).You should only use sparse files when 
	handing files on the build machine,not when transferring them to an actual device that will be used on the 
	target.
	8.8 Graphing the dependencies between packages
	One of Buildroot`s jobs is to know the dependencies between packages,and make sure they are built in the right 
	order.Their dependencies can sometimes be quite complicated,and for a given system,it is often not easy to 
	understand why such or such package was brought into the build by Buildroot.
	In order to help understanding the dependencies,and therefore better understand what is the role of the 
	different components in your embedded Linux system,Buildroot is capable of generating dependency graphs.
	To generate a dependency graph of the full system you have compiled run :
		make graph-depends
	You will find the generated graph in output/graphs/graph-depends.pdf
	If your system is quite large,the dependency graph may be too aomplex and difficult to read.It is therefore 
	possible to generate the dependency graph just for a gives package:
		make <pkg>-graph-depends
	You will find the generated graph in output/graph/<pkg>-graph-depends.pdf.
	Note that the dependency graphs are generated using the dot tool from the Graphviz project,which you must 
	have installed on your system to use this feature.In most distributions,it is available as the graphviz package.
	By default,the dependency graphs are generated in the PDF format.However,by passing the BR2_GRAPH_OUT
	environment variable,you can switch to other output formats,such as PNG,PostScript or SVG. All formats
	supported by the -T option of the dot tool are supported .
	BR2_GRAPH_OUT=svg make graph-depends
	The graph-depends behaviour can be controlled by setting options in the BR2_GRAPH_DEPS_OPTS environment 
	variable.The accepted options are:
		--depth N,-d N,to limit the dependency depth to N levels. The default,0,means no limit.
		--stop-on PKG,-s PKG,to stop the graph on the package PKG.PKG can be an actual package name,a glob,the 
		keyword cirtual(to stop on virtual package),or the kayword host (to stop on host package).The package is 
		still present on the graph,but its dependencies are not.
		--exclude PKG,-x PKG,like --stop-on,but also omits PKG from the graph.
		--transitive,--no-transitive,to draw (or not)the transitive dependencies.The default is to not draw 
		transitive,dependencies
		--colour R,T,H,the comma-separeted list of colours to draw the root package (R),the target package(T)
		and the gost packages(H).Default to:lightblue,grey,gainsboro
		BR2_GRAPH_DEPS_OPTS=`-d 3 --no-transitive --colours=red,green,blue` make graph-depends
	8.9 Graphing the build duration 
	When the build of a system takes a long time,it is sometimes useful to be able to understand which packages are 
	the longest to build,to see if anything can be done to speed up the build.In order to help to such build time
	analysis,Buildroot collects the build time of each step of each package,and allows to generate graphs from this 
	data.
	To generate the build time graph after a build,run:
		make graph-build
	This will genrate a set of files in output/graphs:
		build.hist-build.pdf,a histogram of build time for each package,ordered in the build order.
		build.hist-duration.pdf,a histogram of the build time each package,ordered by duration(lomgest first)
		build.hist-name.pdf,a histogram of the build time for each package,order by package name.
		build.pie-packages.pdf,a pie chart of the build time per package 
		build.pie-staps.pdf,a pie chart of the global time spent in each step of the packages build process.
	This graph-build target requires the Python Matplotlib and Numpy libraries to be installed(python-matplotlib
	and python-numpy on most distributions),and also the argparse module if you`re using a Python version older
	than 2.7(python-argparse on most distributions).
	By deault,the output format for the graph is PDF,but a different format can be selected using the BR2_GRAPH_OUT
	environment variable.The only other format supported is PNG.
	BR2_GRAPH_OUT=png make graph-build
	8.10. Graphing the filesystem size contribution of packages
	When your target system grows,it is sometimes useful to understand how much each Buildroot package is 
	contributing to the overall root filesystem size.To help with an analysis,Buildroot collects data about files 
	installed by each package and using this data,generates a graph and CSV files detailing the size contribution of
	the different package.
	To generate these data after a build,run:
		make graph-size
	This will generate:
		output/graps/graph-size.pdf,a pie chart of the contribution of each package to the overall root filesystem size.
		output/graphs/package-size-stats.csv,a CSV file giving the size contribution of each package to the overall
		root filesystem size.
		output/graphs/file-size-ststs.csv,a CSV file giving the size contribution of rach installed file to the 
		package it belongs,and to the overall filesyetem size.
	This graph-size target requires the Python Matplotlib library to be installed(python-matplotlib on most 
	distributions),and also the argparse module if you`re using a Python version older than 2.7(python-argparse on 
	most distributions).
	Just like for the duration graph,a BR2_GRAPH_OUT environment is supported to adjust the output file format.See 
	Section 8.8,:Graphing the dependencies between packages"for details about this environment variable.
	Note. The collected filesystem size data is only meaningful after a complete clean rebuild.Be sure to run make 
	clean all before using make graph-size .
	To compare the root filesyetem size of two different Buildroot compilations,for example after adjusting the 
	configuration or when switching to another Buildroot release,use the size-stats-compare script.It takes two
	file-size-stats,cvs files(produced by make graph-size)as input.Refer to the help text of script for more 
	details:
		utils/size-stats-compare -hand
	8.11. Integration with Eclipse
	While a part of the embeded Linux developers like classical text editors like Vim or Emacs,and command-line
	based interfaces,a number of other embedded Linux developers like richer graphical interfaces to do their
	development work.Eclipse being one of the most popular Integrated Development Environment,Buildroot 
	integrates with Eclipse in order to ease the development work of Eclipse users.
	Our integration with Eclipse simplifiies the compilation,remote execution and renote debugging of applications
	and libraries that are built on top of a Buildroot system.It does integrate the Buildroot configuration and 
	build processes themselves with Eclipse.Therefore,the typical usage model of our Eclipse integration would be:
		Configure your Buildroot system with make menuconfig,make xconfig or any other configuration interface
		provided with Buildroot.
		Build your Buildroot system by running make.
		Start Eclipse to develop,excute and debug your own custom applications and libraries,that will rely on the 
		librayies built and installed by Buildroot.
	The Buildroot Eclipse integration installation process and usage is described in detail at
	https://github.com/mbats/eclipse-buildroot-bundle/woki.
	8.12. Advanced usage 
	8.12.1 Using the generated toolchain outside Buildroot 
	You may want to compile,for your target,your own programs or other software that are not packageed in 
	Buildroot.In order to do this you can use the toolchain that was generated by Buildroot.
	The toolchain generated by Buildroot is located by default in output/host/. The simplest way to use it is to add 
	output/host/bin/ to your PATH environment variable and then to use ARCH-LINUX-GCC,ARCH-linux-objdump,ARCK-linux-ld,etc
	It is possible to relocate the tollchain - but then --sysroot must be passed every time the compiler is called to tell 
	where the libraries and header files are.
	It is also possible to gengrate the Buildroot toolchain in a directory other than putput/host by using the Build option -> 
	Host dir option.This could be useful the toolchain must be shared with other users.
	8.12.2. Using GDB in Buildroot
	Buildroot allows to do cross-debugging,where the debugger runs the build machine and communicates with 
	gdbserver on the target to control the execution of program.
	To achieve this:
		If you are using an internal tollchain(build by Buildroot),you must enable BR2_PACKAGE_HOST_GDB,
		BR2_PACKAGE_GDB and BR2_PACKAGE_GDB_SERVER.This ensures that both the ctoss gdb and gdnserver get built,
		and that gdbserver gets installed to your target.




	Introduction 
	Unix is simple and coherent,but it takes a genius
	(or at any rate a programmer) to understand
	and appreciate the simplicity.
	--Dennis Ritchie
	
	Note grom the authors:Tes,we have lost our minds.
	Be forewarned:you will lose yours too,
	-- Benny Goodheart  James Cox
	
	Unix is distinguished by a simple,coherent,and lelgant design - truly remarkable features that have 
	enabled the system to influence the world for more than a quarter of a century.And especially thanks
	to the growing presence of Linux,the idea is still picking up momentum,which no end of the growth 
	in sight.
	Unix and Linux carry a certain fascination,and two quotations above hopefully capture the spirit of 
	this attraction.Consider Dennis Ritchie`s quote:Is the coinventor of Unix at Bell Labs completely right
	in saying that only a genius can appreciate the simplicity of Unix? Luckily not,because he puts himself
	into perspective immediately by adding that programmers also qualify to value the essence of Unix.
	Understanding the meagerly documented,demanding,and complex sources of Unix as of Linux 
	is not alwalys an easy task.But once has atarted to experience the rich insights that can be gained from
	the kernal sources,it is hard to escape the fascibation of Linux.It seems fair to warn you that it`s easy 
	to get addicted to the joy of the operating system kernel once starting to dive into it.This was already 
	noted by Benny Goodheart and James Cox,whose preface to their book the magic Garden Explained 
	(SECOND QUOTATION ABOVE)explained the internals of Unix System V.And Linux is definitely also capable 
	of helping you to lose your mind!
	This book acts as a guide and companion that takes you through the kernel sources and sharpens your
	awareness of the beauty,elegance,and --last but not least --esthetics of their concepts.There are,however,
	some prerequisites to foster an understanding of the kernel.C should not just be a letter;neither
	should it be a foreign language.Operating systems are supposed to be more than just a "Start"button,and 
	a small amount of algorithmics can also do no harm.finally,it is preferable if computer architecture is not
	just about how how to build the most fansy case.From an academic piont of view,this comes closest to the 
	lectures "Systems Programming,""Algorithmics"and "Fundamentals of Operating Systems"The previous
	edition of this book has been used to tesvh the fundamentals of Linux to advenced undergraduate
	stufents in several universities,and I hope that the current edition will serve the same purpose.
	Discussing all aforementioned in topics in details right now(or maybe you are not holding it,for this 
	very reason),you`ll surely agree that this would not be a good ides.When a topic not directly related to 










