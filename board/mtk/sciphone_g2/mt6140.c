/* Driver for RF Transceiver Circuit (MT6140) */

/*
 * (C) 2011 by Marcin Mielczarczyk <marcin.mielczarczyk@tieto.com>
 *
 * Based on MT6139 driver by Harald Welte <laforge@gnumonks.org>
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

#include <common.h>
#include <asm/io.h>
#include <asm/arch-mtk/mt6140.h>
#include <asm/arch-mtk/bsi.h>
#include <asm/arch-mtk/bpi.h>
#include <asm/arch-mtk/bfe.h>
#include <asm/arch-mtk/gpio.h>
#include <asm/arch-mtk/tdma_timer.h>
#include <asm/arch-mtk/system.h>

#include <command.h>

#define QB_BASE		1000

/* BSI timing */
#define QB_ST1		304
#define QB_ST2		140
#define QB_ST2B		120
#define QB_ST3		23

/* BPI timing */
#define QB_PT1		335
#define QB_PT2		18
#define QB_PT2B		6
#define QB_PT3		25

/* APC timing */
#define QB_APCDACON	99
#define QB_APCON	22
#define QB_APCOFF	2

#define ARFCN_PCS       0x8000
#define ARFCN_FLAG_MASK 0xf000  /* Reserve the upper 5 bits for flags */

enum gsm_band {
	GSM_BAND_850    = 1,
	GSM_BAND_900    = 2,
	GSM_BAND_1800   = 4,
	GSM_BAND_1900   = 8,
	GSM_BAND_450    = 0x10,
	GSM_BAND_480    = 0x20,
	GSM_BAND_750    = 0x40,
	GSM_BAND_810    = 0x80,
};

enum gsm_band gsm_arfcn2band(uint16_t arfcn)
{
	int is_pcs = arfcn & ARFCN_PCS;

	arfcn &= ~ARFCN_FLAG_MASK;

	if (is_pcs)
		return GSM_BAND_1900;
	else if (arfcn <= 124)
		return GSM_BAND_900;
	else if (arfcn >= 955 && arfcn <= 1023)
		return GSM_BAND_900;
	else if (arfcn >= 128 && arfcn <= 251)
		return GSM_BAND_850;
	else if (arfcn >= 512 && arfcn <= 885)
		return GSM_BAND_1800;
	else if (arfcn >= 259 && arfcn <= 293)
		return GSM_BAND_450;
	else if (arfcn >= 306 && arfcn <= 340)
		return GSM_BAND_480;
	else if (arfcn >= 350 && arfcn <= 425)
		return GSM_BAND_810;
	else if (arfcn >= 438 && arfcn <= 511)
		return GSM_BAND_750;
	else
		return GSM_BAND_1800;
}

/* Convert an ARFCN to the frequency in MHz * 10 */
uint16_t gsm_arfcn2freq10(uint16_t arfcn, int uplink)
{
	uint16_t freq10_ul;
	uint16_t freq10_dl;
	int is_pcs = arfcn & ARFCN_PCS;

	arfcn &= ~ARFCN_FLAG_MASK;

	if (is_pcs) {
		/* DCS 1900 */
		arfcn &= ~ARFCN_PCS;
		freq10_ul = 18502 + 2 * (arfcn-512);
		freq10_dl = freq10_ul + 800;
	} else if (arfcn <= 124) {
		/* Primary GSM + ARFCN 0 of E-GSM */
		freq10_ul = 8900 + 2 * arfcn;
		freq10_dl = freq10_ul + 450;
	} else if (arfcn >= 955 && arfcn <= 1023) {
		/* E-GSM and R-GSM */
		freq10_ul = 8900 + 2 * (arfcn - 1024);
		freq10_dl = freq10_ul + 450;
	} else if (arfcn >= 128 && arfcn <= 251) {
		/* GSM 850 */
		freq10_ul = 8242 + 2 * (arfcn - 128);
		freq10_dl = freq10_ul + 450;
	} else if (arfcn >= 512 && arfcn <= 885) {
		/* DCS 1800 */
		freq10_ul = 17102 + 2 * (arfcn - 512);
		freq10_dl = freq10_ul + 950;
	} else if (arfcn >= 259 && arfcn <= 293) {
		/* GSM 450 */
		freq10_ul = 4506 + 2 * (arfcn - 259);
		freq10_dl = freq10_ul + 100;
	} else if (arfcn >= 306 && arfcn <= 340) {
		/* GSM 480 */
		freq10_ul = 4790 + 2 * (arfcn - 306);
		freq10_dl = freq10_ul + 100;
	} else if (arfcn >= 350 && arfcn <= 425) {
		/* GSM 810 */
		freq10_ul = 8060 + 2 * (arfcn - 350);
		freq10_dl = freq10_ul + 450;
	} else if (arfcn >= 438 && arfcn <= 511) {
		/* GSM 750 */
		freq10_ul = 7472 + 2 * (arfcn - 438);
		freq10_dl = freq10_ul + 300;
	} else
		return 0xffff;

	if (uplink)
		return freq10_ul;
	else
		return freq10_dl;
}

/* ############################### TDMA ################################### */

static void wait_tdma(int qbit_no)
{
	uint16_t curr, last;
	int diff;

	last = readw(TDMA_TQCNT);
	while (qbit_no > 0) {
		curr = readw(TDMA_TQCNT);
		diff = curr - last;
		if (diff < 0)
			diff = (readw(TDMA_WRAP) - last) + curr + 1;
		qbit_no -= diff;
		last = curr;
	}
}

/* Currently TDMA timer is not used */
#if 0
/* BSI */

static void tdma_bsi_set_mask(uint16_t bsi_no)
{
	if (bsi_no < 16)
		writew(readw(TDMA_EVTENA2) | (1 << bsi_no), TDMA_EVTENA2);
	else
		writew(readw(TDMA_EVTENA3) | (1 << (bsi_no % 16)), TDMA_EVTENA3);
}

static void tdma_bsi_clear_mask(uint16_t bsi_no)
{
	if (bsi_no < 16)
		writew(readw(TDMA_EVTENA2) & ~(1 << bsi_no), TDMA_EVTENA2);
	else
		writew(readw(TDMA_EVTENA3) & ~(1 << (bsi_no % 16)), TDMA_EVTENA3);
}

static void tdma_bsi_set_qbit(uint16_t bsi_no, uint16_t qbit)
{
	writew(QB_BASE - qbit, TDMA_BSI(bsi_no));
}

static void tdma_bsi_set(uint16_t bsi_event, uint16_t qbit)
{
	tdma_bsi_set_mask(bsi_event);
	tdma_bsi_set_qbit(bsi_event, qbit);
}

/* BPI */

static void tdma_bpi_set_mask(uint16_t bpi_no)
{
	if (bpi_no < 16)
		writew(readw(TDMA_EVTENA4) | (1 << bpi_no), TDMA_EVTENA4);
	else if (bpi_no < 32)
		writew(readw(TDMA_EVTENA5) | (1 << (bpi_no % 16)), TDMA_EVTENA5);
	else
		writew(readw(TDMA_EVTENA6) | (1 << (bpi_no % 16)), TDMA_EVTENA6);
}

static void tdma_bpi_clear_mask(uint16_t bpi_no)
{
	if (bpi_no < 16)
		writew(readw(TDMA_EVTENA4) & ~(1 << bpi_no), TDMA_EVTENA4);
	else if (bpi_no < 32)
		writew(readw(TDMA_EVTENA5) & ~(1 << (bpi_no % 16)), TDMA_EVTENA5);
	else
		writew(readw(TDMA_EVTENA6) & ~(1 << (bpi_no % 16)), TDMA_EVTENA6);
}

static void tdma_bpi_set_qbit(uint16_t bpi_no, uint16_t qbit)
{
	writew(QB_BASE - qbit, TDMA_BPI(bpi_no));
}

static void tdma_bpi_set(uint16_t bpi_event, uint16_t qbit)
{
	tdma_bpi_set_mask(bpi_event);
	tdma_bpi_set_qbit(bpi_event, qbit);
}

/* APC */

static void tdma_apc_set_mask(uint16_t apc_no)
{
	writew(readw(TDMA_EVTENA1) | (1 << apc_no), TDMA_EVTENA1);
}

static void tdma_apc_clear_mask(uint16_t apc_no)
{
	writew(readw(TDMA_EVTENA1) & ~(1 << apc_no), TDMA_EVTENA1);
}

static void tdma_apc_set_qbit(uint16_t apc_no, uint16_t qbit)
{
	writew(QB_BASE - qbit, TDMA_APC(apc_no));
}

static void tdma_apc_set(uint16_t apc_no, uint16_t qbit)
{
	tdma_apc_set_mask(apc_no);
	tdma_apc_set_qbit(apc_no, qbit);
}

/* BFE  uplink */

static void tdma_bfe_ul_clear_mask(uint16_t ul_no)
{
	writew(readw(TDMA_EVTENA1) & ~(1 << (ul_no + 8)), TDMA_EVTENA1);
}

static void tdma_bfe_ul_set(uint16_t ul_no, uint16_t qbit_on, uint16_t qbit_off)
{
	writew(QB_BASE - qbit_on, TDMA_BULON(ul_no));
	writew(QB_BASE - qbit_off, TDMA_BULOFF(ul_no));
	writew(readw(TDMA_EVTENA1) | (1 << (ul_no + 8)), TDMA_EVTENA1);
}

/* BFE  downlink */

static void tdma_bfe_dl_clear_mask(uint16_t dl_no)
{
	writew(readw(TDMA_EVTENA0) & ~(1 << (dl_no + 6)), TDMA_EVTENA0);
}

static void tdma_bfe_dl_set(uint16_t dl_no, uint16_t qbit_on, uint16_t qbit_off)
{
	writew(QB_BASE - qbit_on, TDMA_BDLON(dl_no));
	writew(QB_BASE - qbit_off, TDMA_BDLOFF(dl_no));
	writew(readw(TDMA_EVTENA0) | (1 << (dl_no + 6)), TDMA_EVTENA0);
}

/* ########################## BPI ##################################### */

static void bpi_set_mask(uint16_t bpi_no)
{
	if (bpi_no < 16)
		writew(readw(BPI_ENA0) | (1 << bpi_no), BPI_ENA0);
	else if (bpi_no < 32)
		writew(readw(BPI_ENA1) | (1 << (bpi_no % 16)), BPI_ENA1);
	else
		writew(readw(BPI_ENA2) | (1 << (bpi_no % 16)), BPI_ENA2);
}

static void bpi_clear_mask(uint16_t bpi_no)
{
	if (bpi_no < 16)
		writew(readw(BPI_ENA0) & ~(1 << bpi_no), BPI_ENA0);
	else if (bpi_no < 32)
		writew(readw(BPI_ENA1) & ~(1 << (bpi_no % 16)), BPI_ENA1);
	else
		writew(readw(BPI_ENA2) & ~(1 << (bpi_no % 16)), BPI_ENA2);
}

#endif

static void bpi_imm_write(uint16_t value)
{
	uint16_t reg;

	reg = readw(BPI_CON);
	/* Enable BPI direct access */
	writew(reg | BPI_CON_PETEV, BPI_CON);
	/* Set BPI pins */
	writew(value, BPI_BUFI);
	/* Disable BPI direct access */
	writew(reg, BPI_CON);
}

static void bpi_write(uint16_t bpi_no, uint32_t value)
{
	writel(value, BPI_BUF(bpi_no));
}

/* ########################## BSI #################################### */

static void bsi_imm_write(uint8_t address, uint32_t value)
{
	uint16_t reg;

	reg = readw(BSI_CON);
	/* Write data directly to the active buffer */
	writew(reg | BSI_CON_SETENV, BSI_CON);
	writew(BSI_Dn_LEN(26) | BSI_EVENT_ID_IMMEDIATE, BSI_Dn_32BIT_CON(0));
	writel((value << 4) | (address & 0xF), BSI_Dn_32BIT_DAT(0));
	/* Set IMOD bit */
	writew(reg | BSI_CON_IMOD, BSI_CON);
	/*
	 * Clock is set to 52/8 = 6.5MHz, so send of 1bit takes 0.15us
	 * TDMA 1 qbit takes 1/1.0833M = 0.92us
	 * To send 26bits we need: 26 * 0.15us = 3.9us
	 * Quotation to calculate number of qbits: 3.9/0.92 = 4.24 qbits
	 */
	wait_tdma(5);
}

static void bsi_write(uint16_t bsi_no, uint16_t bsi_event, uint8_t address,
			uint32_t value)
{
	writew(BSI_Dn_LEN(26) | BSI_EVENT_ID(bsi_event), BSI_Dn_32BIT_CON(bsi_no));
	writel((value << 4) | (address & 0xF), BSI_Dn_32BIT_DAT(bsi_no));
}

/* ############################### RF ###################################### */

static void mt6140_compute_pll(uint32_t f_vco_10Mhz,
			       uint16_t *nint, uint16_t *nfrac)
{
	/* To compute Nint, we assume Nfrac is zero */
	*nint = (f_vco_10Mhz / (10 * 2 * 26)) - (0 / 130);

	if (*nint > 127)
		printf("VCO Frequency %u MHz is out of spec\n", f_vco_10Mhz * 10);

	/* Compute Nfract using the pre-computed Nint */
	/* Nfrac = ( (Fvco/2*26) - Nint) * 130 */
	/* Nfrac = ( (Fvco*130)/(2*26) - (Nint * 130) */
	*nfrac = (f_vco_10Mhz*130)/(52*10) - (*nint * 130);
}

/* Set ARFCN.  Takes 2 reg_write, i.e. 8 TPU instructions */
uint32_t mt6140_set_arfcn(uint16_t arfcn, int uplink)
{
	uint32_t regval = 0;
	uint32_t vco_mult;
	uint32_t freq_10Mhz, f_vco_10Mhz;
	uint16_t nint, nfrac;

	freq_10Mhz = gsm_arfcn2freq10(arfcn, uplink);
	printf("ARFCN %u -> %u MHz\n", arfcn, freq_10Mhz * 10);

	switch (gsm_arfcn2band(arfcn)) {
	case GSM_BAND_850:
		if (uplink)
			regval |= MT6140_CW1_TRX | MT6140_CW1_BUF_MODE;
		regval |= (0 << MT6140_CW1_BAND_SHIFT);
		vco_mult = 4;
		break;
	case GSM_BAND_900:
		if (uplink)
			regval |= MT6140_CW1_TRX | MT6140_CW1_BUF_MODE;
		regval |= (1 << MT6140_CW1_BAND_SHIFT);
		vco_mult = 4;
		break;
	case GSM_BAND_1800:
		regval |= (2 << MT6140_CW1_BAND_SHIFT);
		vco_mult = 2;
		break;
	case GSM_BAND_1900:
		regval |= (3 << MT6140_CW1_BAND_SHIFT);
		vco_mult = 2;
		break;
	default:
		printf("Unsupported rf_band.\n");
		break;
	}

	/* Compute VCO frequency for channel frequency */
	f_vco_10Mhz = freq_10Mhz * vco_mult;

	/* Compute Nint and Nfract */
	mt6140_compute_pll(f_vco_10Mhz, &nint, &nfrac);

	/* mask-in the Nint / Nfrac bits in CW1 */
	regval |= (nfrac & 0xff) << MT6140_CW1_NFRACT_SHIFT;
	regval |= (nint & 0x7f) << MT6140_CW1_NINT_SHIFT;

	return regval;
}

void mt6140_tx_burst(uint16_t arfcn)
{
	uint32_t val;
	//int bpi_no = 0, bsi_no = 0;

	/* Enable RFVCO */
	bpi_imm_write(BPI_RFVCOEN);
	wait_tdma(21);
	/*bpi_write(bpi_no, BPI_RFVCOEN);
	tdma_bpi_set(bpi_no++, QB_PT1);*/

	/* Turn on the synthesizer and move into Warm-up mode */
	val = (0x3e << MT6140_CW2_GAINTBL_SHIFT) |
	      (MODE_WARM_UP << MT6140_CW2_MODE_SHIFT) |
	      MT6140_CW2_AUTO_CAL |
	      (0x20 << MT6140_CW2_DCD_AQ_SHIFT) |
	      (0x20 << MT6140_CW2_DCD_AI_SHIFT);
	bsi_imm_write(2, val);
	wait_tdma(164);
	/* bsi_write(bsi_no++, 0, 2, val);
	tdma_bsi_set(0, QB_ST1); */

	/* Program the frequency synthesizer N counter and band selection */
	val = mt6140_set_arfcn(arfcn, 1);
	/* Set CW1 register values */
	bsi_imm_write(1, val);
	/* bsi_write(bsi_no++, 1, 1, val);
	tdma_bsi_set(1, QB_ST2); */

	/* Set Transmit mode */
	val = (MODE_TRANSMIT << MT6140_CW2_MODE_SHIFT) | MT6140_CW2_AUTO_CAL;
	bsi_imm_write(2, val);
	//bsi_write(bsi_no++, 2, 2, val);

	/* Set TX settings */
	val = (1 << MT6140_CW11_TXMODGAIN_SHIFT) |
	      (9 << MT6140_CW11_TXFLT_SHIFT) |
	      (2 << MT6140_CW11_TXAPC_SHIFT) |
	      (1 << MT6140_CW11_TXPW_SHIFT) |
	      MT6140_CW11_TXDIV_GC1;
	bsi_imm_write(11, val);
	wait_tdma(122);
	/* bsi_write(bsi_no++, 2, 11, val);
	tdma_bsi_set(2, QB_ST2B); */

	/* Enable PA and keep RVFCOEN */
	bpi_imm_write(BPI_PAEN | BPI_RFVCOEN);
	wait_tdma(12);
	/* bpi_write(bpi_no, BPI_PAEN | BPI_RFVCOEN);
	tdma_bpi_set(bpi_no++, QB_PT2); */
	/* Enable LB_TX and keep PA and RFVCOEN */
	bpi_imm_write(BPI_LB_TX | BPI_PAEN | BPI_RFVCOEN);
	/* bpi_write(bpi_no, BPI_LB_TX | BPI_PAEN | BPI_RFVCOEN);
	tdma_bpi_set(bpi_no++, QB_PT2B); */

	/* Do the actual burst Tx */
	writew(0x7A0, TDMA_BULON0);
	writew(0x2700, TDMA_BULOFF0);
	writew(readw(TDMA_EVTENA1) | 0x100, TDMA_EVTENA1);
	//tdma_bfe_ul_set(0, QB_BASE, QB_BASE + 8000);

#if 0
	/* Set Sleep mode */
	val = (0x3e << MT6139_CW2_GAINTBL_SHIFT) |
	      (MODE_SLEEP << MT6139_CW2_MODE_SHIFT) |
	      MT6139_CW2_AUTO_CAL |
	      (0x20 << MT6139_CW2_DCD_AQ_SHIFT) |
	      (0x20 << MT6139_CW2_DCD_AI_SHIFT);
	mt6139_reg_write(2, val);
#endif
}

static void bsi_init(void)
{
	int i;
	uint32_t bsi_conf;

	/* Invert clock, clock speed 52/8 MHz, short chip select */
	bsi_conf = BSI_CON_CLK_POL_INV | BSI_CON_CLK_SPD_52_8 |
			BSI_CON_EN0_LEN_SHORT;

	/* Setup BSI settings */
	writew(bsi_conf, BSI_CON);

	/* Setup mode for writing data directly to the active buffer */
	writew(bsi_conf | BSI_CON_SETENV, BSI_CON);

	/* Reset values of BSI registers */
	for (i = 0; i < 40; ++i) {
		writel(0, BSI_Dn_32BIT_CON(i));
		writel(0, BSI_Dn_32BIT_DAT(i));
	}
	for (i = 0; i < 4; ++i) {
		writel(0, BSI_Dn_78BIT_CON(i));
		writel(0, BSI_Dn_78BIT_DAT0(i));
		writel(0, BSI_Dn_78BIT_DAT1(i));
		writel(0, BSI_Dn_78BIT_DAT2(i));
	}

	/* Turn off direct mode */
	writew(bsi_conf, BSI_CON);
}

static void bpi_init(void)
{
	/* Select 8mA driving capability for BPI_BUS0, BPI_BUS1 and BPI_BUS2 */
	writew(BPI_CON_PINM0 | BPI_CON_PINM1 | BPI_CON_PINM2, BPI_CON);
	bpi_imm_write(0);
}

static void bfe_init(void)
{
	writew(BFE_TX_CFG_RPSEL_RAMP2, BFE_TX_CFG);
	writew(BFE_TX_CON_IQSWP, BFE_TX_CON);

}

struct mt6140_elem {
	unsigned char	address;
	unsigned int	data;
};

/* MT6140 init sequence sniffed on BSI bus */
static struct mt6140_elem mt6140_init_seq[]=
{
	{1,	0x013F00}, // 1:CW1
	{2,	0x20823E}, // 2:CW2
	{3,	0x07F3FF}, // 3:CW3
	{4,	0x00A871}, // 4:CW4
	{5,	0x00041F}, // 5:CW5
	{6,	0x001D01}, // 6:CW6
	{7,	0x000195}, // 7:CW7
	{8,	0x0FA040}, // 8:CW8
	{9,	0x00E040}, // 9:CW9
	{10,	0x002040}, //10:CW10
	{11,	0x200070}, //11:CW11
	{12,	0x201370}, //12:CW12
	{15,	0x000000}, //13:CW15
	{2,	0x000300}, //14:CW2, set TX mode for power on TX calibration
};

void rf_init(void)
{
	int i;

	/* Power on TDMA, BPI, BSI, AFCi, APC and BFE */
	writew(PDN_CON2_TDMA | PDN_CON2_BPI | PDN_CON2_BSI | PDN_CON2_AFC |
		PDN_CON2_APC | PDN_CON2_BFE, MTK_CONFG_PDN_CLR2);
	/* Some of the bits are also available in PDN_CON4 register */
	writew(PDN_CON2_BPI | PDN_CON2_BSI | PDN_CON2_APC | PDN_CON2_AFC,
		MTK_CONFG_PDN_CLR4);

	/* Select BPI_BUS7 (MODE_PA) and BPI_BUS9 (RFVCOEN) pins */
	writew(MTK_GPIO_MODE3_GPIO21_ALT1 | MTK_GPIO_MODE3_GPIO23_ALT1,
		MTK_GPIO_MODE3);

	/* Initialize BSI */
	bsi_init();

	/* Initialize BPI */
	bpi_init();

	/* Initialize BFE */
	bfe_init();

	/* Reset MT6140 */
	bsi_imm_write(0, 0x01 << MT6140_CW0_SYNCP_SHIFT |
			MT6140_CW0_DIEN |
			0x20 << MT6140_CW0_AFC_SHIFT |
			MT6140_CW0_POR);
	/* Take MT6140 out of reset */
	bsi_imm_write(0, 0x01 << MT6140_CW0_SYNCP_SHIFT |
			MT6140_CW0_DIEN |
			0x60 << MT6140_CW0_AFC_SHIFT);

	for (i = 0; i < sizeof(mt6140_init_seq)/sizeof(struct mt6140_elem); ++i)
		bsi_imm_write(mt6140_init_seq[i].address,
				mt6140_init_seq[i].data);

	/* Move to SLEEP mode */
	bsi_imm_write(2, (0x3e << MT6140_CW2_GAINTBL_SHIFT) |
			(MODE_SLEEP << MT6140_CW2_MODE_SHIFT) |
			MT6140_CW2_AUTO_CAL |
			(0x20 << MT6140_CW2_DCD_AQ_SHIFT) |
			(0x20 << MT6140_CW2_DCD_AI_SHIFT));
}

#if 0
void mt6139_rx_burst()
{
	uint8_t pga_gain;

	/* Turn on the synthesizer and move into Warm-up mode */
	val = (0x3e << MT6139_CW2_GAINTBL_SHIFT) |
	      (MODE_WARM_UP << MT6139_CW2_MODE_SHIFT) |
	      MT6139_CW2_AUTO_CAL |
	      (0x20 << MT6139_CW2_DCD_AQ_SHIFT) |
	      (0x20 << MT6139_CW2_DCD_AI_SHIFT);
	mt6139_reg_write(2, val);

	/* Program the frequency synthesizer N counter and band selection */
	/* FIXME: see above for mt6139_set_arfcn() */

	/* Set receive mode, PGA gain */
	val = (pga_gain << MT6139_CW2_GAINTBL_SHIFT) |
	      (MODE_RECEIVE << MT6139_CW2_MODE_SHIFT) |
	      MT6139_CW2_AUTO_CAL |
	      (0x20 << MT6139_CW2_DCD_AQ_SHIFT) |
	      (0x20 << MT6139_CW2_DCD_AI_SHIFT);
	mt6139_reg_write(2, val);

	/* FIXME: Do the actual burst Rx */

	/* Set Sleep mode */
	val = (0x3e << MT6139_CW2_GAINTBL_SHIFT) |
	      (MODE_SLEEP << MT6139_CW2_MODE_SHIFT) |
	      MT6139_CW2_AUTO_CAL |
	      (0x20 << MT6139_CW2_DCD_AQ_SHIFT) |
	      (0x20 << MT6139_CW2_DCD_AI_SHIFT);
	mt6139_reg_write(2, val);
}
#endif

int rf_command(struct cmd_tbl_s *cmdtp, int flag, int argc, char * const argv[])
{
	if (argc != 2)
		return cmd_usage(cmdtp);

	rf_init();
	mt6140_tx_burst(simple_strtoul(argv[1], NULL, 10));
	return 0;
}

U_BOOT_CMD(
	rf_tx, CONFIG_SYS_MAXARGS, 1, rf_command,
	"Sends data using GMSK on given ARFCN",
	"<arfcn>"
);

