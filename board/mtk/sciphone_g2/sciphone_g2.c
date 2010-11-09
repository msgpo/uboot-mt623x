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
#include <asm/io.h>
#include <asm/arch-mtk/emi.h>
#include <asm/arch-mtk/system.h>

DECLARE_GLOBAL_DATA_PTR;

#ifdef CONFIG_SHOW_BOOT_PROGRESS
void show_boot_progress(int progress)
{
	printf("%i\n", progress);
}
#endif

int board_init(void)
{
	gd->bd->bi_arch_number = MACH_TYPE_SCIPHONE_G2;
	gd->bd->bi_boot_params = 0x00000100;

	/* Powerup BB */
	writew(POWERKEY1_MAGIC, MTK_RTC_POWERKEY1);
	writew(POWERKEY2_MAGIC, MTK_RTC_POWERKEY2);
	writew(BBPU_MAGIC | RTC_BBPU_WRITE_EN |
	       RTC_BBPU_BBPU | RTC_BBPU_AUTO,
	       MTK_RTC_BBPU);
	writew(1, MTK_RTC_WRTGR);

	/* Disable watchdog */
	writew(WDT_MODE_KEY, MTK_RGU_WDT_MODE);
	writew(WDT_MODE_KEY, MTK_RGU_WDT_MODE);

	/*
	 * PLL configuration influents JTAG clock.
	 * Currently it's disabled to avoid problems with JTAG.
	 */
#if 0
	/* Power on PLL */
	writew(0, MTK_PLL_PDN_CON);

	/* Turn on MCU and DSP dividers, mark that SYSCLK is 26MHz */
	writew(PLL_CLKSQ_DIV2_DSP | PLL_CLKSQ_DIV2_MCU | PLL_SRCCLK,
	       MTK_PLL_CLK_CON);

	/* Reset PLL */
	writew(PLL_RST, MTK_PLL_PLL);
	writew(0, MTK_PLL_PLL);
	udelay(100);

	/* Turn on PLL for MCU, DSP and USB */
	writew(PLL_MPLLSEL_PLL | PLL_DPLLSEL | PLL_UPLLSEL, MTK_PLL_PLL);

	/*
	 * Setup MCU clock register:
	 * ARMCLK = 208MHz, AHBx4CLK = 52MHz, AHBx8CLK = 104MHz
	 * we have to write to the read-only part (EMICLK) as well, otherwise
	 * the EMI won't work! (datasheet lies)
	 */
	writew(7 << MCUCLK_CON_AHBX8CLK_SHIFT |
	      3 << MCUCLK_CON_AHBX4CLK_SHIFT |
	      15 << MCUCLK_CON_ARMCLK_SHIFT |
	      7 << MCUCLK_CON_EMICLK_SHIFT,
	      MTK_CONFG_MCUCLK_CON);
#endif
	return 0;
}

int dram_init(void)
{
	gd->ram_size = get_ram_size(PHYS_SDRAM_1, CONFIG_MAX_RAM_BANK_SIZE);

	/*
	 * Currently UBoot is executed from external RAM and below configuration
	 * will make it to stop working. This code will be uncommented when
	 * UBoot will be placed in NAND memory.
	 */
#if 0
	/* Configure DRAM controller */
	writel(0x0001000E, MTK_EMI_GEND);
	writel(0x00088E3A, MTK_EMI_GENA);
	writel(0x000000C0, MTK_EMI_GENB);
	writel(0x18C618C6, MTK_EMI_GENC);
	writel(0x18007505, MTK_EMI_CONL);
	writel(0x00002828, MTK_EMI_CONM);
	writel(0x00332000, MTK_EMI_CONI);
	writel(0x3CD24431, MTK_EMI_CONJ);
	writel(0x02000000, MTK_EMI_CONK);

	for (i = 0; i < 5; ++i) {
		/* Setup five single bits, one by one for DRAM init */
		writel((1 << (24 + i)) | (0x500013), MTK_EMI_CONN);
		udelay(1);
		writel(0x500013, MTK_EMI_CONN);
		udelay(1);
	}
#endif
	return 0;
}
