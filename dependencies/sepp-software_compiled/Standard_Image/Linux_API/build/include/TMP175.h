/** TMP175 IC Device Library
 * 
 * \file TMP175.h
 *
 * TMP175 temperature sensor library for I2C device communication.
 * The library contains the device register map and commands for register access.
 * 
 * \project    OPS-SAT Project
 *
 * \copyright  Graz University of Technology
 * \date       25. May. 2020
 * \author     Reinhard Zeif, MSc
 *
 * Version History:
 * \version 	1.0		Release with all functionalities
 * \version 	1.1		Bugfix: Measure temperature requires byte order flipping and shift
 * \version 	1.2		Some remaining test output removed 
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
 
#ifndef TMP175_H_
#define TMP175_H_

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


#define			TMP175_MAX_AVG_SAMPLES				64			//!< maximum number of allowed averaging samples
#define			TMP175_RESOLUTION					0.0625f
#define			TMP175_TMIN_12B						-55.0f
#define			TMP175_TMAX_12B						127.9375f


/* TEMPERATURE Register - Read */
#define         TMP175_REG_TEMP					0x00		//!< Temperature register, Measured Temperature Value (2 Byte)
#define         TMP175_MASK_TEMP_MSB			0x8000		//!< bit 7, byte 1 - Temperature Value Bit. This is the MSB in 12-bit or 13-bit temperature capturing mode. The captured temperature value consists of both TEMP data bytes and starts at MSB bit 15 and ends at bit 3 or 2. The last bits are zero except bit 0 which is 1 for 13-bit mode.
#define         TMP175_MASK_TEMP_LSB			0x0010		//!< bit 4, byte 2 - Temperature data least significant bit0 = for 12-bit mode, 1 = for 13-bit mode
/* CONFIG Register - Read/Write */
#define         TMP175_REG_CONFIG				0x01		//!< Configuration register, configure the device (2 Byte)
#define         TMP175_MASK_CONFIG_OS			0x80		//!< bit 7, byte 1 - Trigger One-Shot Temperature Conversion (OS).When the device is in shutdown mode, writing a 1 to the OS bit begins a single temperature conversion. During the conversion, the OS bit reads 0. The device returns to the SHUTDOWN state at the completion of the single conversion.0 = nothing (default), 1 = Trigger One-Shot Conversion
#define         TMP175_MASK_CONFIG_R			0x60		//!< bit 6:5, byte 1 - Converter Resolution Bit 1 (R1) and Bit 0 (R0).The converter resolution bits, R1 and R0, are read-only bits. The TMP175 converter resolution is set on start up to 0x11(default) which sets the temperature register to a 12 bit-resolution.
#define         TMP175_MASK_CONFIG_F			0x18		//!< bit 4:3, byte 1 - Fault Queue Bit 1 (F1) and Bit 0 (F0). A fault condition exists when the measured temperature exceeds the user-defined limits set in the THIGH and TLOW registers. Additionally, the number of fault conditions required to generate an alert may be programmed using the fault queue. The fault queue is provided to prevent a false alert as a result of environmental noise. The fault queue requires consecutive fault measurements in order to trigger the alert function.(F1,F0) 0x00 = 1 Consecutive Faults required for Alert (default),  0x01 = 2 Consecutive Faults required, 0x10 = 4 Consecutive Faults required, 0x11 = 6 consequitive faults required
#define         TMP175_MASK_CONFIG_POL			0x04		//!< bit 2, byte 1 - Polarity (POL)  of the ALERT pin output. If POL bit is set to 0 (default), the ALERT pin becomes active low . When the POL bit is set to 1, the ALERT pin becomes active high and the state of the ALERT pin is inverted.0 = active low Alert (default), 1 = active high Alert
#define         TMP175_MASK_CONFIG_TM			0x02		//!< bit 1, byte 1 - Thermostat Mode bit. Indicates to the device whether to operate in Comperator Mode  or Interrupt Mode. For more information on interrupt and comperator mode see TLOW and THIGH registers.In Comparator mode (TM = 0), the Alert pin is activated when the temperature equals or exceeds the value in the THIGH register and it remains active until the temperature falls below the value in the TLOW register. In Interrupt mode (TM = 1), the Alert pin is activated when the temperature exceeds THIGH or goes below TLOW registers. The Alert pin is cleared when the host controller reads the temperature register.0 = Comparator Mode (default), 1 = Interrupt Mode
#define         TMP175_MASK_CONFIG_SD			0x01		//!< bit 0, byte 1 - Enable Shutdown Mode (SD). Shuts down internal circuit for power reduction except serial interface. When SD=0 the device maintains a continous conversion state.0 = disabled (default), 1 = enabled
/* TLOW Register - Read/Write */
#define         TMP175_REG_TLOW					0x02		//!< TLOW register, configure the temperature LOW limit (2 Byte)
#define         TMP175_MASK_TLOW_MSB			0x8000		//!< bit 7, byte 1 - Temperature LOW limit MSB.The temperature limits are stored in the TLOW and THIGH registers in the same format as the temperature result, and their values are compared to the temperature result on every conversion. The outcome of the comparison drives the behavior of the ALERT pin, which operates as a comparator output or an interrupt, and is set by the TM bit in the configuration register. The temperature limit value consists of  two data bytes and starts at MSB bit 15 and ends at bit 3 or 2. The last bits are zero except bit 0 which is 1 for 13-bit mode.
#define         TMP175_MASK_TLOW_LSB			0x0010		//!< bit 4, byte 2 - Low temperature limit least significant bit0 = for 12-bit mode, 1 = for 13-bit mode
/* THIGH Register - Read/Write */
#define         TMP175_REG_THIGH				0x03		//!< THIGH register, configure the temperature HIGH limit (2 Byte)
#define         TMP175_MASK_THIGH_MSB			0x8000		//!< bit 7, byte 1 - Temperature HIGH limit MSB.The temperature limits are stored in the TLOW and THIGH registers in the same format as the temperature result, and their values are compared to the temperature result on every conversion. The outcome of the comparison drives the behavior of the ALERT pin, which operates as a comparator output or an interrupt, and is set by the TM bit in the configuration register. The temperature limit value consists of  two data bytes and starts at MSB bit 15 and ends at bit 3 or 2. The last bits are zero except bit 0 which is 1 for 13-bit mode.
#define         TMP175_MASK_THIGH_LSB			0x0010		//!< bit 4, byte 2 - Low temperature limit least significant bit0 = for 12-bit mode, 1 = for 13-bit mode


/* Register Parameter Positions (bit numbers) */
/* LSB value is used for multi-bit parameters (for shift operations)
    Config register is only byte 1 of word (byte2, byte1)! Hence, byte 2 can be ignored */
#define			TMP175_POS_CONFIG_OS			7
#define         TMP175_POS_CONFIG_R				5
#define         TMP175_POS_CONFIG_F				3
#define         TMP175_POS_CONFIG_POL			2
#define         TMP175_POS_CONFIG_TM			1
#define         TMP175_POS_CONFIG_SD			0



/*************** PRE-DEFINED LIMIT DEFINITIONS *********************/
#define         THIGH_LIMIT_HOT           0x500     //!< THIGH Limit = 80 degC
#define         TLOW_LIMIT_HOT            0x4B0     //!< THIGH Limit = 75 degC
#define         THIGH_LIMIT_COLD          0xDD0     //!< THIGH Limit = -35 degC
#define         TLOW_LIMIT_COLD           0xD80     //!< THIGH Limit = -40 degC
/********************************************************************/

/** TMP175 configuration data structure
* Contains all device configuration parameters
*/
typedef struct
{
  unsigned int OS; 
  unsigned int R; 
  unsigned int F; 
  unsigned int POL; 
  unsigned int TM; 
  unsigned int SD; 
} s_TMP175_CONFIG; 


/** TMP175 IC device class.
* 	\brief Contains members for device address and general information. 
*	  Set and Get functions for device address, description and interface type.
* 	Methods to generate correct message sequences for write and read commands.
*/
class TMP175 : public SEPP_I2C_DEV
{
	public:
		TMP175();
		TMP175(unsigned int i2c_address, std::string i2c_device_name);
		virtual ~TMP175();

		/* public Methods for Device Configuration */
		void Connect(unsigned int i2c_address, std::string i2c_device_name);
		void Disconnect();	
		void PrintDeviceInfo();                               //!< Print device summary / info

		/* ----- Custom user functions for device ----- */
		void Set_Configuration_Register(s_TMP175_CONFIG config);    //!< Set config Register (16-bit)
		void Set_Thermostat_Mode(unsigned int TM);       //!< Set Thermostat Mode (0,1), defines behaviour of TLOW, THIGH and Alert Signal
		void Set_Polarity(unsigned int POL);       //!< Set Polarity (0,1), defines if active low or high alert signal is used
		void Set_Resolution(unsigned int R);        //!< Set ADC resolution (0,1,2,3)
		void Set_Alert_Limits(float TLOW_degC, float THIGH_degC);               //!< Set TLOW and THIGH alert limit value

		s_TMP175_CONFIG Get_Configuration_Register(void);   //!< Get config Register (16-bit)
		unsigned int Get_Thermostat_Mode(void);      //!< Get Thermostat Mode (0,1), defines behaviour of TLOW, THIGH and Alert Signal
		unsigned int Get_Polarity(void);       //!< Get Polarity (0,1), defines if active low or high alert signal is used
		unsigned int Get_Resolution(void);       //!< Get ADC Resolution (0,1,2,3)
		void Get_Alert_Limits(float &TLOW_degC, float &THIGH_degC);              //!< Get TLOW and THIGH alert limit value

 		float Measure_Temperature(void);   //!< Get Temperature Value (single sample)
		void Calc_Statistics_from_Samples(unsigned int *samples, unsigned int nr_of_samples, float &mean, float &stddev);

};

/** TMP175 IC exception class.
* 	\brief This exception type is returned in case of an exception in the TMP175 device class.
*/
class TMP175_Exception : public std::exception
{
	private:
		std::string e_;
	
	public:
		TMP175_Exception(std::string e) : e_(e) {}
		~TMP175_Exception() throw () {}
		const char* what() const throw() { return e_.c_str(); }
};

#endif
