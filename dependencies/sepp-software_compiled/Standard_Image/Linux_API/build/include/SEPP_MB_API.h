/** OPS-SAT SEPP C++ API Library - SEPP Mainboard
*
* \file SEPP_MB_API.h
*
* \project  		OPS-SAT SEPP C++ API Library
*
* \organisation IKS, TU Graz, Inffeldgasse 12/I, 8010 Graz
* \author 			Reinhard Zeif, MSc
* \date 				18. Dec. 2017
*
* \version		0.1  first draft						Oct.2015
* \version		0.2	 EM test    						Nov.2017
* \version		0.3	 Further testing and bugfixing		Dec. 2017
* \version		1.0	 Commissioning version in-orbit						08.May.2020
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

#ifndef SEPP_MB_API_H_
#define SEPP_MB_API_H_

#include <iostream>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <math.h>

#include "sepp_mainboard.h"
#include "SEPP_API_Debug.h"

/*! \defgroup SEPP_MB_API SEPP Mainboard C++ API Library
* \{
*/

/********************** GENERAL Definitions *************************/

#define SEPP_MB_API_VERSION_STRING_BASE		"v1.0, 08.May.2020"
#ifdef GIT_VERSION
#define SEPP_MB_API_VERSION_STRING   SEPP_MB_API_VERSION_STRING_BASE << " " << GIT_VERSION
#else
#define SEPP_MB_API_VERSION_STRING   SEPP_MB_API_VERSION_STRING_BASE
#endif


/********************************************************************/

/*! Temperature Sensor Conversion Rate Setting */
enum SEPP_MB_API_TSENSOR_CONVERSIONRATE
{
  CR_4000MS = 0, // Convert every 4 sec
  CR_1000MS = 1, // Convert every 1 sec
  CR_250MS = 2,  // Convert every 250 ms = default T-Sense setting
  CR_125MS = 3   // Convert every 125 ms
};

/*! Real-Time Clock Time and Date */
typedef struct
{
  unsigned int second;
  unsigned int minute;
  unsigned int hour;
  unsigned int day;
  unsigned int date;
  unsigned int month;
  unsigned int year;
} SEPP_MB_API_RTC_DATA_TIME;

/*! structure for on-board temperature sensor alert mode configuration */
typedef struct sepp_api_tsensor_al_mode
{
	unsigned int thermostatmode;
	unsigned int polarity;
} SEPP_MB_API_TSENSOR_ALERTMODE;

/*! structure for board manufacturing data stored in eeprom */
typedef struct sepp_api_eeprom_mfr_data
{
	unsigned char device_id[32];
	unsigned char hardware_version[16];
	unsigned char firmware_version[16];
	unsigned int device_serial;
	unsigned short mfr_year;
} SEPP_MB_API_BOARD_MFR_DATA;


/** SEPP C++ API Library - SEPP mainboard API class
*   \brief Contains members for device address and general information. 
*   Set and Get functions for device address, description and interface type.
*   Methods to generate correct message sequences for write and read commands.
*/
class SEPP_MB_API
{
    private:
        SEPP_MAINBOARD *mb_;            //!< SEPP mainboard object
        
    public:
        SEPP_MB_API();
        virtual ~SEPP_MB_API();
        
        /* --------- Temperature Sensor methods -------- */
        float Get_Temperature_degC(unsigned int sensor);
        void Set_TSensor_Conversion_Rate(unsigned int sensor, SEPP_MB_API_TSENSOR_CONVERSIONRATE rate);
        SEPP_MB_API_TSENSOR_CONVERSIONRATE Get_TSensor_Conversion_Rate(unsigned int sensor);
        SEPP_MB_API_TSENSOR_ALERTMODE Get_TSensor_Alert_Mode(unsigned int sensor);
        bool Check_TSensor_Alert_Flag(unsigned int sensor);

        /* ----------- Real-Time Clock methods --------- */
        void Set_RTC_Date_Time(SEPP_MB_API_RTC_DATA_TIME date_time);
        SEPP_MB_API_RTC_DATA_TIME Get_RTC_Date_Time(void);
        unsigned int Check_RTC_Status(void);

        /* ------------- HPS GPIO methods -------------- */
        bool Check_TSENSE_ALERTn_Signal_State(void);		//!< check GPIO TSENSE_ALERTn signal state

		/* ------------- EEProm methods ---------------- */
		SEPP_MB_API_BOARD_MFR_DATA Get_Board_MFR_Data(void);	//!< read board manufacturing data
		
		/* -------------- GPS methods ------------------ */
		// TBD: Get_GPS_Data(void);
		
		/* -------------- Telemetry methods ------------ */
		// TBD: Get_DataStore_Parameter(...)
		
        /* ------------------ Misc --------------------- */
        void Print_Info(void);
};

/** SEPP C++ API Library - SEPP Mainboard Device Exception
*/
class SEPP_MB_API_Exception : public std::exception
{
  private:
      std::string e_;
    
  public:
      SEPP_MB_API_Exception(std::string e) : e_(e) {}
      ~SEPP_MB_API_Exception() throw () {}
      const char* what() const throw() { return e_.c_str(); }
};

/** \} */ // End of Group

#endif 
