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

#include <common.h>
#include <nand.h>
#include <asm/io.h>
#include <asm/arch-mtk/nfi.h>
#include <asm/arch-mtk/system.h>
#include <linux/err.h>

/*
 * MT62XX NAND controller (NFI) can suppport small page (512B) and
 * large page (2048B) NAND devices. It also supports hardware ECC control.
 * ECC is calculated by hardware for given block of bytes (128, 256, 512, 1024).
 * NFI controller can acutomatically detect if read page has no errors -
 * comparing calculated ECC with ECC bytes written in spare area.
 * Problem with hardware ECC is that ECC calculation is available after
 * addtional cycle of read/write (after reading of whole page).
 * I.e. page size is 512, so after reading whole page addtional read/write cycle
 * has to be issued to get ECC calculation (NFI_ADDRCNTR has to show more than
 * 512).
 *
 * In case of reading it's not a problem as NFI controller has 16 bytes FIFO and
 * in burst mode it'll issue 16 reads in advance, so after reading whole page
 * from FIFO, ECC calculation will be already available (as 16 addtional bytes
 * are already placed in FIFO). To have ECC calculations working, SPARE_EN bit
 * has to be enabled, otherwise there will be no additional read cycle. In this
 * case AUTO_ECC_DEC_EN mode works (automatic ECC errors detection).
 *
 * Problem is with writing mode, as additional byte has to be written to spare
 * area and only after that ECC calculation will be available. It means that
 * when using HW ECC it's not possible to place calculation at first byte of
 * spare area. There is AUTO_ECC_ENC_EN mode which doesn't seem to work.
 * In this mode NFI controller should automatically write calculated ECC bytes
 * in spare area. To have it working SPARE_EN bit should be disabled, which
 * indicates that NFI controller has control over spare area, not the user.
 * Unfortunatelly even after disabling SPARE_EN bit, NFI controller doesn't
 * write ECC calculations in spare automatically.
 * Because of these NFI controller issues, driver has to have some hacks.
 *
 * Sciphone G2 specific informations:
 *
 * Sciphone G2 comes with small page and large page NAND devices.
 * For both configurations ecc block size is the same:
 *
 *	ecc_block_size = 256
 *
 * Placement of ecc bytes in spare area is as follows:
 *
 * --------------------------------------------------------------
 * |				SPARE				|
 * --------------------------------------------------------------
 * |	| ECC0	|	| ECC1	|	| ECC2	|	| ECC3	|
 * --------------------------------------------------------------
 * 0	8	16	24	32	40	48	56	64
 *
 * ECC0 = 12 bits (from 1st ECC block) + 12 bits (from 2nd ECC block)
 * ECC1 = 12 bits (from 3rd ECC block) + 12 bits (from 4th ECC block)
 * ECC2 = 12 bits (from 5th ECC block) + 12 bits (from 6th ECC block)
 * ECC2 = 12 bits (from 7th ECC block) + 12 bits (from 8th ECC block)
 *
 * This information is pretty important as built-in bootloader in MT62xx chips
 * has hardware ECC enabled and it won't load code from NAND if ECC layout will
 * not match.
 */

/* MT62XX NFI controller has 2 chipselects */
#define NAND_CHIPS_MAX		2

/*
 * Configure which byte in spare area will store ECC calculations.
 * This value should be dword (4 bytes) aligned.
 */
#define ECC_SPARE_BYTE_POS	8

/*
 * Macro which counts zeroes until first set bit.
 * This is used to avoid dividing, so no additional library is needed.
 * It's important as this file is compiled also in SPL and there is no need
 * to link additional library.
 */
#ifdef CONFIG_ARM926EJS
  #define COUNT_ZEROES(x)       __builtin_ctz(x)
#else
  #define COUNT_ZEROES(x)       (ffs(x) - 1)
#endif

#ifdef CONFIG_PRELOADER
  #define nand_print(x)
#else
  #define nand_print(x)	printk(KERN_ERR (x))
#endif

static void nand_ctrl_change(int cmd, int address, int *addr_cycle,
				int write_size)
{
	int burst_read = 0;

	if (write_size > 512) {
		/* Large page device - set burst read on READSTART command */
		if (cmd == NAND_CMD_READSTART)
			burst_read = 1;
	} else {
		/* Small page device - set burst read on all read commands */
		if ((cmd == NAND_CMD_READ0) |
		    (cmd == NAND_CMD_READ1) |
		    (cmd == NAND_CMD_READOOB))
			burst_read = 1;
	}

	/* Any address to write? */
	if (*addr_cycle) {
		writel(address, MTK_NFI_ADDRL);

		if (cmd == NAND_CMD_READID) {
			/* For READID command just one address byte is used */
			writel(1, MTK_NFI_ADDRNOB);
			while(readl(MTK_NFI_PSTA) & NFI_PSTA_ADDR)
				;
			/* Set single read mode */
			writel(NFI_OPCON_SRD, MTK_NFI_OPCON);
			while(readl(MTK_NFI_PSTA) & NFI_PSTA_DATAR)
				;
		} else {
			/* Write calculated number of address bytes */
			writel(*addr_cycle, MTK_NFI_ADDRNOB);
			while(readl(MTK_NFI_PSTA) & NFI_PSTA_ADDR)
				;
		}

		*addr_cycle = 0;
	}

	if (burst_read) {
		/* Set burst read mode */
		writel(NFI_OPCON_BRD, MTK_NFI_OPCON);
		while(readl(MTK_NFI_PSTA) & NFI_PSTA_DATAR)
			;
	} else if (cmd == NAND_CMD_STATUS) {
		/* Set single read mode */
		writel(NFI_OPCON_SRD, MTK_NFI_OPCON);
		while(readl(MTK_NFI_PSTA) & NFI_PSTA_DATAR)
			;
	}
}

static void nand_write_command(int cmd)
{
	/*
	 * Clear BRD and SRD bits before issuing command,
	 * otherwise there can be some zeroes at the beggining of read.
	 */
	writew(readl(MTK_NFI_OPCON) & ~(NFI_OPCON_BRD | NFI_OPCON_SRD),
	       MTK_NFI_OPCON);
	writel(cmd, MTK_NFI_CMD);
	while(readl(MTK_NFI_PSTA) & NFI_PSTA_CMD)
		;
}

static void mt62xx_nand_cmd_ctrl(struct mtd_info *mtd, int cmd,
					unsigned int ctrl)
{
	static int addr_cycle = 0;
	static int address = 0;
	static int command = 0;

	if (ctrl == (NAND_CTRL_CHANGE | NAND_NCE))
		nand_ctrl_change(command, address, &addr_cycle, mtd->writesize);
	else if ((ctrl & NAND_CLE) && (cmd != NAND_CMD_NONE)) {
		command = cmd;
		nand_write_command(cmd);
	} else if (ctrl & NAND_ALE) {
		/*
		 * Calculate address and address bytes which will be written
		 * in nand_ctrl_change() function.
		 */
		if (!addr_cycle)
			address = cmd;
		else
			address |= cmd << (addr_cycle*8);

		addr_cycle++;
	}
}

static int mt62xx_nand_dev_ready(struct mtd_info *mtd)
{
	return !(readl(MTK_NFI_PSTA) & NFI_PSTA_NAND_BUSY);
}

static void mt62xx_nand_select_chip(struct mtd_info *mtd, int chip)
{
	if (chip > NAND_CHIPS_MAX) {
		nand_print("Wrong NAND chip number!\n");
		return;
	}

	if (chip != -1)
		writel(chip, MTK_NFI_CSEL);
}

static void mt62xx_nand_read_buf(struct mtd_info *mtd, uint8_t *buf, int len)
{
	uint32_t *buf_32 = (uint32_t *)buf;
	struct nand_chip *chip = mtd->priv;
	int i;

	if (len % 4)
		nand_print("Length parameter is not aligned\n");

	for (i = 0; i < len/4; ++i)
		buf_32[i] = readl(chip->IO_ADDR_R);
}

static uint8_t mt62xx_nand_read_byte(struct mtd_info *mtd)
{
	struct nand_chip *chip = mtd->priv;
	uint16_t nfi_con = readl(MTK_NFI_CON);
	uint8_t byte;

	/* Enable byte mode reading */
	writel(nfi_con | NFI_CON_BYTE_RW, MTK_NFI_CON);
	byte = readb(chip->IO_ADDR_R);
	/* Disable byte mode reading */
	writel(nfi_con & ~NFI_CON_BYTE_RW, MTK_NFI_CON);
	return byte;
}

static void mt62xx_nand_write_ecc(struct mtd_info *mtd, int len)
{
	int i, ecc_nr, ecc_blocks;
	struct nand_chip *chip = mtd->priv;
	uint8_t ecc[8];

	/*
	 * Two ECC blocks are combined on MT62xx platform,
	 * that's why writesize is multiplied by 2.
	 */
	ecc_blocks = (mtd->writesize * 2) >> COUNT_ZEROES(chip->ecc.size);

	for (ecc_nr = 0; ecc_nr < ecc_blocks; ++ecc_nr) {
		int ecc_p, ecc_c, pos = 0;

		/*
		 * After first write in this loop, ECC calculations will
		 * be available in NFI_PAR_P and NFI_PAR_C registers.
		 */
		for (i = 0; i < ECC_SPARE_BYTE_POS/4; ++i)
			writel(~0, chip->IO_ADDR_W);

		/*
		 * Read calculated ECC bytes and write them
		 * to buffer in proper format (as Sciphone G2 expects).
		 */
		for (i = 0; i < 2; ++i) {
			ecc_p = readw((uint32_t *)MTK_NFI_PAR_0P + ecc_nr*2 + i*2);
			ecc_c = readw((uint32_t *)MTK_NFI_PAR_0C + ecc_nr*2 + i*2);
			ecc[pos++] = ecc_p >> 4;
			ecc[pos++] = ((ecc_p & 0x0F) << 4) | (ecc_c >> 8);
			ecc[pos++] = ecc_c & 0xFF;
		}

		/* Fill rest of buffer with 0xFF */
		while (pos < sizeof(ecc))
			ecc[pos++] = 0xFF;

		/* Write calculated ECC bytes */
		for (i = 0; i < sizeof(ecc)/4; ++i)
			writel(((uint32_t *)ecc)[i], chip->IO_ADDR_W);
	}
}

static void mt62xx_nand_write_buf(struct mtd_info *mtd, const uint8_t *buf,
					int len)
{
	uint32_t *buf_32 = (uint32_t *)buf;
	struct nand_chip *chip = mtd->priv;
	int i;

	/* Writing in spare area? */
	if (readw(MTK_NFI_ADDRCNTR) >= mtd->writesize) {
		/*
		 * Due to issue that ECC calculations are available after
		 * additional read/write cycle, ECC calculations
		 * are handled here.
		 */
		mt62xx_nand_write_ecc(mtd, len);
	} else {
		if (len % 4)
			nand_print("Length parameter is not aligned\n");

		for (i = 0; i < len/4; ++i)
			writel(buf_32[i], chip->IO_ADDR_W);
	}
}

static int mt62xx_nand_ecc_calculate(struct mtd_info *mtd, const uint8_t *dat,
				uint8_t *ecc_code)
{
	struct nand_chip *chip = mtd->priv;

	/*
	 * Calculations are done automatically and there is no need for
	 * additional operations.
	 * This function is used to pass ECC block number to ecc_correct()
	 * function to be able to correct proper ECC block.
	 * MT62XX chips have address counter which points to address in page
	 * which is currently read. We can use it to calculate which ECC block
	 * has been already read.
	 */
	*ecc_code = readw(MTK_NFI_ADDRCNTR) >> COUNT_ZEROES(chip->ecc.size);
	return 0;
}

static int mt62xx_nand_ecc_correct(struct mtd_info *mtd, uint8_t *dat,
				uint8_t *read_ecc, uint8_t *ecc_block_nr)
{
	int ecc_block_mask, ecc_status, ret;

	/* ECC block number is calculated in ecc_calculate() function */
	if (*ecc_block_nr == 0)
		/* Should neve happen */
		return -1;

	/* Decrease value to get proper index */
	(*ecc_block_nr)--;
	ecc_block_mask = 1 << *ecc_block_nr;
	ecc_block_mask |= ecc_block_mask << 16;

	ecc_status = readl(MTK_NFI_ERRDET) & ecc_block_mask;
	if (ecc_status & 0xFF)
		/* Uncorrectable error detected */
		ret = -1;
	else if(ecc_status >> 16) {
		/* Correctable error detected */
		int address;
		uint32_t *buffer = (uint32_t *)dat;

		/* Read address (for given block) where error occured */
		address = readw((uint32_t *)MTK_NFI_SYM0_ADDR + *ecc_block_nr);
		/* Correct error using syndrome word for given block */
		buffer[address >> 2] ^=
			readw((uint32_t *)MTK_NFI_SYM0_DATA + *ecc_block_nr);
		ret = 1;
	} else
		/* Data read without errors */
		ret = 0;

	return ret;
}

static void mt62xx_nand_ecc_hwctl(struct mtd_info *mtd, int mode)
{
	/* HW ECC doesn't need to be controlled, it's turned on during init */
}

int board_nand_init(struct nand_chip *chip)
{
	static int chip_nr = 0;
	struct mtd_info *mtd;

	/* Power on NFI controller */
	writel(PDN_CON1_NFI, MTK_CONFG_PDN_CLR1);

	/* Configure for max wait times */
	writel(3 << NFI_ACCCON_RLT_SHIFT |
	       3 << NFI_ACCCON_WST_SHIFT |
	       3 << NFI_ACCCON_WH_SHIFT  |
	       3 << NFI_ACCCON_W2R_SHIFT |
	       3 << NFI_ACCCON_C2R_SHIFT |
	       7 << NFI_ACCCON_LCD2NAND_SHIFT,
	       MTK_NFI_ACCCON);

	/*
	 * Reset NFI page format control register.
	 * After processor reset it's not always zero what renders problems.
	 */
	writel(0, MTK_NFI_PAGEFMT);

	/* Flush and reset NFI FIFO */
	writel(NFI_OPCON_FIFO_FLUSH | NFI_OPCON_FIFO_RST, MTK_NFI_OPCON);
	while(readl(MTK_NFI_OPCON))
		;

	/* Enable spare area and ECC decoding for main */
	writel(NFI_CON_AUTOECC_DEC_EN |
	       NFI_CON_MAIN_ECC_EN    |
	       NFI_CON_SPARE_EN,
	       MTK_NFI_CON);
	/* Setup byte number in spare for ECC */
	writel(ECC_SPARE_BYTE_POS, MTK_NFI_SCON);

	chip->IO_ADDR_R = (uint32_t *)MTK_NFI_DATAR;
	chip->IO_ADDR_W = (uint32_t *)MTK_NFI_DATAW;
	chip->cmd_ctrl = mt62xx_nand_cmd_ctrl;
	chip->dev_ready = mt62xx_nand_dev_ready;
	chip->read_buf = mt62xx_nand_read_buf;
	chip->read_byte = mt62xx_nand_read_byte;
	chip->write_buf = mt62xx_nand_write_buf;
	chip->select_chip = mt62xx_nand_select_chip;

	/* ECC settings */
	chip->ecc.mode = NAND_ECC_HW;
	chip->ecc.calculate = mt62xx_nand_ecc_calculate;
	chip->ecc.correct = mt62xx_nand_ecc_correct;
	chip->ecc.hwctl = mt62xx_nand_ecc_hwctl;
	chip->ecc.size = 256;

	mtd = &nand_info[chip_nr++];

	/* Detect NAND chip */
	if (nand_scan_ident(mtd, 1, NULL))
		return -ENXIO;

	if (mtd->writesize > 512)
		/* Large page NAND detected */
		writel(NFI_PAGEFMT_PSIZE_2048		|
		       NFI_PAGEFMT_ADRMODE_LARGE_8IO	|
		       NFI_PAGEFMT_ECCBLKSIZE_256,
		       MTK_NFI_PAGEFMT);
	else
		/* Small page NAND detected */
		writel(NFI_PAGEFMT_ECCBLKSIZE_256,
		       MTK_NFI_PAGEFMT);

	return 0;
}
