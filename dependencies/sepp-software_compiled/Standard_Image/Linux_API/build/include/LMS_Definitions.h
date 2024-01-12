/** LMS6002 Device Library - Definitions
 *
 * \file LMS_Definitions.h
 *
 * \project    OPS-SAT Project
 * \copyright  Graz University of Technology, IKS
 * \author     Reinhard Zeif, Fabian Golser
 * \date       15. Nov. 2017
 * \version    1.0
 *
 * \brief Global source code and device definitions.
 *
 * \copyright (C) 2018 Graz University of Technology / IKS. All rights reserved.
 * Your use of TU Graz design tools, logic functions and other 
 * software and tools and any output files any of the foregoing 
 * (including device programming or simulation files), and any associated 
 * documentation or information are expressly subject to the terms and 
 * conditions of the applicable license agreement, including, without 
 * limitation, that your use is for the sole purpose of programming the 
 * OPS-SAT processing platform.
 *
 */

#ifndef LMS_DEFINITIONS_H
#define LMS_DEFINITIONS_H

/*------------------ RX and TX usage definition -------------------------------*/
//#define 	LMS_USE_TRANSMITTER			// remove definition if transmitter is not used
#define 	LMS_USE_RECEIVER			// remove definition if receiver is not used

/*------------------ register write mode selection ----------------------------*/
#define 	REGMAP_WR						1			// write without verify
#define		REGMAP_WR_VERIFY				2			// verify written value

// define the register map write mode for the SPI bus
#define		REGMAP_WR_MODE					(REGMAP_WR)
/*-----------------------------------------------------------------------------*/

// calibration: LMS::calibrationGeneralDCCalibrationProcedure()
#define 	CALIBRATION_MAX_CNT 			10

// Default PLL Reference Frequency (external LO reference input)
#define 	DEFAULT_REFCLK_FREQ_MHZ 		40.0    // WARNING: depends on SDR PCB version. v1.0.0 = 26.0, v1.0.1 = 40.0

/*--------------- Define Check Lock and Calibration Done Method ---------------*/
//#define 		CHECK_LOCK_V1
#define 		CHECK_LOCK_V2
/*-----------------------------------------------------------------------------*/


#endif // LMS_DEFINITIONS_H
