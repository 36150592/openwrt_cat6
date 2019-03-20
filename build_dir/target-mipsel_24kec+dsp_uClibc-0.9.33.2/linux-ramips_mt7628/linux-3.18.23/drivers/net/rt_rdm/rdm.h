
#ifndef	__RDM__H__
#define	__RDM__H__

#include <linux/fs.h>       /* everything... */
//#include <asm/rt2880/rt_mmap.h>

#if 1 // mt7628 reg base
#define RALINK_SYSCTL_BASE		0xB0000000
#define RALINK_TIMER_BASE		0xB0000100
#define RALINK_INTCL_BASE		0xB0000200
#define RALINK_MEMCTRL_BASE		0xB0000300
#define RALINK_RBUS_MATRIXCTL_BASE	0xB0000400
#define RALINK_MIPS_CNT_BASE		0x10000500
#define RALINK_PIO_BASE			0xB0000600
#define RALINK_SPI_SLAVE_BASE		0xB0000700
#define RALINK_I2C_BASE			0xB0000900
#define RALINK_I2S_BASE			0xB0000A00
#define RALINK_SPI_BASE			0xB0000B00
#define RALINK_UART_LITE1_BASE		0x10000C00
#define RALINK_UART_LITE_BASE		RALINK_UART_LITE1_BASE
#define RALINK_UART_LITE2_BASE		0x10000D00
#define RALINK_UART_BASE		RALINK_UART_LITE2_BASE
#define RALINK_UART_LITE3_BASE		0x10000E00
#define RALINK_PCM_BASE			0xB0002000
#define RALINK_GDMA_BASE		0xB0002800
#define RALINK_AES_ENGINE_BASE		0xB0004000
#define RALINK_FRAME_ENGINE_BASE	0xB0100000
#define RALINK_PPE_BASE			0xB0100C00
#define RALINK_ETH_SW_BASE		0xB0110000
#define RALINK_USB_DEV_BASE		0xB0120000
#define RALINK_MSDC_BASE		0xB0130000
#define RALINK_PCI_BASE                 0xB0140000
#define RALINK_11N_MAC_BASE		0xB0180000
#define RALINK_USB_HOST_BASE		0x101C0000

#define RALINK_MCNT_CFG			0xB0000500
#define RALINK_COMPARE			0xB0000504
#define RALINK_COUNT			0xB0000508


//Interrupt Controller
#define RALINK_INTCTL_SYSCTL            (1<<0)
#define RALINK_INTCTL_TIMER0            (1<<1)
#define RALINK_INTCTL_WDTIMER           (1<<2)
#define RALINK_INTCTL_ILL_ACCESS        (1<<3)
#define RALINK_INTCTL_PCM               (1<<4)
#define RALINK_INTCTL_UART              (1<<5)
#define RALINK_INTCTL_PIO               (1<<6)
#define RALINK_INTCTL_DMA               (1<<7)
#define RALINK_INTCTL_PC                (1<<9)
#define RALINK_INTCTL_I2S               (1<<10)
#define RALINK_INTCTL_SPI               (1<<11)
#define RALINK_INTCTL_UARTLITE          (1<<12)
#define RALINK_INTCTL_CRYPTO            (1<<13)
#define RALINK_INTCTL_ESW               (1<<17)
#define RALINK_INTCTL_UHST              (1<<18)
#define RALINK_INTCTL_UDEV              (1<<19)
#define RALINK_INTCTL_GLOBAL            (1<<31)

//Reset Control Register
#define RALINK_SYS_RST                  (1<<0)
#define RALINK_TIMER_RST                (1<<8)
#define RALINK_INTC_RST                 (1<<9)
#define RALINK_MC_RST                   (1<<10)
#define RALINK_PCM_RST                  (1<<11)
#define RALINK_UART_RST                 (1<<12)
#define RALINK_PIO_RST                  (1<<13)
#define RALINK_DMA_RST                  (1<<14)
#define RALINK_I2C_RST                  (1<<16)
#define RALINK_I2S_RST                  (1<<17)
#define RALINK_SPI_RST                  (1<<18)
#define RALINK_UARTL_RST                (1<<19)
#define RALINK_FE_RST                   (1<<21)
#define RALINK_UHST_RST                 (1<<22)
#define RALINK_ESW_RST                  (1<<23)
#define RALINK_EPHY_RST                 (1<<24)
#define RALINK_UDEV_RST                 (1<<25)
#define RALINK_PCIE0_RST                (1<<26)
#define RALINK_PCIE1_RST                (1<<27)
#define RALINK_MIPS_CNT_RST             (1<<28)
#define RALINK_CRYPTO_RST               (1<<29)

//Clock Conf Register
#define RALINK_UPHY0_CLK_EN		(1<<25)
#define RALINK_UPHY1_CLK_EN		(1<<22)
#define RALINK_PCIE0_CLK_EN		(1<<26)
#define RALINK_PCIE1_CLK_EN		(1<<27)

//CPU PLL CFG Register
#define CPLL_SW_CONFIG                  (0x1UL << 31)
#define CPLL_MULT_RATIO_SHIFT           16
#define CPLL_MULT_RATIO                 (0x7UL << CPLL_MULT_RATIO_SHIFT)
#define CPLL_DIV_RATIO_SHIFT            10
#define CPLL_DIV_RATIO                  (0x3UL << CPLL_DIV_RATIO_SHIFT)
#define BASE_CLOCK                      40      /* Mhz */

//AGPIO
#define MT7628_P0_EPHY_AIO_EN          (0x1<<16)
#define MT7628_P1_EPHY_AIO_EN          (0x1<<17)
#define MT7628_P2_EPHY_AIO_EN          (0x1<<18)
#define MT7628_P3_EPHY_AIO_EN          (0x1<<19)
#define MT7628_P4_EPHY_AIO_EN          (0x1<<20)
#endif // mt7628 reg base end

#define RTPRIV_ICOTL_SYSCTL		(SIOCIWFIRSTPRIV + 0x01) // system control
#define RTPRIV_ICOTL_TIMER		(SIOCIWFIRSTPRIV + 0x02) // timer
#define RTPRIV_ICOTL_ITRCTL		(SIOCIWFIRSTPRIV + 0x03) // interrupt control
#define RTPRIV_ICOTL_MEMCTL		(SIOCIWFIRSTPRIV + 0x04) // memory control
#define RTPRIV_ICOTL_TSTCTL		(SIOCIWFIRSTPRIV + 0x05) // Test Control
#define RTPRIV_ICOTL_UART		(SIOCIWFIRSTPRIV + 0x06) // UART
#define RTPRIV_ICOTL_PRGIO		(SIOCIWFIRSTPRIV + 0x07) // Programming I/O
#define RTPRIV_ICOTL_LCD		(SIOCIWFIRSTPRIV + 0x08) // LCD
#define RTPRIV_ICOTL_I2C		(SIOCIWFIRSTPRIV + 0x09) // I2C
#define RTPRIV_ICOTL_CLOCK		(SIOCIWFIRSTPRIV + 0x0A) // realtime clock
#define RTPRIV_ICOTL_SPI		(SIOCIWFIRSTPRIV + 0x0B) // SPI
#define RTPRIV_ICOTL_UARTL		(SIOCIWFIRSTPRIV + 0x0C) // UART Lite
#define RTPRIV_ICOTL_PCI		(SIOCIWFIRSTPRIV + 0x0D) // PCI Controller

#define RDM_SYSCTL_ADDR			RALINK_SYSCTL_BASE // system control

#define RT_RDM_CMD_SHOW			0x6B01
#define RT_RDM_CMD_WRITE		0x6B02
#define RT_RDM_CMD_READ			0x6B03
#define RT_RDM_CMD_WRITE_SILENT		0x6B04
#define RT_RDM_CMD_DUMP			0x6B05
#define RT_RDM_CMD_DUMP_FPGA_EMU	0x6B06
#define RT_RDM_CMD_SHOW_BASE		0x6B0C
#define RT_RDM_CMD_SET_BASE		0x6B0D
#define RT_RDM_CMD_SET_BASE_SYS		0x6B0E
#define RT_RDM_CMD_SET_BASE_WLAN	0x6B0F
#define RT_RDM_CMD_DUMP_QUEUE_OFFSET 0x6B10
#define RT_RDM_CMD_DUMP_QUEUE 0x6B11

#define RT_RDM_DUMP_RANGE		16  // unit=16bytes
#endif
