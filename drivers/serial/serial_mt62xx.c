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
#include <watchdog.h>
#include <asm/io.h>
#include <asm/arch-mtk/serial.h>

static volatile unsigned char *const port[] = CONFIG_MT62XX_PORTS;
#define NUM_PORTS (sizeof(port)/sizeof(port[0]))

struct baud_div {
	unsigned int baud;
	unsigned int div;
};

/* Divisor needed to generate given baudrate (52MHz clock) */
static struct baud_div baud_table[] = {
	{110,	29545},
	{300,	10833},
	{1200,	2708},
	{2400,	1354},
	{4800,	677},
	{9600,	339},
	{19200,	169},
	{38400,	85},
	{57600,	56},
	{115200,28},
};

unsigned int baudrate = CONFIG_BAUDRATE;
DECLARE_GLOBAL_DATA_PTR;

int serial_init(void)
{
	int i;

	/* First, clear and setup FIFOs */
	writew(UART_FCR_FIFOE | UART_FCR_CLRR | UART_FCR_CLRT,
	       port[CONSOLE_PORT] + MTK_UART_FCR);

	/* Set baud rate */
	writew(UART_LCR_DLAB, port[CONSOLE_PORT] + MTK_UART_LCR);

	for (i = 0; i < sizeof(baud_table)/sizeof(baud_table[0]); ++i) {
		if (baud_table[i].baud == baudrate) {
			writew(baud_table[i].div & 0xFFFF,
				port[CONSOLE_PORT] + MTK_UART_DLL);
			writew(baud_table[i].div >> 16,
				port[CONSOLE_PORT] + MTK_UART_DLM);
			break;
		}
	}

	if (i >= sizeof(baud_table)/sizeof(baud_table[0]))
		printf("Warning! Not supported baudrate: %d\n", baudrate);

	writew(0, port[CONSOLE_PORT] + MTK_UART_LCR);

	/* Set the UART to be 8 bits, 1 stop bit, no parity */
	writew(UART_LCR_WLS0 | UART_LCR_WLS1,
	       port[CONSOLE_PORT] + MTK_UART_LCR);

	return 0;
}

static void mt62xx_putc(int portnum, char c)
{
	/* Wait until there is space in the FIFO */
	while(!(readw(port[portnum] + MTK_UART_LSR) & UART_LSR_THRE))
		WATCHDOG_RESET();

	/* Send the character */
	writew(c, port[portnum] + MTK_UART_DR);
}

static int mt62xx_getc(int portnum)
{
	/* Wait until there is data in the FIFO */
	while (!(readl(port[portnum] + MTK_UART_LSR) & UART_LSR_DR))
		WATCHDOG_RESET();

	return readl(port[portnum] + MTK_UART_DR);
}

static int mt62xx_tstc(int portnum)
{
	WATCHDOG_RESET();
	return (readl(port[portnum] + MTK_UART_LSR) & UART_LSR_DR);
}

void serial_putc(const char c)
{
	if (c == '\n')
		mt62xx_putc(CONSOLE_PORT, '\r');

	mt62xx_putc(CONSOLE_PORT, c);
}

void serial_puts (const char *s)
{
	while(*s)
		serial_putc(*s++);
}

int serial_getc (void)
{
	return mt62xx_getc(CONSOLE_PORT);
}

int serial_tstc (void)
{
	return mt62xx_tstc(CONSOLE_PORT);
}

void serial_setbrg(void)
{
	baudrate = gd->baudrate;
	serial_init();
}
