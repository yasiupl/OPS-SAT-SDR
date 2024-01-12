/** TMP112 IC Device Library
 * 
 * \file TMP112.h
 *
 * TMP112 temperature sensor library for I2C device communication.
 * The library contains the device register map and commands for register access.
 * 
 * \project    OPS-SAT Project
 *
 * \copyright  Graz University of Technology
 * \date       15. Apr. 2019
 * \author     Reinhard Zeif, MSc
 *
 * Version History:
 * \version 1.0		Release with all functionalities
 * \version 1.1		Bugfix: Measure temperature requires byte order flipping and shift
 *
 * \copyright (C) 2015-2019 Graz University of Technology / IKS. All rights reserved.
 * Your use of TU Graz design tools, logic functions and other 
 * software and tools and any output files any of the foregoing 
 * (including device programming or simulation files), and any associated 
 * documentation or information are expressly subject to the terms and 
 * conditions of the applicable license agreement, including, without 
 * limitation, that your use is for the sole purpose of programming the 
 * OPS-SAT processing platform.
 *
 **/
 
#ifndef TMP112_H_
#define TMP112_H_

#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <errno.h>
#include <sys/ioctl.h>

#include "sepp_i2c_dev.h"


#define			TMP112_MAX_AVG_SAMPLES				64			//!< maximum number of allowed averaging samples
#define			TMP112_RESOLUTION					0.0625f			//!< resolution in degC
#define			TMP112_TMIN_12B						-55.0f				//!< 12-bit temperature low limit
#define			TMP112_TMAX_12B						127.9375f		//!< 12-bit temperature high limit
#define			TMP112_TMIN_13B						-55.0f				//!< 13-bit temperature low limit
#define			TMP112_TMAX_13B						155.0f				//!< 13-bit temperature high limit

/* NOTE: Order of word offsets [byte 2 | byte 1] !!! -> Byte 2 is MSB, Byte 1 is LSB */

/* TEMPERATURE Register - Read */
#define         TMP112_REG_TEMP					0x00		//!< Temperature register, Measured Temperature Value (2 Byte)
#define         TMP112_MASK_TEMP_RES			0x0100		//!< bit 0, byte 2 - Temperature data least significant bit: 0 = for 12-bit mode, 1 = for 13-bit mode
/* CONFIG Register - Read/Write */
#define         TMP112_REG_CONFIG				0x01		//!< Configuration register, configure the device (2 Byte)
#define         TMP112_MASK_CONFIG_OS			0x0080		//!< bit 7, byte 1 - Trigger One-Shot Temperature Conversion (OS).When the device is in shutdown mode, writing a 1 to the OS bit begins a single temperature conversion. During the conversion, the OS bit reads 0. The device returns to the SHUTDOWN state at the completion of the single conversion.0 = nothing (default), 1 = Trigger One-Shot Conversion
#define         TMP112_MASK_CONFIG_R			0x0060		//!< bit 6:5, byte 1 - Converter Resolution Bit 1 (R1) and Bit 0 (R0).The converter resolution bits, R1 and R0, are read-only bits. The TMP112 converter resolution is set on start up to 0x11(default) which sets the temperature register to a 12 bit-resolution.
#define         TMP112_MASK_CONFIG_F			0x0018		//!< bit 4:3, byte 1 - Fault Queue Bit 1 (F1) and Bit 0 (F0). A fault condition exists when the measured temperature exceeds the user-defined limits set in the THIGH and TLOW registers. Additionally, the number of fault conditions required to generate an alert may be programmed using the fault queue. The fault queue is provided to prevent a false alert as a result of environmental noise. The fault queue requires consecutive fault measurements in order to trigger the alert function.(F1,F0) 0x00 = 1 Consecutive Faults required for Alert (default),  0x01 = 2 Consecutive Faults required, 0x10 = 4 Consecutive Faults required, 0x11 = 6 consequitive faults required
#define         TMP112_MASK_CONFIG_POL			0x0004		//!< bit 2, byte 1 - Polarity (POL)  of the ALERT pin output. If POL bit is set to 0 (default), the ALERT pin becomes active low . When the POL bit is set to 1, the ALERT pin becomes active high and the state of the ALERT pin is inverted.0 = active low Alert (default), 1 = active high Alert
#define         TMP112_MASK_CONFIG_TM			0x0002		//!< bit 1, byte 1 - Thermostat Mode bit. Indicates to the device whether to operate in Comperator Mode  or Interrupt Mode. For more information on interrupt and comperator mode see TLOW and THIGH registers.In Comparator mode (TM = 0), the Alert pin is activated when the temperature equals or exceeds the value in the THIGH register and it remains active until the temperature falls below the value in the TLOW register. In Interrupt mode (TM = 1), the Alert pin is activated when the temperature exceeds THIGH or goes below TLOW registers. The Alert pin is cleared when the host controller reads the temperature register.0 = Comparator Mode (default), 1 = Interrupt Mode
#define         TMP112_MASK_CONFIG_SD			0x0001		//!< bit 0, byte 1 - Enable Shutdown Mode (SD). Shuts down internal circuit for power reduction except serial interface. When SD=0 the device maintains a continous conversion state.0 = disabled (default), 1 = enabled
#define         TMP112_MASK_CONFIG_CR			0xC000		//!< bit 7:6, byte 2 - Continuous Conversion Rate Bit 1 (CR1) and Bit 0 (CR0). The default mode of the TMP112 device is continuos conversion mode. During continuous-conversion mode, the ADC performs continuos temperature conversions and stores each results to the temperature register, overwriting the result from the previous conversion. The conversion rate bits, CR1 and CR0, configure the TMP112 device for conversion rates of 0.25 Hz, 1 Hz, 4 Hz, or 8 Hz. The default rate is 4 Hz. The TMP112 device has a typical conversion time of 26 ms. To achieve different conversion rates, the TMP112 device makes a conversion and then powers down and waits for the appropriate delay set by CR1 and CR0.(CR1,CR0) = 0x00 - 0.25Hz, 0x01 - 1Hz, 0x02 - 4Hz (default), 0x03 - 8Hz
#define         TMP112_MASK_CONFIG_AL			0x2000		//!< bit 5, byte 2 - Alert Bit (AL). Reading the AL bit provides information about the comparator mode status. The state of the POL bit inverts the polarity of data returned from the AL bit. When the POL bit equals 0, the AL bit reads as 1 until the temperature equals or exceeds THIGH for the programmed number of consecutive faults, causing the AL bit to read as 0. The AL bit continues to read as 0 until the temperature falls below TLOW for the programmed number of consecutive faults, when it again reads as 1. The status of the TM bit does not affect the status of the AL bit.
#define         TMP112_MASK_CONFIG_EM			0x1000		//!< bit 4, byte 2 - Enable Extended Mode Bit (EM). In normal mode the TEMP, TLOW and THIGH registers use a 12-bit value. In extended mode a 13-bit value is used.0 = normal mode operation (default), 1 = extended mode operation.
/* TLOW Register - Read/Write */
#define         TMP112_REG_TLOW					0x02		//!< TLOW register, configure the temperature LOW limit (2 Byte)
#define         TMP112_MASK_TLOW_RES			0x0100		//!< bit 0, byte 2 - Low temperature limit least significant bit0 = for 12-bit mode, 1 = for 13-bit mode
/* THIGH Register - Read/Write */
#define         TMP112_REG_THIGH				0x03		//!< THIGH register, configure the temperature HIGH limit (2 Byte)
#define         TMP112_MASK_THIGH_RES			0x0100		//!< bit 0, byte 2 - Low temperature limit least significant bit0 = for 12-bit mode, 1 = for 13-bit mode


/* Register Parameter Positions (bit numbers) */
/* LSB value is used for multi-bit parameters (for shift operations)*/
#define			TMP112_POS_CONFIG_OS			7		//!< OS Parameter Bit offset
#define         TMP112_POS_CONFIG_R				5		//!< R Parameter Bit offset
#define         TMP112_POS_CONFIG_F				3		//!< F Parameter Bit offset
#define         TMP112_POS_CONFIG_POL			2		//!< POL Parameter Bit offset
#define         TMP112_POS_CONFIG_TM			1		//!< TM Parameter Bit offset
#define         TMP112_POS_CONFIG_SD			0		//!< SD Parameter Bit offset
#define         TMP112_POS_CONFIG_CR			14 		//!< CR Parameter Bit offset
#define         TMP112_POS_CONFIG_AL			13 		//!< AL Parameter Bit offset
#define         TMP112_POS_CONFIG_EM			12 		//!< EM Parameter Bit offset




/*************** PRE-DEFINED LIMIT DEFINITIONS *********************/
#define         THIGH_LIMIT_HOT           0x500     //!< THIGH Limit = 80 degC
#define         TLOW_LIMIT_HOT            0x4B0     //!< THIGH Limit = 75 degC
#define         THIGH_LIMIT_COLD          0xDD0     //!< THIGH Limit = -35 degC
#define         TLOW_LIMIT_COLD           0xD80     //!< THIGH Limit = -40 degC
/********************************************************************/

/** TMP175 configuration data structure
* \brief Contains all device configuration parameters
*/
typedef struct tmp112_config_parameters
{
  unsigned int OS; 
  unsigned int R; 
  unsigned int F; 
  unsigned int POL; 
  unsigned int TM; 
  unsigned int SD;
  unsigned int CR;
  unsigned int AL;
  unsigned int EM;
} s_TMP112_CONFIG; 


/** TMP112 IC device class
* 	\brief Contains members for device address and general information. 
*	  Set and Get functions for device address, description and interface type.
* 	Methods to generate correct message sequences for write and read commands.
*/
class TMP112 : public SEPP_I2C_DEV
{
	public:
		TMP112();
		TMP112(unsigned int i2c_address, std::string i2c_device_name);
		virtual ~TMP112();

 		/* public Methods for Device Configuration */

		void Connect(unsigned int i2c_address, std::string i2c_device_name);
		void Disconnect();
		void PrintDeviceInfo();                               			//!< Print device summary / info

    /* ----- Custom user functions for device ----- */
		 
		void Set_Configuration_Register(s_TMP112_CONFIG config);    //!< Set config Register (16-bit)
		void Set_Thermostat_Mode(unsigned int TM);       						//!< Set Thermostat Mode (0,1), defines behaviour of TLOW, THIGH and Alert Signal
		void Set_Polarity(unsigned int POL);       									//!< Set Polarity (0,1), defines if active low or high alert signal is used
		void Set_Conversion_Rate(unsigned int CR);        					//!< Set conversion rate (0,1,2,3)
		void Set_Alert_Limits(float TLOW_degC, float THIGH_degC);               //!< Set TLOW and THIGH alert limit value

		s_TMP112_CONFIG Get_Configuration_Register(void);   				//!< Get config Register (16-bit)
		unsigned int Get_Thermostat_Mode();      										//!< Get Thermostat Mode (0,1), defines behaviour of TLOW, THIGH and Alert Signal
		unsigned int Get_Polarity();       													//!< Get Polarity (0,1), defines if active low or high alert signal is used
		void Get_Alert_Limits(float &TLOW_degC, float &THIGH_degC);              //!< Get TLOW and THIGH alert limit value
		unsigned int Get_Conversion_Rate();       									//!< Get conversion rate (0,1,2,3)

		unsigned int Check_Alert();																	//!< get Alert flag
  
		float Measure_Temperature(void);   													//!< Get Temperature Value (single sample)
		void Calc_Statistics_from_Samples(unsigned int *samples, unsigned int nr_of_samples, float &mean, float &stddev);

};

/** TMP112 IC exception class
* 	\brief This exception type is returned in case of an exception in the TMP112 device class.
*/
class TMP112_Exception : public std::exception
{
  private:
	std::string e_;
	
  public:
	TMP112_Exception(std::string e) : e_(e) {}
	~TMP112_Exception() throw () {}
	const char* what() const throw() { return e_.c_str(); }
};

#endif
