/** OPT-RX Mainboard Device Library
* 
* \file OPTRX_MAINBOARD.h
*
* \project 		OPS-SAT
* \organisation	Graz University of Technology
* \author     		Reinhard Zeif, MSc
* \date       		14. April 2019
*
* \brief OPT-RX Mainboard library for device communication, monitoring and control.
* The library contains the software commands for all on-board components.
*
* Revision History:
* \version 0.1		Release for agreed set of methods and registers (before firmware delivery)
* \version 0.2		Most MCU relevant methods from v0.1 removed, since MEW did a deliver incomplete firmware
* \version 0.3		Improvements and adaption for shared memory functionality
* \version 0.4		Shared memory functionality added to this class
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
 
#ifndef OPTRX_MAINBOARD_H_
#define OPTRX_MAINBOARD_H_

#include <typeinfo>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>


#include "MAX7310.h"
#include "TMP175.h"
#include "optrx_mcu.h"
#include "optrx_sharedmemory.h"

#include "SEPP_API_Debug.h"


/* General OPTRX Definitions */
#define         OPTRX_HW_VERSION                          "1.0.0"
#define         OPTRX_CONFIG_SW_VERSION                   "1.0.0"
#define         OPTRX_TEMP_ALERT_LIMIT_HIGH                   85          // Temperature sensor alert limit HIGH in degree Celsius
#define         OPTRX_TEMP_ALERT_LIMIT_LOW                    75          // Temperature sensor alert limit LOW in degree Celsius
#define         OPTRX_OPERATING_TEMP_HIGH                     85          // Maximum operating temperature in degree Celsius
#define         OPTRX_OPERATING_TEMP_LOW                      -40         // Minimum operating temperature in degree Celsius

/* OPTRX Board I2C component addresses */
#define         OPTRX_HWCTRL_I2C_ADDR                     0x1A			// Board configuration bus expander
#define         OPTRX_TSENSOR_I2C_ADDR                    0x4C			// T-Sensor
#define			OPTRX_MCU_I2C_ADDR						  0x11			// MCU controller
/* OPTRX Board SPI component addresses */
#define			OPTRX_SHARED_MEM_SPI_DEV				"spidev0.2"		// SPI device for shared memory
/* OPTRX Board HW controller initial Register values */
#define         OPTRX_HWCTRL_POLINV_INIT                  0x00        // init value for input pin polarity inversion: No Inputs Inverted
#define         OPTRX_HWCTRL_CONFIG_INIT                  0x7C        // init value for IO pin directions: IO7-0 = [Out,In,In,In,In,In,Out,Out]
#define         OPTRX_HWCTRL_OUT_INIT                     0x81        // init value for output levels: Inputs = Low, Outputs IO7 = High, IO1 = Low, IO0 = High
/* OPTRX Board HW controller register masks */
#define         OPTRX_HWCTRL_MASK_HW_RESET_N           	  0x01        // register bit mask of hardware reset (IO0)
#define         OPTRX_HWCTRL_MASK_FW_UPDATE_EN            0x02        // register bit mask of firmware update enable (IO1)
#define         OPTRX_HWCTRL_MASK_SHARED_MEM_IF_SEL       0x80        // register bit mask of shared memory interface select (IO7)
/* OPTRX Board HW controller Pin Numbers */
#define         OPTRX_HWCTRL_PIN_HW_RESET_N                  0        // pin number of hardware reset (HW_RESETn) signal
#define         OPTRX_HWCTRL_PIN_FW_UPDATE_EN                1        // pin number of firmware update enable (FW_UPDATE_EN) signal
#define         OPTRX_HWCTRL_PIN_SHARED_MEM_IF_SEL           7        // pin number of shared memory interface select (SHARED_MEM_IF_SEL) signal
/* OPTRX memory interface select values */
#define			OPTRX_HWCTRL_SHARED_MEM_TO_MCU			 	 1		  // SHARED_MEM_IF_SEL signal: 1 = switch memory to internal MCU
#define			OPTRX_HWCTRL_SHARED_MEM_TO_SEPP			 	 0		  // SHARED_MEM_IF_SEL signal: 0 = switch memory to external SEPP


/** OPT-RX Mainboard Device Class
*   \brief Contains members for device used to monitor and control the
*   board signals and components.
*   Methods to set board configuration signals and to retrieve board status.
*/
class OPTRX_MAINBOARD
{
    private:

        MAX7310 *ctrl_ = NULL;
        TMP175 *tsense_ = NULL;
        OPTRX_MCU * mcu_ = NULL;
		OPTRX_SHAREDMEMORY *shared_mem_ = NULL;		// OPT-RX shared memory

        std::string description_;

    public:
        OPTRX_MAINBOARD();
        virtual ~OPTRX_MAINBOARD();

        void Initialize_Components();							//!< Initialize on-board component (device register initialization)

        /* -------------- Board Description and Infos --------------  */
        std::string GetDeviceDescription(void);            		//!< Get OPTRX device description
        void PrintDeviceInfo(void);                          	//!< Print device info of the different board devices

        /* -------------- Microcontroller Unit Methods -------------- */
		//void Enable_RX(void);										// Not supported by MEW firmware
		//void Disable_RX(void);									// Not supported by MEW firmware
		//unsigned int Get_RX_Comparator_Reference(void);			// Not supported by MEW firmware
		//void Set_RX_Comparator_Reference(unsigned int ref_sel);	// Not supported by MEW firmware
		//void Clear_RX_Buffer(void);								// Not supported by MEW firmware
		//unsigned int Get_Buffer_Full_Flag(void);					// Not supported by MEW firmware
		//unsigned int Get_Buffer_Overflow_Flag(void);				// Not supported by MEW firmware
		//unsigned int Read_MCU_Status_Register(void);				// Not supported by MEW firmware
		//unsigned int Read_MCU_Config_Register(void);				// Not supported by MEW firmware
		//unsigned int Read_MCU_Control_Register(void);				// Not supported by MEW firmware
        //unsigned int Check_TempSensor_HW_Alert(void);				// Not supported by MEW firmware
        /* ---------------- On-Board Temperature Sensor ------------- */
        float Get_Temperature_degC(void);						//!< get on-board temperature in degC
        /* -------------- Shared-Memory Interface Methods ----------- */
		void Set_SharedMemory_IF_Switch(unsigned int sepp_or_mcu);
		unsigned int Get_SharedMemory_IF_Switch(void);
		/* -------------- Shared-Memory Access Methods -------------- */
		unsigned int Check_Memory_Access();
		void Read_Memory(unsigned char *data_buffer, unsigned int number_of_bytes); //!< Read Memory
		unsigned int Erase_Memory_4KB_SubSectors(unsigned int subsector_address, unsigned int number_of_subsectors);		//!< Erase Memory
		/* --------------- Board Operating Mode Methods ------------- */
		void Goto_Normal_Mode(void);
		void Goto_FW_Update_Mode(void);
		unsigned int Get_Operation_Mode(void);

};

/** OPT-RX Mainboard Exception Class
*   \brief This exception type is returned in case of an exception in the OPTRX board class.
*/
class OPTRX_MAINBOARD_Exception : public std::exception
{
  private:
    std::string e_;
    
  public:
    OPTRX_MAINBOARD_Exception(std::string e) : e_(e) {}
    ~OPTRX_MAINBOARD_Exception() throw () {}
    const char* what() const throw() { return e_.c_str(); }
};

#endif
