/** OPS-SAT SEPP C++ API Library - CCSDS Engine 
*
* \file SEPP_CCSDS_ENGINE_API.h
*
* Generic Class for SEPP CCSDS-Engine API library.
* The class can be used to monitor and control CCSDS-Engine from the SEPP SoC.
*
* \project  		OPS-SAT SEPP C++ API Library
*
* \organisation IKS, TU Graz, Inffeldgasse 12/I, 8010 Graz
* \author 			Reinhard Zeif, MSc
* \date 		18. Apr. 2019
*
* version history
* \version	0.1  first draft  														Nov. 2017
* \version	0.2  telemetry readout functions added 									Jan. 2018
* \version	0.3  new registers added 										Feb. 2018
* \version	0.4  Test with FM flatsat									Oct. 2018
*
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
**/


#ifndef SEPP_CCSDS_ENGINE_API_H_
#define SEPP_CCSDS_ENGINE_API_H_


#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "ccsds_engine_i2c.h"
#include "ccsds_engine_gpio.h"


/* -------------- Device Configuration for I2C slave interface -------------------*/
#define		CCSDS_ENGINE_I2C_ADDRESS						0x2A					//!< CCSDS Engine I2C address
#define		CCSDS_ENGINE_I2C_BUS							"i2c-1"					//!< CCSDS Engine I2C bus name used by SEPP

#define		CCSDS_ENGINE_BANDCTRL_READBACK_DELAY_US			100		//!< delay between setting Band_Ctrl and readout of Band_State


/*! \defgroup SEPP_CCSDS_ENGINE_API SEPP CCSDS-Engine C++ API Library
* \{
*/


/* ----------------------------- Struct Definitions ----------------------------- */

/** \struct SEPP_CCSDSENGINE_API_CLCW_STATUS
*	\brief CCSDS Engine CLCW Status Telemetry 
*/
typedef struct
{
	bool CONTROL_WORD_TYPE;					//!<	0x80	CCSDS_CLCW_CWT	Control Word Type flag, which shall be zero
	unsigned char VERSION_NUMBER;			//!<	0x60	CCSDS_CLCW_VN	CLCW Version Number, which shall be zero
	unsigned char STATUS_FIELD;				//!<	0x1C	CCSDS_CLCW_STATUS_FIELD	Status Field, which shall be zero
	unsigned char COP_MODE;					//!<	0x03	CCSDS_CLCW_COP	COP In Effect, which shall be 0x1
	unsigned char VIRTUAL_CHANNEL_ID;		//!<	0xFC	CCSDS_CLCW_VCI	Virtual Channel Identifier, which identifies the TC channel of the PDEC
	unsigned char RES_FIELD;				//!<	0x03	CCSDS_CLCW_RES_FIELD	Reserved field, which shall be 0x0
	bool NO_RF_AVAILABLE;					//!<	0x80	CCSDS_CLCW_NO_RF_AV	No RF Available flag, which is connected to negated LOCK DETECT pin
	bool NO_BIT_LOCK;						//!<	0x40	CCSDS_CLCW_NO_BIT_LOCK	No Bit Lock flag, which is connected to negated DATA VALID pin
	bool LOCKOUT;							//!<	0x20	CCSDS_CLCW_LOCKOUT	Lockout flag, part of FARM-1
	bool WAIT;								//!<	0x10	CCSDS_CLCW_WAIT	Wait flag, part of FARM-1
	bool RETRANSMIT;						//!<	0x08	CCSDS_CLCW_RETRANSMIT	Retransmit flag, part of FARM-1
	unsigned char FARM_B_CCOUNT; 			//!<	0x06	CCSDS_CLCW_FARM_B_COUNT	FARM-B Count, part of FARM-1
	bool REPORT_TYPE;						//!<	0x01	CCSDS_CLCW_REPORT_TYPE	Report Type, part of FARM-1
	bool UPLINK_BYPASS;						//!<	0x80	CCSDS_CLCW_UPLINK	uplink bypass state	1 Bit
	bool HW_CMD_ERROR;						//!<	0x40	CCSDS_CLCW_HW_CMD	error: HW command not recognized	1 Bit
	bool FRAMES_DISCARDED;					//!<	0x20	CCSDS_CLCW_DISC_FRAMES	Discarded TC frames > 0 	1 Bit
	bool SPACEWIRE_LINK_CONNECTED;			//!<	0x10	CCSDS_CLCW_SPW_STATUS	SpaceWire Light link status, '1' if connected	1 Bit
	bool FSM_HEARTBEAT;						//!<	0x08	CCSDS_CLCW_HEARTBEAT	Heartbeat flag for FSM (1bit) aka IDLE meter	1 Bit
	bool PLL_ENABLED;						//!<	0x04	CCSDS_CLCW_PLL_EN	OR'ed "Enable" PLLs pins (1bit)	1 Bit
	bool UPLINK_UNCORR_ERROR;				//!<	0x02	CCSDS_CLCW_UPLINK_UNC_ERROR	OR'ed uncorrectable errors from EDACs in Uplink (1bit)	1 Bit
	bool DONWLINK_UNCORR_ERROR;				//!<	0x01	CCSDS_CLCW_DOWNLINK_UNC_ERROR	OR'ed uncorrectable errors from EDACs in Downlink (1bit)	1 Bit
} SEPP_CCSDSENGINE_API_CLCW_STATUS;

/** \struct SEPP_CCSDSENGINE_API_POWER_SUPPLY_TELEMETRY
*	\brief CCSDS Engine Power Supply Telemetry 
*/
typedef struct
{
	float FPGA_3V3_VOLTAGE_mV;					//!< FPGA 3.3V supply Voltage in mV
	float FPGA_3V3_CURRENT_mA;					//!< FPGA 3.3V supply Current in mV
	float PERIPH_3V3_VOLTAGE_mV;				//!< PERIPHERAL 3.3V supply Voltage in mV
	float PERIPH_3V3_CURRENT_mA;				//!< PERIPHERAL 3.3V supply Current in mA
	float ENGINE_1V5_VOLTAGE_mV;				//!< ENGINE 1.5V supply Voltage in mV
	float ENGINE_1V5_CURRENT_mA;				//!< ENGINE 1.5V supply Current in mA
	float ENGINE_2V5_VOLTAGE_mV;				//!< ENGINE 2.5V supply Voltage in mV
	float ENGINE_2V5_CURRENT_mA;				//!< ENGINE 2.5V supply Current in mA
	float ENGINE_7V0_VOLTAGE_mV;				//!< ENGINE 7.0V supply Voltage in mV
	float ENGINE_2V0_VOLTAGE_mV;				//!< ENGINE 2.0V supply Voltage in mV
} SEPP_CCSDSENGINE_API_POWER_SUPPLY_TELEMETRY;

/** \struct SEPP_CCSDSENGINE_API_CLOCK_STATUS
*	\brief CCSDS Engine Clock Status Telemetry 
*/
typedef struct
{
	bool PLL_24MHZ_ENABLED;		//!<	0x08	CCSDS_CLK_STATUS_24MHz_PLL_EN	PLL 10 MHz to 24 MHz is enabled
	bool PLL_24MHZ_LOCKED;		//!<	0x04	CCSDS_CLK_STATUS_24MHz_PLL_LOCK	PLL 10 MHz to 24 MHz is locked
	bool PLL_10MHZ_ENABLED;		//!<	0x02	CCSDS_CLK_STATUS_10MHz_PLL_EN	PLL 24 MHz to 10 MHz is enabled
	bool PLL_10MHZ_LOCKED;		//!<	0x01	CCSDS_CLK_STATUS_10MHz_PLL_LOCK	PLL 24 MHz to 10 MHz is locked
} SEPP_CCSDSENGINE_API_CLOCK_STATUS;

/** \struct SEPP_CCSDSENGINE_API_CCSDS_STATUS
*	\brief CCSDS Engine CCSDS Status Telemetry 
*/
typedef struct
{
	bool HW_COMMAND_ERROR;			//!<	0x40	CCSDS_REG_HW_COMMAND_ERROR	'1' if received HW command is wrong
	bool BAND_CONTROL_SELECT;		//!<	0x20	CCSDS_REG_BAND_CONTROL_REG	Value of BAND CONTROL register
	bool BAND_CONTROL_PIN_INPUT;	//!<	0x10	CCSDS_REG_BAND_CONTROL_PIN	Value of BAND CONTROL input pin
	bool BYPASS_CONTROL_SELECT;		//!<	0x08	CCSDS_REG_BYPASS_DOWNLINK_REG	Value of BYPASS DOWNLINK register
	bool BYPASS_CONTROL_PIN_INPUT;	//!<	0x04	CCSDS_REG_BYPASS_DOWNLINK_PIN	Value of BYPASS DOWNLINK input pin
	bool BYPASS_UPLINK_SELECT;		//!<	0x02	CCSDS_REG_BYPASS_UPLINK_REG	Value of BYPASS UPLINK register
	bool BYPASS_UPLINK_PIN_INPUT;	//!<	0x01	CCSDS_REG_BYPASS_UPLINK_PIN	Value of BYPASS UPLINK input pin
} SEPP_CCSDSENGINE_API_CCSDS_STATUS;

/** \struct SEPP_CCSDSENGINE_API_ERROR_STATUS
*	\brief CCSDS Engine Error Status Telemetry 
*/
typedef struct
{
	unsigned char PTME_EDAC_CORRECTABLE_ERROR_COUNT;			//!<	CCSDS_PTME_EDAC_corr	PTME EDAC correctable error counter
	unsigned char PTME_EDAC_UNCORRECTABLE_ERROR_COUNT;			//!<	CCSDS_PTME_EDAC_uncorr	PTME EDAC uncorrectable error counter
	unsigned char PDEC_EDAC_MAIN_CORRECTABLE_ERROR_COUNT;		//!<	CCSDS_PDEC_EDAC_main_corr	PDEC EDAC main correctable error counter
	unsigned char PDEC_EDAC_MAIN_UNCORRECTABLE_ERROR_COUNT;		//!<	CCSDS_PDEC_EDAC_main_uncorr	PDEC EDAC main uncorrectable error counter
	unsigned char PDEC_EDAC_HOLDING_CORRECTABLE_ERROR_COUNT;	//!<	CCSDS_PDEC_EDAC_holding_corr	PDEC EDAC holding correctable error counter
	unsigned char PDEC_EDAC_HOLDING_UNCORRECTABLE_ERROR_COUNT;	//!<	CCSDS_PDEC_EDAC_holding_uncorr	PDEC EDAC holding uncorrectable error counter
	unsigned char MAP_ID_1_EDAC_CORRECTABLE_ERROR_COUNT;		//!<	CCSDS_MAP_ID_1_EDAC_corr	MAP ID 1 EDAC correctable error counter
	unsigned char MAP_ID_1_EDAC_UNCORRECTABLE_ERROR_COUNT;		//!<	CCSDS_MAP_ID_1_EDAC_uncorr	MAP ID 1 EDAC uncorrectable error counter
	unsigned char CAN_EDAC_CORRECTABLE_ERROR_COUNT;				//!<	CCSDS_CAN_EDAC_corr	CAN EDAC correctable error counter
	unsigned char CAN_EDAC_UNCORRECTABLE_ERROR_COUNT;			//!<	CCSDS_CAN_EDAC_uncorr	CAN EDAC uncorrectable error counter
	unsigned char CAN_ABORT_COUNT;								//!<	CCSDS_CAN_ABORT_cnt	CAN ABORT message counter
} SEPP_CCSDSENGINE_API_ERROR_STATUS;


/* ----------------------- Enumeration Definitions ------------------------ */

enum SEPP_CCSDSENGINE_API_DOWNLINK_BITRATE_e
{
	SEPP_CCSDSENGINE_API_DOWNLINK_BITRATE_24MHZ = 0,
	SEPP_CCSDSENGINE_API_DOWNLINK_BITRATE_12MHZ = 1,
	SEPP_CCSDSENGINE_API_DOWNLINK_BITRATE_6MHZ = 2,
	SEPP_CCSDSENGINE_API_DOWNLINK_BITRATE_4MHZ = 3,
	SEPP_CCSDSENGINE_API_DOWNLINK_BITRATE_3MHZ = 4,
	SEPP_CCSDSENGINE_API_DOWNLINK_BITRATE_1MHZ = 5,
	SEPP_CCSDSENGINE_API_DOWNLINK_BITRATE_500KHZ = 6,
	SEPP_CCSDSENGINE_API_DOWNLINK_BITRATE_200KHZ = 7,
	SEPP_CCSDSENGINE_API_DOWNLINK_DISABLE_CLOCK = 8
};


/** SEPP C++ API Library - CCSDS Engine Device Class
* 	\brief CCSDS Engine API class
*/
class SEPP_CCSDS_ENGINE_API
{
	private:

    	CCSDS_Engine_GPIO *gpio_;		// GPIO interface for control and status pins
		CCSDS_Engine_I2C *i2c_;			// i2C interface to monitor CCSDS engine
		
		// CAN interface CFP (fragmentation)
		// LVDS Configuration ???


	public:

		SEPP_CCSDS_ENGINE_API(void);
		virtual ~SEPP_CCSDS_ENGINE_API(void);

		void Print_Info(void);
				
		/* ----- I2C Interface Methods ----- */
		int Get_Temperature_degC(void);
		SEPP_CCSDSENGINE_API_CLCW_STATUS Get_CLCW_Status(void);
		SEPP_CCSDSENGINE_API_POWER_SUPPLY_TELEMETRY Get_Power_Supply_Telemetry(void);
		void Set_Downlink_Bitrate(SEPP_CCSDSENGINE_API_DOWNLINK_BITRATE_e bitrate);
		SEPP_CCSDSENGINE_API_DOWNLINK_BITRATE_e Get_Downlink_Bitrate(void);
		unsigned int Get_Rejected_TC_Count(void);
		SEPP_CCSDSENGINE_API_CLOCK_STATUS Get_Clock_Status(void);
		SEPP_CCSDSENGINE_API_CCSDS_STATUS Get_CCSDS_Status(void);
		SEPP_CCSDSENGINE_API_ERROR_STATUS Get_Error_Status(void);
		
		// unsigned int Get_GPIO_Lock_State(void);


		/* ----- GPIO Interface Methods ----- */
		void Set_RF_Band_Control(unsigned int rf_band);
		unsigned int Get_RF_Band_State(void);
		void Set_Downlink_Bypass_Ctrl(unsigned int high_or_low);
		void Set_Uplink_Bypass_Ctrl(unsigned int high_or_low);
		unsigned int Get_Downlink_Bypass_State(void);
		unsigned int Get_Uplink_Bypass_State(void);

};

/** SEPP C++ API Library - CCSDS Engine Device Exception
*
*/
class SEPP_CCSDS_ENGINE_API_Exception : public std::exception
{
  private:
	std::string e_;

  public:
	SEPP_CCSDS_ENGINE_API_Exception(std::string e) : e_(e) {}
	~SEPP_CCSDS_ENGINE_API_Exception() throw () {}
	const char* what() const throw() { return e_.c_str(); }
};

/** \} */ // End of Group

#endif /* SEPP_CCSDS_ENGINE_API_H_ */
