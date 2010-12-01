/*
 * (C) 2010 by Tieto <www.tieto.com>
 *      Marcin Mielczarczyk <marcin.mielczarczyk@tieto.com>
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

#include <config.h>
#include <asm/arch-mtk/mt6235.h>
#include <asm/arch-mtk/emi.h>
#include <asm/arch-mtk/system.h>

.globl lowlevel_init
.type lowlevel_init,function
lowlevel_init:

/* -----------------------------
 * Initialize PLL
 * -----------------------------
 */
	/* Power on PLL */
	ldr	r0, =MTK_PLL_PDN_CON
	mov	r1, #0
	str	r1, [r0]
	/* Turn on MCU and DSP dividers, mark that SYSCLK is 26MHz */
	ldr	r0, =MTK_PLL_CLK_CON
	mov	r1, #0x83
	str	r1, [r0]
	/* Reset PLL */
	ldr	r0, =MTK_PLL_PLL
	mov	r1, #0x80
	str	r1, [r0]
	mov	r1, #0
	str	r1, [r0]
	mov	r1, #0xFF
PLL_DELAY_loop:
	subs	r1, r1, #1
	bgt	PLL_DELAY_loop
	/* Turn on PLL for MCU, DSP and USB */
	mov	r1, #0x70
	str	r1, [r0]
	/*
	 * Setup MCU clock register:
	 * ARMCLK = 208MHz, AHBx4CLK = 52MHz, AHBx8CLK = 104MHz
	 * we have to write to the read-only part (EMICLK) as well, otherwise
	 * the EMI won't work! (datasheet lies)
	 */
	ldr	r0, =MTK_CONFG_MCUCLK_CON
	ldr	r1, =0x7F37
	str	r1, [r0]

/* -----------------------------
 * Initialize SDRAM controller
 * -----------------------------
 */
	/* Reset index of SDRAM config table */
	mov	r7, #0
SDRAM_TRY_CONFIG:
	adr	r1, SDRAM_CONFIG_TABLE
	/* Calculate config table offset (index * element_size) */
	lsl	r2, r7, #3
	/* Calculate address of element in config table */
	add	r1, r1, r2
	/* Load start address of SDRAM configuration */
	ldr	r0, [r1]
	/* Check if end of SDRAM config table is reached */
	cmp	r0, #0
	beq	SDRAM_PASSED
	/* Load end address of SDRAM configuration */
	ldr	r6, [r1, #4]

/* Configure SDRAM controller */
SDRAM_REGSET_LOOP:
	ldr	r1, [r0]
	ldr	r2, [r0, #4]
	str	r1, [r2]
	add	r0, r0, #8
	mov	r1, #0xFF
SDRAM_DELAY_LOOP:
	subs	r1, r1, #1
	bgt	SDRAM_DELAY_LOOP
	cmp	r0, r6
	blt	SDRAM_REGSET_LOOP

/* Perform test to check if SDRAM is properly configured */
	mov	r0, #0x1000
	mov	r1, #0x0
	mov	r2, #16384
SDRAM_WRITE_LOOP:
	str	r1, [r0]
	add	r0, r0, #4
	add	r1, r1, #1
	cmp	r1, r2
	blt	SDRAM_WRITE_LOOP

	mov	r0, #0x1000
	mov	r3, #0x0
SDRAM_VERIFY_LOOP:
	ldr	r1, [r0]
	add	r0, r0, #4
	cmp	r1, r3
	bne	SDRAM_FAILED
	add	r3, r3, #1
	cmp	r3, r2
	blt	SDRAM_VERIFY_LOOP

SDRAM_PASSED:
	/* return from function */
	mov	pc, lr

/* SDRAM configuration failed, try another one */
SDRAM_FAILED:
	/* Increment SDRAM config table index */
	add	r7, r7, #1
	b	SDRAM_TRY_CONFIG

SDRAM_CONFIG_TABLE:
	.word	SDRAM_64MB_CONFIG
	.word	SDRAM_64MB_CONFIG_END
	.word	SDRAM_32MB_CONFIG
	.word	SDRAM_32MB_CONFIG_END
	.word	0

/* This configuration is for 64MB SDRAM memory */
SDRAM_64MB_CONFIG:
	.word	0x00088E3A
	.word	MTK_EMI_GENA
	.word	0x000000C0
	.word	MTK_EMI_GENB
	.word	0x18C618C6
	.word	MTK_EMI_GENC
	.word	0x0001000E
	.word	MTK_EMI_GEND
	.word	0x00332000
	.word	MTK_EMI_CONI
	.word	0x3CD24431
	.word	MTK_EMI_CONJ
	.word	0x02000000
	.word	MTK_EMI_CONK
	.word	0x18007505
	.word	MTK_EMI_CONL
	.word	0x00002828
	.word	MTK_EMI_CONM
	.word	0x1500013
	.word	MTK_EMI_CONN
	.word	0x500013
	.word	MTK_EMI_CONN
	.word	0x2500013
	.word	MTK_EMI_CONN
	.word	0x500013
	.word	MTK_EMI_CONN
	.word	0x4500013
	.word	MTK_EMI_CONN
	.word	0x500013
	.word	MTK_EMI_CONN
	.word	0x8500013
	.word	MTK_EMI_CONN
	.word	0x500013
	.word	MTK_EMI_CONN
	.word	0x80500013
	.word	MTK_EMI_CONN
	.word	0x500013
	.word	MTK_EMI_CONN
SDRAM_64MB_CONFIG_END:

/* This configuration is for 32MB SDRAM memory */
SDRAM_32MB_CONFIG:
	.word	0x00088a0a
	.word	MTK_EMI_GENA
	.word	0x00000280
	.word	MTK_EMI_GENB
	.word	0x52945294
	.word	MTK_EMI_GENC
	.word	0x0001000E
	.word	MTK_EMI_GEND
	.word	0x02334000
	.word	MTK_EMI_CONI
	.word	0x16c12212
	.word	MTK_EMI_CONJ
	.word	0x032d0000
	.word	MTK_EMI_CONK
	.word	0x1c016605
	.word	MTK_EMI_CONL
	.word	0x00002828
	.word	MTK_EMI_CONM
	.word	0x1400013
	.word	MTK_EMI_CONN
	.word	0x400013
	.word	MTK_EMI_CONN
	.word	0x2400013
	.word	MTK_EMI_CONN
	.word	0x400013
	.word	MTK_EMI_CONN
	.word	0x4400013
	.word	MTK_EMI_CONN
	.word	0x400013
	.word	MTK_EMI_CONN
	.word	0x8400013
	.word	MTK_EMI_CONN
	.word	0x400013
	.word	MTK_EMI_CONN
	.word	0x80400013
	.word	MTK_EMI_CONN
	.word	0x400013
	.word	MTK_EMI_CONN
SDRAM_32MB_CONFIG_END:
