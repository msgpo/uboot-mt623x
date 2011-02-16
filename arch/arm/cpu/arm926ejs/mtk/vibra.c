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

#include <common.h>
#include <asm/io.h>
#include <asm/arch-mtk/mt6235.h>
#include <asm/arch-mtk/system.h>
#include <asm/arch-mtk/pwm.h>

#define PWM2_TRESH_VALUE	16
#define PWM2_DATA_WIDTH_VALUE	16
#define PWM2_GDURATION_VALUE	0

void vibra_on(void)
{
	/* Power up PWM2 */
	writew(PDN_CON1_PWM2, MTK_CONFG_PDN_CLR1);

	/* Enable VIBRA in PMIC. */
	writew(readw(MTK_PMIC_CON5) | PMIC_CON5_VIBR_EN, MTK_PMIC_CON5);

	/* Configure PWM2 output in old mode and with 32kHz. */
	writew(PWM_CON_OLD_PWM_MODE | PWM_CON_CLKSEL, PWM_PWM2_CON);

	writew(PWM2_TRESH_VALUE, PWM_PWM2_THRESH);
	writew(PWM2_DATA_WIDTH_VALUE, PWM_PWM2_DATA_WIDTH);
	writew(PWM2_GDURATION_VALUE, PWM_PWM2_GDURATION);

	/* Start vibrating. */
	writew(readw(PWM_ENABLE) | PWM_ENABLE_PWM2_EN, PWM_ENABLE);
}

void vibra_off(void)
{
	writew(readw(PWM_ENABLE) & (~PWM_ENABLE_PWM2_EN), PWM_ENABLE);
	//writew(PDN_CON1_PWM2, MTK_CONFG_PDN_SET1);
}
