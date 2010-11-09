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

#ifndef __MT62XX_SERIAL_H_
#define __MT62XX_SERIAL_H_

/* MT62XX UART register definitions */
#define MTK_UART_DR			0x00    /*  RX or TX buffer register */
#define MTK_UART_IER			0x04    /*  Interrupt enable register */
#define MTK_UART_FCR			0x08    /*  FIFO control register */
#define MTK_UART_LCR			0x0C    /*  Line control register */
#define MTK_UART_MCR			0x10    /*  Modem control register */
#define MTK_UART_LSR			0x14    /*  Line status register */
#define MTK_UART_MSR			0x14    /*  Modem status register */
#define MTK_UART_SCR			0x1C    /*  Scratch register */
#define MTK_UART_AUTOBAUD_EN		0x20    /*  Autobaud enable */
#define MTK_UART_HIGHSPEED		0x24    /*  High speed UART */
#define MTK_UART_SAMPLE_COUNT		0x28
#define MTK_UART_SAMPLE_POINT		0x2C
#define MTK_UART_AUTOBAUD_REG		0x30
#define MTK_UART_RATEFIX_AD		0x34    /* Rate fix address */
#define MTK_UART_AUTOBAUD_SAMPLE	0x38
#define MTK_UART_GUARD			0x3C    /* Guard time added */
#define MTK_UART_ESCAPE_DAT		0x40    /* Escape character */
#define MTK_UART_ESCAPE_EN		0x44    /* Escape enable */
#define MTK_UART_SLEEP_EN		0x48    /* Sleep enable */
#define MTK_UART_VFIFO_EN		0x4C    /* Virtual FIFO enable */
#define MTK_UART_RXTRI_AD		0x50    /* RX trigger address */

/* Registers available when LCR[7] = 1 (bit DLAB) */
#define MTK_UART_DLL			0x00    /*  Divisor latch (LS) */
#define MTK_UART_DLM			0x04    /*  Divisor latch (MS) */

/* Registers available when LCR = 0xBF */
#define MTK_UART_EFR			0x08    /*  Enhanced feature register */
#define MTK_UART_XON1			0x10
#define MTK_UART_XON2			0x14
#define MTK_UART_XOFF1			0x18
#define MTK_UART_XOFF2			0x1C

/* UART_FCR bit fields definitions */
#define UART_FCR_FIFOE			(1 << 0)
#define UART_FCR_CLRR			(1 << 1)
#define UART_FCR_CLRT			(1 << 2)
#define UART_FCR_DMA1			(1 << 3)
#define UART_FCR_TFTL0			(1 << 4)
#define UART_FCR_TFTL1			(1 << 5)
#define UART_FCR_RTFL0			(1 << 6)
#define UART_FCR_RTFL1			(1 << 7)

/* UART_LCR bit fields definitions */
#define UART_LCR_WLS0			(1 << 0)
#define UART_LCR_WLS1			(1 << 1)
#define UART_LCR_STB			(1 << 2)
#define UART_LCR_PEN			(1 << 3)
#define UART_LCR_EPS			(1 << 4)
#define UART_LCR_SP			(1 << 5)
#define UART_LCR_SB			(1 << 6)
#define UART_LCR_DLAB			(1 << 7)

/* UART_LSR bit fields definitions */
#define UART_LSR_DR			(1 << 0)
#define UART_LSR_OE			(1 << 1)
#define UART_LSR_PE			(1 << 2)
#define UART_LSR_FE			(1 << 3)
#define UART_LSR_BI			(1 << 4)
#define UART_LSR_THRE			(1 << 5)
#define UART_LSR_TEMT			(1 << 6)
#define UART_LSR_FIFOERR		(1 << 7)

#endif
