#ifndef _MTK_BSI_H
#define _MTK_BSI_H

/* MTK Baseband Serial Interface */

#define	BSI_CON		(MTK_BSI_BASE + 0x0000)
#define	BSI_D0_CON	(MTK_BSI_BASE + 0x0004)
#define	BSI_D0_DAT	(MTK_BSI_BASE + 0x0008)
#define	BSI_D40_CON	(MTK_BSI_BASE + 0x0144)
#define	BSI_D40_DAT2	(MTK_BSI_BASE + 0x0148)
#define	BSI_D40_DAT1	(MTK_BSI_BASE + 0x014C)
#define	BSI_D40_DAT0	(MTK_BSI_BASE + 0x0150)
#define BSI_ENA_0	(MTK_BSI_BASE + 0x0190)
#define	BSI_ENA_1	(MTK_BSI_BASE + 0x0194)
#define	BSI_IO_CON	(MTK_BSI_BASE + 0x0198)
#define BSI_DOUT	(MTK_BSI_BASE + 0x019c)
#define	BSI_DIN		(MTK_BSI_BASE + 0x01a0)
#define	BSI_PAIR_NUM	(MTK_BSI_BASE + 0x01a4)

/* Compute offset of BSI_D0_CON / BSI_D0_DAT registers */
#define BSI_Dn_32BIT_CON(x)	(BSI_D0_CON + (x * 8))
#define BSI_Dn_32BIT_DAT(x)	(BSI_D0_DAT + (x * 8))
#define BSI_Dn_78BIT_CON(x)	(BSI_D40_CON + (x * 16))
#define BSI_Dn_78BIT_DAT0(x)	(BSI_D40_DAT0 + (x * 16))
#define BSI_Dn_78BIT_DAT1(x)	(BSI_D40_DAT1 + (x * 16))
#define BSI_Dn_78BIT_DAT2(x)	(BSI_D40_DAT2 + (x * 16))

/* MT6235 Section 9.1.1 */
#define BSI_CON_CLK_POL_INV	(1 << 0)
#define BSI_CON_CLK_SPD_52_2	(0 << 1)	/* 26 MHz */
#define BSI_CON_CLK_SPD_52_4	(1 << 1)	/* 13 MHz */
#define BSI_CON_CLK_SPD_52_6	(2 << 1)	/* 8.67 MHz */
#define BSI_CON_CLK_SPD_52_8	(3 << 1)	/* 6.50 MHz */
#define BSI_CON_IMOD		(1 << 3)
#define BSI_CON_EN0_LEN_SHORT	(1 << 4)
#define BSI_CON_EN0_POL_INV	(1 << 5)
#define BSI_CON_EN1_LEN_SHORT	(1 << 6)
#define BSI_CON_EN1_POL_INV	(1 << 7)
#define BSI_CON_SETENV		(1 << 8)

/* how the length is encoded in BSI_Dn_CON */
#define BSI_Dn_LEN(n)		(((n-1) & 0x7f) << 8)
#define BSI_Dn_ISB		0x8000		/* select device 1 */

/* Immediate download EVENT ID */
#define BSI_EVENT_ID_IMMEDIATE	0x1F

#define BSI_EVENT_ID(n)		(n)

#endif /* _MTK_BSI_H */
