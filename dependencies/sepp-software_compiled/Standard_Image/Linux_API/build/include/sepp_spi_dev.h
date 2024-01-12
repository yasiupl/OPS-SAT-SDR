/** OPS-SAT SEPP C++ API CORE Library - SEPP SPI Device
 *
 * \file SEPP_SPI_DEV.h
 *
 * \brief Generic Class for SEPP SPI-DEV library.
 * The class can be used to access SPI Slave devices connected to the SEPP SoC.
 *
 * \project  		OPS-SAT SEPP C++ API Library
 *
 * \organisation 	IKS, TU Graz, Inffeldgasse 12/I, 8010 Graz
 * \author 			Reinhard Zeif, MSc
 * \date			30.Nov.2020
 *
 * \version     0.1  first draft						Oct.2015
 * \version     0.2	 EM test    						Nov.2017
 * \version     0.3	 Further testing and bugfixing		Dec.2017
 * \version		0.4  MTU definition added				30.Nov.2020
 *
 *
 * \copyright (C) 2018-2020 Graz University of Technology / IKS. All rights reserved.
 * Your use of TU Graz design tools, logic functions and other 
 * software and tools and any output files any of the foregoing 
 * (including device programming or simulation files), and any associated 
 * documentation or information are expressly subject to the terms and 
 * conditions of the applicable license agreement, including, without 
 * limitation, that your use is for the sole purpose of programming the 
 * OPS-SAT processing platform.
 *
 */
 
#ifndef SEPP_SPI_DEV_h
#define SEPP_SPI_DEV_h

#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/spi/spidev.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <stdint.h>

/* ------------------ General Definitions ---------------------- */
#define			SEPP_SPI_DEV_MTU			2000		//!< SPI maximum transfer unit size

/* ------------------------------------------------------------- */


/** SEPP C++ API CORE Library - SEPP SPI Device Class
* 	\brief Contains members for device address and general information. 
*		Set and Get functions for device address, description and interface type.
* 	Methods to generate correct message sequences for write and read commands.
*/
class SEPP_SPI_DEV
{
	private:
		uint32_t mode_;		//!< SPI device mode (3 or 4-wire) used for communication
		uint8_t bits_per_word_;     //!< number of bits per byte
		uint32_t data_rate_;    //!< Data Rate used for the communication
		uint16_t delay_us_;

		std::string description_;		//!< Description of the device, e.g. device name and purpose
		std::string interface_type_;		//!< Hardware interface type used for communication, e.g. I2C or SPI
		unsigned int interface_status_;   //!< status of interface: 1 = connected, 0 = disconnected

		int SPI_dev_;       // SPI device file handler

		/* private Methods for communication with device */
		
	public:
		SEPP_SPI_DEV();
		SEPP_SPI_DEV(std::string spi_dev_name, unsigned int mode, unsigned int data_rate_in_Hz);
		virtual ~SEPP_SPI_DEV();

		void Connect(std::string spi_dev_name, unsigned int mode, unsigned int data_rate_in_Hz);
		void Connect(std::string spi_dev_name, unsigned int mode, unsigned int data_rate_in_Hz, unsigned int bits_per_word,  unsigned int delay_us);
		void Disconnect();
		
		/* public Methods for Configuration */
		//unsigned int GetDeviceAddress(void);
		// void SetDeviceAddress(unsigned int SPI_address);
		std::string GetInterfaceType();
		unsigned int GetInterfaceStatus();
		std::string GetDeviceDescription();
		void SetDeviceDescription(std::string description);
		void SetMode(unsigned int mode);
		unsigned int GetMode(void);
		void PrintInfo();

		/* public Methods for Device Register Read and Write */
		void transfer(unsigned char *tx_buf, unsigned char *rx_buf, unsigned int buf_length);

};

/** SEPP C++ API CORE Library - SEPP SPI Device Exception
*/
class SEPP_SPI_DEV_Exception : public std::exception
{
  private:
	std::string e_;
	
  public:
	SEPP_SPI_DEV_Exception(std::string e) : e_(e) {}
	~SEPP_SPI_DEV_Exception() throw () {}
	const char* what() const throw() { return e_.c_str(); }
};

#endif
