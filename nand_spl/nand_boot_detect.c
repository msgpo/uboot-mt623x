/*
 * (C) Copyright 2006-2008
 * Stefan Roese, DENX Software Engineering, sr@denx.de.
 *
 * Rework by Marcin Mielczarczyk <marcin.mielczarczyk@tieto.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <nand.h>
#include <asm/io.h>
#include <linux/err.h>
#include <asm/arch-mtk/vibra.h>

/*
 * SPL has no malloc availble and below are definitions of some temporary
 * buffers in RAM for general purpose.
 */
#define RAM_BUF1	(CONFIG_SYS_SDRAM_BASE + 0x1000)
#define RAM_BUF2	(RAM_BUF1 + 0x1000)
#define RAM_BUF3	(RAM_BUF2 + 0x1000)

/*
 * Macro which counts zeroes until first set bit.
 * This is used to avoid dividing, so no additional library is needed.
 * It's important as this file is compiled also in SPL and there is no need
 * to link additional library.
 */
#ifdef CONFIG_ARM926EJS
  #define COUNT_ZEROES(x)	__builtin_ctz(x)
#else
  #define COUNT_ZEROES(x)	(ffs(x) - 1)
#endif

nand_info_t nand_info[1];

#define CONFIG_SYS_NAND_READ_DELAY \
	{ volatile int dummy; int i; for (i=0; i<10000; i++) dummy = i; }

/*
 * NAND command for small page NAND devices (512)
 */
static void nand_command(struct mtd_info *mtd, unsigned int cmd,
			int column, int page_addr)
{
	struct nand_chip *chip = mtd->priv;
	int ctrl = NAND_CTRL_CLE | NAND_CTRL_CHANGE;

	if (chip->dev_ready)
		while (!chip->dev_ready(mtd))
			;
	else
		CONFIG_SYS_NAND_READ_DELAY;

	/* Begin command latch cycle */
	chip->cmd_ctrl(mtd, cmd, NAND_CTRL_CLE | NAND_CTRL_CHANGE);

	ctrl = NAND_CTRL_ALE | NAND_CTRL_CHANGE;
	/* Serially input address */
	if (column != -1) {
		/* Adjust columns for 16 bit buswidth */
		if (chip->options & NAND_BUSWIDTH_16)
			column >>= 1;
		chip->cmd_ctrl(mtd, column, ctrl);
		ctrl &= ~NAND_CTRL_CHANGE;
	}
	if (page_addr != -1) {
		chip->cmd_ctrl(mtd, page_addr, ctrl);
		ctrl &= ~NAND_CTRL_CHANGE;
		chip->cmd_ctrl(mtd, page_addr >> 8, ctrl);
		/* One more address cycle for devices > 32MiB */
		if (chip->chipsize > (32 << 20))
			chip->cmd_ctrl(mtd, page_addr >> 16, ctrl);
	}
	chip->cmd_ctrl(mtd, NAND_CMD_NONE, NAND_NCE | NAND_CTRL_CHANGE);

	/*
	 * Wait a while for the data to be ready
	 */
	if (chip->dev_ready)
		while (!chip->dev_ready(mtd))
			;
	else
		CONFIG_SYS_NAND_READ_DELAY;
}

/*
 * NAND command for large page NAND devices (2k)
 */
static void nand_command_lp(struct mtd_info *mtd, unsigned int cmd, int column,
				int page_addr)
{
	struct nand_chip *chip = mtd->priv;

	if (chip->dev_ready)
		while (!chip->dev_ready(mtd))
			;
	else
		CONFIG_SYS_NAND_READ_DELAY;

	/* Emulate NAND_CMD_READOOB */
	if (cmd == NAND_CMD_READOOB) {
		page_addr += mtd->writesize;
		cmd = NAND_CMD_READ0;
	}

	/* Command latch cycle */
	chip->cmd_ctrl(mtd, cmd & 0xff,
			NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);

	if (column != -1 || page_addr != -1) {
		int ctrl = NAND_CTRL_CHANGE | NAND_NCE | NAND_ALE;

		/* Serially input address */
		if (column != -1) {
			/* Adjust columns for 16 bit buswidth */
			if (chip->options & NAND_BUSWIDTH_16)
				column >>= 1;
			chip->cmd_ctrl(mtd, column, ctrl);
			ctrl &= ~NAND_CTRL_CHANGE;
			chip->cmd_ctrl(mtd, column >> 8, ctrl);
		}
		if (page_addr != -1) {
			chip->cmd_ctrl(mtd, page_addr, ctrl);
			chip->cmd_ctrl(mtd, page_addr >> 8,
					NAND_NCE | NAND_ALE);
			/* One more address cycle for devices > 128MiB */
			if (chip->chipsize > (128 << 20))
				chip->cmd_ctrl(mtd, page_addr >> 16,
						NAND_NCE | NAND_ALE);
		}
	}
	chip->cmd_ctrl(mtd, NAND_CMD_NONE, NAND_NCE | NAND_CTRL_CHANGE);

	chip->cmd_ctrl(mtd, NAND_CMD_READSTART,
		       NAND_CTRL_CLE | NAND_CTRL_CHANGE);
	chip->cmd_ctrl(mtd, NAND_CMD_NONE, NAND_NCE | NAND_CTRL_CHANGE);

	/*
	 * Wait a while for the data to be ready
	 */
	if (chip->dev_ready)
		while (!chip->dev_ready(mtd))
			;
	else
		CONFIG_SYS_NAND_READ_DELAY;
}

static int nand_is_bad_block(struct mtd_info *mtd, int page_addr)
{
	struct nand_chip *chip = mtd->priv;

	if ((page_addr & (mtd->erasesize - 1)) != 0)
		/*
		 * Page address is not aligned to block address,
		 * in this case there is no reason to check bad block.
		 */
		return 0;

	chip->cmdfunc(mtd, NAND_CMD_READOOB, 0, page_addr);
	chip->read_buf(mtd, chip->oob_poi, mtd->oobsize);

	return (chip->oob_poi[5] != 0xFF) ? 1: 0;
}

static int nand_read_page(struct mtd_info *mtd, int page_addr,
				unsigned char *dst)
{
	struct nand_chip *chip = mtd->priv;
	uint8_t *ecc_calc;
	uint8_t *ecc_code;
	int i;
	int eccsize = chip->ecc.size;
	int eccbytes = chip->ecc.bytes;
	int eccsteps = chip->ecc.steps;
	uint32_t *eccpos = chip->ecc.layout->eccpos;
	uint8_t *p = dst;

	chip->cmdfunc(mtd, NAND_CMD_READ0, 0, page_addr);

	/*
	 * No malloc available for now, just use some temporary locations
	 * in SDRAM.
	 */
	ecc_calc = (uint8_t *)RAM_BUF2;
	ecc_code = (uint8_t *)RAM_BUF3;

	for (i = 0; eccsteps; eccsteps--, i += eccbytes, p += eccsize) {
		chip->ecc.hwctl(mtd, NAND_ECC_READ);
		chip->read_buf(mtd, p, eccsize);
		chip->ecc.calculate(mtd, p, &ecc_calc[i]);
	}
	chip->read_buf(mtd, chip->oob_poi, mtd->oobsize);

	/* Pick the ECC bytes out of the oob data */
	for (i = 0; i < chip->ecc.total; i++)
		ecc_code[i] = chip->oob_poi[eccpos[i]];

	eccsteps = chip->ecc.steps;
	p = dst;

	for (i = 0 ; eccsteps; eccsteps--, i += eccbytes, p += eccsize) {
		/*
		 * No chance to do something with the possible error message
		 * from correct_data(). We just hope that all possible errors
		 * are corrected by this routine.
		 */
		chip->ecc.correct(mtd, p, &ecc_code[i], &ecc_calc[i]);
	}

	return 0;
}

static void nand_load(struct mtd_info *mtd, unsigned int offset,
		     unsigned int size, uint8_t *dst)
{
	unsigned int page, end_page, pages_per_block;

	/*
	 * Offset should be aligned to page and block size, otherwise
	 * there will be no chance to detect bad block of start offset.
	 */
	page = offset >> COUNT_ZEROES(mtd->writesize);
	end_page = page + (size >> COUNT_ZEROES(mtd->writesize));
	/* If size is not aligned to page then read one more page */
	if (size & (mtd->writesize - 1))
		end_page++;
	pages_per_block = mtd->erasesize >> COUNT_ZEROES(mtd->writesize);

	while (page < end_page) {
		/* If this read is in new block, check for babd block*/
		if ((page & (pages_per_block - 1)) == 0) {
			if (nand_is_bad_block(mtd, page)) {
				/*
				 * Bad block is detected.
				 * Jump to next block and continue code loading.
				 * In this case not full image will be loaded.
				 */
				page += pages_per_block;
				continue;
			}
		}

		nand_read_page(mtd, page, dst);
		page++;
		dst += mtd->writesize;
	}
}

#if defined(CONFIG_ARM)
void board_init_f (ulong bootflag)
{
	relocate_code (CONFIG_SYS_TEXT_BASE - TOTAL_MALLOC_LEN, NULL,
		       CONFIG_SYS_TEXT_BASE);
}
#endif

/*
 * Get the flash and manufacturer id and lookup if the type is supported
 */
static const struct nand_flash_dev *nand_get_flash_type(struct mtd_info *mtd,
						  struct nand_chip *chip,
						  int busw, int *maf_id)
{
	const struct nand_flash_dev *type = NULL;
	int i, dev_id, maf_idx;
	int tmp_id, tmp_manf;

	/* Select the device */
	chip->select_chip(mtd, 0);

	/*
	 * Reset the chip, required by some chips (e.g. Micron MT29FxGxxxxx)
	 * after power-up
	 */
	chip->cmdfunc(mtd, NAND_CMD_RESET, -1, -1);

	/* Send the command for reading device ID */
	chip->cmdfunc(mtd, NAND_CMD_READID, 0x00, -1);

	/* Read manufacturer and device IDs */
	*maf_id = chip->read_byte(mtd);
	dev_id = chip->read_byte(mtd);

	/* Try again to make sure, as some systems the bus-hold or other
	 * interface concerns can cause random data which looks like a
	 * possibly credible NAND flash to appear. If the two results do
	 * not match, ignore the device completely.
	 */
	chip->cmdfunc(mtd, NAND_CMD_READID, 0x00, -1);

	/* Read manufacturer and device IDs */
	tmp_manf = chip->read_byte(mtd);
	tmp_id = chip->read_byte(mtd);

	if (tmp_manf != *maf_id || tmp_id != dev_id)
		return ERR_PTR(-ENODEV);

	/* Lookup the flash id */
	for (i = 0; nand_flash_ids[i].name != NULL; i++) {
		if (dev_id == nand_flash_ids[i].id) {
			type =  &nand_flash_ids[i];
			break;
		}
	}

	if (!type)
		return ERR_PTR(-ENODEV);

	if (!mtd->name)
		mtd->name = type->name;

	chip->chipsize = (uint64_t)type->chipsize << 20;

	/* Newer devices have all the information in additional id bytes */
	if (!type->pagesize) {
		int extid;
		/* The 3rd id byte holds MLC / multichip data */
		chip->cellinfo = chip->read_byte(mtd);
		/* The 4th id byte is the important one */
		extid = chip->read_byte(mtd);
		/* Calc pagesize */
		mtd->writesize = 1024 << (extid & 0x3);
		extid >>= 2;
		/* Calc oobsize */
		mtd->oobsize = (8 << (extid & 0x01)) * (mtd->writesize >> 9);
		extid >>= 2;
		/* Calc blocksize. Blocksize is multiples of 64KiB */
		mtd->erasesize = (64 * 1024) << (extid & 0x03);
		extid >>= 2;
		/* Get buswidth information */
		busw = (extid & 0x01) ? NAND_BUSWIDTH_16 : 0;

	} else {
		/*
		 * Old devices have chip data hardcoded in the device id table
		 */
		mtd->erasesize = type->erasesize;
		mtd->writesize = type->pagesize;
		mtd->oobsize = mtd->writesize / 32;
		busw = type->options & NAND_BUSWIDTH_16;
	}

	/* Try to identify manufacturer */
	for (maf_idx = 0; nand_manuf_ids[maf_idx].id != 0x0; maf_idx++) {
		if (nand_manuf_ids[maf_idx].id == *maf_id)
			break;
	}

	/*
	 * Check, if buswidth is correct. Hardware drivers should set
	 * chip correct !
	 */
	if (busw != (chip->options & NAND_BUSWIDTH_16))
		return ERR_PTR(-EINVAL);

	/* Calculate the address shift from the page size */
	chip->page_shift = ffs(mtd->writesize) - 1;

	chip->bbt_erase_shift = chip->phys_erase_shift =
		ffs(mtd->erasesize) - 1;
	if (chip->chipsize & 0xffffffff)
		chip->chip_shift = ffs((unsigned)chip->chipsize) - 1;
	else
		chip->chip_shift = ffs((unsigned)(chip->chipsize >> 32)) + 31;

	/* Set the bad block position */
	chip->badblockpos = mtd->writesize > 512 ?
		NAND_LARGE_BADBLOCK_POS : NAND_SMALL_BADBLOCK_POS;

	/* Get chip options, preserve non chip based options */
	chip->options &= ~NAND_CHIPOPTIONS_MSK;
	chip->options |= type->options & NAND_CHIPOPTIONS_MSK;

	/*
	 * Set chip as a default. Board drivers can override it, if necessary
	 */
	chip->options |= NAND_NO_AUTOINCR;

	/* Check if chip is a not a samsung device. Do not clear the
	 * options for chips which are not having an extended id.
	 */
	if (*maf_id != NAND_MFR_SAMSUNG && !type->pagesize)
		chip->options &= ~NAND_SAMSUNG_LP_OPTIONS;

	/* Do not replace user supplied command function ! */
	if (mtd->writesize > 512 && chip->cmdfunc == nand_command)
		chip->cmdfunc = nand_command_lp;

	return type;
}

int nand_scan_ident(struct mtd_info *mtd, int maxchips,
			const struct nand_flash_dev *table)
{
	int busw, nand_maf_id;
	struct nand_chip *chip = mtd->priv;
	const struct nand_flash_dev *type;

	/* Get buswidth to select the correct functions */
	busw = chip->options & NAND_BUSWIDTH_16;

	/* Read the flash type */
	type = nand_get_flash_type(mtd, chip, busw, &nand_maf_id);

	if (IS_ERR(type)) {
		chip->select_chip(mtd, -1);
		return PTR_ERR(type);
	}

	/* Store the number of chips and calc total size for mtd */
	chip->numchips = 1;
	mtd->size = chip->chipsize;

	return 0;
}

/*
 * The main entry for NAND booting. It's necessary that SDRAM is already
 * configured and available since this code loads the main U-Boot image
 * from NAND into SDRAM and starts it from there.
 */
void nand_boot(void)
{
	struct nand_chip chip;
	nand_info_t *mtd = nand_info;
	__attribute__((noreturn)) void (*uboot)(void);

	vibra_on();

	/*
	 * Init board specific nand support
	 */
	mtd->priv = &chip;
	chip.IO_ADDR_R = (void __iomem *)CONFIG_SYS_NAND_BASE;
	chip.IO_ADDR_W = (void __iomem *)CONFIG_SYS_NAND_BASE;
	chip.dev_ready = NULL;
	chip.cmdfunc = nand_command;
	chip.options = 0;

	board_nand_init(&chip);

	chip.ecc.steps = mtd->writesize >> COUNT_ZEROES(chip.ecc.size);
	chip.oob_poi = (uint8_t *)RAM_BUF1;

	if (chip.select_chip)
		chip.select_chip(mtd, 0);

	/*
	 * Load U-Boot image from NAND into RAM
	 */
	nand_load(mtd, CONFIG_SYS_NAND_U_BOOT_OFFS, CONFIG_SYS_NAND_U_BOOT_SIZE,
			(uint8_t *)CONFIG_SYS_NAND_U_BOOT_DST);

#ifdef CONFIG_NAND_ENV_DST
	nand_load(mtd, CONFIG_ENV_OFFSET, CONFIG_ENV_SIZE,
		  (uint8_t *)CONFIG_NAND_ENV_DST);

#ifdef CONFIG_ENV_OFFSET_REDUND
	nand_load(mtd, CONFIG_ENV_OFFSET_REDUND, CONFIG_ENV_SIZE,
		  (uint8_t *)CONFIG_NAND_ENV_DST + CONFIG_ENV_SIZE);
#endif
#endif

	if (chip.select_chip)
		chip.select_chip(mtd, -1);

	vibra_off();

	/*
	 * Jump to U-Boot image
	 */
	uboot = (void *)CONFIG_SYS_NAND_U_BOOT_START;
	(*uboot)();
}
