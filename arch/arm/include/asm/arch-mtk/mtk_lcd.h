/*
 * (C) 2010 by Tieto <www.tieto.com>
 *	Krzysztof Antonowicz <krzysztof.antonowicz@tieto.com>
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

#ifndef __MTK_LCD_H
#define __MTK_LCD_H

/* LCD controller registry definitions. */
#define LCD_STA					(MTK_LCD_BASE + 0x0000)
#define LCD_INTEN				(MTK_LCD_BASE + 0x0004)
#define LCD_INTSTA				(MTK_LCD_BASE + 0x0008)
#define LCD_START				(MTK_LCD_BASE + 0x000c)
#define LCD_RSTB				(MTK_LCD_BASE + 0x0010)
#define LCD_SCNF				(MTK_LCD_BASE + 0x0014)
#define LCD_PCNF0				(MTK_LCD_BASE + 0x0018)
#define LCD_PCNF1				(MTK_LCD_BASE + 0x001c)
#define LCD_PCNF2				(MTK_LCD_BASE + 0x0020)
#define LCD_TECON				(MTK_LCD_BASE + 0x0024)
#define LCD_MWINSIZE				(MTK_LCD_BASE + 0x0040)
#define LCD_WROI_W2MOFS				(MTK_LCD_BASE + 0x0044)
#define LCD_WROI_W2MCON				(MTK_LCD_BASE + 0x0048)
#define LCD_WROI_W2MADD				(MTK_LCD_BASE + 0x004c)
#define LCD_WROICON				(MTK_LCD_BASE + 0x0050)
#define LCD_WROIOFS				(MTK_LCD_BASE + 0x0054)
#define LCD_WROICADD				(MTK_LCD_BASE + 0x0058)
#define LCD_WROIDADD				(MTK_LCD_BASE + 0x005c)
#define LCD_WROISIZE				(MTK_LCD_BASE + 0x0060)
#define LCD_WROI_HWREF				(MTK_LCD_BASE + 0x0064)
#define LCD_WROI_DC				(MTK_LCD_BASE + 0x0068)
#define LCD_WROI_BGCLR				(MTK_LCD_BASE + 0x006C)
#define LCD_L0WINCON				(MTK_LCD_BASE + 0x0070)
#define LCD_L0WINSKEY				(MTK_LCD_BASE + 0x0074)
#define LCD_L0WINOFS				(MTK_LCD_BASE + 0x0078)
#define LCD_L0WINADD				(MTK_LCD_BASE + 0x007c)
#define LCD_L0WINSIZE				(MTK_LCD_BASE + 0x0080)
#define LCD_L1WINCON				(MTK_LCD_BASE + 0x0090)
#define LCD_L1WINSKEY				(MTK_LCD_BASE + 0x0094)
#define LCD_L1WINOFS				(MTK_LCD_BASE + 0x0098)
#define LCD_L1WINADD				(MTK_LCD_BASE + 0x009c)
#define LCD_L1WINSIZE				(MTK_LCD_BASE + 0x00a0)
#define LCD_L2WINCON				(MTK_LCD_BASE + 0x00b0)
#define LCD_L2WINSKEY				(MTK_LCD_BASE + 0x00b4)
#define LCD_L2WINOFS				(MTK_LCD_BASE + 0x00b8)
#define LCD_L2WINADD				(MTK_LCD_BASE + 0x00bc)
#define LCD_L2WINSIZE				(MTK_LCD_BASE + 0x00c0)
#define LCD_L3WINCON				(MTK_LCD_BASE + 0x00d0)
#define LCD_L3WINSKEY				(MTK_LCD_BASE + 0x00d4)
#define LCD_L3WINOFS				(MTK_LCD_BASE + 0x00d8)
#define LCD_L3WINADD				(MTK_LCD_BASE + 0x00dc)
#define LCD_L3WINSIZE				(MTK_LCD_BASE + 0x00e0)
#define LCD_L4WINCON				(MTK_LCD_BASE + 0x00f0)
#define LCD_L4WINSKEY				(MTK_LCD_BASE + 0x00f4)
#define LCD_L4WINOFS				(MTK_LCD_BASE + 0x00f8)
#define LCD_L4WINADD				(MTK_LCD_BASE + 0x00fc)
#define LCD_L4WINSIZE				(MTK_LCD_BASE + 0x0100)
#define LCD_L5WINCON				(MTK_LCD_BASE + 0x0110)
#define LCD_L5WINSKEY				(MTK_LCD_BASE + 0x0114)
#define LCD_L5WINOFS				(MTK_LCD_BASE + 0x0118)
#define LCD_L5WINADD				(MTK_LCD_BASE + 0x011c)
#define LCD_L5WINSIZE				(MTK_LCD_BASE + 0x0120)
#define LCD_PDAT0				(MTK_LCD_BASE + 0x4000)
#define LCD_PCMD0				(MTK_LCD_BASE + 0x4100)
#define LCD_PDAT1				(MTK_LCD_BASE + 0x5000)
#define LCD_PCMD1				(MTK_LCD_BASE + 0x5100)
#define LCD_PDAT2				(MTK_LCD_BASE + 0x6000)
#define LCD_PCMD2				(MTK_LCD_BASE + 0x6100)
#define LCD_SDAT1				(MTK_LCD_BASE + 0x8000)
#define LCD_SCMD1				(MTK_LCD_BASE + 0x8100)
#define LCD_SDAT0				(MTK_LCD_BASE + 0x9000)
#define LCD_SCMD0				(MTK_LCD_BASE + 0x9100)
#define LCD_GAMMA				(MTK_LCD_BASE + 0xc000)
#define LCD_PAL					(MTK_LCD_BASE + 0xc400)
#define LCD_COMD0				(MTK_LCD_BASE + 0xc800)
#define LCD_COMD1				(MTK_LCD_BASE + 0xc880)

/* Bit field definitions for LCD_START register. */
#define LCD_START_START				(1 << 15)

/* Bit field definitions for LCD_PCNF0. */
#define LCD_PCNF0_RLT				0
#define LCD_PCNF0_WST				8
#define LCD_PCNF0_26M				(1 << 14)
#define LCD_PCNF0_52M				(1 << 15)
#define LCD_PCNF0_DW				16
#define LCD_PCNF0_GAMMA_ID_B			18
#define LCD_PCNF0_GAMMA_ID_G			20
#define LCD_PCNF0_GAMMA_ID_R			22
#define LCD_PCNF0_C2RS				24
#define LCD_PCNF0_C2WH				28
#define LCD_PCNF0_C2WS				30

/* Definition of LCD_PCNF0 field values. */
#define LCD_PCNF0_DW_8BIT			0
#define LCD_PCNF0_DW_9BIT			1
#define LCD_PCNF0_DW_16BIT			2
#define LCD_PCNF0_DW_18BIT			3

#define LCD_PCNF0_GAMMA_ID_X_TABLE_0		0
#define LCD_PCNF0_GAMMA_ID_X_TABLE_1		1
#define LCD_PCNF0_GAMMA_ID_X_TABLE_2		2
#define LCD_PCNF0_GAMMA_ID_X_NO_TABLE		3

/* Bit field definitions for LCD_TECON register. */
#define LCD_TECON_TE_EN				(1 << 0)
#define LCD_TECON_TE_EDGE_SEL			(1 << 1)

/* Definition of LCD_MWINSIZE field values. */
#define LCD_MWINSIZE_ROW			16

/* Bit field definitions for LCD_RSTB register. */
#define LCD_RSTB_RSTB				(1 << 0)

/* Bit field definitions for W2MCON register. */
#define LCD_WROI_W2MCON_W2LCM			(1 << 0)
#define LCD_WROI_W2MCON_W2M_FORMAT		1
#define LCD_WROI_W2MCON_DISCON			(1 << 3)
#define LCD_WROI_W2MCON_ADDINC_DISABLE		(1 << 4)
#define LCD_WROI_W2MCON_DC_OUT_EN		(1 << 5)
#define LCD_WROI_W2MCON_OUTPUT_ALPHA		8

/* Definition of LCD_WROI_W2MCON field values. */
#define LCD_WROI_W2MCON_W2M_FORMAT_RGB565	0
#define LCD_WROI_W2MCON_W2M_FORMAT_RGB888	1
#define LCD_WROI_W2MCON_W2M_FORMAT_RGB8888	2

/* Bit field definitions for LCD_WROICON register. */
#define LCD_WROICON_COMMAND			8
#define LCD_WROICON_COM_SEL			(1 << 13)
#define LCD_WROICON_W2M				(1 << 14)
#define LCD_WROICON_ENC				(1 << 15)
#define LCD_WROICON_PERIOD			16
#define LCD_WROICON_EN5				(1 << 26)
#define LCD_WROICON_EN4				(1 << 27)
#define LCD_WROICON_EN3				(1 << 28)
#define LCD_WROICON_EN2				(1 << 29)
#define LCD_WROICON_EN1				(1 << 30)
#define LCD_WROICON_EN0				(1 << 31)

/* Bit field definitions for LCD_WROI_W2MOFS register. */
#define LCD_WROI_W2MOFS_Y_OFFSET		16

/* Bit field definitions for LCD_WROI_BGCLR register. */
#define LCD_WROI_BGCLR_BLUE			0
#define LCD_WROI_BGCLR_GREEN			8
#define LCD_WROI_BGCLR_RED			16

/* Bit field definitions for LCD_WROI_HWREF register. */
#define LCD_WROI_HWREF_HWREF			(1 << 0)
#define LCD_WROI_HWREF_HWEN			(1 << 7)
#define LCD_WROI_HWREF_SEL			16
#define LCD_WROI_HWREF_EN5			(1 << 26)
#define LCD_WROI_HWREF_EN4			(1 << 27)
#define LCD_WROI_HWREF_EN3			(1 << 28)
#define LCD_WROI_HWREF_EN2			(1 << 29)
#define LCD_WROI_HWREF_EN1			(1 << 30)
#define LCD_WROI_HWREF_EN0			(1 << 31)

/* Definition of LCD_WROI_HWREF field values. */
#define LCD_WROI_HWREF_SEL_IRT1			0
#define LCD_WROI_HWREF_SEL_IBW1			1
#define LCD_WROI_HWREF_SEL_IRT2			2
#define LCD_WROI_HWREF_SEL_IBW2			3

/* Bit field definitions for LCD_L0WINCON register. */
#define LCD_L0WINCON_OPA_EN			(1 << 8)
#define LCD_L0WINCON_CLRDPT			9
#define LCD_L0WINCON_ROTATE			11
#define LCD_L0WINCON_KEYEN			(1 << 14)
#define LCD_L0WINCON_SRC			(1 << 15)
#define LCD_L0WINCON_SWP			(1 << 16)
#define LCD_L0WINCON_READ_CACHE_DIS		(1 << 20)

/* Definition of LCD_WROI_HWREF field values. */
#define LCD_L0WINCON_CLRDPT_8BPP		0
#define LCD_L0WINCON_CLRDPT_RGB565		1
#define LCD_L0WINCON_CLRDPT_ARGB8888		2
#define LCD_L0WINCON_CLRDPT_RGB888		3

/* Bit field definitions for LCD_L0WINSIZE register. */
#define LCD_L0WINSIZE_COLUMN			0
#define LCD_L0WINSIZE_ROW			16

/* Bit field definitions for LCD_WROISIZE register. */
#define LCD_WROISIZE_COLUMN			0
#define LCD_WROISIZE_ROW			16

/* Bit field definitions for LCD_WROIOFS register. */
#define LCD_WROIFS_X_OFFSET			0
#define LCD_WROIFS_Y_OFFSET			16

#endif /* __MTK_LCD_H */
