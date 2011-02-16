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
#include <command.h>
#include <asm/arch-mtk/vibra.h>

int do_vibrate(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	if (2 != argc)
		goto usage;

	vibra_on();
	udelay(1000 * simple_strtoul(argv[1], NULL, 10));
	vibra_off();

	return 0;
usage:
	return cmd_usage(cmdtp);
}

U_BOOT_CMD(
	vibrate,	2,	1,	do_vibrate,
	"vibrator control",
	"time_in_ms - starts vibrator for given time"
);
