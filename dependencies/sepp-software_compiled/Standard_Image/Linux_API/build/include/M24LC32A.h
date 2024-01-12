/** M24LC32A IC Device Library
 * 
 * \file M24LC32A.h
 *
 * M24LC32A RAM memory 4k x 8bit library for I2C device communication.
 * The library contains the device register map and commands for register access.
 * 
 * \project    OPS-SAT Project
 *
 * \copyright  Graz University of Technology
 * \date       15. Apr. 2019
 * \author     Reinhard Zeif, MSc
 *
 * Version History:
 * \version 0.1		Initial release
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
 
#ifndef M24LC32A_H_
#define M24LC32A_H_

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


/*************** PRE-DEFINED LIMIT DEFINITIONS *********************/
#define			M24LC32A_ADDRESS_MAX				4095			//!< maximum address value
#define			M24LC32A_PAGE_SIZE					32				//!< number of bytes per page

/********************************************************************/


/** M24LC32A IC device class
* 	\brief Contains members for device address and general information. 
*	  Set and Get functions for device address, description and interface type.
* 	Methods to generate correct message sequences for write and read commands.
*/
class M24LC32A : public SEPP_I2C_DEV
{
	public:
		M24LC32A();
		M24LC32A(unsigned int i2c_address, std::string i2c_device_name);
		virtual ~M24LC32A();

 		/* public Methods for Device Configuration */

		void Connect(unsigned int i2c_address, std::string i2c_device_name);
		void Disconnect();
		void PrintDeviceInfo();                               			//!< Print device summary / info

		/* ------- read and write access methods ------*/
		void Write_Byte(unsigned short address, unsigned char byte_value);   	//!< Write one byte to memory
		void Write_Page(unsigned short address, unsigned char *byte_values);	//!< Write 32 byte page to memory
		void Read_Bytes(unsigned short address, unsigned char *byte_values, unsigned short number_of_bytes); 		//!< Read n bytes from memory
		
};

/** M24LC32A IC exception class
* 	\brief This exception type is returned in case of an exception in the M24LC32A device class.
*/
class M24LC32A_Exception : public std::exception
{
  private:
	std::string e_;
	
  public:
	M24LC32A_Exception(std::string e) : e_(e) {}
	~M24LC32A_Exception() throw () {}
	const char* what() const throw() { return e_.c_str(); }
};

#endif
