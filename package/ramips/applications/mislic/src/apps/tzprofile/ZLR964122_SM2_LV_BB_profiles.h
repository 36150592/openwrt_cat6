/*
 * ZLR964122_SM2_LV_BB_profiles.h --
 *
 * This header file exports the Profile data types
 *
 * Project Info --
 *   File:   P:\SysApps\HardwareApps\LineModules_VOB\miSLIC line modules\ZLR964122_Le9641_BB\software\ZLR964122_SM2_BB_profiles.vpw
 *   Type:   miSLIC- Le9641 - Buck-Boost Fixed Ringing
 *   Date:   Friday, August 01, 2014 13:11:30
 *   Device: miSLIC Le9661
 *
 *   This file was generated with Profile Wizard Version: B2.6.1
 */

#ifndef ZLR964122_SM2_LV_BB_PROFILES_H
#define ZLR964122_SM2_LV_BB_PROFILES_H

#ifdef VP_API_TYPES_H
#include "vp_api_types.h"
#else
typedef unsigned char VpProfileDataType;
#endif


/************** Device Profile **************/
extern const VpProfileDataType DEV_PROFILE_Buck_Boost[];/* Device Configuration Data - 5V 22uH 1.5A Buck-Boost */

/************** DC Profile **************/
extern const VpProfileDataType DC_FXS_miSLIC_BB_CN[];/* China DC FXS Parameters - 25mA Current Feed */

/************** AC Profile **************/
extern const VpProfileDataType AC_FXS_RF14_600R_DEF[];/* AC FXS RF14 600R Normal Coefficients (Default)  */

/************** Ringing Profile **************/
extern const VpProfileDataType RING_miSLIC_BB_China[];/* China Ringing 25Hz 60Vrms Fixed, AC Trip */

/************** Tone Profile **************/
extern const VpProfileDataType T_US_SIGNEL[];

/************** Cadence Profile **************/
extern const VpProfileDataType CR_CN[];              /* China Ringing Signal Cadence */
extern const VpProfileDataType RING_CAD_STD[];
extern const VpProfileDataType C_CONT[];
extern const VpProfileDataType C_CALL[];             /* China Tone Signal Cadence */
extern const VpProfileDataType C_BUSYING[];          /* China Tone Signal Cadence */
extern const VpProfileDataType C_EMPTY[];            /* China Tone Signal Cadence */
extern const VpProfileDataType C_DEMO[];            /* China Tone Signal Cadence */

/************** Caller ID Profile **************/
extern const VpProfileDataType CID_TYPE2_US[];
extern const VpProfileDataType CID_TYPE1_UK[];
extern const VpProfileDataType CLI_TYPE1_US[];
/************** Metering_Profile **************/

#endif /* ZLR964122_SM2_LV_BB_PROFILES_H */

