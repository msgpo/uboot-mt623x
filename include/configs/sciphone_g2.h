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

#ifndef __CONFIG_H
#define __CONFIG_H

#include "asm/arch-mtk/mt6235.h"

#define CONFIG_ARM926EJS

/* We have already been loaded to RAM */
#define CONFIG_SKIP_LOWLEVEL_INIT

#define CONFIG_ENV_IS_NOWHERE
#define CONFIG_CMDLINE_TAG

/* Serial port configuration */
#define CONFIG_MT62XX_SERIAL
#define CONFIG_MT62XX_PORTS		{0x81030000, 0x81040000, 0x81050000}
#define CONSOLE_PORT			0
#define CONFIG_BAUDRATE			115200
#define CONFIG_SYS_BAUDRATE_TABLE       {9600, 19200, 38400, 57600, 115200}

/* There is no NOR flash, so undefine these commands */
#undef CONFIG_CMD_FLASH
#undef CONFIG_CMD_IMLS
#define CONFIG_SYS_NO_FLASH

/* Configure NAND storage */
#define CONFIG_NAND_MT62XX
#define CONFIG_CMD_NAND
#define CONFIG_SYS_MAX_NAND_DEVICE	1
#define CONFIG_SYS_NAND_BASE		MTK_NFI_BASE
#define CONFIG_CMD_MEMORY
#define CONFIG_CMD_LOADB
#define CONFIG_CMD_RUN

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

#define CONFIG_ENV_SIZE			0x20000 /* 128 Kb - one sector */
#define CONFIG_ENV_ADDR			(0x00280000 - CONFIG_ENV_SIZE)
#define CONFIG_BOOTDELAY		1
#define CONFIG_BOOTARGS			"console=ttyMTK0,115200n8 mem=64M@0"
#define CONFIG_BOOTCOMMAND		"bootm 0x800000"

/* Memory related information */
#define CONFIG_NR_DRAM_BANKS		1
#define PHYS_SDRAM_1			0x00000000
#define PHYS_SDRAM_1_SIZE		0x04000000 /* 64 MB */
#define CONFIG_SYS_SDRAM_BASE		PHYS_SDRAM_1
#define CONFIG_SYS_INIT_SP_ADDR		(CONFIG_SYS_SDRAM_BASE + 0x1000)
#define CONFIG_MAX_RAM_BANK_SIZE	0x10000000

#define CONFIG_STACKSIZE		(128 * 1024)
#ifdef CONFIG_USE_IRQ
#  define CONFIG_STACKSIZE_IRQ		(4 * 1024)	/* IRQ stack */
#  define CONFIG_STACKSIZE_FIQ		(4 * 1024)	/* FIQ stack */
#endif

#define CONFIG_SYS_MEMTEST_START	0x00000000
#define CONFIG_SYS_MEMTEST_END		0x02FFFFFF
#define CONFIG_SYS_MALLOC_LEN		(CONFIG_ENV_SIZE + 256 * 1024)
#define CONFIG_SYS_GBL_DATA_SIZE	128

/* This is needed to make hello_world.c happy */
#define CONFIG_SYS_MAX_FLASH_SECT	512
#define CONFIG_SYS_MAX_FLASH_BANKS	1

/* Enable support for mmc. */
#define CONFIG_MMC			1

#ifndef DEBUG
/* #define DEBUG */
#endif

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

#endif /* __CONFIG_H */
