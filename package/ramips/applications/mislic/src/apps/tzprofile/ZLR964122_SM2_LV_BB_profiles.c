/*
 * ZLR964122_SM2_LV_BB_profiles.c --
 *
 * This file contains profile data in byte format
 *
 * Project Info --
 *   File:   P:\SysApps\HardwareApps\LineModules_VOB\miSLIC line modules\ZLR964122_Le9641_BB\software\ZLR964122_SM2_BB_profiles.vpw
 *   Type:   miSLIC- Le9641 - Buck-Boost Fixed Ringing
 *   Date:   Friday, August 01, 2014 13:11:30
 *   Device: miSLIC Le9661
 *
 *   This file was generated with Profile Wizard Version: B2.6.1
 */

#include "ZLR964122_SM2_LV_BB_profiles.h"

/************** Device Profile **************/

/* Device Configuration Data - 5V 22uH 1.5A Buck-Boost */
const VpProfileDataType DEV_PROFILE_Buck_Boost[] =
{
/* Profile Header ---------------------------------------------------------- */
   0x0D, 0xFF, 0x00,       /* Device Profile for Le9661 device */
   0x28,                   /* Profile length = 40 + 4 = 44 bytes */
   0x04,                   /* Profile format version = 4 */
   0x14,                   /* MPI section length = 20 bytes */
/* Raw MPI Section --------------------------------------------------------- */
//   0x46, 0x0A,             /* PCLK = 8.192 MHz; INTM = CMOS-compatible */
//   0x44, 0x06,             /* PCM Clock Slot = 6 TX / 0 RX; XE = Neg. */
   0x46, 0x02,             /* PCLK = 2.048 MHz; INTM = CMOS-compatible */
   0x44, 0x00,             /* PCM Clock Slot = 0 TX / 0 RX; XE = Neg. */
   0x5E, 0x14, 0x00,       /* Device Mode Register */
   0xF6, 0x95, 0x00, 0x94, /* Switching Regulator Timing Params */
         0x40, 0xE4, 0x40,
   0xE4, 0x04, 0x8F, 0x0A, /* Switching Regulator Params */
   0xE6, 0x60,             /* Switching Regulator Control */
/* Formatted Parameters Section -------------------------------------------- */
   0x00,                   /* IO21 Mode: Digital */
                           /* IO22 Mode: Digital */
   0x70,                   /* Dial pulse correction: 7 ms */
                           /* Switcher Configuration =
                              BuckBoost 22uH 1.5A miSLIC (5V in, 100 V out) */
   0x00, 0x00,             /* Over-current event threshold = Off */
   0x01,                   /* Leading edge blanking window = 81 ns */
   0x30, 0x14, 0x30, 0x14, /* FreeRun Timing Parameters */
         0x30, 0x14,
   0xFF,
   0x95, 0x00,             /* Low-Power Timing Parameters */
   0x62, 0x62,             /* Switcher Limit Voltages 98 V, 98 V */
   0x04,                   /* Charge pump disabled */
                           /* Charge Pump Overload Protection: Disabled */
   0x3C                    /* Switcher Input 12 V */
};

/************** DC Profile **************/

/* China DC FXS Parameters - 25mA Current Feed */
const VpProfileDataType DC_FXS_miSLIC_BB_CN[] =
{
/* Profile Header ---------------------------------------------------------- */
   0x0D, 0x01, 0x00,       /* DC Profile for Le9661 device */
   0x0C,                   /* Profile length = 12 + 4 = 16 bytes */
   0x02,                   /* Profile format version = 2 */
   0x03,                   /* MPI section length = 3 bytes */
/* Raw MPI Section --------------------------------------------------------- */
   0xC6, 0x92, 0x27,       /* DC Feed Parameters: ir_overhead = 100 ohm; */
                           /* VOC = 48 V; LI = 50 ohm; VAS = 8.78 V; ILA = 25 mA */
                           /* Maximum Voice Amplitude = 2.93 V */
/* Formatted Parameters Section -------------------------------------------- */
   0x1A,                   /* Switch Hook Threshold = 9 mA */
                           /* Ground-Key Threshold = 18 mA */
   0x84,                   /* Switch Hook Debounce = 8 ms */
                           /* Ground-Key Debounce = 16 ms */
   0x58,                   /* Low-Power Switch Hook Hysteresis = 2 V */
                           /* Ground-Key Hysteresis = 6 mA */
                           /* Switch Hook Hysteresis = 2 mA */
   0x40,                   /* Low-Power Switch Hook Threshold = 18 V */
   0x04,                   /* Floor Voltage = -25 V */
   0x00,                   /* R_OSP = 0 ohms */
   0x07                    /* R_ISP = 7 ohms */
};

/************** AC Profile **************/

/* AC FXS RF14 600R Normal Coefficients (Default)  */
const VpProfileDataType AC_FXS_RF14_600R_DEF[] =
{
  /* AC Profile */
 0xA4, 0x00, 0xF4, 0x4C, 0x01, 0x49, 0xCA, 0xF5, 0x98, 0xAA, 0x7B, 0xAB,
 0x2C, 0xA3, 0x25, 0xA5, 0x24, 0xB2, 0x3D, 0x9A, 0x2A, 0xAA, 0xA6, 0x9F,
 0x01, 0x8A, 0x1D, 0x01, 0xA3, 0xA0, 0x2E, 0xB2, 0xB2, 0xBA, 0xAC, 0xA2,
 0xA6, 0xCB, 0x3B, 0x45, 0x88, 0x2A, 0x20, 0x3C, 0xBC, 0x4E, 0xA6, 0x2B,
 0xA5, 0x2B, 0x3E, 0xBA, 0x8F, 0x82, 0xA8, 0x71, 0x80, 0xA9, 0xF0, 0x50,
 0x00, 0x86, 0x2A, 0x42, 0xA1, 0xCB, 0x1B, 0xA3, 0xA8, 0xFB, 0x87, 0xAA,
 0xFB, 0x9F, 0xA9, 0xF0, 0x96, 0x2E, 0x01, 0x00
};

/************** Ringing Profile **************/

/* China Ringing 25Hz 60Vrms Fixed, AC Trip */
const VpProfileDataType RING_miSLIC_BB_China[] =
{
/* Profile Header ---------------------------------------------------------- */
   0x0D, 0x04, 0x00,       /* Ringing Profile for Le9661 device */
   0x12,                   /* Profile length = 18 + 4 = 22 bytes */
   0x01,                   /* Profile format version = 1 */
   0x0C,                   /* MPI section length = 12 bytes */
/* Raw MPI Section --------------------------------------------------------- */
   0xC0, 0x08, 0x00, 0x00, /* Ringing Generator Parameters: */
         0x00, 0x44, 0x46, /* 24.9 Hz Sine; 1.41 CF; 84.80 Vpk; 0.00 V bias */
         0x4D, 0x00, 0x00, /* Ring trip cycles = 1; RTHALFCYC = 0 */
         0x00, 0x00,
/* Formatted Parameters Section -------------------------------------------- */
   0xA6,                   /* Ring Trip Method = AC; Threshold = 19.0 mA */
   0x00,                   /* Ringing Current Limit = 50 mA */
   0x50,                   /* Fixed; Max Ringing Supply = 85 V */
   0x00                    /* Balanced; Ring Cadence Control Disabled */
};

/************** Tone Profile **************/

/************** Cadence Profile **************/

/* China Ringing Signal Cadence */
const VpProfileDataType CR_CN[] =
{
  /* Cadence Profile */
 0x00, 0x08, 0x00, 0x1E, 0x00, 0x00, 0x00, 0x1A, 0x01, 0x07, 0x20, 0xC8,
 0x01, 0x05, 0x20, 0x28, 0x02, 0x01, 0x22, 0xF8, 0x01, 0x07, 0x20, 0xC8,
 0x01, 0x00, 0x20, 0x28, 0x01, 0x05, 0x22, 0xF8, 0x46, 0x00
};

/* Standard Ringing Cadence (2 sec on 4 sec off) */
const VpProfileDataType _RING_CAD_STD[] = 
{
  /* Cadence Profile */
 0x00, 0x08, 0x01, 0x16, 0x00, 0x00, 0x00, 0x12, 0x01, 0x07, 0x21, 0x90,
 0x01, 0x05, 0x02, 0x01, 0x23, 0x20, 0x01, 0x07, 0x21, 0x90, 0x01, 0x05,
 0x44, 0x00
};

/* Standard Ringing Cadence (1 sec on 3 sec off) */
const VpProfileDataType __RING_CAD_STD[] =
{
  /* Cadence Profile */
 0x00, 0x08, 0x00, 0x16, 0x00, 0x00, 0x00, 0x12, 0x01, 0x07, 0x20, 0xC8,
 0x01, 0x05, 0x02, 0x01, 0x22, 0x58, 0x01, 0x07, 0x20, 0xC8, 0x01, 0x05,
 0x44, 0x00
};


/* Standard Ringing Cadence (1 sec on 4 sec off) */
const VpProfileDataType RING_CAD_STD[] =
{
  /* Cadence Profile */
 0x00, 0x08, 0x00, 0x16, 0x00, 0x00, 0x00, 0x12, 0x01, 0x07, 0x20, 0xC8,
 0x01, 0x05, 0x02, 0x01, 0x23, 0x20, 0x01, 0x07, 0x20, 0xC8, 0x01, 0x05,
 0x44, 0x00
};



/************** Cadence Profile **************/

/* Continuous ON Cadence */
const VpProfileDataType C_CONT[] =
{
  /* Cadence Profile */
 0x00, 0x08, 0x00, 0x12, 0x00, 0x00, 0x00, 0x0E, 0x01, 0x07, 0x20, 0xC8,
 0x01, 0x05, 0x20, 0x64, 0x02, 0x01, 0x22, 0xBC, 0x40, 0x00
};

/************** Caller ID Profile **************/

/* US Caller ID Type II */
const VpProfileDataType CID_TYPE2_US[] =
{
  /* Space=2100Hz, Mark=1300Hz, Amp=-13.50dBm */
  /* Caller ID Profile */
  0x00, 0x05, 0x00, 0x4C, 0x00, 0x00,
  0x09, 0xD4, /* MPI Length and Command */
  0x16, 0x66, 0x12, 0xD8, 0x0D, 0xDD, 0x12, 0xD8, /* MPI Data */
  0x00, 0x01, /* Checksum Computed by device/API */
  0x00, 0x3C, /* Length of Elements Data */
  0x00, 0x02, /* Mute the far end */
  0x00, 0x04, /* Alert Tone */
  /* Call Waiting Tone for Type II Caller ID */
  /* 439.82 Hz, -13.00 dBm0, 0.00 Hz, -100.00 dBm0 */
  0x09, 0xD4, 0x04, 0xB1, 0x13, 0xF7, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x05, 0x01, 0x2C, /* Alert Tone Part 2 - Tone+Cadence = 300ms */
  0x00, 0x06, 0x00, 0x0A, /* Silence Interval for 10ms */
  0x00, 0x04, /* Alert Tone */
  /* Caller ID Alert Tone */
  /* 2129.88 Hz, -13.00 dBm0, 2749.88 Hz, -13.00 dBm0 */
  0x09, 0xD4, 0x16, 0xB8, 0x13, 0xF7, 0x1D, 0x55, 0x13, 0xF7,
  0x00, 0x05, 0x00, 0x50, /* Alert Tone Part 2 - Tone+Cadence = 80ms */
  0x00, 0x08, 0x00, 0xA0, 0x00, 0xD0, 0x00, 0x00, /* Detect Tone A | D, Timeout = 160ms */
  0x00, 0x06, 0x00, 0x64, /* Silence Interval for 100ms */
  0x00, 0x0A, 0x00, 0x46, /* Mark Signal for 70ms */
  0x00, 0x0B, /* Message Data (FSK Format) */
  0x00, 0x03, /* Unmute the far end */
  0x00, 0x0D  /* End of Transmission */
};

/* UK Caller ID Type I */
const VpProfileDataType CID_TYPE1_UK[] =
{
  /* Space=2100Hz, Mark=1300Hz, Amp=-7.00dBm */
  /* Caller ID Profile */
  0x00, 0x05, 0x00, 0x42, 0x00, 0x00,
  0x09, 0xD4, /* MPI Length and Command */
  0x16, 0x66, 0x27, 0xD4, 0x0D, 0xDD, 0x27, 0xD4, /* MPI Data */
  0x00, 0x01, /* Checksum Computed by device/API */
  0x00, 0x32, /* Length of Elements Data */
  0x00, 0x06, 0x00, 0x32, /* Silence Interval for 50ms */
  0x00, 0x01, /* Line Reversal */
  0x00, 0x06, 0x00, 0x64, /* Silence Interval for 100ms */
  0x00, 0x04, /* Alert Tone */
  /* Caller ID Alert Tone */
  /* 2129.88 Hz, -13.00 dBm0, 2749.88 Hz, -13.00 dBm0 */
  0x09, 0xD4, 0x16, 0xB8, 0x13, 0xF7, 0x1D, 0x55, 0x13, 0xF7,
  0x00, 0x05, 0x00, 0x64, /* Alert Tone Part 2 - Tone+Cadence = 100ms */
  0x00, 0x07, 0x00, 0x64, 0x00, 0x14, /* Silence Interval for 100ms, Masked-hook Interval for 20ms */
  0x00, 0x09, 0x00, 0x64, /* Channel Seizure for 100ms */
  0x00, 0x0A, 0x00, 0x32, /* Mark Signal for 50ms */
  0x00, 0x0B, /* Message Data (FSK Format) */
  0x00, 0x06, 0x00, 0xC8, /* Silence Interval for 200ms */
  0x00, 0x01, /* Line Reversal */
  0x00, 0x0D  /* End of Transmission */
};

/* US Caller ID (Type 1 - On-Hook) */
const VpProfileDataType CLI_TYPE1_US[] =
{
  /* Space=2200Hz, Mark=1200Hz, Amp=-7.00dBm */
  /* Caller ID Profile */
  0x00, 0x05, 0x00, 0x20, 0x00, 0x00,
  0x09, 0xD4, /* MPI Length and Command */
  0x17, 0x77, 0x27, 0xD4, 0x0C, 0xCC, 0x27, 0xD4, /* MPI Data */
  0x00, 0x01, /* Checksum Computed by device/API */
  0x00, 0x10, /* Length of Elements Data */
  0x00, 0x06, 0x02, 0x08, /* Silence Interval for 520ms */
  0x00, 0x09, 0x00, 0xFA, /* Channel Seizure for 250ms */
  0x00, 0x0A, 0x00, 0x96, /* Mark Signal for 150ms */
  0x00, 0x0B, /* Message Data (FSK Format) */
  0x00, 0x0D  /* End of Transmission */
};


const VpProfileDataType T_US_SIGNEL[] =
{


   0x0D, 0x02, 0x00,       /* Tone Profile for Le9641 device */
   0x0F,                   /* Profile length = 15 + 4 = 19 bytes */
   0x02,                   /* Profile format version = 2 */
   0x0C,                   /* MPI section length = 12 bytes */
   0xD2, 0x00, 0x00, 0x00, /* Signal Generator A, B, Bias Parameters */
         0x04, 0xCD, 0x0E,
         0x22, 0x00, 0x00,
         0x13, 0xF7,
   0x01                    /* FM_EN = 0; WB = 0; Active Generators: A */
};
const VpProfileDataType C_CALL[] =
{
  /* Cadence Profile */
 0x00, 0x03, 0x00, 0x20, 0x00, 0x00, 0x00, 0x1C, 0x00, 0x81, 0x20, 0xC8,
 0x40, 0x1E, 0x00, 0x80, 0x20, 0x78, 0x00, 0x81, 0x20, 0x78, 0x43, 0x06,
 0x00, 0x80, 0x20, 0x46, 0x00, 0x81, 0x20, 0x46, 0x00, 0x80, 0x48, 0x00

};
const VpProfileDataType C_BUSYING[] =
{
 0x00, 0x03, 0x00, 0x0E, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x81, 0x20, 0x46,
 0x00, 0x80, 0x20, 0x46, 0x40, 0x00
};
const VpProfileDataType C_EMPTY[] =
{
 0x00, 0x03, 0x00, 0x26, 0x00, 0x00, 0x00, 0x22, 0x00, 0x81, 0x20, 0x14,
 0x00, 0x80, 0x20, 0x14, 0x00, 0x81, 0x20, 0x14, 0x00, 0x80, 0x20, 0x14,
 0x00, 0x81, 0x20, 0x14, 0x00, 0x80, 0x20, 0x14, 0x00, 0x81, 0x20, 0x50,
 0x00, 0x80, 0x20, 0x50, 0x40, 0x00
};


const VpProfileDataType C_DEMO[] =
{
  /* Cadence Profile */
  0x00, 0x03, 0x00, 0x0E, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x81, 0x20, 0x46,
  0x00, 0x80, 0x20, 0x46, 0x40, 0x1C

};


/************** Metering_Profile **************/

/* end of file ZLR964122_SM2_LV_BB_profiles.c */
