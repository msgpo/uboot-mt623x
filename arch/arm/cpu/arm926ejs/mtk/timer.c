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

/*
 * Timers in MT62xx are just 16-bit counters and
 * maximum clock is 16384Hz. This gives tick every 61ms
 * and overrun in about 4s.
 * GPT3 timer is working as free-runninng timer in incrementing mode.
 */
#define TIMER_CLOCK		(16384)
#define TIMER_MAX_LOAD		(0xFFFF)
#define COUNT_TO_USEC(x)	((x) * 1000000 / TIMER_CLOCK)
#define USEC_TO_COUNT(x)	((x) * TIMER_CLOCK / 1000000 + 1)
#define TICKS_PER_HZ		(TIMER_CLOCK / CONFIG_SYS_HZ)
#define TICKS_TO_HZ(x)		((x) / TICKS_PER_HZ)

/* Reads GPT3 timer counter */
#define READ_TIMER()		readw(CONFIG_SYS_TIMERBASE + 0x20)

/*
 * tcounter is helper variable which increases overflow
 * of built-in timer's counter from 4s to 72h.
 */
ulong tcounter;
ulong last_value;

int timer_init(void)
{
	tcounter = 0;
	last_value = 0;
	/* Set 16384Hz clock */
	writew(0x00, CONFIG_SYS_TIMERBASE + 0x24);
	/* Enable free-running timer */
	writew(0x01, CONFIG_SYS_TIMERBASE + 0x1C);

	return 0;
}

void reset_timer(void)
{
	/* Enable and disable timer ro reset counter */
	writew(0x00, CONFIG_SYS_TIMERBASE + 0x1C);
	/*
	 * Unfortunatelly this busy loop has to be here, otherwise
	 * counter won't be zeroed. After immediate disable/enable cycle,
	 * GPT3 counter doesn't reset its value.
	 */
	while(READ_TIMER() != 0)
		;

	timer_init();
}

static ulong get_counter(void)
{
	unsigned int now = READ_TIMER();

	if (now < last_value)
		/* Overflow occured, increase tcounter properly */
		tcounter += TIMER_MAX_LOAD - last_value + now;
	else
		tcounter += now - last_value;

	last_value = now;
	return tcounter;
}

ulong get_timer(ulong base)
{
	/* Return how many HZ passed since start */
	return  TICKS_TO_HZ(get_counter()) - base;
}

void __udelay(unsigned long usec)
{
	ulong end, start;

	start = get_counter();
	end = start + USEC_TO_COUNT((uint64_t)usec);

	while (get_counter() < end)
		;
}
