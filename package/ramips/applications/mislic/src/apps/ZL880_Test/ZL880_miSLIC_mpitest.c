#include "vp_api.h"
#include "vp886_registers.h"

#define DEVICE_ID 0x0400
//#undef SINGLE_CHANNEL
#define SINGLE_CHANNEL

VpStatusType
Vp886QuickMpiTest(
    VpDeviceIdType deviceId);

VpStatusType
Vp886QuickMpiTest1Ch(
    VpDeviceIdType deviceId);

int main(void)
{
    VpSysInit();

    #ifdef SINGLE_CHANNEL
    Vp886QuickMpiTest1Ch(0);
    #else
    Vp886QuickMpiTest(0);
    #endif

    return 0;
}


/** Vp886QuickMpiTest()
  Performs some basic tests of the MPI and HAL implementation to make sure that
  we can properly communicate with the device before starting.
*/
VpStatusType
Vp886QuickMpiTest(
    VpDeviceIdType deviceId)
{
    VpStatusType status = VP_STATUS_SUCCESS;
    uint8 writeBuffer[255];
    uint8 readBuffer[16];
    uint8 i;

    printf("Vp886QuickMpiTest(): deviceId 0x%X\n", deviceId);

    VpMemSet(writeBuffer, VP886_R_NOOP_WRT, sizeof(writeBuffer));

    /* Perform at least 16 NO_OPS to make sure the MPI command/data state
       machine is ready to accept a command */
    VpMpiCmd(deviceId, VP886_EC_GLOBAL, VP886_R_NOOP_WRT, 16, writeBuffer);

    /* Reset the device */
    VpMpiCmd(deviceId, VP886_EC_GLOBAL, VP886_R_HWRESET_WRT, 0, VP_NULL);
    VpSysWait(20);

    /* Read the revision and product code */
    VpMpiCmd(deviceId, VP886_EC_GLOBAL, VP886_R_RCNPCN_RD, VP886_R_RCNPCN_LEN, &readBuffer[0]);
    printf("Revision code 0x%02X, product code 0x%02X\n", readBuffer[0], readBuffer[1]);

    /*
        Test 1:
        Write to single-byte global register
    */
    VpMemSet(writeBuffer, 0xD6, sizeof(writeBuffer));
    VpMemSet(readBuffer, 0x94, sizeof(readBuffer));

    /* Write 0xE5 to the clock slots register.  This should read back as 0x65
       later because the top bit always reads as 0.  We have to make sure to
       use a value with the 0x40 bit set, because that bit will always read
       back as 1 in ZSI mode. */
    writeBuffer[1] = 0xE5;
    VpMpiCmdWrapper(deviceId, VP886_EC_GLOBAL, VP886_R_CLKSLOTS_WRT, VP886_R_CLKSLOTS_LEN, &writeBuffer[1]);
    
    /* Make sure the write buffer was not corrupted */
    if (writeBuffer[0] != 0xD6 || writeBuffer[1] != 0xE5 || writeBuffer[2] != 0xD6) {
        printf("Vp886QuickMpiTest: Test 1 failed. Write buffer corrupted: 0x%02X 0x%02X 0x%02X\n",
            writeBuffer[0], writeBuffer[1], writeBuffer[2]);
        status = VP_STATUS_ERR_SPI;
    }
    
    /* Read back the register */
    VpMpiCmdWrapper(deviceId, VP886_EC_GLOBAL, VP886_R_CLKSLOTS_RD, VP886_R_CLKSLOTS_LEN, &readBuffer[1]);
    
    /* Make sure there was no overflow into the data around byte 1 */
    if (readBuffer[0] != 0x94 || readBuffer[2] != 0x94) {
        printf("Vp886QuickMpiTest: Test 1 failed. Read buffer overflow: 0x%02X 0x%02X 0x%02X\n",
            readBuffer[0], readBuffer[1], readBuffer[2]);
        status = VP_STATUS_ERR_SPI;
    }
    
    /* The high bit of this register always returns 0, so what we read should
       not be exactly what we wrote.  0xE5 becomes 0x65 */
    if (readBuffer[1] != 0x65) {
        printf("Vp886QuickMpiTest: Test 1 failed. Read value incorrect: 0x%02X, expected 0x65\n",
            readBuffer[1]);
        status = VP_STATUS_ERR_SPI;
    }
    
    if (status != VP_STATUS_SUCCESS) {
        return status;
    }
    
    /*
        Test 2:
        Write to multi-byte channel registers
    */
    VpMemSet(writeBuffer, 0xD6, sizeof(writeBuffer));
    VpMemSet(readBuffer, 0x94, sizeof(readBuffer));
    
    /* Write to the cadence register for channel 0 */
    writeBuffer[1] = 0xAB;
    writeBuffer[2] = 0xCD;
    writeBuffer[3] = 0xEF;
    writeBuffer[4] = 0x01;
    VpMpiCmdWrapper(deviceId, VP886_EC_1, VP886_R_CADENCE_WRT, VP886_R_CADENCE_LEN, &writeBuffer[1]);
    
    /* Write to the cadence register for channel 1 */
    writeBuffer[6] = 0x12;
    writeBuffer[7] = 0x34;
    writeBuffer[8] = 0x56;
    writeBuffer[9] = 0x78;
    VpMpiCmdWrapper(deviceId, VP886_EC_2, VP886_R_CADENCE_WRT, VP886_R_CADENCE_LEN, &writeBuffer[6]);
    
    /* Make sure the write buffer was not corrupted */
    if (writeBuffer[0] != 0xD6 || writeBuffer[1] != 0xAB || writeBuffer[2] != 0xCD ||
        writeBuffer[3] != 0xEF || writeBuffer[4] != 0x01 || writeBuffer[5] != 0xD6 ||
        writeBuffer[6] != 0x12 || writeBuffer[7] != 0x34 || writeBuffer[8] != 0x56 ||
        writeBuffer[9] != 0x78 || writeBuffer[10] != 0xD6)
    {
        printf("Vp886QuickMpiTest: Test 2 failed. Write buffer corrupted: 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X\n",
            writeBuffer[0], writeBuffer[1], writeBuffer[2], writeBuffer[3], writeBuffer[4], writeBuffer[5],
            writeBuffer[6], writeBuffer[7], writeBuffer[8], writeBuffer[9], writeBuffer[10]);
        status = VP_STATUS_ERR_SPI;
    }
    
    /* Read back both registers */
    VpMpiCmdWrapper(deviceId, VP886_EC_1, VP886_R_CADENCE_RD, VP886_R_CADENCE_LEN, &readBuffer[1]);
    VpMpiCmdWrapper(deviceId, VP886_EC_2, VP886_R_CADENCE_RD, VP886_R_CADENCE_LEN, &readBuffer[6]);

    /* Make sure there was no overflow into the surrounding data */
    if (readBuffer[0] != 0x94 || readBuffer[5] != 0x94 || readBuffer[10] != 0x94) {
        printf("Vp886QuickMpiTest: Test 2 failed. Read buffer overflow: 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X\n",
            readBuffer[0], readBuffer[1], readBuffer[2], readBuffer[3], readBuffer[4], readBuffer[5],
            readBuffer[6], readBuffer[7], readBuffer[8], readBuffer[9], readBuffer[10]);
        status = VP_STATUS_ERR_SPI;
    }
    
    /* Make sure the ch0 data read back correctly.
       Some bits of the register will always read as 0, so we should read
       0x03CD0701 instead of 0xABCDEF01 */
    if (readBuffer[1] != 0x03 || readBuffer[2] != 0xCD || readBuffer[3] != 0x07 || readBuffer[4] != 0x01) {
        printf("Vp886QuickMpiTest: Test 2 failed. Ch0 read value incorrect: 0x%02X 0x%02X 0x%02X 0x%02X\n",
            readBuffer[1], readBuffer[2], readBuffer[3], readBuffer[4]);
        printf("Vp886QuickMpiTest: expected 0x03 0xCD 0x07 0x01\n");
        status = VP_STATUS_ERR_SPI;
    }
    
    /* Make sure the ch1 data read back correctly.
       Some bits of the register will always read as 0, so we should read
       0x02340678 instead of 0x12345678 */
    if (readBuffer[6] != 0x02 || readBuffer[7] != 0x34 || readBuffer[8] != 0x06 || readBuffer[9] != 0x78) {
        printf("Vp886QuickMpiTest: Test 2 failed. Ch1 read value incorrect: 0x%02X 0x%02X 0x%02X 0x%02X\n",
            readBuffer[6], readBuffer[7], readBuffer[8], readBuffer[9]);
        printf("Vp886QuickMpiTest: expected 0x02 0x34 0x06 0x78\n");
        status = VP_STATUS_ERR_SPI;
    }

    if (status != VP_STATUS_SUCCESS) {
        return status;
    }

    /*
        Test 3:
        Test large write length
    */
    VpMemSet(writeBuffer, 0xD6, sizeof(writeBuffer));
    VpMemSet(readBuffer, 0x94, sizeof(readBuffer));

    /* Build a large write buffer where the first command is a write to the ch0
       cadence register, followed by repeated writes to the ch1 cadence register. */

    /* The first data is written to ch0.  The EC and command are built in to the
       VpMpiCmdWrapper() function call later */
    writeBuffer[1] = 0xAA;
    writeBuffer[2] = 0x55;
    writeBuffer[3] = 0x43;
    writeBuffer[4] = 0x21;
    /* Switch to the second channel */
    writeBuffer[5] = VP886_R_EC_WRT;
    writeBuffer[6] = VP886_R_EC_EC2;
    /* Fill up the middle of the buffer with writes to the ch1 cadence register.
       These should be overwritten by the final values. */
    i = 7;
    while (i < 240) {
        writeBuffer[i] = VP886_R_CADENCE_WRT;
        writeBuffer[i+1] = i;
        writeBuffer[i+2] = i;
        writeBuffer[i+3] = i;
        writeBuffer[i+4] = i;
        i += 5;
    }
    /* Final write to the ch1 register */
    writeBuffer[i++] = VP886_R_CADENCE_WRT;
    writeBuffer[i++] = 0x99;
    writeBuffer[i++] = 0xBB;
    writeBuffer[i++] = 0x56;
    writeBuffer[i] = 0x78;
    /* Send the whole buffer */
    VpMpiCmdWrapper(deviceId, VP886_EC_1, VP886_R_CADENCE_WRT, i, &writeBuffer[1]);

    /* Read back both register values */
    VpMpiCmdWrapper(deviceId, VP886_EC_1, VP886_R_CADENCE_RD, VP886_R_CADENCE_LEN, &readBuffer[1]);
    VpMpiCmdWrapper(deviceId, VP886_EC_2, VP886_R_CADENCE_RD, VP886_R_CADENCE_LEN, &readBuffer[6]);

    /* Make sure that both the first and last writes of the large buffer read
       back correctly.
       Some bits of the register read back as 0, so AA554321 becomes 02550321
       and 99BB5678 becomes 01BB0678 */
    if (readBuffer[1] != 0x02 || readBuffer[2] != 0x55 || readBuffer[3] != 0x03 || readBuffer[4] != 0x21) {
        printf("Vp886QuickMpiTest: Test 3 failed. Ch0 read value incorrect: 0x%02X 0x%02X 0x%02X 0x%02X,\n",
            readBuffer[1], readBuffer[2], readBuffer[3], readBuffer[4]);
        printf("Vp886QuickMpiTest: expected 0x02 0x55 0x03 0x21\n");
        status = VP_STATUS_ERR_SPI;
    }
    if (readBuffer[6] != 0x01 || readBuffer[7] != 0xBB || readBuffer[8] != 0x06 || readBuffer[9] != 0x78) {
        printf("Vp886QuickMpiTest: Test 3 failed. Ch1 read value incorrect: 0x%02X 0x%02X 0x%02X 0x%02X,\n",
            readBuffer[6], readBuffer[7], readBuffer[8], readBuffer[9]);
        printf("Vp886QuickMpiTest: expected 0x01 0xBB 0x06 0x78\n");
        status = VP_STATUS_ERR_SPI;
    }

    if (status != VP_STATUS_SUCCESS) {
        return status;
    }

    /*
        Test 4:
        Make sure we can write and read 0x00 and 0xFF bytes properly.  We have
        had a customer issue where the low level driver did not handle these
        properly.
    */
    VpMemSet(writeBuffer, 0xD6, sizeof(writeBuffer));
    VpMemSet(readBuffer, 0x94, sizeof(readBuffer));
    
    /* Write to the cadence register for channel 0 */
    writeBuffer[1] = 0x00;
    writeBuffer[2] = 0xFF;
    writeBuffer[3] = 0x00;
    writeBuffer[4] = 0xFF;
    VpMpiCmdWrapper(deviceId, VP886_EC_1, VP886_R_CADENCE_WRT, VP886_R_CADENCE_LEN, &writeBuffer[1]);
    
    /* Make sure the write buffer was not corrupted */
    if (writeBuffer[0] != 0xD6 || writeBuffer[1] != 0x00 || writeBuffer[2] != 0xFF ||
        writeBuffer[3] != 0x00 || writeBuffer[4] != 0xFF || writeBuffer[5] != 0xD6)
    {
        printf("Vp886QuickMpiTest: Test 4 failed. Write buffer corrupted: 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X\n",
            writeBuffer[0], writeBuffer[1], writeBuffer[2], writeBuffer[3], writeBuffer[4], writeBuffer[5]);
        status = VP_STATUS_ERR_SPI;
    }
    
    /* Read back the register */
    VpMpiCmdWrapper(deviceId, VP886_EC_1, VP886_R_CADENCE_RD, VP886_R_CADENCE_LEN, &readBuffer[1]);

    /* Make sure there was no overflow into the surrounding data */
    if (readBuffer[0] != 0x94 || readBuffer[5] != 0x94) {
        printf("Vp886QuickMpiTest: Test 4 failed. Read buffer overflow: 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X\n",
            readBuffer[0], readBuffer[1], readBuffer[2], readBuffer[3], readBuffer[4], readBuffer[5]);
        status = VP_STATUS_ERR_SPI;
    }
    
    /* Make sure the data was read back correctly. */
    if (readBuffer[1] != 0x00 || readBuffer[2] != 0xFF || readBuffer[3] != 0x00 || readBuffer[4] != 0xFF) {
        printf("Vp886QuickMpiTest: Test 4 failed. Read value incorrect: 0x%02X 0x%02X 0x%02X 0x%02X\n",
            readBuffer[1], readBuffer[2], readBuffer[3], readBuffer[4]);
        printf("Vp886QuickMpiTest: expected 0x00 0xFF 0x00 0xFF\n");
        status = VP_STATUS_ERR_SPI;
    }
    
    if (status != VP_STATUS_SUCCESS) {
        return status;
    }

    printf("Vp886QuickMpiTest: Tests passed\n\n");

    return status;
}


/** Vp886QuickMpiTest1Ch()
  Single-channel version of Vp886QuickMpiTest().
  Performs some basic tests of the MPI and HAL implementation to make sure that
  we can properly communicate with the device before starting.
*/
VpStatusType
Vp886QuickMpiTest1Ch(
    VpDeviceIdType deviceId)
{
    VpStatusType status = VP_STATUS_SUCCESS;
    uint8 writeBuffer[255];
    uint8 readBuffer[16];
    uint8 i;

    printf("Vp886QuickMpiTest1Ch(): deviceId 0x%X\n", deviceId);

    VpMemSet(writeBuffer, VP886_R_NOOP_WRT, sizeof(writeBuffer));

    /* Perform at least 16 NO_OPS to make sure the MPI command/data state
       machine is ready to accept a command */
    VpMpiCmd(deviceId, VP886_EC_GLOBAL, VP886_R_NOOP_WRT, 16, writeBuffer);

    /* Reset the device */
    VpMpiCmd(deviceId, VP886_EC_GLOBAL, VP886_R_HWRESET_WRT, 0, VP_NULL);
    VpSysWait(20);

    /* Read the revision and product code */
    VpMpiCmd(deviceId, VP886_EC_GLOBAL, VP886_R_RCNPCN_RD, VP886_R_RCNPCN_LEN, &readBuffer[0]);
    printf("Revision code 0x%02X, product code 0x%02X\n", readBuffer[0], readBuffer[1]);

    /*
        Test 1:
        Write to single-byte global register
    */
    VpMemSet(writeBuffer, 0xD6, sizeof(writeBuffer));
    VpMemSet(readBuffer, 0x94, sizeof(readBuffer));

    /* Write 0xE5 to the clock slots register.  This should read back as 0x65
       later because the top bit always reads as 0.  We have to make sure to
       use a value with the 0x40 bit set, because that bit will always read
       back as 1 in ZSI mode. */
    writeBuffer[1] = 0xE5;
    VpMpiCmdWrapper(deviceId, VP886_EC_GLOBAL, VP886_R_CLKSLOTS_WRT, VP886_R_CLKSLOTS_LEN, &writeBuffer[1]);
    
    /* Make sure the write buffer was not corrupted */
    if (writeBuffer[0] != 0xD6 || writeBuffer[1] != 0xE5 || writeBuffer[2] != 0xD6) {
        printf("Vp886QuickMpiTest1Ch: Test 1 failed. Write buffer corrupted: 0x%02X 0x%02X 0x%02X\n",
            writeBuffer[0], writeBuffer[1], writeBuffer[2]);
        status = VP_STATUS_ERR_SPI;
    }
    
    /* Read back the register */
    VpMpiCmdWrapper(deviceId, VP886_EC_GLOBAL, VP886_R_CLKSLOTS_RD, VP886_R_CLKSLOTS_LEN, &readBuffer[1]);
    
    /* Make sure there was no overflow into the data around byte 1 */
    if (readBuffer[0] != 0x94 || readBuffer[2] != 0x94) {
        printf("Vp886QuickMpiTest1Ch: Test 1 failed. Read buffer overflow: 0x%02X 0x%02X 0x%02X\n",
            readBuffer[0], readBuffer[1], readBuffer[2]);
        status = VP_STATUS_ERR_SPI;
    }
    
    /* The high bit of this register always returns 0, so what we read should
       not be exactly what we wrote.  0xE5 becomes 0x65 */
    if (readBuffer[1] != 0x65) {
        printf("Vp886QuickMpiTest1Ch: Test 1 failed. Read value incorrect: 0x%02X, expected 0x65\n",
            readBuffer[1]);
        status = VP_STATUS_ERR_SPI;
    }
    
    if (status != VP_STATUS_SUCCESS) {
        return status;
    }
    
    /*
        Test 2:
        Write to multi-byte channel register
    */
    VpMemSet(writeBuffer, 0xD6, sizeof(writeBuffer));
    VpMemSet(readBuffer, 0x94, sizeof(readBuffer));
    
    /* Write to the cadence register */
    writeBuffer[1] = 0xAB;
    writeBuffer[2] = 0xCD;
    writeBuffer[3] = 0xEF;
    writeBuffer[4] = 0x01;
    VpMpiCmdWrapper(deviceId, VP886_EC_1, VP886_R_CADENCE_WRT, VP886_R_CADENCE_LEN, &writeBuffer[1]);
    
    /* Make sure the write buffer was not corrupted */
    if (writeBuffer[0] != 0xD6 || writeBuffer[1] != 0xAB || writeBuffer[2] != 0xCD ||
        writeBuffer[3] != 0xEF || writeBuffer[4] != 0x01 || writeBuffer[5] != 0xD6)
    {
        printf("Vp886QuickMpiTest1Ch: Test 2 failed. Write buffer corrupted: 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X\n",
            writeBuffer[0], writeBuffer[1], writeBuffer[2], writeBuffer[3], writeBuffer[4], writeBuffer[5]);
        status = VP_STATUS_ERR_SPI;
    }
    
    /* Read back the register */
    VpMpiCmdWrapper(deviceId, VP886_EC_1, VP886_R_CADENCE_RD, VP886_R_CADENCE_LEN, &readBuffer[1]);

    /* Make sure there was no overflow into the surrounding data */
    if (readBuffer[0] != 0x94 || readBuffer[5] != 0x94 || readBuffer[10] != 0x94) {
        printf("Vp886QuickMpiTest1Ch: Test 2 failed. Read buffer overflow: 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X\n",
            readBuffer[0], readBuffer[1], readBuffer[2], readBuffer[3], readBuffer[4], readBuffer[5]);
        status = VP_STATUS_ERR_SPI;
    }
    
    /* Make sure the data read back correctly.
       Some bits of the register will always read as 0, so we should read
       0x03CD0701 instead of 0xABCDEF01 */
    if (readBuffer[1] != 0x03 || readBuffer[2] != 0xCD || readBuffer[3] != 0x07 || readBuffer[4] != 0x01) {
        printf("Vp886QuickMpiTest1Ch: Test 2 failed. Read value incorrect: 0x%02X 0x%02X 0x%02X 0x%02X\n",
            readBuffer[1], readBuffer[2], readBuffer[3], readBuffer[4]);
        printf("Vp886QuickMpiTest1Ch: expected 0x03 0xCD 0x07 0x01\n");
        status = VP_STATUS_ERR_SPI;
    }
    
    if (status != VP_STATUS_SUCCESS) {
        return status;
    }

    /*
        Test 3:
        Test large write length
    */
    VpMemSet(writeBuffer, 0xD6, sizeof(writeBuffer));
    VpMemSet(readBuffer, 0x94, sizeof(readBuffer));

    /* Build a large write buffer where the first command is a write to the clock
       slot register, followed by repeated writes to the cadence register. */
    writeBuffer[1] = 0xDA;
    /* Switch to the channel EC */
    writeBuffer[2] = VP886_R_EC_WRT;
    writeBuffer[3] = VP886_R_EC_EC1;
    /* Fill up the middle of the buffer with writes to the cadence register.
       These should be overwritten by the final values. */
    i = 4;
    while (i < 240) {
        writeBuffer[i] = VP886_R_CADENCE_WRT;
        writeBuffer[i+1] = i;
        writeBuffer[i+2] = i;
        writeBuffer[i+3] = i;
        writeBuffer[i+4] = i;
        i += 5;
    }
    /* Final write to the cadence register */
    writeBuffer[i++] = VP886_R_CADENCE_WRT;
    writeBuffer[i++] = 0x99;
    writeBuffer[i++] = 0xBB;
    writeBuffer[i++] = 0x56;
    writeBuffer[i] = 0x78;
    /* Send the whole buffer */
    VpMpiCmdWrapper(deviceId, VP886_EC_GLOBAL, VP886_R_CLKSLOTS_WRT, i, &writeBuffer[1]);

    /* Read back both register values */
    VpMpiCmdWrapper(deviceId, VP886_EC_GLOBAL, VP886_R_CLKSLOTS_RD, VP886_R_CLKSLOTS_LEN, &readBuffer[1]);
    VpMpiCmdWrapper(deviceId, VP886_EC_1, VP886_R_CADENCE_RD, VP886_R_CADENCE_LEN, &readBuffer[2]);

    /* Make sure that both the first and last writes of the large buffer read
       back correctly.
       Some bits of the registers read back as 0, so DA becomes 5A
       and 99BB5678 becomes 01BB0678 */
    if (readBuffer[1] != 0x5A) {
        printf("Vp886QuickMpiTest1Ch: Test 3 failed. First read value incorrect: 0x%02X, expected 0x5A\n",
            readBuffer[1]);
        status = VP_STATUS_ERR_SPI;
    }
    if (readBuffer[2] != 0x01 || readBuffer[3] != 0xBB || readBuffer[4] != 0x06 || readBuffer[5] != 0x78) {
        printf("Vp886QuickMpiTest1Ch: Test 3 failed. Second read value incorrect: 0x%02X 0x%02X 0x%02X 0x%02X,\n",
            readBuffer[2], readBuffer[3], readBuffer[4], readBuffer[5]);
        printf("Vp886QuickMpiTest1Ch: expected 0x01 0xBB 0x06 0x78\n");
        status = VP_STATUS_ERR_SPI;
    }

    if (status != VP_STATUS_SUCCESS) {
        return status;
    }

    /*
        Test 4:
        Make sure we can write and read 0x00 and 0xFF bytes properly.  We have
        had a customer issue where the low level driver did not handle these
        properly.
    */
    VpMemSet(writeBuffer, 0xD6, sizeof(writeBuffer));
    VpMemSet(readBuffer, 0x94, sizeof(readBuffer));
    
    /* Write to the cadence register */
    writeBuffer[1] = 0x00;
    writeBuffer[2] = 0xFF;
    writeBuffer[3] = 0x00;
    writeBuffer[4] = 0xFF;
    VpMpiCmdWrapper(deviceId, VP886_EC_1, VP886_R_CADENCE_WRT, VP886_R_CADENCE_LEN, &writeBuffer[1]);
    
    /* Make sure the write buffer was not corrupted */
    if (writeBuffer[0] != 0xD6 || writeBuffer[1] != 0x00 || writeBuffer[2] != 0xFF ||
        writeBuffer[3] != 0x00 || writeBuffer[4] != 0xFF || writeBuffer[5] != 0xD6)
    {
        printf("Vp886QuickMpiTest: Test 4 failed. Write buffer corrupted: 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X\n",
            writeBuffer[0], writeBuffer[1], writeBuffer[2], writeBuffer[3], writeBuffer[4], writeBuffer[5]);
        status = VP_STATUS_ERR_SPI;
    }
    
    /* Read back the register */
    VpMpiCmdWrapper(deviceId, VP886_EC_1, VP886_R_CADENCE_RD, VP886_R_CADENCE_LEN, &readBuffer[1]);

    /* Make sure there was no overflow into the surrounding data */
    if (readBuffer[0] != 0x94 || readBuffer[5] != 0x94) {
        printf("Vp886QuickMpiTest: Test 4 failed. Read buffer overflow: 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X\n",
            readBuffer[0], readBuffer[1], readBuffer[2], readBuffer[3], readBuffer[4], readBuffer[5]);
        status = VP_STATUS_ERR_SPI;
    }
    
    /* Make sure the data was read back correctly. */
    if (readBuffer[1] != 0x00 || readBuffer[2] != 0xFF || readBuffer[3] != 0x00 || readBuffer[4] != 0xFF) {
        printf("Vp886QuickMpiTest: Test 4 failed. Read value incorrect: 0x%02X 0x%02X 0x%02X 0x%02X\n",
            readBuffer[1], readBuffer[2], readBuffer[3], readBuffer[4]);
        printf("Vp886QuickMpiTest: expected 0x00 0xFF 0x00 0xFF\n");
        status = VP_STATUS_ERR_SPI;
    }
    
    if (status != VP_STATUS_SUCCESS) {
        return status;
    }

    printf("Vp886QuickMpiTest1Ch: Tests passed\n\n");

    return status;
}

