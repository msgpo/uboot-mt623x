#ifndef _MTK_BFE_H
#define _MTK_BFE_H

/* MTK Baseband Front End */

/* Chapter 10 of MT6235 Data Sheet */
#define	BFE_CON			(MTK_BFE_BASE + 0x0000)
#define	BFE_STA			(MTK_BFE_BASE + 0x0004)

/* RX path */
#define	BFE_RX_CFG		(MTK_BFE_BASE + 0x0010)
#define	BFE_RX_CON		(MTK_BFE_BASE + 0x0014)
#define	BFE_RX_PM_CON		(MTK_BFE_BASE + 0x0018)
#define	BFE_RX_FIR_CSID_CON	(MTK_BFE_BASE + 0x001C)
#define	BFE_RX_RAM0_CS0		(MTK_BFE_BASE + 0x0070)
#define	BFE_RX_RAM1_CS0		(MTK_BFE_BASE + 0x0020)
#define	BFE_RX_HPWR_STS		(MTK_BFE_BASE + 0x00B0)
#define	BFE_RX_BPWR_STS		(MTK_BFE_BASE + 0x00B4)

#define BFE_RX_RAM0_CS(n)	(BFE_RX_RAM0_CS0 + 4 * n)
#define BFE_RX_RAM1_CS(n)	(BFE_RX_RAM0_CS1 + 4 * n)

/* TX path */
#define	BFE_TX_CFG		(MTK_BFE_BASE + 0x0060)
#define	BFE_TX_CON		(MTK_BFE_BASE + 0x0064)
#define	BFE_TX_OFF		(MTK_BFE_BASE + 0x0068)

/* BFE_TX_CFG register fields */
#define BFE_TX_CFG_APNDEN	(1 << 0)
#define BFE_TX_CFG_RPSEL_RAMP1	(0 << 1)
#define BFE_TX_CFG_RPSEL_RAMP2	(1 << 1)
#define BFE_TX_CFG_RPSEL_RAMP3	(3 << 1)
#define BFE_TX_CFG_INTEN	(1 << 3)
#define BFE_TX_CFG_MDBYP	(1 << 4)
#define BFE_TX_CFG_SGEN		(1 << 5)
#define BFE_TX_CFG_NORMAL	(0 << 6)
#define BFE_TX_CFG_ALL0GEN	(1 << 6)
#define BFE_TX_CFG_ALL1GEN	(2 << 6)
#define BFE_TX_CFG_SW_QBCNT_SHIFT	8
#define BFE_TX_CFG_GMSK_1TAP	(0 << 13)
#define BFE_TX_CFG_GMSK_0TAP	(1 << 13)
#define BFE_TX_CFG_GMSK_2TAP	(2 << 13)

/* BFE_TX_CON register fields */
#define BFE_TX_CON_IQSWP	(1 << 0)
#define BFE_TX_CON_MDSEL1	(1 << 2)
#define BFE_TX_CON_MDSEL2	(1 << 3)
#define BFE_TX_CON_MDSEL3	(1 << 4)
#define BFE_TX_CON_MDSEL4	(1 << 5)
#define BFE_TX_CON_PHSEL_SHIFT	8
#define BFE_TX_CON_GMSK_0QB	(0 << 12)
#define BFE_TX_CON_GMSK_1QB	(1 << 12)
#define BFE_TX_CON_GMSK_2QB	(2 << 12)
#define BFE_TX_CON_GMSK_3QB	(3 << 12)

#endif /* _MTK_BFE_H */
