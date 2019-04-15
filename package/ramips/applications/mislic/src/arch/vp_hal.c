/** \file vp_hal.c
 * vp_hal.c
 *
 * This file contains the platform dependent code for the Hardware Abstraction
 * Layer (HAL). This is example code only to be used by the customer to help
 * clarify HAL requirements.
 *
 * Copyright (c) 2011, Microsemi Corporation
 */
#include "vp_api_types.h"
#include "sys_service.h"
#include "hbi_hal.h"
#include "mpi_hal.h"

#include <stdint.h>  
#include <unistd.h>  
#include <stdio.h>  
#include <stdlib.h>  
#include <getopt.h>  
#include <fcntl.h>  
#include <sys/ioctl.h>  
#include <linux/types.h>  
#include "spidev.h"


/*****************************************************************************
 * HAL functions for VCP. Not necessary for CSLAC devices.
 ****************************************************************************/
/* Changed the HBI deviceId data to match the MPI format S.H.
#define GET_CONNECTOR_ID(deviceId) (((deviceId) & 0x04)?1:0 )
#define GET_CHIP_SELECT(deviceId) ((deviceId) & 0x03)
#define GPI_WrData8(connector, cs, data, size) \
        GPI_Data(connector, cs, 0, GPI_WR_CMD, data, size)
*/
#define GET_CONNECTOR_ID(deviceId) (((deviceId) & 0x10)?0:1 )
#define GET_CHIP_SELECT(deviceId) ((deviceId) & 0x0C)
#define GET_TRANSFER_SIZE(deviceId) ((deviceId) & 0x01)
#define WRITE_COMMAND 0
#define READ_COMMAND 1
#if 0
static uint8 tempBuf[512]; /* All zero buffer */
#endif
/**
 * VpHalHbiInit(): Configures the HBI bus and glue logic (if any)
 *
 * This function performs any tasks necessary to prepare the system for
 * communicating through the HBI, including writing the HBI configuration
 * register.  The HBI read and write functions should work after HbiHalInit()
 * is successfully executed. HbiHalInit() should be well-behaved even if called
 * more than once between system resets. HbiHalInit() is called from
 * VpBootLoad() since VpBootLoad() is normally the first VoicePath function
 * that the host application will call.
 *
 * This function is called by VpBootLoad() before sending the VCP firmware
 * image through the HBI.
 *
 * Params:
 *  uint8 deviceId: Device Id (chip select ID)
 *
 * Returns:
 *  This function returns FALSE if some error occurred during HBI initialization
 *  or TRUE otherwise.
 */
bool VpHalHbiInit(
    VpDeviceIdType deviceId)
{
    /*
     * Note that Setting up the basic device should be handled by the
     * some board bring up function. That function should setup the
     * CPLD based on the line module that is plugged in. Those functions
     * would configure the CPLD so that basic communication to the part
     * can happen between the HAL and the line module.
     */
    /* Write the HBI configuration register. */
    tz_dbg(LOG_LEVEL_HAL, "VpHalHbiInit");
    return VpHalHbiCmd(deviceId, HBI_CMD_CONFIGURE_INT + HBI_PINCONFIG);
} /* VpHalHbiInit() */
/**
 * VpHalHbiCmd(): Sends a command word over the HBI, with no data words.
 *
 *  Accepts a uint16 HBI command which is little-endian or big-endian,
 * depending on the host architecture.  Command words on the HBI bus are always
 * big-endian. This function is responsible for byte-swapping if required.
 *
 * Params:
 * uint8 deviceId: Device Id (chip select ID)
 * uint16 cmd: the command word to send
 *
 * Returns:
 *   TRUE on success, FALSE on failure
 */
bool VpHalHbiCmd(
    VpDeviceIdType deviceId,
    uint16 cmd)
{
#if 0
    if(!GPI_Command(GET_CONNECTOR_ID(deviceId), GET_CHIP_SELECT(deviceId),
            GET_TRANSFER_SIZE(deviceId), WRITE_COMMAND, cmd, NULL, 0)) {
        return TRUE;    /* success */
    } else {
        return FALSE;    /* Failure */
    }
#else
	skip_param_warnings(deviceId);
	skip_param_warnings(cmd);
	tz_dbg(LOG_LEVEL_HAL, "VpHalHbiCmd");
    return TRUE;
#endif
} /* VpHalHbiCmdWr() */
/**
 * VpHalHbiWrite(): Sends a command word and up to 256 data words over the HBI.
 *
 *  Accepts a uint16 HBI command which is little-endian or big-endian, depending
 * on the host architecture.  Command words on the HBI bus are always big-
 * endian.  This function is responsible for byte-swapping the command word, if
 * required.
 *
 *  Accepts an array of uint16 data words.  No byte-swapping is necessary on
 * data words in this function.  Instead, the HBI bus can be configured in
 * VpHalHbiInit() to match the endianness of the host platform.
 *
 * Params:
 *   uint8 deviceId: Device Id (chip select ID)
 *   uint16 cmd: the command word to send
 *   uint8 numwords: the number of data words to send, minus 1
 *   uint16p data: the data itself; use data = (uint16p)0 to send
 *      zeroes for all data words
 *
 * Returns:
 *   TRUE on success, FALSE on failure
 */
bool VpHalHbiWrite(
    VpDeviceIdType deviceId,
    uint16 cmd,
    uint8 numwords,
    uint16p data)
{
#if 0
    int numBytes = (numwords + 1)* 2;
    /* Convert from uint 16 to uint8 as necessary, including endianess. */
    uint8 *srcPtr = (uint8 *)data;
    if(data == (uint16p)0) {
        srcPtr = tempBuf;
    }
    if(!GPI_Command(GET_CONNECTOR_ID(deviceId), GET_CHIP_SELECT(deviceId),
            GET_TRANSFER_SIZE(deviceId), WRITE_COMMAND, cmd, srcPtr, numBytes)) {
        return TRUE;    /* success */
    } else {
        return FALSE;    /* Failure */
    }
#else
	skip_param_warnings(deviceId);
	skip_param_warnings(cmd);
	skip_param_warnings(numwords);
	skip_param_warnings(data);
	tz_dbg(LOG_LEVEL_HAL, "VpHalHbiWrite");
    return TRUE;
#endif
} /* VpHalHbiWrite() */
/**
 * VpHalHbiRead(): Sends a command, and receives up to 256 data words over the
 * HBI.
 *
 *  Accepts a uint16 HBI command which is little-endian or big-endian, depending
 * on the host architecture.  Command words on the HBI bus are always big-
 * endian.  This function is responsible for byte-swapping the command word, if
 * required.
 *
 * Retrieves an array of uint16 data words.  No byte-swapping is necessary on
 * data words in this function.  Instead, the HBI bus can be configured in
 * VpHalHbiInit() to match the endianness of the host platform.
 *
 * Params:
 *   uint8 deviceId: Device Id (chip select ID)
 *   uint8 numwords: the number of words to receive, minus 1
 *   uint16p data: where to put them
 *
 * Returns:
 *   TRUE on success, FALSE on failure
 */
bool VpHalHbiRead(
    VpDeviceIdType deviceId,
    uint16 cmd,
    uint8 numwords,
    uint16p data)
{
#if 0
    int numBytes = (numwords + 1)* 2;
    /* Convert from uint 16 to uint8 as necessary, including endianess. */
    /* Perform read command to the device */
     if(!GPI_Command(GET_CONNECTOR_ID(deviceId), GET_CHIP_SELECT(deviceId),
          GET_TRANSFER_SIZE(deviceId), READ_COMMAND, cmd,
           (unsigned char *)data, numBytes)) {
         return TRUE;    /* success */
     } else {
       return FALSE;    /* Failure */
     }
#else
	skip_param_warnings(deviceId);
	skip_param_warnings(cmd);
	skip_param_warnings(numwords);
	skip_param_warnings(data);
	tz_dbg(LOG_LEVEL_HAL, "VpHalHbiRead");
    return TRUE;
#endif
} /* VpHalHbiRead() */

/**
 * VpHalHbiBootWr():
 *
 *  This is used by the VpBootLoad() function to send the boot stream to the
 * VCP.  This function is separate from VpHalHbiWrite(), for the following
 * reasons:
 *
 *  1. This function does not accept a command word; only data words.
 *  2. This function accepts uint8 data, instead of uint16 data.  Be careful
 *     not to assume that this data is word-aligned in memory.
 *  3. The HBI must be configured for big-endian data words while the boot
 *     stream is being transmitted, regardless of the endianness of the host
 *     platform.  This is because the boot image is an opaque stream of HBI
 *     command words and data words.  Therefore, commands and data cannot be
 *     distinguished for separate treatment by this function.  Since HBI
 *     command words are always big-endian, data words have to be big-endian,
 *     too.  The boot stream is stored big-endian in memory, even on little-
 *     endian hosts.
 *        If VpHalHbiInit() configures the HBI for little-endian data words,
 *     then this function must temporarily change the configuration by calling
 *     VpHalHbiCmd(HBI_CMD_CONFIGURE(...)), and change it back before
 *     returning.  In such a case, this function will need to swap each pair
 *     of bytes in the boot stream before sending.
 *        Another possibility is a little-endian host architecture, with the HBI
 *     bus configured for big-endian data words.  In this case, byte-swapping
 *     has to be done in VpHalHbiWrite() or in the glue logic between the host
 *     and the VCP. In these setups, VpHalHbiBootWr() does not need to
 *     reconfigure the  HBI.
 *  4. This function takes a VpImagePtrType pointer to char data, which is a
 *     platform-dependent type defined in vp_hal.h.
 *
 * Params
 *   uint8 deviceId: Device Id (chip select ID)
 *  'length' specifies the number of 16-bit words to write to the VCP.
 *  'pBuf' points into the boot stream.
 *
 * Returns
 *  HbiHalBootWr() returns TRUE on success, FALSE on failure.
 *
 * Notes
 *  THIS FUNCTION IS NOT REENTRANT!
 */
bool VpHalHbiBootWr(
    VpDeviceIdType deviceId,
    uint8 numwords,
    VpImagePtrType data)
{
#if 0
    int numBytes = (numwords + 1)*2;
    if(!GPI_Data(GET_CONNECTOR_ID(deviceId), GET_CHIP_SELECT(deviceId),
            GET_TRANSFER_SIZE(deviceId), GPI_WR_CMD, data, numBytes)) {
        return TRUE;    /* success */
    } else {
        return FALSE;    /* Failure */
    }
#else
	skip_param_warnings(deviceId);
	skip_param_warnings(numwords);
	skip_param_warnings(data);
	tz_dbg(LOG_LEVEL_HAL, "VpHalHbiBootWr");
    return TRUE;
#endif
} /* VpHalHbiBootWr() */



extern void tz_print_bin(char *title, char *bin, int len);

/*****************************************************************************
 * HAL functions for CSLAC devices. Not necessary for VCP
 ****************************************************************************/
/**
 * VpMpiCmd()
 *  This function executes a Device MPI command through the MPI port. It
 * executes both read and write commands. The read or write operation is
 * determined by the "cmd" argument (odd = read, even = write). The caller must
 * ensure that the data array is large enough to hold the data being collected.
 * Because this command used hardware resources, this procedure is not
 * re-entrant.
 *
 * Note: For API-II to support multi-threading, this function has to write to
 * the EC register of the device to set the line being controlled, in addition
 * to the command being passed. The EC register write/read command is the same
 * for every CSLAC device and added to this function. The only exception is
 * if the calling function is accessing the EC register (read), in which case
 * the EC write cannot occur.
 *
 * This example assumes the implementation of two byte level commands:
 *
 *    MpiReadByte(VpDeviceIdType deviceId, uint8 *data);
 *    MpiWriteByte(VpDeviceIdType deviceId, uint8 data);
 *
 * Preconditions:
 *  The device must be initialized.
 *
 * Postconditions:
 *   The data pointed to by dataPtr, using the command "cmd", with length
 * "cmdLen" has been sent to the MPI bus via the chip select associated with
 * deviceId.
 */

#define SILABS_SPIDEV "/dev/spidev32766.1"
#define DEF_SPI_RATE 2048000
#define SPI_BATCH_SIZE 16

static int spidev_fd = -1;

#if 1
void VpMpiCmd(VpDeviceIdType deviceId,    /**< Chip select, connector and 3 or 4 wire
				                                 * interface for command
				                                 */
    uint8 ecVal,        /**< Value to write to the EC register */
    uint8 cmd,          /**< Command number */
    uint8 cmdLen,       /**< Number of bytes used by command (cmd) */
    uint8 *dataPtr)     /**< Pointer to the data location */
{
    uint8 isRead = (cmd & READ_COMMAND);

    static char buff_tx[1024];

    char *ptr_tx = buff_tx;
    int len_tx=0;

    int rt;

	if(spidev_fd==-1)
	{
		int fd = open(SILABS_SPIDEV, O_RDWR);
		if(fd<0)
		{
			tz_dbg(LOG_LEVEL_HAL, "open spidev fail");
			return ;
		}
		else
		{
			spidev_fd = fd;
			tz_dbg(LOG_LEVEL_HAL, "open spidev ok");
		}
	}

#define CSLAC_EC_REG_RD    0x4B   /* Same for all CSLAC devices */
#define CSLAC_EC_REG_WRT   0x4A   /* Same for all CSLAC devices */
#define CSLAC_EC_REG_LEN   0x01   /* Same for all CSLAC devices */

    VpSysEnterCritical(deviceId, VP_MPI_CRITICAL_SEC);
    
    if (CSLAC_EC_REG_RD == cmd) 
    {
    
    } 
    else 
    {
        buff_tx[0]=CSLAC_EC_REG_WRT;
        buff_tx[1]=ecVal;
        
        len_tx = 2;
        ptr_tx += len_tx;
    }
    
    *ptr_tx = cmd;
    ptr_tx++;
    len_tx++;

    if (isRead) 
    {
    	int batch_size;
		int counter=0;
		char *p=(char *)dataPtr;
		
		char buff_rx[SPI_BATCH_SIZE]={0};
		
        struct spi_ioc_transfer tr_cmd_head[] = 
        {
			{
				.tx_buf = (unsigned long)buff_tx,  
				.rx_buf = (unsigned long)NULL, 
				.len = len_tx,  
				.delay_usecs = 0,	
				.speed_hz = DEF_SPI_RATE,  
				.bits_per_word = 8,  
			},
			{
				.tx_buf = (unsigned long)NULL,  
				.rx_buf = (unsigned long)buff_rx, 
				.len = 0,  
				.delay_usecs = 0,	
				.speed_hz = DEF_SPI_RATE,  
				.bits_per_word = 8,  
			}
		};

		struct spi_ioc_transfer tr_cmd = 
		{
			.tx_buf = (unsigned long)NULL,  
			.rx_buf = (unsigned long)buff_rx, 
			.len = len_tx,  
			.delay_usecs = 0,	
			.speed_hz = DEF_SPI_RATE,  
			.bits_per_word = 8,  
		};

		if(cmdLen<=SPI_BATCH_SIZE)
			batch_size = cmdLen;
		else
			batch_size = SPI_BATCH_SIZE;

		tr_cmd_head[1].len = batch_size;
		
        //tz_print_bin("spi send data", buff_tx, len_tx);
        rt = ioctl(spidev_fd, SPI_IOC_MESSAGE(2), tr_cmd_head); 
		if (rt < 1)  
		{
			VpSysExitCritical(deviceId, VP_MPI_CRITICAL_SEC);
			tz_dbg(LOG_LEVEL_HAL, "can't send spi msg cmd=0x%02x", cmd);	
			return ;
		}

		//tz_print_bin("spi recv data", buff_rx, batch_size);
		memcpy(p, buff_rx, batch_size);
		p += batch_size;
		counter = batch_size;

		while(counter<cmdLen)
		{
			if(counter>=cmdLen)
				break;
				
			if(counter+SPI_BATCH_SIZE <= cmdLen)
				batch_size = SPI_BATCH_SIZE;
			else
				batch_size = cmdLen-counter;
				
			tr_cmd.tx_buf = (unsigned long)NULL;
			memset(buff_rx, 0, sizeof(buff_rx));
			tr_cmd.rx_buf = (unsigned long)buff_rx;
			tr_cmd.len = batch_size;

			rt = ioctl(spidev_fd, SPI_IOC_MESSAGE(1), &tr_cmd); 
			if (rt < 1) 
			{
				VpSysExitCritical(deviceId, VP_MPI_CRITICAL_SEC);
				tz_dbg(LOG_LEVEL_HAL, "can't send spi msg cmd=0x%02x", cmd);	
				return ;
			}

			//tz_print_bin("spi recv data", buff_rx, batch_size);

			memcpy(p, buff_rx, batch_size);
			p += batch_size;
			counter += batch_size;
		}
    } 
    else 
    {
        struct spi_ioc_transfer tr_cmd = 
		{
			.tx_buf = (unsigned long)NULL,  
			.rx_buf = (unsigned long)NULL, 
			.len = 0,  
			.delay_usecs = 0,	
			.speed_hz = DEF_SPI_RATE,  
			.bits_per_word = 8,  
		};
		
        memcpy(ptr_tx, dataPtr, cmdLen);
        len_tx += cmdLen;

		int batch_size;
		int counter=0;
		char *p=buff_tx;

		while(counter<len_tx)
		{
			if(counter>=len_tx)
				break;
				
			if(counter+SPI_BATCH_SIZE <= len_tx)
				batch_size = SPI_BATCH_SIZE;
			else
				batch_size = len_tx-counter;
			
			tr_cmd.tx_buf = (unsigned long)p;
			tr_cmd.rx_buf = (unsigned long)NULL;
			tr_cmd.len = batch_size;

			rt = ioctl(spidev_fd, SPI_IOC_MESSAGE(1), &tr_cmd); 

			//tz_print_bin("spi send data", p, batch_size);

			p += batch_size;
			counter += batch_size;
		}
    }
    
    VpSysExitCritical(deviceId, VP_MPI_CRITICAL_SEC);
	
    return;
} /* End VpMpiCmd */
#endif
