/** LTC2309 IC Device Library
 * 
 * \file LTC2309.h
 *
 * LTC2309 8-bit bus expander library for I2C device communication.
 * The library contains the device register map and commands for register access.
 * 
 * \project    OPS-SAT Project
 *
 * \copyright  Graz University of Technology
 * \date       15. June 2016
 * \author     Reinhard Zeif, MSc
 * \version    1.0
 *
 *(C) 2018 Graz University of Technology / IKS. All rights reserved.
 * Your use of TU Graz design tools, logic functions and other 
 * software and tools and any output files any of the foregoing 
 * (including device programming or simulation files), and any associated 
 * documentation or information are expressly subject to the terms and 
 * conditions of the applicable license agreement, including, without 
 * limitation, that your use is for the sole purpose of programming the 
 * OPS-SAT processing platform.
 *
 **/
 
#ifndef LTC2309_H_
#define LTC2309_H_

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


#define			LTC2309_INPUT_CHANNELS				8			// number of available input channels
#define			LTC2309_MAX_AVG_SAMPLES				64			// maximum number of allowed averaging samples

/* Register Map Definitions for LTC2309 */
/* There is only a single D_IN CONFIG byte for device configuration (bit 7:2) */
#define			LTC2309_MASK_DIN_REGISTER			  0xFC		  // the lowed 2 bits are not used for D_IN config byte
#define         LTC2309_MASK_SINGLE_NOTDIFF           0x80        // Command Bit 7: 1 = Single-Ended Conversion, 0 = Differential Conversion (default)
#define         LTC2309_MASK_ODD_NOTSIGN              0x40        // Command Bit 6: 1 = Select ODD channel (= channel 1,3,5,7), 0 = Select EVEN channel (default) (= channel 0,2,4,6)
#define         LTC2309_MASK_CHANNEL_SEL1             0x20        // Command Bit 5: Bit 1 to select the ADC input channel (see datasheet) (0 = default)
#define         LTC2309_MASK_CHANNEL_SEL0             0x10        // Command Bit 4: Bit 0 to select the ADC input channel (see datasheet) (0 = default)
#define         LTC2309_MASK_UNIPOLAR_NOTBIPOLAR      0x08        // Command Bit 3: 1 = Unipolar Conversion = 0 to 4095, 0 = Bipolar Conversion = -2048 to +2047 (default)
#define         LTC2309_MASK_SLEEP                    0x04        // Command Bit 2: 1 = Activate Sleep Mode after Conversion, 0 = Deaktivate Sleep befor Conversion (see Datasheet) (0 = default)

/** LTC2309 device class.
* 	Contains members for device address and general information. 
*	  Set and Get functions for device address, description and interface type.
* 	Methods to generate correct message sequences for write and read commands.
*/

class LTC2309 : public SEPP_I2C_DEV
{
	private:

		// SEPP_I2C_DEV *i2c_dev_;       // i2c device file handler
		unsigned int input_mode_;		// ADC input mode: 1 = single-ended input, 0 = differential input.
		unsigned int polarity_mode_;	// ADC polarity mode: 1 = unipolar, 0 = bipolar.

		void WriteConfigByte(unsigned int din_byte);			// write the DIN byte to the ADC
		void Read_ADC_Values(unsigned int din_byte, unsigned int *adc_samples, unsigned int nr_of_samples);
		void Calc_Statistics_from_Samples(unsigned int *adc_samples, unsigned int nr_of_samples, float &average, float &stddev);
		
	public:
		LTC2309();
		LTC2309(unsigned int i2c_address, std::string i2c_device_name);
		virtual ~LTC2309();

		void Connect(unsigned int i2c_address, std::string i2c_device_name);
		void Disconnect();
		
		/* public Methods for Device Configuration */
		void PrintDeviceInfo();                               //!< Print device summary / info


		/* ----- Custom user functions for device ----- */
		 
		void Set_Input_Mode(unsigned int mode);
		unsigned int Get_Input_Mode();
		void Set_Polarity_Mode(unsigned int mode);
		unsigned int Get_Polarity_Mode();

		void Get_ADC_Sample(unsigned int channel_nr, unsigned int nr_of_samples, bool gotoSleepMode, float &mean, float &stddev);

};

/** LTC2309 exception class.
* 	This exception type is returned in case of an exception in the LTC2309 device class.
*/
class LTC2309_Exception : public std::exception
{
  private:
	std::string e_;
	
  public:
	LTC2309_Exception(std::string e) : e_(e) {}
	~LTC2309_Exception() throw () {}
	const char* what() const throw() { return e_.c_str(); }
};

#endif
