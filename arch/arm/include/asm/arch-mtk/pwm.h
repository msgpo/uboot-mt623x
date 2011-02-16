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

#ifndef __PWM_H
#define __PWM_H

/* PWM registry addresses. */
#define PWM_ENABLE		(MTK_PWM_BASE + 0x0000)
#define PWM_PWM4_DELAY		(MTK_PWM_BASE + 0x0004)
#define PWM_PWM5_DELAY		(MTK_PWM_BASE + 0x0008)
#define PWM_PWM6_DELAY		(MTK_PWM_BASE + 0x000C)
#define PWM_PWM1_CON		(MTK_PWM_BASE + 0x0010)
#define PWM_PWM1_HDURATION	(MTK_PWM_BASE + 0x0014)
#define PWM_PWM1_LDURATION	(MTK_PWM_BASE + 0x0018)
#define PWM_PWM1_GDURATION	(MTK_PWM_BASE + 0x001C)
#define PWM_PWM1_BUF0_BASE_ADDR	(MTK_PWM_BASE + 0x0020)
#define PWM_PWM1_BUF0_SIZE	(MTK_PWM_BASE + 0x0024)
#define PWM_PWM1_BUF1_BASE_ADDR	(MTK_PWM_BASE + 0x0028)
#define PWM_PWM1_BUF1_SIZE	(MTK_PWM_BASE + 0x002C)
#define PWM_PWM1_SEND_DATA0	(MTK_PWM_BASE + 0x0030)
#define PWM_PWM1_SEND_DATA1	(MTK_PWM_BASE + 0x0034)
#define PWM_PWM1_WAVE_NUM	(MTK_PWM_BASE + 0x0038)
#define PWM_PWM1_DATA_WIDTH	(MTK_PWM_BASE + 0x003C)
#define PWM_PWM1_THRESH		(MTK_PWM_BASE + 0x0040)
#define PWM_PWM1_SEND_WAVENUM	(MTK_PWM_BASE + 0x0044)
#define PWM_PWM1_VALID		(MTK_PWM_BASE + 0x0048)
#define PWM_PWM2_CON		(MTK_PWM_BASE + 0x0050)
#define PWM_PWM2_HDURATION	(MTK_PWM_BASE + 0x0054)
#define PWM_PWM2_LDURATION	(MTK_PWM_BASE + 0x0058)
#define PWM_PWM2_GDURATION	(MTK_PWM_BASE + 0x005C)
#define PWM_PWM2_BUF0_BASE_ADDR	(MTK_PWM_BASE + 0x0060)
#define PWM_PWM2_BUF0_SIZE	(MTK_PWM_BASE + 0x0064)
#define PWM_PWM2_BUF1_BASE_ADDR	(MTK_PWM_BASE + 0x0068)
#define PWM_PWM2_BUF1_SIZE	(MTK_PWM_BASE + 0x006C)
#define PWM_PWM2_SEND_DATA0	(MTK_PWM_BASE + 0x0070)
#define PWM_PWM2_SEND_DATA1	(MTK_PWM_BASE + 0x0074)
#define PWM_PWM2_WAVE_NUM	(MTK_PWM_BASE + 0x0078)
#define PWM_PWM2_DATA_WIDTH	(MTK_PWM_BASE + 0x007C)
#define PWM_PWM2_THRESH		(MTK_PWM_BASE + 0x0080)
#define PWM_PWM2_SEND_WAVENUM	(MTK_PWM_BASE + 0x0084)
#define PWM_PWM2_VALID		(MTK_PWM_BASE + 0x0088)
#define PWM_PWM4_CON		(MTK_PWM_BASE + 0x00D0)
#define PWM_PWM4_HDURATION	(MTK_PWM_BASE + 0x00D4)
#define PWM_PWM4_LDURATION	(MTK_PWM_BASE + 0x00D8)
#define PWM_PWM4_GDURATION	(MTK_PWM_BASE + 0x00DC)
#define PWM_PWM4_BUF0_BASE_ADDR	(MTK_PWM_BASE + 0x00E0)
#define PWM_PWM4_BUF0_SIZE	(MTK_PWM_BASE + 0x00E4)
#define PWM_PWM4_BUF1_BASE_ADDR	(MTK_PWM_BASE + 0x00E8)
#define PWM_PWM4_BUF1_SIZE	(MTK_PWM_BASE + 0x00EC)
#define PWM_PWM4_SEND_DATA0	(MTK_PWM_BASE + 0x00F0)
#define PWM_PWM4_SEND_DATA1	(MTK_PWM_BASE + 0x00F4)
#define PWM_PWM4_WAVE_NUM	(MTK_PWM_BASE + 0x00F8)
#define PWM_PWM4_SEND_WAVENUM	(MTK_PWM_BASE + 0x00FC)
#define PWM_PWM4_VALID		(MTK_PWM_BASE + 0x0100)

#define PWM_INT_ENABLE		(MTK_PWM_BASE + 0x0190)
#define PWM_INT_STATUS		(MTK_PWM_BASE + 0x0194)
#define PWM_INT_ACK		(MTK_PWM_BASE + 0x0198)

/* Bit field definitions for PWM_ENABLE register. */
#define PWM_ENABLE_PWM1_EN		(1 << 0)
#define PWM_ENABLE_PWM2_EN		(1 << 1)
#define PWM_ENABLE_PWM3_EN		(1 << 2)
#define PWM_ENABLE_PWM4_EN		(1 << 3)
#define PWM_ENABLE_PWM5_EN		(1 << 4)
#define PWM_ENABLE_PWM6_EN		(1 << 5)
#define PWM_ENABLE_PWM_SEQ_MODE		(1 << 6)
#define PWM_ENABLE_PWM_DELAY_FIX_CLK	(1 << 7)

/* Bit field definitions for PWM_PWM1_CON register. */
#define PWM_CON_CLKDIV_MASK		7
#define PWM_CON_CLKSEL			(1 << 3)
#define PWM_CON_FIX_CLK_MODE		(1 << 4)
#define PWM_CON_SRC_SEL			(1 << 5)
#define PWM_CON_MODE			(1 << 6)
#define PWM_CON_IDLE_VALUE		(1 << 7)
#define PWM_CON_GUARD_VALUE		(1 << 8)
#define PWM_CON_STOP_BITPOS		9
#define PWM_CON_STOP_BITPOS_MASK	(0x3F << PWM_PWM1_CON_STOP_BITPOS)
#define PWM_CON_OLD_PWM_MODE		(1 << 15)

#endif /* __PWM_H */
