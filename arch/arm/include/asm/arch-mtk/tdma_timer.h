#ifndef _MTK_TDMA_H
#define _MTK_TDMA_H

/* MTK TDMA Timer */

/* MT6235 Section 11 */

/* Read current quarter bit count */
#define TDMA_TQCNT		(MTK_TDMA_BASE + 0x0000)
/* Latched Qbit counter reset position */
#define	TDMA_WRAP		(MTK_TDMA_BASE + 0x0004)
/* Direct Qbit counter reset position */
#define TDMA_WRAPIMD		(MTK_TDMA_BASE +  0x0008)
/* Event latch position */
#define TDMA_EVTVAL		(MTK_TDMA_BASE + 0x000c)
/* DSP software control */
#define TDMA_DTIRQ		(MTK_TDMA_BASE +  0x0010)
/* MCU software control */
#define DMA_CTIRQ1		(MTK_TDMA_BASE + 0x0014)
#define TDMA_CTIRQ2		(MTK_TDMA_BASE + 0x0018)
/* AFC control */
#define TDMA_AFC0		(MTK_TDMA_BASE + 0x0020)
#define TDMA_AFC1		(MTK_TDMA_BASE + 0x0024)
#define TDMA_AFC2		(MTK_TDMA_BASE + 0x0028)
#define TDMA_AFC3		(MTK_TDMA_BASE + 0x002c)

/* Downlink control */
#define TDMA_BDLON0		(MTK_TDMA_BASE + 0x0030)
#define TDMA_BDLOFF0		(MTK_TDMA_BASE + 0x0034)
#define TDMA_BDLON1		(MTK_TDMA_BASE + 0x0038)
#define TDMA_BDLOFF1		(MTK_TDMA_BASE + 0x003C)
#define TDMA_BDLON2		(MTK_TDMA_BASE + 0x0040)
#define TDMA_BDLOFF2		(MTK_TDMA_BASE + 0x0044)
#define TDMA_BDLON3		(MTK_TDMA_BASE + 0x0048)
#define TDMA_BDLOFF3		(MTK_TDMA_BASE + 0x004C)
#define TDMA_BDLON4		(MTK_TDMA_BASE + 0x0050)
#define TDMA_BDLOFF4		(MTK_TDMA_BASE + 0x0054)
#define TDMA_BDLON5		(MTK_TDMA_BASE + 0x0058)
#define TDMA_BDLOFF5		(MTK_TDMA_BASE + 0x005C)

/* Uplink control */
#define TDMA_BULON0		(MTK_TDMA_BASE + 0x0060)
#define TDMA_BULOFF0		(MTK_TDMA_BASE + 0x0064)
#define TDMA_BULON1		(MTK_TDMA_BASE + 0x0068)
#define TDMA_BULOFF1		(MTK_TDMA_BASE + 0x006C)
#define TDMA_BULON2		(MTK_TDMA_BASE + 0x0070)
#define TDMA_BULOFF2		(MTK_TDMA_BASE + 0x0074)
#define TDMA_BULON3		(MTK_TDMA_BASE + 0x0078)
#define TDMA_BULOFF3		(MTK_TDMA_BASE + 0x007C)

/* APC control */
#define TDMA_APC0		(MTK_TDMA_BASE + 0x0090)
/* BSI event */
#define TDMA_BSI0		(MTK_TDMA_BASE + 0x00b0)
/* BPI event */
#define TDMA_BPI0		(MTK_TDMA_BASE + 0x0100)
/* Auxiliary ADC event */
#define TDMA_AUXEV0		(MTK_TDMA_BASE + 0x0400)
#define TDMA_AUXEV1		(MTK_TDMA_BASE + 0x0404)
/* Event Control */
#define TDMA_EVTENA0		(MTK_TDMA_BASE + 0x0150)
#define TDMA_EVTENA1		(MTK_TDMA_BASE + 0x0154)
#define TDMA_EVTENA2		(MTK_TDMA_BASE + 0x0158)
#define TDMA_EVTENA3		(MTK_TDMA_BASE + 0x015c)
#define TDMA_EVTENA4		(MTK_TDMA_BASE + 0x0160)
#define TDMA_EVTENA5		(MTK_TDMA_BASE + 0x0164)
#define TDMA_EVTENA6		(MTK_TDMA_BASE + 0x0168)
#define TDMA_EVTENA7		(MTK_TDMA_BASE + 0x016c)
#define TDMA_WRAPOFS		(MTK_TDMA_BASE + 0x0170)
#define TDMA_REGBIAS		(MTK_TDMA_BASE + 0x0174)
#define TDMA_DTXCON		(MTK_TDMA_BASE + 0x0180)
#define TDMA_RXCON		(MTK_TDMA_BASE + 0x0184)
#define TDMA_BDLCON		(MTK_TDMA_BASE + 0x0188)
#define TDMA_BULCON1		(MTK_TDMA_BASE + 0x018c)
#define TDMA_BULCON2		(MTK_TDMA_BASE + 0x0190)
#define TDMA_FB_FLAG		(MTK_TDMA_BASE + 0x0194)
#define TDMA_FB_CLRI		(MTK_TDMA_BASE + 0x0198)

#define TDMA_BSI(n)		(TDMA_BSI0 + (n)*4)
#define TDMA_BPI(n)		(TDMA_BPI0 + (n)*4)
#define TDMA_APC(n)		(TDMA_APC0 + (n)*4)
#define TDMA_BULON(n)		(TDMA_BULON0 + (n)*8)
#define TDMA_BULOFF(n)		(TDMA_BULOFF0 + (n)*8)
#define TDMA_BDLON(n)		(TDMA_BDLON0 + (n)*8)
#define TDMA_BDLOFF(n)		(TDMA_BDLOFF0 + (n)*8)

#endif /* _MTK_TDMA_H */
