/** OPS-SAT SEPP C++ API CORE Library - SEPP I2C Device
 *
 * \file SEPP_I2C_DEV.h
 *
 * \brief Generic Class for SEPP I2C-DEV library.
 * The class can be used to access I2C Slave devices connected to the SEPP SoC.
 *
 * \project  			OPS-SAT SEPP C++ API Library
 *
 * \Organisation		IKS, TU Graz, Inffeldgasse 12/I, 8010 Graz
 * \Author				Reinhard Zeif, MSc
 * \Date 				17.Nov.2020
 *
 * \version		0.1		first draft													Oct.2015
 * \version		0.2		EM test    													Nov.2017
 * \version		0.3		Further testing and bugfixing								Dec. 2017
 * \version		0.4		Retry functionality implemented so that exceptions
 * 						thrown only after a predefined number of retries			15.Apr.2019
 * \version		1.0		Retry delay and count updated								17.Nov.2020
 *
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
 */
 
#ifndef SEPP_I2C_DEV_h
#define SEPP_I2C_DEV_h

#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "i2c-dev.h"   // use local lib for compile instead of <linux/i2c-dev.h>
#include <errno.h>
#include <sys/ioctl.h>


#include "SEPP_API_Debug.h"

/* ---------------------- I2C automatic retry ---------------------- */
#define		SEPP_I2C_DEV_RETRY_COUNT					5			// number of retries before throwing an exception
#define 	SEPP_I2C_DEV_RETRY_DELAY_USEC				200000		// Delay between retries = 200000 us = 200 ms


/* ----------------------------------------------------------------- */

/** SEPP C++ API CORE Library - SEPP I2C Device Class
* 	\brief Contains members for device address and general information. 
*		Set and Get functions for device address, description and interface type.
* 	Methods to generate correct message sequences for write and read commands.
*/
class SEPP_I2C_DEV
{
	private:
		unsigned int address_;		//!< I2C device address used for communication
		std::string description_;		//!< Description of the device, e.g. device name and purpose
		std::string interface_type_;		//!< Hardware interface type used for communication, e.g. I2C or SPI
		unsigned int interface_status_;   //!< status of interface: 1 = connected, 0 = disconnected
		std::string dev_name_;    //! <linux device name as string: e.g. "i2c-0" or "i2c-1"

		int i2c_dev_;       // i2c device file handler

		/* private Methods for communication with device */
		
	public:
		SEPP_I2C_DEV();
		SEPP_I2C_DEV(unsigned int i2c_address, std::string sepp_i2c_dev_name);
		virtual ~SEPP_I2C_DEV();

		void Connect(unsigned int i2c_address, std::string sepp_i2c_dev_name);
		void Disconnect();
		
		/* public Methods for Configuration */
		unsigned int GetDeviceAddress(void);
		void SetDeviceAddress(unsigned int i2c_address);
		std::string GetInterfaceType();
		unsigned int GetInterfaceStatus();
		std::string GetDeviceDescription();
		void SetDeviceDescription(std::string description);
		void PrintInfo();

	protected:
		/* public Methods for Device Register Read and Write*/
		unsigned int SMBusReadRegisterByte(unsigned int reg_addr);
		unsigned int SMBusReadRegisterWord(unsigned int reg_addr);
		void SMBusWriteRegisterByte(unsigned int reg_addr, unsigned int reg_value);
		void SMBusWriteRegisterWord(unsigned int reg_addr, unsigned int reg_value);
		unsigned int SMBusReadByte(void);
		void SMBusWriteByte(unsigned int data_byte);
		unsigned int SMBusBlockRead(unsigned int reg_addr, unsigned char *buffer);

		void I2CWriteByteArray(unsigned char *data_bytes, int length);
		void I2CReadByteArray(unsigned char *data_bytes, int length);
};

/** SEPP C++ API CORE Library - SEPP I2C Device Exception
*/
class SEPP_I2C_DEV_Exception : public std::exception
{
  private:
	std::string e_;
	
  public:
	SEPP_I2C_DEV_Exception(std::string e) : e_(e) {}
	~SEPP_I2C_DEV_Exception() throw () {}
	const char* what() const throw() { return e_.c_str(); }
};

#endif
