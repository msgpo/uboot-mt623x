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
#include <asm/arch-mtk/system.h>
#include <asm/arch-mtk/pwm.h>
#include <asm/arch-mtk/gpio.h>

#define BACK_PWM4_CON_H_L_DURATION		0x3F79
#define	BACK_PWM4_CON_CLKDIV			3
#define BACK_PWM4_CON_STOP_BITPOS		1
#define	BACK_PWM4_SEND_DATA0			1

/*
 * Turns on LCD backlight.
 */
void lcd_backlight_on(void)
{
	/* Turn on the PWM block. */
	writew(PDN_CON0_PWM, MTK_CONFG_PDN_CLR0);
	/* Choose alternative function for GPIO40 - PWM output. */
	writew((readw(MTK_GPIO_MODE6) & (~MTK_GPIO_MODE6_GPIO40_ALT3)) |
		MTK_GPIO_MODE6_GPIO40_ALT1, MTK_GPIO_MODE6);

	/* Set periodical mode. */
	writew(BACK_PWM4_CON_CLKDIV | (BACK_PWM4_CON_STOP_BITPOS <<
		PWM_CON_STOP_BITPOS), PWM_PWM4_CON);

	writew(BACK_PWM4_CON_H_L_DURATION, PWM_PWM4_HDURATION);
	writew(BACK_PWM4_CON_H_L_DURATION, PWM_PWM4_LDURATION);

	writew(BACK_PWM4_SEND_DATA0, PWM_PWM4_SEND_DATA0);

	/* Enable PWM4. */
	writew(readw(PWM_ENABLE) | PWM_ENABLE_PWM4_EN, PWM_ENABLE);
}

/*
 * Turns off LCD backlight.
 */
void lcd_backlight_off(void)
{
	/* Enable PWM4. */
	writew(readw(PWM_ENABLE) & (~PWM_ENABLE_PWM4_EN), PWM_ENABLE);
}
