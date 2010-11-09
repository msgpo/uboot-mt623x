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

static void mtk_cmd_ctrl(struct mtd_info *mtd, int cmd, unsigned int ctrl)
{
	struct nand_chip *chip = mtd->priv;
	static int addr_bit = 0;
	static int address = 0;
	static int command = 0;

	if (ctrl == (NAND_CTRL_CHANGE | NAND_NCE)) {
		chip->IO_ADDR_R = (void *)MTK_NFI_DATARB;
		if (addr_bit) {
/*
			while(!((readw(MTK_NFI_FIFOSTA) & 0xFF) == NFI_FIFOSTA_RD_EMPTY))
				readb(MTK_NFI_DATARB);*/
			writel(address, MTK_NFI_ADDRL);
			if (command == NAND_CMD_READID ||
			    command == NAND_CMD_STATUS) {
				writeb(1, MTK_NFI_ADDRNOB);
				while(readl(MTK_NFI_PSTA) & NFI_PSTA_ADDR)
					;
				writel(NFI_OPCON_SRD, MTK_NFI_OPCON);
			} else {
				writeb(addr_bit >> 3, MTK_NFI_ADDRNOB);
				while(readl(MTK_NFI_PSTA) & NFI_PSTA_ADDR)
					;
				writel(NFI_OPCON_BRD, MTK_NFI_OPCON);
			}
			addr_bit = 0;
		}
	} else if ((ctrl & NAND_CLE) && (cmd != NAND_CMD_NONE)) {
		command = cmd;
		writel(cmd, MTK_NFI_CMD);
		while(readl(MTK_NFI_PSTA) & NFI_PSTA_CMD)
			;
	} else if (ctrl & NAND_ALE) {
		if (!addr_bit)
			address = cmd;
		else
			address |= cmd << addr_bit;

		addr_bit += 8;
	}
}

static int mtk_dev_ready(struct mtd_info *mtd)
{
	return !(readl(MTK_NFI_PSTA) & NFI_PSTA_NAND_BUSY);
}

int board_nand_init(struct nand_chip *nand)
{
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

	/* Flush and reset NFI FIFO */
	writel(NFI_OPCON_FIFO_FLUSH | NFI_OPCON_FIFO_RST, MTK_NFI_OPCON);
	while(readl(MTK_NFI_OPCON))
		;
	writel(NFI_PAGEFMT_PSIZE_2048		|
	       NFI_PAGEFMT_ADRMODE_LARGE_8IO	|
	       NFI_PAGEFMT_ECCBLKSIZE_1024,
	       MTK_NFI_PAGEFMT);

	writel(NFI_CON_SPARE_EN, MTK_NFI_CON);

	nand->cmd_ctrl = mtk_cmd_ctrl;
	nand->dev_ready = mtk_dev_ready;
	nand->ecc.mode = NAND_ECC_SOFT;

	return 0;
}
