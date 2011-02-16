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
#include <asm/errno.h>
#include <asm/io.h>
#include <asm/arch-mtk/system.h>
#include <asm/arch-mtk/gpio.h>
#include <asm/arch-mtk/lcd_backlight.h>
#include <asm/arch-mtk/mtk_lcd.h>
#include <lcd.h>
#include <version.h>


#ifdef CONFIG_MTK_LCD

#ifdef CONFIG_MTK_LCD_ILI93XX
/* 320x240, 16 bit color in RGB565 mode. */

/* LCD configuration structure for ILI93XX. */
vidinfo_t panel_info = {
	.vl_col		= 240,
	.vl_row		= 320,
	.vl_bpix	= LCD_BPP
};

#else
#error "LCD module type not defined!"
#endif /* CONFIG_MTK_LCD_ILI93XX */
#else
#error "This file shouldn't be compiled!"
#endif /* CONFIG_MTK_LCD */

#define DRIVER_NAME				"mtk_lcd"

/* Reset timing. */
#define LCD_RESET_LOW_LEVEL_WIDTH_US		5000
#define LCD_RESET_HIGH_LEVEL_WIDTH_US		50000
#define LCD_DISCHARGE_CAPACITOR_TIME_US		200000
#define LCD_POWER_PARAM_STABILIZING_TIME_US	50000

#define LCD_RLT					10
#define LCD_WST					3
#define LCD_C2WS				2

#define LCD_WAITING_PERIOD_BETWEEN_TRANSFERS	0x1F

#define LCD_DEFAULT_WROI_X_OFFSET		0
#define LCD_DEFAULT_WROI_Y_OFFSET		0
#define LCD_COMMAND_SET_GRAM_ADDRESS		0x00800022

/* 1 transfer/1 pixel in RGB565 mode. */
#define LCD_FORMAT				0x50

/* RS line low. */
#define LCD_WRITE_REGISTER_INDEX(index)		writew(index, LCD_PDAT0)

/* RS line high. */
#define	LCD_READ_REGISTER_DATA()		readw(LCD_PCMD0)
#define LCD_WRITE_REGISTER_DATA(data)		writew(data, LCD_PCMD0)

#define LCD_WRITE_CONTROL_REGISTER(index, data)				\
	do {								\
		LCD_WRITE_REGISTER_INDEX(index);			\
		LCD_WRITE_REGISTER_DATA(data);				\
	} while (0)

/* Printing macros. */
#define DEBUG_LINE() do {						\
		debug("%s[%s]: called.\n", DRIVER_NAME, __func__);	\
	} while (0)

#define DEBUG_MSG(fmt, arg...) do {					\
		debug("%s[%s]: " fmt "\n", DRIVER_NAME,			\
			__func__, ##arg);				\
	} while (0)

#define ERROR(fmt, arg...) do {						\
		printf("%s[%s] ERROR: " fmt "\n", DRIVER_NAME,		\
		__func__, ##arg);					\
	} while (0)

#define INFO(fmt, arg...) do {						\
		printf("%s INFO: " fmt "\n", DRIVER_NAME, ##arg);	\
	} while (0)

#define SEQUENCE_LENGTH(sequence) (sizeof(sequence) / sizeof(*sequence))

/* Start address of frame buffer memory - layer 0. */
void *lcd_base;

/* Start of console buffer. */
void *lcd_console_address;

int lcd_line_length;
int lcd_color_fg;
int lcd_color_bg;

short console_col;
short console_row;

struct register_write_transaction {
	u8	register_index;
	u16	register_value;
	ulong	delay_us;
};

struct configuration_sequence {
	u32 device_code;
	const struct register_write_transaction * const init_sequence;
	const u16 init_sequence_length;
	const struct register_write_transaction * const powerup_sequence;
	const u16 powerup_sequence_length;
};

/* Device configuration. */
struct fb_device_config {
	u32				device_code;
	struct configuration_sequence	*config_sequence;
};

static const struct register_write_transaction ILI9325_init_sequence[] = {

	/* SS = 1, the shift direction of outputs is from S720 to S1. */
	{0x01, 0x0100, 0},
	/* Set line inversion: EOR = 1 and B/C = 1. */
	{0x02, 0x0700, 0},
	/* Set BGR = 1 and GRAM access direction. */
	{0x03, 0x1030, 0},
	/* Set resizing factor. */
	{0x04, 0x0000, 0},
	/* Set number of lines for front porch and back porch. */
	{0x08, 0x0202, 0},
	/* Specify scan cycle interval. */
	{0x09, 0x0000, 0},
	/* Set the output interval of FMARK signal to 1 frame.
	   Start marking. */
	{0x0A, 0x0008, 0},
	/* RIM0 = 1, 16-bit RGB interface (1 transfer/pixel),
	   DB[17:13] and DB[11:1] */
	{0x0C, 0x0001, 0},
	/* Set the output position of frame marker (0th line). */
	{0x0D, 0x0000, 0},
	/* Set RGB interface polarity. */
	{0x0F, 0x0000, 0}
};

static const struct register_write_transaction ILI9325_powerup_sequence[] = {

	/* SAP, BT[3:0], AP, DSTB, SLP, STB */
	{0x10, 0x0000, 0},
	/* DC1[2:0], DC0[2:0], VC[2:0] */
	{0x11, 0x0007, 0},
	/* Set VREG1OUT voltage = HALT. */
	{0x12, 0x0000, 0},
	/* Select the factor for VREG1OUT.
	   Discharge capacitor power voltage. */
	{0x13, 0x0000, LCD_DISCHARGE_CAPACITOR_TIME_US},
	/* SAP, BT[3:0], AP, DSTB, SLP, STB */
	{0x10, 0x1690, 0},
	/* R11h = 0x0221 at VCI = 3.3V, DC1[2:0], DC0[2:0], VC[2:0] */
	{0x11, 0x0227, LCD_POWER_PARAM_STABILIZING_TIME_US},
	/* External reference voltage = VCI. */
	{0x12, 0x001C, LCD_POWER_PARAM_STABILIZING_TIME_US},
	/* VCOM = VREG1OUT x 1.24 */
	{0x13, 0x1800, 0},
	/* VCOMH = VREG1OUT x 0.965 */
	{0x29, 0x001C, 0},
	/* Frame rate = 91Hz (default value). */
	{0x2B, 0x000D, LCD_POWER_PARAM_STABILIZING_TIME_US},
	/* GRAM horizontal address. */
	{0x20, 0x0000, 0},
	/* GRAM vertical address.*/
	{0x21, 0x0000, 0},
	/* Adjust the Gamma Curve. */
	{0x30, 0x0007, 0}, {0x31, 0x0302, 0}, {0x32, 0x0105, 0},
	{0x35, 0x0206, 0}, {0x36, 0x0808, 0}, {0x37, 0x0206, 0},
	{0x38, 0x0504, 0}, {0x39, 0x0007, 0}, {0x3C, 0x0105, 0},
	{0x3D, 0x0808, 0},
	/* Horizontal GRAM start address. */
	{0x50, 0x0000, 0},
	/* Horizontal GRAM end address. */
	{0x51, 0x00EF, 0},
	/* Vertical GRAM start address. */
	{0x52, 0x0000, 0},
	/* Vertical GRAM end address. */
	{0x53, 0x013F, 0},
	/* Gate scan line. */
	{0x60, 0xA700, 0},
	/* NDL,VLE, REV */
	{0x61, 0x0001, 0},
	/* Set scrolling line. */
	{0x6A, 0x0000, 0},
	/* Partial Display Control. */
	{0x80, 0x0000, 0}, {0x81, 0x0000, 0}, {0x82, 0x0000, 0},
	{0x83, 0x0000, 0}, {0x84, 0x0000, 0}, {0x85, 0x0000, 0},
	/* Panel Control. */
	{0x90, 0x0010, 0}, {0x92, 0x0000, 0}, {0x93, 0x0003, 0},
	{0x95, 0x0110, 0}, {0x97, 0x0000, 0}, {0x98, 0x0000, 0},
	/* Set 262K color and turn on the display. */
	{0x07, 0x0133, 0},
};

static const struct register_write_transaction ILI9331_init_sequence[] = {

	{0x01, 0x0100, 0}, {0x02, 0x0200, 0}, {0x03, 0x1030, 0},
	{0x04, 0x0000, 0}, {0x08, 0x0202, 0}, {0x09, 0x0000, 0},
	{0x0A, 0x0008, 0}, {0x0C, 0x0001, 0}, {0x0D, 0x0000, 0},
	{0x0F, 0x0000, 0}
};

static const struct register_write_transaction ILI9331_powerup_sequence[] = {

	{0x10, 0x0000, 0}, {0x11, 0x0007, 0}, {0x12, 0x0000, 0},
	{0x13, 0x0000, LCD_DISCHARGE_CAPACITOR_TIME_US},
	{0x10, 0x1690, 0},
	{0x11, 0x0227, LCD_POWER_PARAM_STABILIZING_TIME_US},
	{0x12, 0x000C, LCD_POWER_PARAM_STABILIZING_TIME_US},
	{0x13, 0x1800, 0}, {0x29, 0x001C, 0},
	{0x2B, 0x000D, LCD_POWER_PARAM_STABILIZING_TIME_US},
	{0x20, 0x0000, 0}, {0x21, 0x0000, 0}, {0x30, 0x0007, 0},
	{0x31, 0x0302, 0}, {0x32, 0x0105, 0}, {0x35, 0x0206, 0},
	{0x36, 0x0808, 0}, {0x37, 0x0206, 0}, {0x38, 0x0504, 0},
	{0x39, 0x0007, 0}, {0x3C, 0x0105, 0}, {0x3D, 0x0808, 0},
	{0x50, 0x0000, 0}, {0x51, 0x00EF, 0}, {0x52, 0x0000, 0},
	{0x53, 0x013F, 0}, {0x60, 0x2700, 0}, {0x61, 0x0001, 0},
	{0x6A, 0x0000, 0}, {0x80, 0x0000, 0}, {0x81, 0x0000, 0},
	{0x82, 0x0000, 0}, {0x83, 0x0000, 0}, {0x84, 0x0000, 0},
	{0x85, 0x0000, 0}, {0x90, 0x0010, 0}, {0x92, 0x0000, 0},
	{0x93, 0x0003, 0}, {0x95, 0x0100, 0}, {0x97, 0x0000, 0},
	{0x98, 0x0000, 0}, {0x07, 0x0133, 0},
};

struct configuration_sequence chip_config_sequences[] = {
	/* Configuration for ILI9325. */
	{
		0x9325,
		ILI9325_init_sequence,
		SEQUENCE_LENGTH(ILI9325_init_sequence),
		ILI9325_powerup_sequence,
		SEQUENCE_LENGTH(ILI9325_powerup_sequence)
	},
	/* Configuration for ILI9328. */
	{
		0x9328,
		ILI9325_init_sequence,
		SEQUENCE_LENGTH(ILI9325_init_sequence),
		ILI9325_powerup_sequence,
		SEQUENCE_LENGTH(ILI9325_powerup_sequence)
	},

	/* Configuration for ILI9331. */
	{
		0x9331,
		ILI9331_init_sequence,
		SEQUENCE_LENGTH(ILI9331_init_sequence),
		ILI9331_powerup_sequence,
		SEQUENCE_LENGTH(ILI9331_powerup_sequence)
	},
};

static struct fb_device_config fb_dev_config;

void lcd_show_board_info(void)
{
	DEBUG_LINE();

	lcd_printf("%s\n", U_BOOT_VERSION);
	lcd_printf("(C) 2010 Tieto\n");
	lcd_printf("www.tieto.com\n");
}

/*
 * Configures GPIO for LCD controller.
 */
static void lcd_configure_gpio(void)
{
	DEBUG_LINE();

	/* Choose alternative function for GPIO30 to have LPTE working. */
	writew((readw(MTK_GPIO_MODE4) & (~MTK_GPIO_MODE4_GPIO30_ALT3)) |
		MTK_GPIO_MODE4_GPIO30_ALT1, MTK_GPIO_MODE4);
}

/*
 * Performs reset of LCD.
 */
static void lcd_reset(void)
{
	DEBUG_LINE();

	/* Set RSTB (reset) line low. */
	writew(readw(LCD_RSTB) & (~LCD_RSTB_RSTB), LCD_RSTB);

	/* Keep RSTB line low at least 1ms. */
	udelay(LCD_RESET_LOW_LEVEL_WIDTH_US);
	writew(LCD_RSTB_RSTB, LCD_RSTB);

	/* Keep RSTB line high at least 50ms. */
	udelay(LCD_RESET_HIGH_LEVEL_WIDTH_US);
}

/*
 * Reads device code.
 */
static int lcd_read_device_code(void)
{
	u8 index;
	u32 device_code;
	int ret_val = 0;

	DEBUG_LINE();

	LCD_WRITE_REGISTER_INDEX(0x00);
	device_code = LCD_READ_REGISTER_DATA();

	INFO("Read LCD device code: %x.", device_code);

	fb_dev_config.device_code = 0;

	/* Check if the device is supported. */
	for (index = 0; index < SEQUENCE_LENGTH(chip_config_sequences);
		index++) {
		if (device_code == chip_config_sequences[index].device_code) {
			fb_dev_config.device_code =  device_code;
			fb_dev_config.config_sequence =
				&chip_config_sequences[index];
			DEBUG_MSG("Configuration found for %x.", device_code);
		}
	}

	if (!fb_dev_config.device_code) {
		ERROR("Device configuration unknown for %x!", device_code);
		ret_val = -ENODEV;
	}

	return ret_val;
}

/*
 * Performs initial sequence of LCD module.
 */
static void lcd_perform_sequence(
	const struct register_write_transaction *trans,
	const u32 sequence_length)
{
	u8 trans_index;

	DEBUG_LINE();
	DEBUG_MSG("Performing sequence %d elements long...", sequence_length);

	/* Run the sequence. */
	for (trans_index = 0; trans_index < sequence_length; trans_index++) {

		LCD_WRITE_CONTROL_REGISTER(trans->register_index,
			trans->register_value);

		if (trans->delay_us)
			udelay(trans->delay_us);

		trans++;
	}
}

#if 0
/*
 * Calculate frame buffer size.
 */
u32 calc_fbsize(void)
{
	u32 size;

	u16 line_length = (panel_info.vl_col * NBITS(panel_info.vl_bpix)) / 8;
	size = line_length * panel_info.vl_row;

	return size;
}
#endif

/*
 * LCD initialization.
 */
void lcd_ctrl_init(void *lcdbase)
{
	DEBUG_LINE();
	DEBUG_MSG("Initializing LCD...");

	lcd_configure_gpio();

	/* Turn on the LCD block in MTK. */
	writew(PDN_CON1_LCD, MTK_CONFG_PDN_CLR1);

	/* Configure parallel interface:
	   - 52MHz clock
	   - 16-bits data width */
	writel(LCD_RLT | (LCD_WST << LCD_PCNF0_WST) |
		(LCD_C2WS << LCD_PCNF0_C2WS) | LCD_PCNF0_52M |
		(LCD_PCNF0_DW_16BIT << LCD_PCNF0_DW) |
		(LCD_PCNF0_GAMMA_ID_X_NO_TABLE << LCD_PCNF0_GAMMA_ID_B) |
		(LCD_PCNF0_GAMMA_ID_X_NO_TABLE << LCD_PCNF0_GAMMA_ID_G) |
		(LCD_PCNF0_GAMMA_ID_X_NO_TABLE << LCD_PCNF0_GAMMA_ID_R),
		LCD_PCNF0);

	/* Enable tearing control. LCD controller will synchronize
	   LCM refresh timing to rising edge on FMARK line. */
	writew(LCD_TECON_TE_EN, LCD_TECON);

	/* Set window size. */
	writel(panel_info.vl_col | (panel_info.vl_row << LCD_MWINSIZE_ROW),
		LCD_MWINSIZE);

	/* Choose 1 transfer/pixel in RGB565 mode. */
	writel(LCD_FORMAT |
		(LCD_WAITING_PERIOD_BETWEEN_TRANSFERS << LCD_WROICON_PERIOD),
		LCD_WROICON);

	/* Set region of interest (ROI) size. */
	writel(panel_info.vl_col | (panel_info.vl_row << LCD_WROISIZE_ROW),
		LCD_WROISIZE);

	/* Set ROI offset. */
	writel(LCD_DEFAULT_WROI_X_OFFSET |
		(LCD_DEFAULT_WROI_Y_OFFSET << LCD_WROIFS_Y_OFFSET),
		LCD_WROIOFS);

	/* Addresses of registry for execution commands and data. */
	writew((u16)(LCD_PDAT0 & 0xFFFF), LCD_WROICADD);
	writew((u16)(LCD_PCMD0 & 0xFFFF), LCD_WROIDADD);

	writel(0x00, LCD_WROI_HWREF);
	writel(0x00, LCD_WROI_DC);

	/* Reset LCD module. */
	lcd_reset();

	/* Turn on backlight. */
	lcd_backlight_on();

	/* Read device code. */
	if (lcd_read_device_code()) {
		ERROR("LCD initialization failed!");
		memset(&fb_dev_config, 0, sizeof(fb_dev_config));
		return;
	}

	DEBUG_MSG("Start initial sequence.");

	/* Perform LCD initial sequence. */
	lcd_perform_sequence(fb_dev_config.config_sequence->init_sequence,
		fb_dev_config.config_sequence->init_sequence_length);

	DEBUG_MSG("Start power-up sequence.");

	/* Perform LCD power-up sequence. */
	lcd_perform_sequence(fb_dev_config.config_sequence->powerup_sequence,
		fb_dev_config.config_sequence->powerup_sequence_length);

	/* Set up layer 0 for frame buffer. */
	writel(readl(LCD_WROICON) | LCD_WROICON_EN0, LCD_WROICON);
	writel((LCD_L0WINCON_CLRDPT_RGB565 << LCD_L0WINCON_CLRDPT),
		LCD_L0WINCON);

	if (!lcd_base) {
		ERROR("lcd_base is NULL!");

		/* Turn off the LCD block in MTK. */
		writew(PDN_CON1_LCD, MTK_CONFG_PDN_SET1);
		return;
	} else
		DEBUG_MSG("lcd_base: %p.", lcd_base);

	writel((u32)lcd_base, LCD_L0WINADD);

	writel(panel_info.vl_col | (panel_info.vl_row << LCD_L0WINSIZE_ROW),
		LCD_L0WINSIZE);

	writel(0, LCD_L0WINSKEY);
	writel(0, LCD_L0WINOFS);

	/* Setup 1 (COMMAND field = 0) command: 0x0022 (GRAM access port). */
	writel(readl(LCD_WROICON) | LCD_WROICON_ENC, LCD_WROICON);

	/* Store register address to send in commands queue. */
	writel(LCD_COMMAND_SET_GRAM_ADDRESS, LCD_COMD0);

	/* Turn off the LCD block in MTK. */
	writew(PDN_CON1_LCD, MTK_CONFG_PDN_SET1);
}

/*
 * Enables LCD.
 */
void lcd_enable(void)
{
	DEBUG_LINE();

	/* Turn on the LCD block in MTK. */
	writew(PDN_CON1_LCD, MTK_CONFG_PDN_CLR1);

	/* Wait for LCD block stabilization */
	udelay(1);

	/* Start control of LCD frame transfer. */
	writew(LCD_START_START, LCD_START);

	udelay(LCD_POWER_PARAM_STABILIZING_TIME_US);
}

/*
 * Disables LCD.
 */
void lcd_disable(void)
{
	DEBUG_LINE();

	/* Stop control of LCD frame transfer. */
	 writew((unsigned short)~LCD_START_START, LCD_START);

	/* Turn off the LCD block in MTK. */
	writew(PDN_CON1_LCD, MTK_CONFG_PDN_SET1);
}
