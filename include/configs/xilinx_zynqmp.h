/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Configuration for Xilinx ZynqMP
 * (C) Copyright 2014 - 2015 Xilinx, Inc.
 * Michal Simek <michal.simek@xilinx.com>
 *
 * Based on Configuration for Versatile Express
 */

#ifndef __XILINX_ZYNQMP_H
#define __XILINX_ZYNQMP_H

#define CONFIG_REMAKE_ELF

/* #define CONFIG_ARMV8_SWITCH_TO_EL1 */

/* Generic Interrupt Controller Definitions */
#define CONFIG_GICV2
#define GICD_BASE	0xF9010000
#define GICC_BASE	0xF9020000

#ifndef CONFIG_SYS_MEMTEST_SCRATCH
# define CONFIG_SYS_MEMTEST_SCRATCH	0x10800000
#endif

#define CONFIG_SYS_MEMTEST_START	0
#define CONFIG_SYS_MEMTEST_END		1000

#define CONFIG_SYS_INIT_SP_ADDR		CONFIG_SYS_TEXT_BASE

/* Generic Timer Definitions - setup in EL3. Setup by ATF for other cases */
#if !defined(COUNTER_FREQUENCY)
# define COUNTER_FREQUENCY		100000000
#endif

/* Size of malloc() pool */
#define CONFIG_SYS_MALLOC_LEN		(CONFIG_ENV_SIZE + 0x2000000)

/* Serial setup */
#define CONFIG_ARM_DCC
#define CONFIG_CPU_ARMV8

#define CONFIG_SYS_BAUDRATE_TABLE \
	{ 4800, 9600, 19200, 38400, 57600, 115200 }

/* BOOTP options */
#define CONFIG_BOOTP_BOOTFILESIZE
#define CONFIG_BOOTP_MAY_FAIL

#if defined(CONFIG_MMC_SDHCI_ZYNQ)
# define CONFIG_SUPPORT_EMMC_BOOT
#endif

#ifdef CONFIG_NAND_ARASAN
# define CONFIG_SYS_MAX_NAND_DEVICE	1
# define CONFIG_SYS_NAND_ONFI_DETECTION
#endif

#if !defined(CONFIG_SPL_BUILD)
#if defined(CONFIG_ZYNQMP_GQSPI)
/* SPI layer registers with MTD */
#define CONFIG_SPI_FLASH_MTD
#endif
#endif

#if defined(CONFIG_SPL_BUILD)
#define CONFIG_ZYNQMP_PSU_INIT_ENABLED
#endif

/* Miscellaneous configurable options */
#define CONFIG_SYS_LOAD_ADDR		0x8000000

#if defined(CONFIG_ZYNQMP_USB)
#define CONFIG_SYS_DFU_DATA_BUF_SIZE	0x1800000
#define DFU_DEFAULT_POLL_TIMEOUT	300
#define CONFIG_USB_CABLE_CHECK
#define CONFIG_THOR_RESET_OFF
#define DFU_ALT_INFO_RAM \
	"dfu_ram_info=" \
	"setenv dfu_alt_info " \
	"Image ram $kernel_addr $kernel_size\\\\;" \
	"system.dtb ram $fdt_addr $fdt_size\0" \
	"dfu_ram=run dfu_ram_info && dfu 0 ram 0\0" \
	"thor_ram=run dfu_ram_info && thordown 0 ram 0\0"

#define DFU_ALT_INFO  \
		DFU_ALT_INFO_RAM

#ifndef CONFIG_SPL_BUILD
# define PARTS_DEFAULT \
	"partitions=uuid_disk=${uuid_gpt_disk};" \
	"name=""boot"",size=16M,uuid=${uuid_gpt_boot};" \
	"name=""Linux"",size=-M,uuid=${uuid_gpt_Linux}\0"
#endif
#endif

#if !defined(DFU_ALT_INFO)
# define DFU_ALT_INFO
#endif

#if !defined(PARTS_DEFAULT)
# define PARTS_DEFAULT
#endif

/* Xilinx initial environment variables */
#ifndef CONFIG_EXTRA_ENV_BOARD_SETTINGS
#define CONFIG_EXTRA_ENV_BOARD_SETTINGS \
	"kernel_addr=0x80000\0" \
	"initrd_addr=0xa00000\0" \
	"initrd_size=0x2000000\0" \
	"fdt_addr=4000000\0" \
	"fdt_high=0x10000000\0" \
	"loadbootenv_addr=0x100000\0" \
	"sdbootdev=0\0"\
	"kernel_offset=0x280000\0" \
	"fdt_offset=0x200000\0" \
	"kernel_size=0x1e00000\0" \
	"fdt_size=0x80000\0" \
	"bootenv=uEnv.txt\0" \
	"partid=auto\0" \
	"loadbootenv=load mmc $sdbootdev:$partid ${loadbootenv_addr} ${bootenv}\0" \
	"importbootenv=echo Importing environment from SD ...; " \
		"env import -t ${loadbootenv_addr} $filesize\0" \
	"sd_uEnvtxt_existence_test=test -e mmc $sdbootdev:$partid /uEnv.txt\0" \
	"sata_root=if test $scsidevs -gt 0; then setenv bootargs $bootargs root=/dev/sda rw rootfstype=ext4; fi\0" \
	"sataboot=run xilinxcmd && load scsi 0 80000 boot/Image && load scsi 0 $fdt_addr boot/system.dtb && booti 80000 - $fdt_addr\0" \
	"netboot=run xilinxcmd && tftpboot 10000000 image.ub && bootm\0" \
	"qspiboot=run xilinxcmd && sf probe 0 0 0 && sf read $fdt_addr $fdt_offset $fdt_size && " \
		  "sf read $kernel_addr $kernel_offset $kernel_size && " \
		  "booti $kernel_addr - $fdt_addr\0" \
	"uenvboot=" \
		"if run sd_uEnvtxt_existence_test; then " \
			"run loadbootenv; " \
			"echo Loaded environment from ${bootenv}; " \
			"run importbootenv; " \
		"fi; " \
		"if test -n $uenvcmd; then " \
			"echo Running uenvcmd ...; " \
			"run uenvcmd; " \
		"fi\0" \
	"sdboot=run xilinxcmd && mmc dev $sdbootdev && mmcinfo && run uenvboot || run sdroot$sdbootdev; " \
		"load mmc $sdbootdev:$partid $fdt_addr system.dtb && " \
		"load mmc $sdbootdev:$partid $kernel_addr Image && " \
		"booti $kernel_addr - $fdt_addr\0" \
	"emmcboot=run sdboot\0" \
	"nandboot=run xilinxcmd && nand info && nand read $fdt_addr $fdt_offset $fdt_size && " \
		  "nand read $kernel_addr $kernel_offset $kernel_size && " \
		  "booti $kernel_addr - $fdt_addr\0" \
	"xen_prepare_dt=fdt addr $fdt_addr && fdt resize 128 && " \
		"fdt set /chosen \\\\#address-cells <1> && " \
		"fdt set /chosen \\\\#size-cells <1> && " \
		"fdt mknod /chosen dom0 && " \
		"fdt set /chosen/dom0 compatible \"xen,linux-zimage\" \"xen,multiboot-module\" && " \
		"fdt set /chosen/dom0 reg <0x80000 0x$filesize> && " \
		"fdt set /chosen xen,xen-bootargs \"console=dtuart dtuart=serial0 dom0_mem=768M bootscrub=0 maxcpus=1 timer_slop=0\" && " \
		"fdt set /chosen xen,dom0-bootargs \"console=hvc0 earlycon=xen earlyprintk=xen maxcpus=1 clk_ignore_unused\"\0" \
	"xen_prepare_dt_qemu=run xen_prepare_dt && " \
		"fdt set /cpus/cpu@1 device_type \"none\" && " \
		"fdt set /cpus/cpu@2 device_type \"none\" && " \
		"fdt set /cpus/cpu@3 device_type \"none\" && " \
		"fdt rm /cpus/cpu@1 compatible && " \
		"fdt rm /cpus/cpu@2 compatible && " \
		"fdt rm /cpus/cpu@3 compatible\0" \
	"xen=run xilinxcmd && tftpb $fdt_addr system.dtb &&  tftpb 0x80000 Image &&" \
		"run xen_prepare_dt && " \
		"tftpb 6000000 xen.ub && tftpb 0x1000000 image.ub && " \
		"bootm 6000000 0x1000000 $fdt_addr\0" \
	"xen_qemu=run xilinxcmd && tftpb $fdt_addr system.dtb && tftpb 0x80000 Image && " \
		"run xen_prepare_dt_qemu && " \
		"tftpb 6000000 xen.ub && tftpb 0x1000000 image.ub && " \
		"bootm 6000000 0x1000000 $fdt_addr\0" \
	"jtagboot=run xilinxcmd && tftpboot 80000 Image && tftpboot $fdt_addr system.dtb && " \
		 "tftpboot 6000000 rootfs.cpio.ub && booti 80000 6000000 $fdt_addr\0" \
	"nosmp=setenv bootargs $bootargs maxcpus=1\0" \
	"nfsroot=setenv bootargs $bootargs root=/dev/nfs nfsroot=$serverip:/mnt/sata,tcp ip=$ipaddr:$serverip:$serverip:255.255.255.0:zynqmp:eth0:off rw\0" \
	"sdroot0=setenv bootargs $bootargs root=/dev/mmcblk0p2 rw rootwait\0" \
	"sdroot1=setenv bootargs $bootargs root=/dev/mmcblk1p2 rw rootwait\0" \
	"android=setenv bootargs $bootargs init=/init androidboot.selinux=disabled androidboot.hardware=$board\0" \
	"android_debug=run android && setenv bootargs $bootargs video=DP-1:1024x768@60 drm.debug=0xf\0" \
	"usb_dfu_spl=booti $kernel_addr - $fdt_addr\0" \
	"usbhostboot=usb start && load usb 0 $fdt_addr system.dtb && " \
		     "load usb 0 $kernel_addr Image && " \
		     "booti $kernel_addr - $fdt_addr\0" \
	"xilinxcmd=echo !!! && echo !!! Booting cmd is deprecated (will be removed in 2020). && echo !!! Please move to distro bootcmd. && echo !!!\0" \
	PARTS_DEFAULT
#endif

/* Monitor Command Prompt */
/* Console I/O Buffer Size */
#define CONFIG_SYS_CBSIZE		2048
#define CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE
#define CONFIG_PANIC_HANG
#define CONFIG_SYS_MAXARGS		64

/* Ethernet driver */
#if defined(CONFIG_ZYNQ_GEM)
# define CONFIG_SYS_FAULT_ECHO_LINK_DOWN
# define PHY_ANEG_TIMEOUT       20000
#endif

#define CONFIG_SYS_BOOTM_LEN	(60 * 1024 * 1024)

#define CONFIG_CLOCKS

#define ENV_MEM_LAYOUT_SETTINGS \
	"fdt_high=10000000\0" \
	"initrd_high=10000000\0" \
	"fdt_addr_r=0x40000000\0" \
	"pxefile_addr_r=0x10000000\0" \
	"kernel_addr_r=0x18000000\0" \
	"scriptaddr=0x02000000\0" \
	"ramdisk_addr_r=0x02100000\0" \
	"script_offset_f=0x3e80000\0" \
	"script_size_f=0x80000\0" \

#if defined(CONFIG_MMC_SDHCI_ZYNQ)
# define BOOT_TARGET_DEVICES_MMC(func)	func(MMC, mmc, 0) func(MMC, mmc, 1)
#else
# define BOOT_TARGET_DEVICES_MMC(func)
#endif

#if defined(CONFIG_SATA_CEVA)
# define BOOT_TARGET_DEVICES_SCSI(func)	func(SCSI, scsi, 0)
#else
# define BOOT_TARGET_DEVICES_SCSI(func)
#endif

#if defined(CONFIG_ZYNQMP_USB)
# define BOOT_TARGET_DEVICES_USB(func)	func(USB, usb, 0) func(USB, usb, 1)
#else
# define BOOT_TARGET_DEVICES_USB(func)
#endif

#if defined(CONFIG_CMD_PXE) && defined(CONFIG_CMD_DHCP)
# define BOOT_TARGET_DEVICES_PXE(func)	func(PXE, pxe, na)
#else
# define BOOT_TARGET_DEVICES_PXE(func)
#endif

#if defined(CONFIG_CMD_DHCP)
# define BOOT_TARGET_DEVICES_DHCP(func)	func(DHCP, dhcp, na)
#else
# define BOOT_TARGET_DEVICES_DHCP(func)
#endif

#if defined(CONFIG_ZYNQMP_GQSPI)
# define BOOT_TARGET_DEVICES_QSPI(func)	func(QSPI, qspi, 0)
#else
# define BOOT_TARGET_DEVICES_QSPI(func)
#endif

#if defined(CONFIG_NAND_ARASAN)
# define BOOT_TARGET_DEVICES_NAND(func)	func(NAND, nand, 0)
#else
# define BOOT_TARGET_DEVICES_NAND(func)
#endif

#define BOOTENV_DEV_XILINX(devtypeu, devtypel, instance) \
	"bootcmd_xilinx=run $modeboot\0"

#define BOOTENV_DEV_NAME_XILINX(devtypeu, devtypel, instance) \
	"xilinx "

#define BOOTENV_DEV_QSPI(devtypeu, devtypel, instance) \
	"bootcmd_" #devtypel #instance "=sf probe " #instance " 0 0 && " \
		       "sf read $scriptaddr $script_offset_f $script_size_f && " \
		       "source ${scriptaddr}; echo SCRIPT FAILED: continuing...;\0"

#define BOOTENV_DEV_NAME_QSPI(devtypeu, devtypel, instance) \
	#devtypel #instance " "

#define BOOTENV_DEV_NAND(devtypeu, devtypel, instance) \
	"bootcmd_" #devtypel #instance "= nand info && " \
		       "nand read $scriptaddr $script_offset_f $script_size_f && " \
		       "source ${scriptaddr}; echo SCRIPT FAILED: continuing...;\0"

#define BOOTENV_DEV_NAME_NAND(devtypeu, devtypel, instance) \
	#devtypel #instance " "

#define BOOT_TARGET_DEVICES(func) \
	BOOT_TARGET_DEVICES_MMC(func) \
	BOOT_TARGET_DEVICES_QSPI(func) \
	BOOT_TARGET_DEVICES_NAND(func) \
	BOOT_TARGET_DEVICES_USB(func) \
	BOOT_TARGET_DEVICES_SCSI(func) \
	BOOT_TARGET_DEVICES_PXE(func) \
	BOOT_TARGET_DEVICES_DHCP(func) \
	func(XILINX, xilinx, na)

#include <config_distro_bootcmd.h>

/* Initial environment variables */
#ifndef CONFIG_EXTRA_ENV_SETTINGS
#define CONFIG_EXTRA_ENV_SETTINGS \
	CONFIG_EXTRA_ENV_BOARD_SETTINGS \
	ENV_MEM_LAYOUT_SETTINGS \
	BOOTENV \
	DFU_ALT_INFO
#endif

/* SPL can't handle all huge variables - define just DFU */
#if defined(CONFIG_SPL_BUILD) && defined(CONFIG_SPL_DFU_SUPPORT)
#undef CONFIG_EXTRA_ENV_SETTINGS
# define CONFIG_EXTRA_ENV_SETTINGS \
	"dfu_alt_info_ram=uboot.bin ram 0x8000000 0x1000000;" \
			  "atf-uboot.ub ram 0x10000000 0x1000000;" \
			  "Image ram 0x80000 0x3f80000;" \
			  "system.dtb ram 0x4000000 0x100000\0" \
	"dfu_bufsiz=0x1000\0"
#endif

#define CONFIG_SPL_TEXT_BASE		0xfffc0000
#define CONFIG_SPL_STACK		0xfffffffc
#define CONFIG_SPL_MAX_SIZE		0x40000

/* Just random location in OCM */
#define CONFIG_SPL_BSS_START_ADDR	0x0
#define CONFIG_SPL_BSS_MAX_SIZE		0x80000

#if defined(CONFIG_SPL_SPI_FLASH_SUPPORT)
# define CONFIG_SYS_SPI_KERNEL_OFFS	0x80000
# define CONFIG_SYS_SPI_ARGS_OFFS	0xa0000
# define CONFIG_SYS_SPI_ARGS_SIZE	0xa0000

# define CONFIG_SYS_SPI_U_BOOT_OFFS	0x170000
#endif

/* u-boot is like dtb */
#define CONFIG_SPL_FS_LOAD_ARGS_NAME	"u-boot.bin"
#define CONFIG_SYS_SPL_ARGS_ADDR	0x8000000

/* ATF is my kernel image */
#define CONFIG_SPL_FS_LOAD_KERNEL_NAME	"atf-uboot.ub"

/* FIT load address for RAM boot */
#define CONFIG_SPL_LOAD_FIT_ADDRESS	0x10000000

/* MMC support */
#ifdef CONFIG_MMC_SDHCI_ZYNQ
# define CONFIG_SYS_MMCSD_FS_BOOT_PARTITION	1
# define CONFIG_SYS_MMCSD_RAW_MODE_ARGS_SECTOR	0 /* unused */
# define CONFIG_SYS_MMCSD_RAW_MODE_ARGS_SECTORS	0 /* unused */
# define CONFIG_SYS_MMCSD_RAW_MODE_KERNEL_SECTOR	0 /* unused */
# define CONFIG_SPL_FS_LOAD_PAYLOAD_NAME	"u-boot.img"
#endif

#if defined(CONFIG_SPL_BUILD) && defined(CONFIG_SPL_DFU_SUPPORT)
# undef CONFIG_CMD_BOOTD
# define CONFIG_SPL_ENV_SUPPORT
# define CONFIG_SPL_HASH_SUPPORT
# define CONFIG_ENV_MAX_ENTRIES	10
#endif

#define CONFIG_SYS_SPL_MALLOC_START	0x20000000
#define CONFIG_SYS_SPL_MALLOC_SIZE	0x100000

#ifdef CONFIG_SPL_SYS_MALLOC_SIMPLE
# error "Disable CONFIG_SPL_SYS_MALLOC_SIMPLE. Full malloc needs to be used"
#endif

#define CONFIG_BOARD_EARLY_INIT_F

#endif /* __XILINX_ZYNQMP_H */
