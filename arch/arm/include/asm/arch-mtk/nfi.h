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
#define NFI_CON_AUTO_ECC_DEC_EN		(1 << 2)
#define NFI_CON_AUTO_ECC_ENC_EN		(1 << 3)
#define NFI_CON_MULTI_PAGE_RD_EN	(1 << 4)
#define NFI_CON_SPARE_EN		(1 << 5)
#define NFI_CON_DMA_PAUSE_EN		(1 << 6)
#define NFI_CON_SPARE_ECC_EN		(1 << 8)
#define NFI_CON_MAIN_ECC_EN		(1 << 9)
#define NFI_CON_BYTE_RW			(1 << 15)

#endif
