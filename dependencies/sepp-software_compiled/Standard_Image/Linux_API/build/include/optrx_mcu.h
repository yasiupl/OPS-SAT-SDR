/** OPT-RX Microcontroller Device Library
 *
 * \file OPTRX_MCU.h
 *
 * Generic Class for Optical-RX MCU i2c commands and parameters.
 *
 * \project 		OPS-SAT 
 * \organisation	Graz University of Technology
 * \author     		Reinhard Zeif, MSc
 * \date       		05. April 2019
 *
 * \brief The class can be used to access the Optical-RX MCU connected to the SEPP. The
 * library contains the MCU device register map and commands for register access.
 *
 * Revision History:
 * \version  0.1		Release for agreed set of methods and registers (before firmware delivery)
 * \version  0.2		All methods from v0.1 removed, since MEW did deliver a incomplete firmware
 * \version  0.3		Improvements and adaption for shared memory functionality
 *
 * \copyright (C) 2018 Graz University of Technology / IKS. All rights reserved.
 * Your use of TU Graz design tools, logic functions and other 
 * software and tools and any output files any of the foregoing 
 * (including device programming or simulation files), and any associated 
 * documentation or information are expressly subject to the terms and 
 * conditions of the applicable license agreement, including, without 
 * limitation, that your use is for the sole purpose of programming the 
 * OPS-SAT processing platform.
 */
 
#ifndef OPTRX_MCU_I2C_h
#define OPTRX_MCU_I2C_h

#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "sepp_i2c_dev.h"
#include "SEPP_API_Debug.h"


// Comment RZ 01.Apr.2019:
// The MCU firmware documentation is insufficient to understand and implement a API for the OPT-RX.
// The MCU functionality is not clear. Hence, the API is "commented out" because all methods do not work.


/* Register Map Definitions for ATMega328 */
//#define         OPTRX_REG_CONTROL		      0x00		// Control register
//#define         OPTRX_MASK_CONTROL_BC		  0x02		// bit 1 - RX_BUFFER_CLEAR, Clear buffer and delete all buffer entries. The RX is disabled while buffer is cleared. Bit is set to 0 after buffer is cleared.0 = do not clear buffer, 1 = clear buffer.
//#define         OPTRX_MASK_CONTROL_RXEN		  0x01		// bit 0 - RX_ENABLE, Enable RX and start filling up the input buffer.1 = Enable RX, 0 = Disable RX (Idle)

//#define         OPTRX_REG_CONFIG		      0x01		// Configuration register
//#define         OPTRX_MASK_CONFIG_CRSEL		  0x0F		// Bit 3:0 - Analog comparator reference voltage threshold selection. Selects the level of the pulse comparator reference voltage, 0x0 = 0V, 0x1 = 1/16, 0xF = 15/16

//#define         OPTRX_REG_STATUS		      0x02		// Status register
//#define         OPTRX_MASK_STATUS_SD		  0x08		// bit 3 - SIGNAL_DETECT, Analog input signal detection at comparator input0 = no analog input signal detected, 1 = Analog input signal detected
//#define         OPTRX_MASK_STATUS_OF		  0x04		// bit 2 - RX_BUFFER_OVERFLOW, Buffer overflow flag.Is set to 1 if buffer overflow occurs. Cleared after read. Indicates that buffer was full also if  buffer full flag is not set.
//#define         OPTRX_MASK_STATUS_BF		  0x02		// bit 1 - RX_BUFFER_FULL, Buffer full status.0 = Buffer not full, 1 = Buffer full.
//#define         OPTRX_MASK_STATUS_BE		  0x01		// bit 0 - RX_BUFFER_EMPTY, Buffer empty status.0 = RX Buffer not empty, 1 = RX Buffer empty


/** OPT-RX Microcontroller Device Class
* 	\brief Contains members for device address and general information. 
*		Set and Get functions for device address, description and interface type.
* 	Methods to generate correct message sequences for write and read commands.
*/
class OPTRX_MCU : public SEPP_I2C_DEV
{
	private:

		
	public:
		OPTRX_MCU();
		virtual ~OPTRX_MCU();

		/* public Methods for Device Configuration */
		void Connect(unsigned int i2c_address, std::string i2c_device_name);
		void Disconnect();
		
		void PrintDeviceInfo();                               //!< Print device summary / info

		/* ----- Custom user functions for device ----- */
		//void Enable_RX();										// Not supported by MEW firmware
		//void Disable_RX(void);								// Not supported by MEW firmware
		//unsigned int Get_RX_AComp_Reference(void); 			// Not supported by MEW firmware
		//void Set_RX_AComp_Reference(unsigned int ref_sel);  	// Not supported by MEW firmware

		/* -------------- Register Write Functions ------------ */
		//void Write_Config_Register(unsigned int value);		// Not supported by MEW firmware
		//void Write_Control_Register(unsigned int value);		// Not supported by MEW firmware
		/* -------------- Register Read Functions ------------- */
		//unsigned int Read_Status_Register(void);				// Not supported by MEW firmware
		//unsigned int Read_Config_Register(void);				// Not supported by MEW firmware
		//unsigned int Read_Control_Register(void);				// Not supported by MEW firmware
};

/** OPT-RX Microcontroller Exception Class
*/
class OPTRX_MCU_Exception : public std::exception
{
  private:
	std::string e_;
	
  public:
	OPTRX_MCU_Exception(std::string e) : e_(e) {}
	~OPTRX_MCU_Exception() throw () {}
	const char* what() const throw() { return e_.c_str(); }
};

#endif
