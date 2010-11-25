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

#ifndef __MTK_NFI_H_
#define __MTK_NFI_H_

/* Nand Flash Interface register definitions */
#define MTK_NFI_ACCCON		(MTK_NFI_BASE + 0x00)
#define MTK_NFI_PAGEFMT		(MTK_NFI_BASE + 0x04)
#define MTK_NFI_OPCON		(MTK_NFI_BASE + 0x08)
#define MTK_NFI_CMD		(MTK_NFI_BASE + 0x10)
#define MTK_NFI_ADDRNOB		(MTK_NFI_BASE + 0x20)
#define MTK_NFI_ADDRL		(MTK_NFI_BASE + 0x24)
#define MTK_NFI_ADDRM		(MTK_NFI_BASE + 0x28)
#define MTK_NFI_DATAW		(MTK_NFI_BASE + 0x30)
#define MTK_NFI_DATAWB		(MTK_NFI_BASE + 0x34)
#define MTK_NFI_DATAR		(MTK_NFI_BASE + 0x38)
#define MTK_NFI_DATARB		(MTK_NFI_BASE + 0x3C)
#define MTK_NFI_PSTA		(MTK_NFI_BASE + 0x40)
#define MTK_NFI_FIFOSTA		(MTK_NFI_BASE + 0x44)
#define MTK_NFI_CON		(MTK_NFI_BASE + 0x60)
#define MTK_NFI_INTR_EN		(MTK_NFI_BASE + 0x68)
#define MTK_NFI_PAGECNTR	(MTK_NFI_BASE + 0x70)
#define MTK_NFI_ADDRCNTR	(MTK_NFI_BASE + 0x74)
#define MTK_NFI_SYM0_ADDR	(MTK_NFI_BASE + 0x80)
#define MTK_NFI_SYM1_ADDR	(MTK_NFI_BASE + 0x84)
#define MTK_NFI_SYM2_ADDR	(MTK_NFI_BASE + 0x88)
#define MTK_NFI_SYM3_ADDR	(MTK_NFI_BASE + 0x8C)
#define MTK_NFI_SYM4_ADDR	(MTK_NFI_BASE + 0x90)
#define MTK_NFI_SYM5_ADDR	(MTK_NFI_BASE + 0x94)
#define MTK_NFI_SYM6_ADDR	(MTK_NFI_BASE + 0x98)
#define MTK_NFI_SYM7_ADDR	(MTK_NFI_BASE + 0x9C)
#define MTK_NFI_SYMS0_ADDR	(MTK_NFI_BASE + 0xA0)
#define MTK_NFI_SYMS1_ADDR	(MTK_NFI_BASE + 0xA4)
#define MTK_NFI_SYMS2_ADDR	(MTK_NFI_BASE + 0xA8)
#define MTK_NFI_SYMS3_ADDR	(MTK_NFI_BASE + 0xAC)
#define MTK_NFI_SYM0_DATA	(MTK_NFI_BASE + 0xB0)
#define MTK_NFI_SYM1_DATA	(MTK_NFI_BASE + 0xB4)
#define MTK_NFI_SYM2_DATA	(MTK_NFI_BASE + 0xB8)
#define MTK_NFI_SYM3_DATA	(MTK_NFI_BASE + 0xBC)
#define MTK_NFI_SYM4_DATA	(MTK_NFI_BASE + 0xC0)
#define MTK_NFI_SYM5_DATA	(MTK_NFI_BASE + 0xC4)
#define MTK_NFI_SYM6_DATA	(MTK_NFI_BASE + 0xC8)
#define MTK_NFI_SYM7_DATA	(MTK_NFI_BASE + 0xCC)
#define MTK_NFI_SYMS0_DATA	(MTK_NFI_BASE + 0xD0)
#define MTK_NFI_SYMS1_DATA	(MTK_NFI_BASE + 0xD4)
#define MTK_NFI_SYMS2_DATA	(MTK_NFI_BASE + 0xD8)
#define MTK_NFI_SYMS3_DATA	(MTK_NFI_BASE + 0xDC)
#define MTK_NFI_PAR_0P		(MTK_NFI_BASE + 0xE0)
#define MTK_NFI_PAR_0C		(MTK_NFI_BASE + 0xE4)
#define MTK_NFI_PAR_1P		(MTK_NFI_BASE + 0xE8)
#define MTK_NFI_PAR_1C		(MTK_NFI_BASE + 0xEC)
#define MTK_NFI_PAR_2P		(MTK_NFI_BASE + 0xF0)
#define MTK_NFI_PAR_2C		(MTK_NFI_BASE + 0xF4)
#define MTK_NFI_PAR_3P		(MTK_NFI_BASE + 0xF8)
#define MTK_NFI_PAR_3C		(MTK_NFI_BASE + 0xFC)
#define MTK_NFI_PAR_4P		(MTK_NFI_BASE + 0x100)
#define MTK_NFI_PAR_4C		(MTK_NFI_BASE + 0x104)
#define MTK_NFI_PAR_5P		(MTK_NFI_BASE + 0x108)
#define MTK_NFI_PAR_5C		(MTK_NFI_BASE + 0x10C)
#define MTK_NFI_PAR_6P		(MTK_NFI_BASE + 0x110)
#define MTK_NFI_PAR_6C		(MTK_NFI_BASE + 0x114)
#define MTK_NFI_PAR_7P		(MTK_NFI_BASE + 0x118)
#define MTK_NFI_PAR_7C		(MTK_NFI_BASE + 0x11C)
#define MTK_NFI_PARS_0P		(MTK_NFI_BASE + 0x120)
#define MTK_NFI_PARS_0C		(MTK_NFI_BASE + 0x124)
#define MTK_NFI_PARS_1P		(MTK_NFI_BASE + 0x128)
#define MTK_NFI_PARS_1C		(MTK_NFI_BASE + 0x12C)
#define MTK_NFI_PARS_2P		(MTK_NFI_BASE + 0x130)
#define MTK_NFI_PARS_2C		(MTK_NFI_BASE + 0x134)
#define MTK_NFI_PARS_3P		(MTK_NFI_BASE + 0x138)
#define MTK_NFI_PARS_3C		(MTK_NFI_BASE + 0x13C)
#define MTK_NFI_ERRDET		(MTK_NFI_BASE + 0x140)
#define MTK_NFI_PARERR		(MTK_NFI_BASE + 0x144)
#define MTK_NFI_SCON		(MTK_NFI_BASE + 0x148)
#define MTK_NFI_CSEL		(MTK_NFI_BASE + 0x200)
#define MTK_NFI_IOCON		(MTK_NFI_BASE + 0x204)

/* NFI_ACCCON bit fields definitions */
#define NFI_ACCCON_RLT_SHIFT		(0)
#define NFI_ACCCON_WST_SHIFT		(4)
#define NFI_ACCCON_WH_SHIFT		(8)
#define NFI_ACCCON_W2R_SHIFT		(12)
#define NFI_ACCCON_C2R_SHIFT		(16)
#define NFI_ACCCON_LCD2NAND_SHIFT	(28)

/* NFI_PAGEFMT bit fields definitions */
#define NFI_PAGEFMT_PSIZE_512		(0)
#define NFI_PAGEFMT_PSIZE_2048		(1 << 0)
#define NFI_PAGEFMT_ADRMODE_NORMAL	(0 << 2)
#define NFI_PAGEFMT_ADRMODE_LARGE_8IO	(1 << 2)
#define NFI_PAGEFMT_ADRMODE_LARGE_16IO	(2 << 2)
#define NFI_PAGEFMT_ECCBLKSIZE_128	(0 << 4)
#define NFI_PAGEFMT_ECCBLKSIZE_256	(1 << 4)
#define NFI_PAGEFMT_ECCBLKSIZE_512	(2 << 4)
#define NFI_PAGEFMT_ECCBLKSIZE_1024	(3 << 4)
#define NFI_PAGEFMT_B16EN		(1 << 8)

/* NFI_OPCON bit fields definitions */
#define NFI_OPCON_BRD		(1 << 0)
#define NFI_OPCON_BWR		(1 << 1)
#define NFI_OPCON_SRD		(1 << 8)
#define NFI_OPCON_FIFO_FLUSH	(1 << 10)
#define NFI_OPCON_FIFO_RST	(1 << 11)
#define NFI_OPCON_NOB		(1 << 12)

/* NFI_PSTA bit fields definitions */
#define NFI_PSTA_CMD		(1 << 0)
#define NFI_PSTA_ADDR		(1 << 1)
#define NFI_PSTA_DATAR		(1 << 2)
#define NFI_PSTA_DATAW		(1 << 3)
#define NFI_PSTA_BUSY		(1 << 8)
#define NFI_PSTA_NAND_BUSY	(1 << 9)

/* NFI_FIFOSTA bit fields definitions */
#define NFI_FIFOSTA_RD_EMPTY		(1 << 6)
#define NFI_FIFOSTA_RD_FULL		(1 << 7)
#define NFI_FIFOSTA_WR_EMPTY		(1 << 14)
#define NFI_FIFOSTA_WR_FULL		(1 << 15)

/* NFI_CON bit fields definitions */
#define NFI_CON_DMA_RD_EN		(1 << 0)
#define NFI_CON_DMA_WR_EN		(1 << 1)
#define NFI_CON_AUTOECC_DEC_EN		(1 << 2)
#define NFI_CON_AUTOECC_ENC_EN		(1 << 3)
#define NFI_CON_MULTI_PAGE_RD_EN	(1 << 4)
#define NFI_CON_SPARE_EN		(1 << 5)
#define NFI_CON_DMA_PAUSE_EN		(1 << 6)
#define NFI_CON_SPARE_ECC_EN		(1 << 8)
#define NFI_CON_MAIN_ECC_EN		(1 << 9)
#define NFI_CON_BYTE_RW			(1 << 15)

#endif
