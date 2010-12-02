/*
 * (C) 2010 by Tieto <www.tieto.com>
 *	Marcin Mielczarczyk <marcin.mielczarczyk@tieto.com>
 *
 * All Rights Reserved
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

/*
 * U-Boot configuration is split in two parts:
 * - SPL configuration
 * - normal configuration
 *
 * MT62XX platform has IPL (Initial Program Loader) in ROM.
 * After processor power up IPL execution is started. IPL tries to load
 * code from NAND to internal RAM (64KB) and starts executing loaded code.
 * SBL (Secondary Program Loader) is loaded by IPL. SBL configures basic
 * peripherals (PLL, SDRAM memory) and loads U-Boot from NAND to SDRAM.
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#include "asm/arch-mtk/mt6235.h"

#define CONFIG_ARM926EJS

/* DRAM memory related configurations */
#define CONFIG_NR_DRAM_BANKS		1
#define PHYS_SDRAM_1			0x00000000
#define PHYS_SDRAM_1_SIZE		0x04000000 /* 64 MB */
#define CONFIG_SYS_SDRAM_BASE		PHYS_SDRAM_1

/* NAND memory related configurations */
#define CONFIG_NAND_MT62XX
#define CONFIG_SYS_MAX_NAND_DEVICE	1
#define CONFIG_SYS_NAND_BASE		MTK_NFI_BASE

/* There is no NOR flash, so undefine these commands */
#undef CONFIG_CMD_FLASH
#undef CONFIG_CMD_IMLS
#define CONFIG_SYS_NO_FLASH


#ifndef CONFIG_PRELOADER

/*
 * Configuration of U-Boot when running from DRAM (normal operation).
 */

/* We have already been loaded to RAM */
#define CONFIG_SKIP_LOWLEVEL_INIT

/* Serial port configuration */
#define CONFIG_MT62XX_SERIAL
#define CONFIG_MT62XX_PORTS		{0x81030000, 0x81040000, 0x81050000}
#define CONSOLE_PORT			0
#define CONFIG_BAUDRATE			115200
#define CONFIG_SYS_BAUDRATE_TABLE       {9600, 19200, 38400, 57600, 115200}

/* Turn on some U-Boot commands */
#define CONFIG_CMD_NAND
#define CONFIG_CMD_MEMORY
#define CONFIG_CMD_LOADB
#define CONFIG_CMD_RUN
#define CONFIG_CMD_SAVEENV
#define CONFIG_CMD_MTDPARTS
#define CONFIG_CMDLINE_TAG

/* Timing configuration */
#define CONFIG_SYS_HZ			1000
#define CONFIG_SYS_TIMERBASE		MTK_GPT_BASE

/* Support for command auto completion */
#define CONFIG_AUTO_COMPLETE

/* User interface */
#define CONFIG_SYS_LONGHELP
#define CONFIG_SYS_HUSH_PARSER
#define CONFIG_SYS_PROMPT		"Sciphone> "
#define CONFIG_SYS_PROMPT_HUSH_PS2	"> "
#define CONFIG_CMDLINE_EDITING
#define CONFIG_SYS_CBSIZE		256
#define CONFIG_SYS_PBSIZE		(CONFIG_SYS_CBSIZE \
					+ sizeof(CONFIG_SYS_PROMPT) + 16)
#define CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE
#define CONFIG_SYS_MAXARGS		16
#define CONFIG_SYS_LOAD_ADDR		0x800000
#define CONFIG_SYS_LOADS_BAUD_CHANGE

#define CONFIG_ENV_IS_IN_NAND
#define CONFIG_ENV_SIZE			0x20000	/* 128 KB - one block */
#define CONFIG_ENV_OFFSET		0x20000

#define CONFIG_BOOTDELAY		1
#define CONFIG_BOOTARGS			"console=ttyMTK0,115200n8 mem=64M@0"
#define CONFIG_BOOTCOMMAND		\
"nand read 0x20000 kernel; bootm 0x20000"

#define CONFIG_MTD_DEVICE
#define CONFIG_MTD_PARTITIONS
#define MTDIDS_DEFAULT			"nand0=mt62xx_nand.0"
#define MTDPARTS_DEFAULT		\
"mtdparts=mt62xx_nand.0:128k(sbl),128k(env),2M(u-boot),2M(kernel),-(root)"

#define CONFIG_STACKSIZE		(128 * 1024)
#ifdef CONFIG_USE_IRQ
#  define CONFIG_STACKSIZE_IRQ		(4 * 1024)	/* IRQ stack */
#  define CONFIG_STACKSIZE_FIQ		(4 * 1024)	/* FIQ stack */
#endif

#define CONFIG_SYS_MEMTEST_START	0x00000000
#define CONFIG_SYS_MEMTEST_END		0x02FFFFFF
#define CONFIG_SYS_GBL_DATA_SIZE	128
#define CONFIG_SYS_MALLOC_LEN		(CONFIG_ENV_SIZE + 256 * 1024)

#define CONFIG_MAX_RAM_BANK_SIZE	0x10000000
#define CONFIG_SYS_INIT_SP_ADDR		(CONFIG_SYS_SDRAM_BASE + CONFIG_STACKSIZE)

/* This is needed to make hello_world.c happy */
#define CONFIG_SYS_MAX_FLASH_SECT	512
#define CONFIG_SYS_MAX_FLASH_BANKS	1

/*
 * Below option allows U-Boot to save BBT table in NAND.
 * Without this option BBT table is created everytime when first nand
 * command is executed (except "nand dump"). Full scanning of NAND
 * takes long time and unnecessarily delays start of platform.
 *
 * NOTE! This option is disabled by defaut as at startup it deletes last
 * two blocks of NAND. Most of people run code from RAM and don't have
 * NAND memory dumped yet. If you don't like to wait on first nand
 * command, you should enable below option.
 */
//#define MT62XX_NAND_BBT_IN_NAND

/* Enable support for mmc. */
#define CONFIG_MMC			1

#ifdef CONFIG_MMC

/*
 * The MMC/SD support for is done through the Generic MMC framework
 * of u-boot. Following options shall be enabled in the default configuration
 * to include MMC/SD support.
 */
#define CONFIG_GENERIC_MMC
#define CONFIG_CMD_MMC
#define CONFIG_MTK_MMC

/*
 * To include File system support for MMC, following configuration options
 * shall be enabled
 */
#define CONFIG_DOS_PARTITION
#define CONFIG_CMD_EXT2
#define CONFIG_CMD_FAT

#endif /* CONFIG_MMC */

#else /* CONFIG_PRELOADER */

/*
 * Configuration of U-Boot for SPL.
 */
#define CONFIG_STACKSIZE		1024
#define CONFIG_SYS_MALLOC_LEN		1024
#define INTERNAL_RAM_BASE		0x40000000
#define CONFIG_SYS_INIT_SP_ADDR		(INTERNAL_RAM_BASE + CONFIG_STACKSIZE)
#define CONFIG_ENV_SIZE			0x0

/* Address of U-Boot in NAND */
#define CONFIG_SYS_NAND_U_BOOT_OFFS	0x40000
#define CONFIG_SYS_NAND_U_BOOT_SIZE	1000000		/* 1MB */
#define CONFIG_SYS_NAND_U_BOOT_DST	0x500000
#define CONFIG_SYS_NAND_U_BOOT_START	0x500000

#endif /* CONFIG_PRELOADER */
#endif /* __CONFIG_H */
