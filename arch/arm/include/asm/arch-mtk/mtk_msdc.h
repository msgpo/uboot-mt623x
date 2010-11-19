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

#ifndef MTK_MSDC_H
#define MTK_MSDC_H

/*
 * Register definitions
 */

/* MS/SD Memory Card Controller Configuration Register */
#define MTK_MSDC_CFG		(MTK_MSDC_BASE + 0x0000)

/* MS/SD Memory Card Controller Status Register MSDC_STA */
#define MTK_MSDC_STA		(MTK_MSDC_BASE + 0x0004)

/* MS/SD Memory Card Controller Interrupt Register MSDC_INT */
#define MTK_MSDC_INT		(MTK_MSDC_BASE + 0x0008)

/* MS/SD Memory Card Controller Data Register MSDC_DAT */
#define MTK_MSDC_DAT		(MTK_MSDC_BASE + 0x000C)

/* MS/SD Memory Card Pin Status Register MSDC_PS */
#define MTK_MSDC_PS		(MTK_MSDC_BASE + 0x0010)

/* MS/SD Memory Card Controller IO Control Register MSDC_IOCON */
#define MTK_MSDC_IOCON		(MTK_MSDC_BASE + 0x0014)

/* SD Memory Card Controller Configuration Register SDC_CFG */
#define MTK_MSDC_SDC_CFG	(MTK_MSDC_BASE + 0x0020)

/* SD Memory Card Controller Command Register SDC_CMD */
#define MTK_MSDC_SDC_CMD	(MTK_MSDC_BASE + 0x0024)

/* SD Memory Card Controller Argument Register SDC_ARG */
#define MTK_MSDC_SDC_ARG	(MTK_MSDC_BASE + 0x0028)

/* SD Memory Card Controller Status Register SDC_STA */
#define MTK_MSDC_SDC_STA	(MTK_MSDC_BASE + 0x002C)

/* SD Memory Card Controller Response Register 0 SDC_RESP0 */
#define MTK_MSDC_SDC_RESP0	(MTK_MSDC_BASE + 0x0030)

/* SD Memory Card Controller Response Register 1 SDC_RESP1 */
#define MTK_MSDC_SDC_RESP1	(MTK_MSDC_BASE + 0x0034)

/* SD Memory Card Controller Response Register 2 SDC_RESP2 */
#define MTK_MSDC_SDC_RESP2	(MTK_MSDC_BASE + 0x0038)

/* SD Memory Card Controller Response Register 3 SDC_RESP3 */
#define MTK_MSDC_SDC_RESP3	(MTK_MSDC_BASE + 0x003C)

/* SD Memory Card Controller Command Status Register SDC_CMDSTA */
#define MTK_MSDC_SDC_CMDSTA	(MTK_MSDC_BASE + 0x0040)

/* SD Memory Card Controller Data Status Register SDC_DATSTA */
#define MTK_MSDC_SDC_DATSTA	(MTK_MSDC_BASE + 0x0044)

/* SD Memory Card Status Register SDC_CSTA */
#define MTK_MSDC_SDC_CSTA	(MTK_MSDC_BASE + 0x0048)

/* SD Memory Card IRQ Mask Register 0 SDC_IRQMASK0 */
#define MTK_MSDC_SDC_IRQMASK0	(MTK_MSDC_BASE + 0x004C)

/* SD Memory Card IRQ Mask Register 1 SDC_IRQMASK1 */
#define MTK_MSDC_SDC_IRQMASK1	(MTK_MSDC_BASE + 0x0050)

/* SDIO Configuration Register SDIO_CFG */
#define MTK_MSDC_SDIO_CFG	(MTK_MSDC_BASE + 0x0054)

/* SDIO Status Register SDIO_STA */
#define MTK_MSDC_SDIO_STA	(MTK_MSDC_BASE + 0x0058)

/* Memory Stick Controller Configuration Register MSC_CFG */
#define MTK_MSDC_MSC_CFG	(MTK_MSDC_BASE + 0x0060)

/* Memory Stick Controller Command Register MSC_CMD */
#define MTK_MSDC_MSC_CMD	(MTK_MSDC_BASE + 0x0064)

/* Memory Stick Controller Auto Command Register MSC_ACMD */
#define MTK_MSDC_MSC_ACMD	(MTK_MSDC_BASE + 0x0068)

/* Memory Stick Controller Status Register MSC_STA */
#define MTK_MSDC_MSC_STA	(MTK_MSDC_BASE + 0x006C)


/* MSDC_CFG bit field definitions */
#define MTK_MSDC_CFG_MSDC			(1 << 0)
#define MTK_MSDC_CFG_NOCRC			(1 << 2)
#define MTK_MSDC_CFG_RST			(1 << 3)
#define MTK_MSDC_CFG_CLKSRC			(1 << 4)
#define MTK_MSDC_CFG_STDBY			(1 << 5)
#define MTK_MSDC_CFG_RED			(1 << 6)
#define MTK_MSDC_CFG_SCLKON			(1 << 7)
#define MTK_MSDC_CFG_SCLKF			8
#define MTK_MSDC_CFG_SCLKF_MASK			(0xFF << MTK_MSDC_CFG_SCLKF)
#define MTK_MSDC_CFG_INTEN			(1 << 16)
#define MTK_MSDC_CFG_DMAEN			(1 << 17)
#define MTK_MSDC_CFG_PINEN			(1 << 18)
#define MTK_MSDC_CFG_DIRQEN			(1 << 19)
#define MTK_MSDC_CFG_RCDEN			(1 << 20)
#define MTK_MSDC_CFG_VDDP			(1 << 21)

/* MSDC_SDC_CFG bit field definitions */
#define MTK_MSDC_SDC_CFG_SIEN			(1 << 16)
#define MTK_MSDC_SDC_CFG_MDLEN			(1 << 17)
#define MTK_MSDC_SDC_CFG_MDLW8			(1 << 18)
#define MTK_MSDC_SDC_CFG_SDIO			(1 << 19)
#define MTK_MSDC_SDC_CFG_BSYDLY			12

#define MTK_MSDC_SDC_CFG_BLKLEN_MASK		0x00000FFF

/* MSDC_SDC_CMD bit field definitions */
#define	MTK_MSDC_SDC_CMD_BREAK			(1 << 6)
#define MTK_MSDC_SDC_CMD_RSPTYP			7
#define MTK_MSDC_SDC_CMD_RSPTYP_MASK		(7 << MTK_MSDC_SDC_CMD_RSPTYP)
#define MTK_MSDC_SDC_CMD_IDRT			(1 << 10)

#define MTK_MSDC_SDC_CMD_DTYPE			11
#define MTK_MSDC_SDC_CMD_DTYPE_MASK		(3 << MTK_MSDC_SDC_CMD_DTYPE)
#define MTK_MSDC_SDC_CMD_RW			(1 << 13)
#define MTK_MSDC_SDC_CMD_STOP			(1 << 14)
#define MTK_MSDC_SDC_CMD_INTC			(1 << 15)
#define MTK_MSDC_SDC_CMD_CMDFAIL		(1 << 16)

#define MTK_MSDC_SDC_CMD_CMD_MASK		0x0000003F

#define MTK_MSDC_SDC_CMD_DTYPE_NO_DATA		0
#define MTK_MSDC_SDC_CMD_DTYPE_SINGLE_BLOCK	1
#define MTK_MSDC_SDC_CMD_DTYPE_MULTI_BLOCK	2
#define MTK_MSDC_SDC_CMD_DTYPE_MULTI_STREAM	3

#define MTK_MSDC_SDC_CMD_RSPTYP_NONE		0
#define MTK_MSDC_SDC_CMD_RSPTYP_R1		1
#define MTK_MSDC_SDC_CMD_RSPTYP_R2		2
#define MTK_MSDC_SDC_CMD_RSPTYP_R3		3
#define MTK_MSDC_SDC_CMD_RSPTYP_R4		4
#define MTK_MSDC_SDC_CMD_RSPTYP_R5		5
#define MTK_MSDC_SDC_CMD_RSPTYP_R6		6
#define MTK_MSDC_SDC_CMD_RSPTYP_R1B		7


/* Possible values of PRCFG0 field - MTK_MSDC_CFG register. */
#define MTK_MSDC_CFG_PRCFG0_PU_DS_PD_DS		(0 << 22)
#define MTK_MSDC_CFG_PRCFG0_PU_DS_PD_EN		(1 << 22)
#define	MTK_MSDC_CFG_PRCFG0_PU_EN_PD_DS		(2 << 22)
#define MTK_MSDC_CFG_PRCFG0_KEEPER		(3 << 22)

/* MSDC_PS bit field definitions */
#define MTK_MSDC_PS_CDEN			(1 << 0)
#define MTK_MSDC_PS_PIEN0			(1 << 1)
#define MTK_MSDC_PS_POEN0			(1 << 2)
#define MTK_MSDC_PS_PIN0			(1 << 3)
#define MTK_MSDC_PS_PINCHG			(1 << 4)
#define MTK_MSDC_PS_CMD				(1 << 24)

/* MSDC_STA bit field definitions */
#define MTK_MSDC_STA_BF				(1 << 0)
#define MTK_MSDC_STA_BE				(1 << 1)
#define MTK_MSDC_STA_DRQ			(1 << 2)
#define MTK_MSDC_STA_INT			(1 << 3)
#define MTK_MSDC_STA_FIFOCLR			(1 << 14)
#define MTK_MSDC_STA_BUSY			(1 << 15)
#define MTK_MSDC_STA_FIFOCNT			4
#define MTK_MSDC_STA_FIFOCNT_MASK		0x00F0

/* MSDC_SDC_CMDSTA bit field definitions */
#define MTK_MSDC_SDC_CMDSTA_CMDRDY		(1 << 0)
#define MTK_MSDC_SDC_CMDSTA_CMDTO		(1 << 1)
#define MTK_MSDC_SDC_CMDSTA_RSPCRCERR		(1 << 2)
#define MTK_MSDC_SDC_CMDSTA_MMCIRQ		(1 << 3)

/* MSDC_SDC_DATSTA bit field definitions */
#define MTK_MSDC_SDC_DATSTA_BLKDONE		(1 << 0)
#define MTK_MSDC_SDC_DATSTA_DATTO		(1 << 1)
#define MTK_MSDC_SDC_DATSTA_DATCRCERR		(1 << 2)

/* MSDC_SDC_STA bit field definitions */
#define MTK_MSDC_SDC_STA_SDCBUSY		(1 << 0)
#define MTK_MSDC_SDC_STA_CMDBUSY		(1 << 1)
#define MTK_MSDC_SDC_STA_DATBUSY		(1 << 2)
#define MTK_MSDC_SDC_STA_RSV			(1 << 3)
#define MTK_MSDC_SDC_STA_R1BS			(1 << 4)
#define MTK_MSDC_SDC_STA_WP			(1 << 15)

#endif /* MTK_MSDC_H */
