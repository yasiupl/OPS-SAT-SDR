/** SEPP Mainboard Device Library
* 
* \file 	sepp_mainboard.h
*
* \brief SEPP mainboard configuration library for on-board components.
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
* \version		0.4	 EEProm functionality added			May. 2019
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
 
#ifndef SEPP_MAINBOARD_H_
#define SEPP_MAINBOARD_H_

#include <iostream>
#include <string>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>


#include "TMP112.h"
#include "DS1339A.h"
#include "sepp_gpio.h"
#include "M24LC32A.h"

#include "SEPP_API_Debug.h"


/* General SEPP Definitions */
#define         SEPP_MB_HW_VERSION                          	"1.0.0"
#define         SEPP_MB_CONFIG_SW_VERSION                  		"1.0.0"
#define         SEPP_MB_TEMP_TMAX_ALERT_LIMIT_HIGH				85			//!< Temperature sensor Tmax alert limit HIGH in degree Celsius
#define         SEPP_MB_TEMP_TMAX_ALERT_LIMIT_LOW				80			//!< Temperature sensor Tmax alert limit LOW in degree Celsius
#define         SEPP_MB_TEMP_TMIN_ALERT_LIMIT_HIGH				-35			//!< Temperature sensor Tmin alert limit HIGH in degree Celsius
#define         SEPP_MB_TEMP_TMIN_ALERT_LIMIT_LOW				-40			//!< Temperature sensor Tmin alert limit LOW in degree Celsius
#define         SEPP_MB_OPERATING_TEMP_HIGH						85			//!< Maximum operating temperature in degree Celsius
#define         SEPP_MB_OPERATING_TEMP_LOW						-40			//!< Minimum operating temperature in degree Celsius

/* SEPP Board I2C device name */
#define			SEPP_MB_I2C_DEV_NAME							"i2c-1"

/* SEPP Board I2C component addresses */
#define         SEPP_MB_TSENSOR1_I2C_ADDR          				0x48
#define         SEPP_MB_TSENSOR2_I2C_ADDR          				0x49
#define         SEPP_MB_RTC_I2C_ADDR                			0x68
#define			SEPP_MB_EEPROM_I2C_ADDR							0x55

/* EEProm memory parameters */
#define			SEPP_MB_EEPROM_DEVICE_ID_ADDR					0x20			//!< address offset for first byte of "device ID" string, ends with '\0'
#define			SEPP_MB_EEPROM_DEVICE_ID_LENGTH					32				//!< length of "device ID" in bytes
#define			SEPP_MB_EEPROM_DEVICE_HW_VERSION_ADDR			0x40			//!< address offset for first byte of "hardware version" string, ends with '\0'
#define			SEPP_MB_EEPROM_DEVICE_HW_VERSION_LENGTH			16				//!< length of "hardware version" in bytes
#define			SEPP_MB_EEPROM_DEVICE_FW_VERSION_ADDR			0x50			//!< address offset for first byte of "firmware version" string, ends with '\0'
#define			SEPP_MB_EEPROM_DEVICE_FW_VERSION_LENGTH			16				//!< length of "firmware version" in bytes
#define			SEPP_MB_EEPROM_DEVICE_PROD_YEAR_ADDR			0x60			//!< address offset for first byte of "production year" integer
#define			SEPP_MB_EEPROM_DEVICE_PROD_YEAR_LENGTH			4				//!< length of "production year" in bytes
#define			SEPP_MB_EEPROM_DEVICE_SERIAL_NUMBER_ADDR		0x60			//!< address offset for first byte of "serial number" integer
#define			SEPP_MB_EEPROM_DEVICE_SERIAL_NUMBER_LENGTH		4				//!< length of "serial number" in bytes

/* Temperature sensor parameters */
#define         SEPP_MB_TSENSOR_COUNT        					2     			//!< number of temperature sensors


/** HPS GPIO Pin Description - Linux OS pin numbers
 * The following HPS GPIO chip devices are available in the /sys/class/gpio/ folder (for the current Linux OS):
 *
 * gpiochip427 = GPIO device 2 with 27 pins (23 pins used, index 9-12 not used)
 * -> HPS GPIO [66:58] = GPIO device 2 [8:0] Input/Output  = Export Pin# 427 - 435
 * -> HPS HLGPI [13:0] = GPIO device 2 [26:13] Input only  = Export Pin# 440 - 453
 * gpiochip454 -> GPIO device 1
 * -> HPS GPIO [57:29] = GPIO device 1 [28:0] Input/Output = Export Pin# 454 - 482
 * gpiochip483 -> GPIO device 0
 * -> HPS GPIO [28:0]  = GPIO device 0 [28:0] Input/Output = Export Pin# 483 - 501
 */

/* SEPP Board HPS GPIO parameters */
#define			SEPP_MB_HPS_GPIO_TSENSE_ALERTn					469				//!< HPS GPIO44 pin number for T-Sensor Alert active-low pin


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
} SEPP_MB_RTC_DATETIME;


/** SEPP Mainboard Class
*   \brief Contains members for device used to monitor and control the
*   board signals and components.
*   Methods to set board configuration signals and to retrieve board status.
*/
class SEPP_MAINBOARD
{
    private:

        TMP112 *tsense1_ = NULL;			//!< Instance of Temperature Sensor 1
        TMP112 *tsense2_ = NULL;			//!< Instance of Temperature Sensor 2
        DS1339A *rtc_ = NULL;				//!< Instance of RTC
        M24LC32A *eeprom_ = NULL;			//!< Instance of EEProm Memory
        SEPP_GPIO *hps_gpio_tsense_alertn_ = NULL;	//!< Instance of GPIO pin for Temperature Alert

        std::string description_;

    public:
        SEPP_MAINBOARD();
        virtual ~SEPP_MAINBOARD();

        void Initialize_Components();							//!< Initialize on-board components (device register initialization)

        /* ----- Board Description and Infos -----  */
        std::string GetDeviceDescription(void);            		//!< Get SEPP device description
        void PrintDeviceInfo(void);                          	//!< Print device info of the different board devices

        /* ----- On-Board Temperature Sensor ------ */
        float Get_Temperature_degC(unsigned int sensor);			//!< get on-board temperature in degC
        
        unsigned int Get_TSensor_Alert_Flag(unsigned int sensor);	//!< Get on-board temperature sensor alert flag from configuration register
        unsigned int Get_TSensor_CR(unsigned int sensor);  			//!< Get on-board temperature sensor conversion rate
        unsigned int Get_TSensor_TM(unsigned int sensor);  			//!< Get on-board temperature sensor thermostat mode
        unsigned int Get_TSensor_POL(unsigned int sensor);  		//!< Get on-board temperature sensor conversion rate
        void Set_TSensor_CR(unsigned int sensor, unsigned int rate);  //!< Set on-board temperature sensor conversion rate
        void Set_TSensor_TM(unsigned int sensor, unsigned int mode);  //!< Set on-board temperature sensor thermostat mode
        void Set_TSensor_POL(unsigned int sensor, unsigned int polarity);  //!< Set on-board temperature sensor conversion rate

        /* ------- On-Board Real-Time Clock ------- */
        void Set_RTC_Date_Time(SEPP_MB_RTC_DATETIME date_time);
        SEPP_MB_RTC_DATETIME Get_RTC_Date_Time(void);
        unsigned int Check_RTC_Osc_Status(void);

        /* --------- HPS GPIO functions ------------*/
        unsigned int Check_TSENSE_ALERTn_Signal_State(void);		//!< check GPIO TSENSE_ALERTn signal state

        /* ---------- EEProm functions ------------ */
        void Get_EEProm_Device_ID(unsigned char *data);					//!< Get the unit/device identifier (board ID)
        void Get_EEProm_Device_HW_Version(unsigned char *data);			//!< Get the unit/device hardware version
        void Get_EEProm_Device_FW_Version(unsigned char *data);			//!< Get the unit/device firmware version
        unsigned int Get_EEProm_Device_Production_Year(void);		//!< Get the unit/device production year (board production year)
        unsigned int Get_EEProm_Device_Serial_Number(void);		//!< Get the unit/device serial number (board serial number)

};

/** SEPP Mainboard Exception Class.
*   \brief This exception type is returned in case of an exception in the SEPP board class.
*/
class SEPP_MAINBOARD_Exception : public std::exception
{
  private:
    std::string e_;
    
  public:
    SEPP_MAINBOARD_Exception(std::string e) : e_(e) {}
    ~SEPP_MAINBOARD_Exception() throw () {}
    const char* what() const throw() { return e_.c_str(); }
};

#endif
