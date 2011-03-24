#ifndef _MTK_MT6140_H
#define _MTK_MT6140_H

enum mt6140_band {
	MTRF_BAND_GSM850	= 0,
	MTRF_BAND_GSM900	= 1,
	MTRF_BAND_GSM1800	= 2,
	MTRF_BAND_GSM1900	= 3,
};

#define MT6140_CW0_SYNCP_SHIFT		0
#define MT6140_CW0_SYNPW		(1 << 2)
#define MT6140_CW0_DIEN			(1 << 3)
#define MT6140_CW0_FLT			(1 << 4)
#define MT6140_CW0_AFC_SHIFT		5
#define MT6140_CW0_VCO_SEL		(1 << 11)
#define MT6140_CW0_GPO			(1 << 12)
/* This field is shifted by 1-bit comparing to MT6139 */
#define MT6140_CW0_POR			(1 << 14)

#define MT6140_CW1_NFRACT_SHIFT		0
#define MT6140_CW1_NINT_SHIFT		8
#define MT6140_CW1_BAND_SHIFT		16
#define MT6140_CW1_BUF_MODE		(1 << 15)
#define MT6140_CW1_TRX			(1 << 18)

#define MT6140_CW2_GAINTBL_SHIFT	0
#define MT6140_CW2_MODE_SHIFT		6
#define MT6140_CW2_AUTO_CAL		(1 << 9)
#define MT6140_CW2_DCD_AQ_SHIFT		10
#define MT6140_CW2_DCD_AI_SHIFT		16

#define MT6140_CW9_DCD_CQ_SHIFT		0
#define MT6140_CW9_DCD_BQ_SHIFT		7
#define MT6140_CW9_PWR_DAC_C		(1 << 14)
#define MT6140_CW9_PWR_DAC_B		(1 << 15)
#define MT6140_CW9_PWR_DAC_A		(1 << 16)
#define MT6140_CW9_AM_ENABLE		(1 << 17)

enum mt6140_cw2_mode {
	MODE_SLEEP	= 0x0,
	MODE_WARM_UP	= 0x1,
	MODE_RECEIVE	= 0x3,
	MODE_TRANSMIT	= 0x4,
};

#define MT6140_CW11_TX_CTL		(1 << 0)
#define MT6140_CW11_TXG_IQM		(1 << 1)
#define MT6140_CW11_TXD_IQM		(1 << 2)
#define MT6140_CW11_TX_DIV2		(1 << 3)
#define MT6140_CW11_TX_DIV4		(1 << 4)
#define MT6140_CW11_TXG_BUF		(1 << 5)
#define MT6140_CW11_TXD_BUF		(1 << 6)
#define MT6140_CW11_TXMODGAIN_SHIFT	7
#define MT6140_CW11_TXFLT_SHIFT	10
#define MT6140_CW11_TXAPC_SHIFT		14
#define MT6140_CW11_TXPW_SHIFT		16
#define MT6140_CW11_TXBIAST_SHIFT	18
#define MT6140_CW11_TXDIV_GC0		(1 << 20)
#define MT6140_CW11_TXDIV_GC1		(1 << 21)

#define BPI_HB_TX			(1 << 0)
#define BPI_PCS_RX			(1 << 1)
#define BPI_LB_TX			(1 << 2)
#define BPI_PAEN			(1 << 4)
#define BPI_BANDSW			(1 << 5)
#define BPI_MODE_PA			(1 << 7)
#define BPI_RFVCOEN			(1 << 9)

#endif /* _MTK_MT6140_H */
