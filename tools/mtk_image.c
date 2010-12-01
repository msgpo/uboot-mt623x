/*
 * (C) 2010 by Tieto <www.tieto.com>
 *      Marcin Mielczarczyk <marcin.mielczarczyk@tieto.com>
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

/*
 * This tool creates boot header for MT62XX platforms.
 * Boot header is parsed by IPL (Initial Program Loader) which is executed
 * just after power up of processor.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>

/* Checksum will be calulated starting from this value */
#define CHECKSUM_SEED		0x33448822
/* MT62xx internal RAM size */
#define SRAM_SIZE		65536
/*
 * Number of headers at the beginning of image.
 * Header is repeated 8 times, because it's read by MTK
 * bootloader with different NFI controller settings.
 * Checking header 8 times guarantees that NAND page is read properly
 * with currently chosen NFI settings.
 */
#define BOOT_HEADERS_NUMBER	8
/* Buffer size for file reading */
#define READ_BUFFER_SIZE	102400

/* Format of boot header which is expected by MTK bootloader */
struct boot_header {
	char		bootloader[12];
	char		version[4];
	uint16_t	size;
	uint32_t	load_address;
	uint32_t	checksum;
	char		nfiinfo[8];
	uint16_t	bus_width;
	uint16_t	page_size;
	uint16_t	address_bytes;
	uint16_t	column_shift;
	uint16_t	reserved;
	uint16_t	unknown;
	uint16_t	block_shift;
	uint16_t	block_nr[6];
	uint16_t	unknown2;
};

static struct boot_header bheader;

static void usage(char *cmd)
{
	printf("Usage %s [ -s size ] [ -o file ] mtk_dump.bin image.bin\n"
		"\t[ -s size ]\t- size of image, if not specified"
		" file size will be taken\n"
		"\t[ -o file ]\t- name of output file\n"
		"\tmtk_dump.bin\t- dump of NAND memory"
		" from address 0 (at least 64 bytes)\n"
		"\timage.bin\t- image to be loaded by MTK bootloader\n",
		cmd);
}

#define BOOT_HEADER_DUMP		\
	"Generated boot header with following parameters:\n"	\
	"\tSize of image:\t\t%d\n"	\
	"\tLoad address:\t\t0x%x\n"	\
	"\tChecksum:\t\t0x%x\n"		\
	"\tBus width:\t\t%s\n"		\
	"\tPage size:\t\t%d\n"		\
	"\tAddress bytes:\t\t%d\n"	\
	"\tColumn address shift:\t%d\n"	\
	"\tBlock address shift:\t%d\n"	\
	"\tBlocks with image:\t%d, %d, %d, %d, %d, %d\n"

static int read_boot_header(const char *file_name)
{
	int fd, ret;
	const char bootloader[] = "BOOTLOADER!";
	const char nfiinfo[] = "NFIINFO";

	fd = open(file_name, O_RDONLY);
	if (fd < 0) {
		perror("Opening file");
		return -EIO;
	}

	ret = read(fd, &bheader, sizeof(bheader));
	if (ret < 0) {
		perror("Reading file");
		ret = -EIO;
		goto err;
	}

	if (ret < sizeof(bheader)) {
		printf("File with MTK boot header to short\n");
		ret = -EIO;
		goto err;
	}

	if ((strcmp(bootloader, bheader.bootloader) != 0) ||
	    (strcmp(nfiinfo, bheader.nfiinfo) != 0)) {
		printf("File %s doesn't contain boot header\n", file_name);
		ret = -EINVAL;
		goto err;
	}

	printf("MTK boot header detected...\n");
err:
	close(fd);
	return ret;
}

static int calculate_checksum(int fd)
{
	int *buf, ret, i;
	int page_size = bheader.page_size;
	int size = bheader.size;
	int checksum = CHECKSUM_SEED;

	buf = malloc(size);
	if (!buf) {
		perror("Allocating memory");
		return -ENOMEM;
	}

	ret = read(fd, buf, size);
	if (ret < 0) {
		perror("Reading file");
		ret = -EIO;
		goto err;
	}

	if (ret < size) {
		printf("File too short!\n");
		ret = -EIO;
		goto err;
	}

	for (i = 0; i < size/4; ++i)
		checksum ^= buf[i];

	bheader.checksum = checksum;
err:
	free(buf);
}

static int create_boot_header(const char *file_name, int size)
{
	int fd, ret, block_size, blocks_nr, fit_blocks_nr, i;
	struct stat file_stat;

	fd = open(file_name, O_RDONLY);
	if (fd < 0) {
		perror("Opening file");
		return -EIO;
	}

	if (size == 0) {
		if (fstat(fd, &file_stat) < 0) {
			perror("Getting file status");
			ret = -EIO;
			goto err;
		}
		size = file_stat.st_size;
	}

	bheader.size = size;
	bheader.checksum = CHECKSUM_SEED;
	ret = calculate_checksum(fd);
	if (ret < 0)
		goto err;

	if (bheader.page_size != 512 && bheader.page_size != 2048) {
		printf("Page size %d is not supported\n", bheader.page_size);
		ret -EINVAL;
		goto err;
	}

	if (bheader.page_size == 512)
		/* Block size = page_size * 32 = 16384 */
		block_size = 512 * 32;
	else
		/* Block size = page_size * 64 = 131072 */
		block_size = 2048 * 64;

	/* Calculate how many blocks are occupied by image */
	blocks_nr = bheader.size / block_size;
	/* Calculate how many blocks will fit to internal SRAM memory */
	fit_blocks_nr = SRAM_SIZE / block_size;

	/* Check if whole image will fit to internal SRAM */
	if (blocks_nr > fit_blocks_nr)
		/* Image is bigger, copy only part of image */
		blocks_nr = fit_blocks_nr;

	/* Decrease one block as image is already part of boot header */
	if (blocks_nr > 0)
		blocks_nr--;

	for (i = 1; i < blocks_nr; ++i)
		bheader.block_nr[i] = i;

	printf(BOOT_HEADER_DUMP, bheader.size, bheader.load_address,
		bheader.checksum, bheader.bus_width?"16bit":"8bit",
		bheader.page_size, bheader.address_bytes, bheader.column_shift,
		bheader.block_shift, bheader.block_nr[0], bheader.block_nr[1],
		bheader.block_nr[2], bheader.block_nr[3], bheader.block_nr[4],
		bheader.block_nr[5]);

err:
	close(fd);
	return ret;
}

static int create_image(const char *read_file, const char *write_file)
{
	int fd_write, fd_read, ret, total_length, i;
	uint8_t *buf;
	struct stat file_stat;

	fd_write = open(write_file, O_WRONLY | O_CREAT | O_TRUNC,
			S_IRUSR | S_IWUSR);
	if (fd_write < 0) {
		perror("Opening file");
		return -EIO;
	}

	for (i = 0; i < BOOT_HEADERS_NUMBER; ++i) {
		ret = write(fd_write, &bheader, sizeof(bheader));
		if (ret < 0) {
			perror("Writing file");
			ret = -EIO;
			goto err_close_write;
		}
		if (ret < sizeof(bheader)) {
			printf("Couldn't write to file\n");
			ret = -EIO;
			goto err_close_write;
		}
	}

	fd_read = open(read_file, O_RDONLY);
	if (fd_read < 0) {
		perror("Opening file");
		ret = -EIO;
		goto err_close_write;
	}

	buf = malloc(READ_BUFFER_SIZE);

	ret = fstat(fd_read, &file_stat);
	if (ret < 0) {
		perror("Getting file status");
		ret = -EIO;
		goto err;
	}
	total_length = file_stat.st_size;

	while (total_length) {
		int length;

		i = 0;
		if (total_length > READ_BUFFER_SIZE)
			length = READ_BUFFER_SIZE;
		else
			length = total_length;

		while (i < length) {
			ret = read(fd_read, &buf[i], length);
			if (ret < 0) {
				perror("Reading file");
				ret = -EIO;
				goto err;
			}
			i += ret;
		}

		i = 0;
		while(i < length) {
			ret = write(fd_write, &buf[i], length);
			if (ret < 0) {
				perror("Writing file");
				ret = -EIO;
				goto err;
			}
			i += ret;
		}
		total_length -= length;
	}
err:
	close(fd_read);
	free(buf);
err_close_write:
	close(fd_write);
	return ret;
}

int main(int argc, char **argv)
{
	int opt, size = 0;
	char *output_name = "mtk_image.bin";

	while ((opt = getopt(argc, argv, "o:s:")) != -1) {
		switch(opt) {
		case 'o':
			output_name = optarg;
			break;
		case 's':
			size = atoi(optarg);
			break;
		default:
			usage(argv[0]);
			goto err;
		}
	}

	if ((optind + 2) != argc) {
		usage(argv[0]);
		goto err;
	}

	if (read_boot_header(argv[optind]) < 0)
		goto err;

	if (create_boot_header(argv[optind+1], size) < 0)
		goto err;

	if (create_image(argv[optind+1], output_name) < 0)
		goto err;

	exit(0);
err:
	exit(1);
}
