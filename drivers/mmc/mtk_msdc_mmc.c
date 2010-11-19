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
#include <malloc.h>
#include <common.h>
#include <mmc.h>
#include <asm/io.h>
#include <asm/errno.h>
#include <asm/arch/mtk_msdc.h>
#include <asm/arch/system.h>
#include <asm/arch/gpio.h>

#define MIN_SCLK_VALUE			0
#define MAX_SCLK_VALUE			255
/* According to documentation MCU is 104 MHz or 52 MHz. */
#define MCU_CLK_FREQ_HZ			104000000
#define MAX_SLAVE_FREQ_HZ		26000000
#define MAX_BLOCK_SIZE			2048
#define COMMAND_STATUS_READ_DELAY_US	10
#define POLL_TRANSFER_COND_TIMEOUT_US	500
#define POLL_CMD_STATUS_TIMEOUT_US	100
#define DATA_READ_TIMEOUT_US		500

#define DEBUG_LINE() do { \
		debug("mtk_msdc_mmc[%s]: called.\n", __func__); \
	} while (0)

#define DEBUG_MSG(fmt, arg...) do { \
		debug("mtk_msdc_mmc[%s]: " fmt "\n", __func__, ##arg); \
	} while (0)

#define ERROR(fmt, arg...) do { \
		printf("mtk_msdc_mmc[%s] ERROR: " fmt "\n", __func__, ##arg); \
	} while (0)

struct mmc_host {
	struct mmc		*mmc;
	u32			clock;
	u32			msdc_sdc_cmd;
	struct mmc_cmd		*cmd;
	struct mmc_data		*data;
};

static struct mmc_host mmc_host;

#ifdef MTK_MMC_CARD_DETECTION_SUPPORTED

/* Card detection mechanism is not supported by Sci-phone G2 hardware:
 *	1. missing RCDEN resistor on PCB,
 *	2. grounded MCINS(GPIO75) pin.
 *
 * Checks if memory card is present according to the algorithm:
 *	1. Pull up CD/DAT3 (INS) pin.
 *	2. Enable card detection and input pin at the same time.
 *	3. Turn on power for memory card.
 *	4. Detect insertion of memory card.
 */
static int check_card_presence(struct mmc_host *mmc_host)
{
	int retval = -ENODEV;

	DEBUG_LINE();

	writel(readl(MTK_MSDC_CFG) | MTK_MSDC_CFG_PRCFG0_PU_EN_PD_DS,
		MTK_MSDC_CFG);

	writel(readl(MTK_MSDC_PS) | MTK_MSDC_PS_CDEN | MTK_MSDC_PS_PIEN0,
		MTK_MSDC_PS);

	/* Turn on the power for memory card. */
	writel(readl(MTK_MSDC_CFG) | MTK_MSDC_CFG_VDDP, MTK_MSDC_CFG);

	/* Check if a card is inserted. */

	if (readl(MTK_MSDC_PS) & MTK_MSDC_PS_PINCHG)
		retval = 0;

	return retval;
}

#endif /* MTK_MMC_CARD_DETECTION_SUPPORTED */

/*
 * Power up MSDC controller.
 */
static void mtk_power_up(void)
{
	DEBUG_LINE();
	/* Disable power down control for MSDC module. */
	writew(PDN_CON1_MSDC, MTK_CONFG_PDN_CLR1);
}

/*
 * Performs software and synchronous reset of MSDC controller.
 */
static void mtk_software_reset(void)
{
	DEBUG_LINE();

	writel(MTK_MSDC_CFG_RST, MTK_MSDC_CFG);
}

/*
 * Configures GPIOs for MSDC controller.
 */
static void mtk_configure_gpios(void)
{
	DEBUG_LINE();

	u16 mode_9_clear_mask =
		MTK_GPIO_MODE9_GPIO67_ALT3 |
		MTK_GPIO_MODE9_GPIO68_ALT3 |
		MTK_GPIO_MODE9_GPIO69_ALT3 |
		MTK_GPIO_MODE9_GPIO70_ALT3 |
		MTK_GPIO_MODE9_GPIO71_ALT3;

	u16 mode_9_set_mask =
		MTK_GPIO_MODE9_GPIO67_ALT1 |
		MTK_GPIO_MODE9_GPIO68_ALT1 |
		MTK_GPIO_MODE9_GPIO69_ALT1 |
		MTK_GPIO_MODE9_GPIO70_ALT1 |
		MTK_GPIO_MODE9_GPIO71_ALT1;

	u16 mode_a_clear_mask =
		MTK_GPIO_MODEA_GPIO72_ALT3 |
		/* GPIO73 skipped (used by USB as regular GPIO) */
		MTK_GPIO_MODEA_GPIO74_ALT3 |
		MTK_GPIO_MODEA_GPIO75_ALT3;

	u16 mode_a_set_mask =
		MTK_GPIO_MODEA_GPIO72_ALT1 |
		/* GPIO73 skipped (used by USB as regular GPIO) */
		MTK_GPIO_MODEA_GPIO74_ALT1 |
		MTK_GPIO_MODEA_GPIO75_ALT1;

	writew((readw(MTK_GPIO_MODE9) & (~mode_9_clear_mask)) |
		mode_9_set_mask, MTK_GPIO_MODE9);

	writew((readw(MTK_GPIO_MODEA) & (~mode_a_clear_mask)) |
		mode_a_set_mask, MTK_GPIO_MODEA);
}

/*
 * Initialization of MMC/SD controller.
 */
static int mtk_init(struct mmc *mmc)
{
	DEBUG_LINE();

	(void)mmc;

	mtk_configure_gpios();
	mtk_power_up();
	mtk_software_reset();

	/* Configure the controller as the host of SD/MMC cards. */
	writel(readl(MTK_MSDC_CFG) | MTK_MSDC_CFG_MSDC, MTK_MSDC_CFG);

	/* Use MCU clock as source clock of memory card. */
	writel(readl(MTK_MSDC_CFG) & (~MTK_MSDC_CFG_CLKSRC), MTK_MSDC_CFG);

	/* Clear FIFO. */
	writew(readl(MTK_MSDC_STA) | MTK_MSDC_STA_FIFOCLR, MTK_MSDC_STA);

	return 0;
}

/*
 * Calculates slave frequency for the given sclk value.
 */
static u32 mtk_calculate_f_slave(const u8 sclk)
{
	u32 f_slave = 0;

	if (!sclk)
		f_slave = MCU_CLK_FREQ_HZ / 2;
	else
		f_slave = MCU_CLK_FREQ_HZ / (4 * sclk);

	return f_slave;
}

/*
 * Calculates max. slave frequency.
 */
static u32 mtk_calculate_f_slave_max(void)
{
	u32 f_slave_max = 0;

	f_slave_max = mtk_calculate_f_slave(MIN_SCLK_VALUE);

	return f_slave_max;
}

/*
 * Calculates min. slave frequency.
 */
static u32 mtk_calculate_f_slave_min(void)
{
	u32 f_slave_min = 0;

	f_slave_min = mtk_calculate_f_slave(MAX_SCLK_VALUE);

	return f_slave_min;
}

/*
 * Configures clock for MSDC.
 */
static void mtk_set_clk_rate(struct mmc_host *mmc_host, const u32 f_slave)
{
	u32	f_host;
	u8	sclk;
	u32	calc_f_slave;
	u32	min_f_slave;
	u32	max_f_slave;

	DEBUG_LINE();
	DEBUG_MSG("Requested new slave frequency: %dHz.", f_slave);

	f_host = MCU_CLK_FREQ_HZ;

	max_f_slave = mtk_calculate_f_slave_max();
	min_f_slave = mtk_calculate_f_slave_min();

	if (max_f_slave < f_slave) {

		DEBUG_MSG("Requested slave frequency is too high: %dHz! "
			"The max. available is: %dHz", f_slave, max_f_slave);

		sclk = MIN_SCLK_VALUE;

	} else if (min_f_slave > f_slave) {

		DEBUG_MSG("Requested slave frequency is too low: %dHz! "
			"The min. available is: %dHz", f_slave, min_f_slave);

		sclk = MAX_SCLK_VALUE;

	} else if (((f_host / 2) == f_slave) || (f_host < (f_slave * 4))) {

		sclk = MIN_SCLK_VALUE;
	} else
		sclk = f_host / (4 * f_slave);

	calc_f_slave = mtk_calculate_f_slave(sclk);

	/* If the calculated frequency is still too high,
	   it should be decreased. */
	while (calc_f_slave > f_slave) {

		sclk++;
		DEBUG_MSG("Requested frequency out of range, it will "
			"be decreased!");
		calc_f_slave = mtk_calculate_f_slave(sclk);
	}

	DEBUG_MSG("f_host: %dHz, calc_f_slave: %dHz(f_slave: %dHz), "
		"SCLK: %d.", f_host, calc_f_slave, f_slave, sclk);

	writel((readl(MTK_MSDC_CFG) & (~MTK_MSDC_CFG_SCLKF_MASK)) | sclk << 8,
		MTK_MSDC_CFG);
}

/*
 * Perform configuration related settings for MMC host.
 */
static void mtk_set_ios(struct mmc *mmc)
{
	struct mmc_host *mmc_host = mmc->priv;

	DEBUG_LINE();

	/* 4 -bit data line enable/disable. */
	if (4 == mmc->bus_width)
		writel(readl(MTK_MSDC_SDC_CFG) | MTK_MSDC_SDC_CFG_MDLEN,
			MTK_MSDC_SDC_CFG);
	else
		writel(readl(MTK_MSDC_SDC_CFG) & (~MTK_MSDC_SDC_CFG_MDLEN),
			MTK_MSDC_SDC_CFG);

	if (mmc->clock && (mmc_host->clock != mmc->clock)) {

		/* Before changing the frequency of serial clock on the bus,
		   it is necessary to disable serial interface
		   of the controller. */
		writel(readl(MTK_MSDC_SDC_CFG) & (~MTK_MSDC_SDC_CFG_SIEN),
			MTK_MSDC_SDC_CFG);

		mtk_set_clk_rate(mmc_host, mmc->clock);

		writel(readl(MTK_MSDC_SDC_CFG) | MTK_MSDC_SDC_CFG_SIEN,
			MTK_MSDC_SDC_CFG);

		DEBUG_MSG("Serial clock rate changed. The clock is enabled.");

		mmc_host->clock = mmc->clock;
	}
}

/*
 * Configures controller for the given data type.
 */
static int mtk_setup_data(struct mmc_host *mmc_host, u32 *msdc_sdc_cmd)
{
	u32 no_of_blocks = mmc_host->data->blocks;
	u32 block_size = mmc_host->data->blocksize;

	DEBUG_LINE();

	/* Read/write command selection. */
	if (mmc_host->data->flags & MMC_DATA_WRITE)
		*msdc_sdc_cmd |= MTK_MSDC_SDC_CMD_RW;
	else
		*msdc_sdc_cmd &= ~MTK_MSDC_SDC_CMD_RW;

	if (MAX_BLOCK_SIZE < block_size)
		DEBUG_MSG("Block size excceeds max. block length!");
	else {
		/* Write length of block. */
		writel((readl(MTK_MSDC_SDC_CFG) &
			(~MTK_MSDC_SDC_CFG_BLKLEN_MASK)) | block_size,
			MTK_MSDC_SDC_CFG);

		DEBUG_MSG("Block size is: %d.", block_size);
	}

	DEBUG_MSG("Number of blocks: %d", no_of_blocks);

	if (1 == no_of_blocks) {
		*msdc_sdc_cmd |= (MTK_MSDC_SDC_CMD_DTYPE_SINGLE_BLOCK <<
			MTK_MSDC_SDC_CMD_DTYPE);
	} else if (1 < no_of_blocks) {
		*msdc_sdc_cmd |= (MTK_MSDC_SDC_CMD_DTYPE_MULTI_BLOCK <<
			MTK_MSDC_SDC_CMD_DTYPE);
	} else {
		DEBUG_MSG("Data supplied but block count is 0!");
		return -EINVAL;
	}

	/* Clear FIFO. */
	writew(readl(MTK_MSDC_STA) | MTK_MSDC_STA_FIFOCLR, MTK_MSDC_STA);

	return 0;
}

/*
 * Configures controller for the given command type.
 */
static int mtk_setup_cmd(struct mmc_host *mmc_host,
	struct mmc_cmd *cmd, u32 *msdc_sdc_cmd)
{
	u32 response_type;

	DEBUG_LINE();
	DEBUG_MSG("CMD ID: %d, CMD ARG: 0x%.8x, CMD RESP TYPE: %d",
		cmd->cmdidx, cmd->cmdarg, cmd->resp_type);

	/* Choose response type. */
	switch (cmd->resp_type) {

	case MMC_RSP_R1:
		/* R1 response token is 48-bit. */
		response_type = MTK_MSDC_SDC_CMD_RSPTYP_R1;
		break;
	case MMC_RSP_R1b:
		response_type = MTK_MSDC_SDC_CMD_RSPTYP_R1B;
		break;
	case MMC_RSP_R2:
		/* R2 response token is 136-bit. */
		response_type = MTK_MSDC_SDC_CMD_RSPTYP_R2;
		break;
	case MMC_RSP_R3:
		/* R3 response token is 48-bit, no CRC */
		response_type = MTK_MSDC_SDC_CMD_RSPTYP_R3;
		break;
#if 0
	case MMC_RSP_R4:
		/* R4 response token is 48-bit. (Only for MMC) */
		response_type = MTK_MSDC_SDC_CMD_RSPTYP_R4;
		break;
	case MMC_RSP_R5:
		/* R5 response token is 48-bit. (Only for MMC) */
		response_type = MTK_MSDC_SDC_CMD_RSPTYP_R5;
		break;
	case MMC_RSP_R6:
		/* R6 response token is 48-bit.*/
		response_type = MTK_MSDC_SDC_CMD_RSPTYP_R6;
		break;
#endif
	case MMC_RSP_NONE:
		response_type = MTK_MSDC_SDC_CMD_RSPTYP_NONE;
		break;
	default:
		ERROR("Unexpected response type!");
		return -EINVAL;
	}

	*msdc_sdc_cmd |= response_type << MTK_MSDC_SDC_CMD_RSPTYP;

	/* Store the command argument first! */
	writel(cmd->cmdarg, MTK_MSDC_SDC_ARG);
	/* ..then the command id. */
	*msdc_sdc_cmd |= cmd->cmdidx;

	if (MMC_CMD_STOP_TRANSMISSION == cmd->cmdidx) {
		*msdc_sdc_cmd |= MTK_MSDC_SDC_CMD_STOP;
		writel(readl(MTK_MSDC_SDC_CFG) | 16 << MTK_MSDC_SDC_CFG_BSYDLY,
			MTK_MSDC_SDC_CFG);
	} else
		*msdc_sdc_cmd &= ~MTK_MSDC_SDC_CMD_STOP;

	return 0;
}

/*
 * Read SD/MMC Memory Card bus response.
 */
static void mtk_read_response(struct mmc_host *mmc_host)
{
	u32	resp0;
	u32	resp1;
	u32	resp2;
	u32	resp3;

	DEBUG_LINE();

	if (!mmc_host->cmd)
		return;

	/* Check if a response is expected and read it.
	   For response of type R2 only bit 127 to 0 of response token is stored
	   in the register field SDC_RESP0, SDC_RESP1, SDC_RESP2 and SDC_RESP3.
	   For response of other types, only bit 39 to 8 of response token
	   is stored in the register field SDC_RESP0. */

	if (mmc_host->cmd->resp_type & MMC_RSP_PRESENT) {

			resp0 = readl(MTK_MSDC_SDC_RESP0);

		if (mmc_host->cmd->resp_type & MMC_RSP_136) {

			resp1 = readl(MTK_MSDC_SDC_RESP1);
			resp2 = readl(MTK_MSDC_SDC_RESP2);
			resp3 = readl(MTK_MSDC_SDC_RESP3);

			mmc_host->cmd->response[0] = resp3;
			mmc_host->cmd->response[1] = resp2;
			mmc_host->cmd->response[2] = resp1;
			mmc_host->cmd->response[3] = resp0;

			DEBUG_MSG("Response[0]: 0x%.8x",
				mmc_host->cmd->response[0]);
			DEBUG_MSG("Response[1]: 0x%.8x",
				mmc_host->cmd->response[1]);
			DEBUG_MSG("Response[2]: 0x%.8x",
				mmc_host->cmd->response[2]);
			DEBUG_MSG("Response[3]: 0x%.8x",
				mmc_host->cmd->response[3]);
		} else {
			mmc_host->cmd->response[0] = resp0;

			DEBUG_MSG("Response[0]: 0x%.8x",
				mmc_host->cmd->response[0]);
		}
	}
}

/*
 * Checks if reading FIFO is possible.
 */
static int mtk_poll_data_transfer_conditions(const u16 transfer_condition)
{
	int retval = 0;
	u16 msdc_sdc_datsta;
	u32 timeout_counter = 0;

	DEBUG_LINE();

	do {
		/* First of all check data status. */
		msdc_sdc_datsta = readw(MTK_MSDC_SDC_DATSTA);

		if (msdc_sdc_datsta & MTK_MSDC_SDC_DATSTA_BLKDONE)
			DEBUG_MSG("A data block was successfully transferred.");

		if (msdc_sdc_datsta & MTK_MSDC_SDC_DATSTA_DATTO) {
			ERROR("MS/SD controller detected a timeout "
				"condition while waiting for data token on "
				"the DAT line.");

			retval = -1;
		}

		if (msdc_sdc_datsta & MTK_MSDC_SDC_DATSTA_DATCRCERR) {
			ERROR("MS/SD controller detected a CRC error!");

			retval = -1;
		}

		/* Check FIFO. */
		if ((!(readw(MTK_MSDC_STA) & transfer_condition)) || retval)
			break;
		else
			DEBUG_MSG("FIFO is empty!");

		udelay(1);

	} while (++timeout_counter != POLL_TRANSFER_COND_TIMEOUT_US);

	if (timeout_counter == POLL_TRANSFER_COND_TIMEOUT_US) {
		ERROR("Timeout occurred during polling "
			"data transfer conditions.");
		retval = TIMEOUT;
	}

	return retval;
}

/*
 * Reads data received from memory card.
 */
static int mtk_read_data(void *dest, u32 length)
{
	int	retval = 0;
	u32	*destl = dest;
	u32	timeout_counter = 0;

	DEBUG_LINE();

	/* Wait for data transfer request. */
	while (!(readw(MTK_MSDC_STA) & MTK_MSDC_STA_DRQ)) {

		udelay(1);

		if (++timeout_counter == DATA_READ_TIMEOUT_US) {
			ERROR("Data read timeout occured!");
			break;
		}
	}

	DEBUG_MSG("Block size is: %d", readl(MTK_MSDC_SDC_CFG) &
		MTK_MSDC_SDC_CFG_BLKLEN_MASK);

	while (length) {

		DEBUG_MSG("Bytes left to read: %d.", length);

		/* Read data if there is any in the FIFO. */
		retval = mtk_poll_data_transfer_conditions(MTK_MSDC_STA_BE);

		if (retval)
			goto finish_read;

		if (3 < length) {

			*destl++ = readl(MTK_MSDC_DAT);
			length -= 4;

			DEBUG_MSG("DATA[R]: 0x%.8x", *(destl - 1));

		} else {
			u8 *dest8 = (u8 *)destl;
			u32 msdc_dat = readl(MTK_MSDC_DAT);
			memcpy(dest8, &msdc_dat, length);

			length = 0;

			DEBUG_MSG("DATA[R]: 0x%.8x", msdc_dat);
		}
	}

	if (!readw(MTK_MSDC_STA) & MTK_MSDC_STA_BE)
		ERROR("There is still some data left in FIFO!");

finish_read:
	return retval;
}

/*
 * Writes data to be sent to memory card.
 */
static int mtk_write_data(const void *src, int length)
{
	int		retval = 0;
	const u32	*srcl = src;
	u32		timeout_counter = 0;

	DEBUG_LINE();

	/* Wait for data transfer request. */
	while (!(readw(MTK_MSDC_STA) & MTK_MSDC_STA_DRQ)) {

		udelay(1);

		if (++timeout_counter == DATA_READ_TIMEOUT_US) {
			ERROR("Data write timeout occured!");
			break;
		}
	}

	while (length) {

		DEBUG_MSG("Length of the data to write: %d.", length);

		/* Write data if there is enough space in the FIFO. */
		retval = mtk_poll_data_transfer_conditions(MTK_MSDC_STA_BF);

		if (retval)
			goto finish_write;

		if (3 < length) {
			writel(*srcl++, MTK_MSDC_DAT);
			length -= 4;

			DEBUG_MSG("DATA[W]: 0x%.8x", *(srcl - 1));
		} else {

			u8 *src8 = (u8 *)srcl;
			u32 temp_val;

			memcpy(&temp_val, src8, length);
			writel(temp_val, MTK_MSDC_DAT);

			length = 0;

			DEBUG_MSG("DATA[W]: 0x%.8x", temp_val);
		}
	}

finish_write:
	return retval;
}

/*
 * Performs data transfer between host and memory card.
 */
static int mtk_transfer_data(struct mmc_host *mmc_host)
{
	int	retval = 0;
	struct	mmc_data *data = mmc_host->data;
	u32	length = data->blocks * data->blocksize;

	DEBUG_LINE();

	if (data->flags & MMC_DATA_READ) {
		DEBUG_MSG("Reading data...");
		retval = mtk_read_data(data->dest, length);
	} else {
		DEBUG_MSG("Writing data...");
		retval = mtk_write_data(data->src, length);
	}

	return retval;
}

/*
 * Sends a command out on the MMC/SD bus.
 */
static int mtk_send_cmd(struct mmc *mmc,
	struct mmc_cmd *cmd, struct mmc_data *data)
{
	int	retval;
	u16	cmd_status;
	u16	controller_status;
	struct	mmc_host *mmc_host = mmc->priv;
	u32	msdc_sdc_cmd = mmc_host->msdc_sdc_cmd;
	u32	timeout_counter;

	DEBUG_LINE();

	mmc_host->cmd = cmd;
	mmc_host->data = data;

#ifdef MTK_MMC_CARD_DETECTION_SUPPORTED

	if (!check_card_presence(mmc_host)) {
		DEBUG_MSG("Memory card inserted!");
	} else {
		DEBUG_MSG("Memory card not inserted");
		retval = -1;
		goto finish;
	}

#endif /* MTK_MMC_CARD_DETECTION_SUPPORTED */

	/* Check data supplied. */
	if (mmc_host->data) {
		retval = mtk_setup_data(mmc_host, &msdc_sdc_cmd);

		if (retval)
			goto finish;
	} else
		msdc_sdc_cmd |= (MTK_MSDC_SDC_CMD_DTYPE_NO_DATA <<
			MTK_MSDC_SDC_CMD_DTYPE);

	/* Prepared command to send. */
	retval = mtk_setup_cmd(mmc_host, cmd, &msdc_sdc_cmd);

	if (retval)
		goto finish;

	/* Clear command status register. */
	(void)readl(MTK_MSDC_SDC_CMDSTA);

	/* Send the command... */
	writel(msdc_sdc_cmd, MTK_MSDC_SDC_CMD);

	timeout_counter = 0;

	/* Wait until the command is sent. */
	do {
		controller_status = readl(MTK_MSDC_SDC_STA);
		DEBUG_MSG("Controller status: 0x%.4x", controller_status);
		udelay(1);
	} while ((controller_status & MTK_MSDC_SDC_STA_CMDBUSY) &&
		(++timeout_counter != POLL_CMD_STATUS_TIMEOUT_US));

	if (POLL_CMD_STATUS_TIMEOUT_US == timeout_counter) {
		ERROR("Waiting for controller status timeout occured "
			"(controller status: %d)!", controller_status);
		retval = TIMEOUT;
		goto finish;
	}

	/* Wait for command status. */
	do {
		cmd_status = readl(MTK_MSDC_SDC_CMDSTA);

		DEBUG_MSG("Command status: 0x%.4x", cmd_status);

		if (cmd_status & MTK_MSDC_SDC_CMDSTA_CMDRDY)
			DEBUG_MSG("Command ready!");

		if (cmd_status & MTK_MSDC_SDC_CMDSTA_CMDTO) {
			ERROR("MS/SD controller detected a timeout "
				"condition while waiting for a response on the "
				"CMD line.");
			retval = TIMEOUT;
			goto finish;
		}

		if (cmd_status & MTK_MSDC_SDC_CMDSTA_RSPCRCERR) {
			ERROR("MS/SD controller detected a CRC error after "
				"reading a response from the CMD line.");
			retval = -1;
			goto finish;
		}

		if (cmd_status & MTK_MSDC_SDC_CMDSTA_MMCIRQ) {
			ERROR("MMC supporting command class 9 issued an "
				"interrupt request.");
			retval = -1;
			goto finish;
		}

		udelay(COMMAND_STATUS_READ_DELAY_US);

	} while (!cmd_status);

	/* Read command response. */
	mtk_read_response(mmc_host);

	/* Read/write data if supplied. */
	if (mmc_host->data) {

		retval = mtk_transfer_data(mmc_host);

		if (retval)
			goto finish;
	}

	/* Read memory card status register. */
	DEBUG_MSG("=> Memory card status register: 0x%.8x.<=",
		readl(MTK_MSDC_SDC_CSTA));
finish:
	mmc_host->cmd = NULL;
	mmc_host->data = NULL;

	return retval;
}

/*
 * Initialization of mmc.
 */
int mtk_mmc_init(bd_t *bis)
{
	struct mmc *mmc = malloc(sizeof(struct mmc));

	DEBUG_LINE();

	if (!mmc)
		return -1;

	strcpy(mmc->name, "msdc_mmc");

	mmc->send_cmd = mtk_send_cmd;
	mmc->set_ios = mtk_set_ios;
	mmc->init = mtk_init;

	mmc->voltages = MMC_VDD_32_33 | MMC_VDD_33_34;
	mmc->host_caps = MMC_MODE_4BIT;

	mmc->priv = &mmc_host;
	mmc_host.mmc = mmc;

	/* Min and max frequencies are determined by
	   max and min values of clock divider (SCLKF in MSDC_CFG). */
	mmc->f_min = mtk_calculate_f_slave_min();
	mmc->f_max = MAX_SLAVE_FREQ_HZ;

	mmc_register(mmc);

	return 0;
}
