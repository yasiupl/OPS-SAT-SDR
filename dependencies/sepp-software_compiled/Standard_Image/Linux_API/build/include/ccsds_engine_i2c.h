/**  CCSDS Engine I2C interface
 *
 * \file ccsds_engine_i2c.h
 *
 * Generic Class for CCSDS engine i2c commands and parameters.
 *
 * The class can be used to access the CCSDS engine I2C Slave interface connected to the SEPP SoC.
 *
 * \project  		OPS-SAT SEPP C++ API Library
 *
 * \organisation 	IKS, TU Graz, Inffeldgasse 12/I, 8010 Graz
 * \date			28. Oct. 2019
 * \author			Reinhard Zeif, MSc
 *
 * \version			0.1		First release
 * \version			1.0		Final SVT-3 release
 * \version			1.1		Bugfix for temperature conversion of raw value register
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
 
#ifndef CCSDS_Engine_I2C_h
#define CCSDS_Engine_I2C_h

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
#include "SEPP_API_Debug.h"

/* Register Map Definitions for CCSDS_Engine_I2C */
#define		    CCSDS_ENGINE_REG_CLCW_STATUS_1 			    	0x00					//!< Telemetry Register CLCW STATUS 1
#define		    CCSDS_ENGINE_REG_CLCW_STATUS_2 			   		0x01					//!< Telemetry Register CLCW STATUS 2
#define		    CCSDS_ENGINE_REG_CLCW_STATUS_3					0x02					//!< Telemetry Register CLCW STATUS 3 
#define		    CCSDS_ENGINE_REG_CLCW_STATUS_4					0x03					//!< Telemetry Register CLCW STATUS 4
#define		    CCSDS_ENGINE_REG_FPGA_V_3V3_MSB	 	 			0x04					//!< Telemetry Register FPGA_V_3V3_MSB
#define		    CCSDS_ENGINE_REG_FPGA_V_3V3_LSB	 	 			0x05					//!< Telemetry Register FPGA_V_3V3_LSB
#define		    CCSDS_ENGINE_REG_FPGA_C_3V3_MSB	 	 			0x06					//!< Telemetry Register FPGA_C_3V3_MSB
#define		    CCSDS_ENGINE_REG_FPGA_C_3V3_LSB	 	 			0x07					//!< Telemetry Register FPGA_C_3V3_LSB
#define		    CCSDS_ENGINE_REG_PERIPH_V_3V3_MSB	 	 		0x08					//!< Telemetry Register PERIPH_V_3V3_MSB
#define		    CCSDS_ENGINE_REG_PERIPH_V_3V3_LSB	 	 		0x09					//!< Telemetry Register PERIPH_V_3V3_LSB
#define		    CCSDS_ENGINE_REG_PERIPH_C_3V3_MSB	 	 		0x0A					//!< Telemetry Register PERIPH_C_3V3_MSB
#define		    CCSDS_ENGINE_REG_PERIPH_C_3V3_LSB	 	 		0x0B					//!< Telemetry Register PERIPH_C_3V3_LSB
#define		    CCSDS_ENGINE_REG_V_1V5_MSB 			    		0x0C					//!< Telemetry Register V_1V5_MSB
#define		    CCSDS_ENGINE_REG_V_1V5_LSB 			    		0x0D					//!< Telemetry Register V_1V5_LSB
#define		    CCSDS_ENGINE_REG_C_1V5_MSB 			    		0x0E					//!< Telemetry Register C_1V5_MSB
#define		    CCSDS_ENGINE_REG_C_1V5_LSB 			    		0x0F					//!< Telemetry Register C_1V5_LSB
#define		    CCSDS_ENGINE_REG_V_2V5_MSB 			    		0x10					//!< Telemetry Register V_1V5_MSB
#define		    CCSDS_ENGINE_REG_V_2V5_LSB 			    		0x11					//!< Telemetry Register V_1V5_LSB
#define		    CCSDS_ENGINE_REG_C_2V5_MSB 			    		0x12					//!< Telemetry Register C_1V5_MSB
#define		    CCSDS_ENGINE_REG_C_2V5_LSB 			    		0x13					//!< Telemetry Register C_1V5_LSB
#define		    CCSDS_ENGINE_REG_TEMP_RAW 			    		0x14					//!< Telemetry Register TEMP_RAW
#define		    CCSDS_ENGINE_REG_V_7V0_MSB 			    		0x15					//!< Telemetry Register V_7V0_MSB
#define		    CCSDS_ENGINE_REG_V_7V0_LSB 			    		0x16					//!< Telemetry Register V_7V0_LSB
#define		    CCSDS_ENGINE_REG_V_2V0_MSB 			    		0x17					//!< Telemetry Register V_2V0_MSB
#define		    CCSDS_ENGINE_REG_V_2V0_LSB 			    		0x18					//!< Telemetry Register V_2V0_LSB
#define		    CCSDS_ENGINE_REG_DOWNLINK_BITRATE	    		0x20					//!< Telemetry Register DOWNLINK_BITRATE
#define		    CCSDS_ENGINE_REG_REJECTED_TC_MSB				0x24					//!< Telemetry Register REJECTED_TC_MSB	
#define		    CCSDS_ENGINE_REG_REJECTED_TC_LSB				0x25					//!< Telemetry Register REJECTED_TC_LSB
#define		    CCSDS_ENGINE_REG_CLOCK_STATUS					0x26					//!< Telemetry Register CLOCK_STATUS
#define		    CCSDS_ENGINE_REG_CCSDS_STATUS					0x27					//!< Telemetry Register CCSDS_STATUS	
#define		    CCSDS_ENGINE_REG_PTME_EDAC_CORR					0x28					//!< Telemetry Register PTME_EDAC_CORR
#define		    CCSDS_ENGINE_REG_PTME_EDAC_UNCORR				0x29					//!< Telemetry Register PTME_EDAC_UNCORR
#define		    CCSDS_ENGINE_REG_PDEC_EDAC_MAIN_CORR			0x2A					//!< Telemetry Register PDEC_EDAC_MAIN_CORR
#define		    CCSDS_ENGINE_REG_PDEC_EDAC_MAIN_UNCORR			0x2B					//!< Telemetry Register PDEC_EDAC_MAIN_UNCORR
#define		    CCSDS_ENGINE_REG_PDEC_EDAC_HOLDING_CORR			0x2C					//!< Telemetry Register PDEC_EDAC_HOLDING_CORR
#define		    CCSDS_ENGINE_REG_PDEC_EDAC_HOLDING_UNCORR		0x2D					//!< Telemetry Register PDEC_EDAC_HOLDING_UNCORR
#define		    CCSDS_ENGINE_REG_MAP_ID_1_EDAC_CORR				0x2E					//!< Telemetry Register MAP_ID_1_EDAC_CORR
#define		    CCSDS_ENGINE_REG_MAP_ID_1_EDAC_UNCORR			0x2F					//!< Telemetry Register MAP_ID_1_EDAC_UNCORR
#define		    CCSDS_ENGINE_REG_CAN_EDAC_CORR					0x30					//!< Telemetry Register CAN_EDAC_CORR
#define		    CCSDS_ENGINE_REG_CAN_EDAC_UNCORR				0x31					//!< Telemetry Register CAN_EDAC_UNCORR
#define		    CCSDS_ENGINE_REG_CAN_ABORT						0x32					//!< Telemetry Register CAN_ABORT


/* Some Helper Definitions */
#define			CCSDS_ENGINE_REG_TEMP_RANGE						256						//!< Temperature value range of Temp Raw Register
#define			CCSDS_ENGINE_REG_TEMP_MAX						205						//!< Max. Temperature value in Temp Raw Register
#define			CCSDS_ENGINE_REG_TEMP_Min						-50						//!< Min. Temperature value in Temp Raw Register

/* --------------------- Struct Definitions --------------------- */




/** CCSDS Engine I2C Device Class
* 	\brief Contains members for device address and general information. 
*	Set and Get functions for device address, description and interface type.
* 	Methods to generate correct message sequences for write and read commands.
*/
class CCSDS_Engine_I2C : public SEPP_I2C_DEV
{
	private:
		
	public:
		CCSDS_Engine_I2C();
		virtual ~CCSDS_Engine_I2C();

		void Connect(int i2c_address, std::string i2c_dev_name);
		void Disconnect();
		
		/* public Methods for Device Configuration */
		void PrintDeviceInfo();
		
		/* ----- Custom user functions for device ----- */
		void Get_CLCW_Status(unsigned char *tm_regs);
		void Get_Power_Supply_Telemetry(unsigned int *tm_regs);
		void Set_Downlink_Bitrate(unsigned char bitrate);
		unsigned char Get_Downlink_Bitrate(void);
		unsigned int Get_Rejected_TC_Count(void);
		unsigned char Get_Clock_Status(void);
		unsigned char Get_CCSDS_Status(void);
		void Get_Error_Status(unsigned char *tm_regs);
		int Get_Temperature_degC(void);
};

/** CCSDS Engine I2C Exception Class
*/
class CCSDS_Engine_I2C_Exception : public std::exception
{
  private:
	std::string e_;
	
  public:
	CCSDS_Engine_I2C_Exception(std::string e) : e_(e) {}
	~CCSDS_Engine_I2C_Exception() throw () {}
	const char* what() const throw() { return e_.c_str(); }
};

#endif
