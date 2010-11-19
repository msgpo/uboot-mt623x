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

#ifndef __MTK_GPIO_H_
#define __MTK_GPIO_H_

/* GPIOs register definitions */
#define MTK_GPIO_DIR1		(MTK_GPIO_BASE + 0x0000)
#define MTK_GPIO_DIR2		(MTK_GPIO_BASE + 0x0040)
#define MTK_GPIO_DIR3		(MTK_GPIO_BASE + 0x0080)
#define MTK_GPIO_DIR4		(MTK_GPIO_BASE + 0x00C0)
#define MTK_GPIO_DIR5		(MTK_GPIO_BASE + 0x0100)

#define MTK_GPIO_PULLEN1	(MTK_GPIO_BASE + 0x0200)
#define MTK_GPIO_PULLEN2	(MTK_GPIO_BASE + 0x0240)
#define MTK_GPIO_PULLEN3	(MTK_GPIO_BASE + 0x0280)
#define MTK_GPIO_PULLEN4	(MTK_GPIO_BASE + 0x02C0)
#define MTK_GPIO_PULLEN5	(MTK_GPIO_BASE + 0x0300)

#define MTK_GPIO_PULLSELL1	(MTK_GPIO_BASE + 0x0400)
#define MTK_GPIO_PULLSELL2	(MTK_GPIO_BASE + 0x0440)
#define MTK_GPIO_PULLSELL3	(MTK_GPIO_BASE + 0x0480)
#define MTK_GPIO_PULLSELL4	(MTK_GPIO_BASE + 0x04C0)
#define MTK_GPIO_PULLSELL5	(MTK_GPIO_BASE + 0x0500)

#define MTK_GPIO_DINV1		(MTK_GPIO_BASE + 0x0600)
#define MTK_GPIO_DINV2		(MTK_GPIO_BASE + 0x0640)
#define MTK_GPIO_DINV3		(MTK_GPIO_BASE + 0x0680)
#define MTK_GPIO_DINV4		(MTK_GPIO_BASE + 0x06C0)
#define MTK_GPIO_DINV5		(MTK_GPIO_BASE + 0x0700)

#define MTK_GPIO_DOUT1		(MTK_GPIO_BASE + 0x0800)
#define MTK_GPIO_DOUT2		(MTK_GPIO_BASE + 0x0840)
#define MTK_GPIO_DOUT3		(MTK_GPIO_BASE + 0x0880)
#define MTK_GPIO_DOUT4		(MTK_GPIO_BASE + 0x08C0)
#define MTK_GPIO_DOUT5		(MTK_GPIO_BASE + 0x0900)

#define MTK_GPIO_DIN1		(MTK_GPIO_BASE + 0x0A00)
#define MTK_GPIO_DIN2		(MTK_GPIO_BASE + 0x0A40)
#define MTK_GPIO_DIN3		(MTK_GPIO_BASE + 0x0A80)
#define MTK_GPIO_DIN4		(MTK_GPIO_BASE + 0x0AC0)
#define MTK_GPIO_DIN5		(MTK_GPIO_BASE + 0x0B00)

#define MTK_GPIO_MODE1		(MTK_GPIO_BASE + 0x1000)
#define MTK_GPIO_MODE2		(MTK_GPIO_BASE + 0x1100)
#define MTK_GPIO_MODE3		(MTK_GPIO_BASE + 0x1200)
#define MTK_GPIO_MODE4		(MTK_GPIO_BASE + 0x1300)
#define MTK_GPIO_MODE5		(MTK_GPIO_BASE + 0x1400)
#define MTK_GPIO_MODE6		(MTK_GPIO_BASE + 0x1500)
#define MTK_GPIO_MODE7		(MTK_GPIO_BASE + 0x1600)
#define MTK_GPIO_MODE8		(MTK_GPIO_BASE + 0x1700)
#define MTK_GPIO_MODE9		(MTK_GPIO_BASE + 0x1800)
#define MTK_GPIO_MODEA		(MTK_GPIO_BASE + 0x1900)

#define MTK_GPIO_CLK_OUT1	(MTK_GPIO_BASE + 0x3000)
#define MTK_GPIO_CLK_OUT2	(MTK_GPIO_BASE + 0x3100)
#define MTK_GPIO_CLK_OUT3	(MTK_GPIO_BASE + 0x3200)
#define MTK_GPIO_CLK_OUT4	(MTK_GPIO_BASE + 0x3300)
#define MTK_GPIO_CLK_OUT5	(MTK_GPIO_BASE + 0x3400)
#define MTK_GPIO_CLK_OUT6	(MTK_GPIO_BASE + 0x3500)
#define MTK_GPIO_CLK_OUT7	(MTK_GPIO_BASE + 0x3600)
#define MTK_GPIO_CLK_OUT8	(MTK_GPIO_BASE + 0x3700)
#define MTK_GPIO_CLK_OUT9	(MTK_GPIO_BASE + 0x3800)
#define MTK_GPIO_CLK_OUT10	(MTK_GPIO_BASE + 0x3900)

/* MTK_GPIO_MODE_9 bit field definitions */
#define MTK_GPIO_MODE9_GPIO64		0
#define MTK_GPIO_MODE9_GPIO64_ALT1	1
#define MTK_GPIO_MODE9_GPIO64_ALT2	2
#define MTK_GPIO_MODE9_GPIO64_ALT3	3

#define MTK_GPIO_MODE9_GPIO65		(0 << 2)
#define MTK_GPIO_MODE9_GPIO65_ALT1	(1 << 2)
#define MTK_GPIO_MODE9_GPIO65_ALT2	(2 << 2)
#define MTK_GPIO_MODE9_GPIO65_ALT3	(3 << 2)

#define MTK_GPIO_MODE9_GPIO66		(0 << 4)
#define MTK_GPIO_MODE9_GPIO66_ALT1	(1 << 4)
#define MTK_GPIO_MODE9_GPIO66_ALT2	(2 << 4)
#define MTK_GPIO_MODE9_GPIO66_ALT3	(3 << 4)

#define MTK_GPIO_MODE9_GPIO67		(0 << 6)
#define MTK_GPIO_MODE9_GPIO67_ALT1	(1 << 6)
#define MTK_GPIO_MODE9_GPIO67_ALT2	(2 << 6)
#define MTK_GPIO_MODE9_GPIO67_ALT3	(3 << 6)

#define MTK_GPIO_MODE9_GPIO68		(0 << 8)
#define MTK_GPIO_MODE9_GPIO68_ALT1	(1 << 8)
#define MTK_GPIO_MODE9_GPIO68_ALT2	(2 << 8)
#define MTK_GPIO_MODE9_GPIO68_ALT3	(3 << 8)

#define MTK_GPIO_MODE9_GPIO69		(0 << 10)
#define MTK_GPIO_MODE9_GPIO69_ALT1	(1 << 10)
#define MTK_GPIO_MODE9_GPIO69_ALT2	(2 << 10)
#define MTK_GPIO_MODE9_GPIO69_ALT3	(3 << 10)

#define MTK_GPIO_MODE9_GPIO70		(0 << 12)
#define MTK_GPIO_MODE9_GPIO70_ALT1	(1 << 12)
#define MTK_GPIO_MODE9_GPIO70_ALT2	(2 << 12)
#define MTK_GPIO_MODE9_GPIO70_ALT3	(3 << 12)

#define MTK_GPIO_MODE9_GPIO71		(0 << 14)
#define MTK_GPIO_MODE9_GPIO71_ALT1	(1 << 14)
#define MTK_GPIO_MODE9_GPIO71_ALT2	(2 << 14)
#define MTK_GPIO_MODE9_GPIO71_ALT3	(3 << 14)

#define MTK_GPIO_MODEA_GPIO72		0
#define MTK_GPIO_MODEA_GPIO72_ALT1	1
#define MTK_GPIO_MODEA_GPIO72_ALT2	2
#define MTK_GPIO_MODEA_GPIO72_ALT3	3

#define MTK_GPIO_MODEA_GPIO73		(0 << 2)
#define MTK_GPIO_MODEA_GPIO73_ALT1	(1 << 2)
#define MTK_GPIO_MODEA_GPIO73_ALT2	(2 << 2)
#define MTK_GPIO_MODEA_GPIO73_ALT3	(3 << 2)

#define MTK_GPIO_MODEA_GPIO74		(0 << 4)
#define MTK_GPIO_MODEA_GPIO74_ALT1	(1 << 4)
#define MTK_GPIO_MODEA_GPIO74_ALT2	(2 << 4)
#define MTK_GPIO_MODEA_GPIO74_ALT3	(3 << 4)

#define MTK_GPIO_MODEA_GPIO75		(0 << 6)
#define MTK_GPIO_MODEA_GPIO75_ALT1	(1 << 6)
#define MTK_GPIO_MODEA_GPIO75_ALT2	(2 << 6)
#define MTK_GPIO_MODEA_GPIO75_ALT3	(3 << 6)

#endif
