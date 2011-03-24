#ifndef _MTK_BPI_H
#define _MTK_BPI_H

/* MTK Baseband Parallel Interface */

/* Chapter 9.2 of MT6235 Data Sheet */
#define	BPI_CON		(MTK_BPI_BASE + 0x0000)
#define BPI_BUF0	(MTK_BPI_BASE + 0x0004)
#define BPI_BUFI	(MTK_BPI_BASE + 0x00ac)
#define	BPI_ENA0	(MTK_BPI_BASE + 0x00b0)
#define BPI_ENA1	(MTK_BPI_BASE + 0x00b4)
#define BPI_ENA2	(MTK_BPI_BASE + 0x00b8)

#define BPI_BUF(n)	(BPI_BUF0 + ((n) * 4))

#define MTK_BPI(n)	(n)

/* BPI_CON bit fields */
#define BPI_CON_PETEV	1
#define BPI_CON_PINM0	2
#define BPI_CON_PINM1	3
#define BPI_CON_PINM2	4

#endif /* _MTK_BPI_H */
