/*
 * BUCK_BOOST_profiles.h --
 *
 * This header file exports the Profile data types
 *
 * Project Info --
 *   File:   X:\trnk\applications\quickstarts\sdk_examples\ZL880_initialization\BUCK_BOOST_profiles.vpw
 *   Type:   Example Profiles for a ZL880/miSLIC Buck Boost design
 *   Date:   Friday, April 17, 2015 11:24:58
 *   Device: miSLIC Le9641
 *
 *   This file was generated with Profile Wizard Version: P2.7.0
 */

#ifndef BUCK_BOOST_PROFILES_H
#define BUCK_BOOST_PROFILES_H

#ifdef VP_API_TYPES_H
#include "vp_api_types.h"
#else
typedef unsigned char VpProfileDataType;
#endif


/************** Device Profile **************/
extern const VpProfileDataType BB_DEV_PROFILE[];     /* Device Configuration Data  */

/************** DC Profile **************/
extern const VpProfileDataType BB_DC_25MA_CC[];      /* USA DC FXS Parameters - 25mA Current Feed */

/************** AC Profile **************/
extern const VpProfileDataType BB_AC_600_FXS[];      /* AC FXS RF14 600R Normal Coefficients (Default)  */

/************** Ringing Profile **************/
extern const VpProfileDataType BB_RING_25HZ_SINE[];  /* Ringing 25Hz 50Vrms Tracking, AC Trip */

/************** Tone Profile **************/

/************** Cadence Profile **************/

/************** Caller ID Profile **************/

/************** Metering_Profile **************/

#endif /* BUCK_BOOST_PROFILES_H */

