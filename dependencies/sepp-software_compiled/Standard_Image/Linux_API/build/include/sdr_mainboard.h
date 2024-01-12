/** SDR Mainboard Device Library
 * 
 * \file SDR_MAINBOARD.h

 * \project    OPS-SAT Project
 * \copyright  Graz University of Technology, IKS
 * \date       15. Nov. 2017
 * \author     Reinhard Zeif
 * \version    1.0
 *
 * \brief SDR mainboard configuration library for I2C device communication.
 * The library contains the device register map and commands for register access.
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
 
#ifndef SDR_MAINBOARD_H_
#define SDR_MAINBOARD_H_

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
#include "SI5338.h"

#include "SEPP_API_Debug.h"


/* General SDR Definitions */
#define         SDR_HW_VERSION                          "1.0.1"
#define         SDR_SW_API_VERSION                   	"1.0.1"

#define         SDR_TEMP_ALERT_LIMIT_HIGH                   85          // Temperature sensor alert limit HIGH in degree Celsius
#define         SDR_TEMP_ALERT_LIMIT_LOW                    75          // Temperature sensor alert limit LOW in degree Celsius
#define         SDR_OPERATING_TEMP_HIGH                     85          // Maximum operating temperature in degree Celsius
#define         SDR_OPERATING_TEMP_LOW                      -40         // Minimum operating temperature in degree Celsius


/* SDR Clock Generator Frequency Plan Definitions */
#define         SDR_FP_NUM_FREQ_PLANS                       22      // number of pre-defined frequency plans
#define         SDR_FP_40M_1M5                              0       // PLL_REFCLK = 40MHz, RX_CLK = 1.5MHz
#define         SDR_FP_40M_1M75                             1
#define         SDR_FP_40M_2M5                              2
#define         SDR_FP_40M_3M                               3
#define         SDR_FP_40M_3M84                             4
#define         SDR_FP_40M_5M                               5       // PLL_REFCLK = 40MHz, RX_CLK = 5MHz
#define         SDR_FP_40M_5M5                              6
#define         SDR_FP_40M_6M                               7
#define         SDR_FP_40M_7M                               8
#define         SDR_FP_40M_8M75                             9
#define         SDR_FP_40M_10M                              10      // PLL_REFCLK = 40MHz, RX_CLK = 10MHz (default)
#define         SDR_FP_40M_12M                              11
#define         SDR_FP_40M_14M                              12
#define         SDR_FP_40M_20M                              13
#define         SDR_FP_40M_24M                              14
#define         SDR_FP_40M_28M                              15      // PLL_REFCLK = 40MHz, RX_CLK = 28MHz
#define         SDR_FP_40M_32M                              16
#define         SDR_FP_40M_36M                              17
#define         SDR_FP_40M_40M                              18      // PLL_REFCLK = 40MHz, RX_CLK = 40MHz
#define			SDR_FP_40M_60M		   		    			19		// PLL_REFCLK = 40MHz, RX_CLK = 60MHz
#define			SDR_FP_40M_76M8				    			20		// PLL_REFCLK = 40MHz, RX_CLK = 76.8MHz
#define			SDR_FP_40M_80M				    			21		// PLL_REFCLK = 40MHz, RX_CLK = 80MHz (max)

/* SDR Board I2C component addresses */
#define         SDR_HWCTRL_I2C_ADDR                     0x18
#define         SDR_TSENSOR_I2C_ADDR                    0x4D
#define         SDR_CLKGEN_I2C_ADDR                     0x70
/* Parameters for methods */
#define         SDR_RFFE_INPUT_LNA1                         1
#define         SDR_RFFE_INPUT_LNA2                         2
#define         SDR_RFFE_INPUT_LNA3                         3
/* SDR Board HW controller initial Register values */
#define         SDR_HWCTRL_POLINV_INIT                  0x00        // init value for input pin polarity inversion: No Inputs Inverted
#define         SDR_HWCTRL_CONFIG_INIT                  0xC2        // init value for IO pin directions: IO7-0 = [In,In,Out,Out,Out,Out,In,Out]
#define         SDR_HWCTRL_OUT_INIT                     0x0D        // init value for output levels: Inputs = Low, Outputs IO5-4 = Low, Outputs IO3,2,0 = High
/* SDR Board HW controller register masks */
#define         SDR_HWCTRL_MASK_RFFE_RX_RESET           0x01        // register bit mask of RFFE RX reset
#define         SDR_HWCTRL_MASK_RFFE_GPIO0              0x04        // register bit mask of RFFE GPIO0 signal
#define         SDR_HWCTRL_MASK_RFFE_GPIO1              0x08        // register bit mask of RFFE GPIO0 signal
#define         SDR_HWCTRL_MASK_RFFE_RX_EN              0x10        // register bit mask of RFFE RX enable (RX_EN) signal
#define         SDR_HWCTRL_MASK_RX_LNA_EN               0x20        // register bit mask of RX LNA enable (RX_LNA_EN) signal
#define         SDR_HWCTRL_MASK_TSENSE_ALERTN           0x80        // register bit mask of TSENSE_ALERTn signal
/* SDR Board HW controller Pin Numbers */
#define         SDR_HWCTRL_PIN_RFFE_RX_RESET                0       // pin number of RFFE RX reset (RX_RESET) signal
#define         SDR_HWCTRL_PIN_RFFE_GPIO0                   2       // pin number of RFFE GPIO0 signal
#define         SDR_HWCTRL_PIN_RFFE_GPIO1                   3       // pin number of RFFE GPIO0 signal
#define         SDR_HWCTRL_PIN_RFFE_RX_EN                   4       // pin number of RFFE RX enable (RX_EN) signal
#define         SDR_HWCTRL_PIN_RX_LNA_EN                    5       // pin number of RX LNA enable (RX_LNA_EN) signal
#define         SDR_HWCTRL_PIN_TSENSE_ALERTN                7       // pin number of temperature sensor Alert_n (TSENSE_ALERTn) signal
/* SDR Board Clock Generator Channel Numbers */
#define         SDR_CHANNEL_LMS_RX_PLL_REFCLK       0       // Clock Generator Channel 0 = LMS PLL Reference clock
#define         SDR_CHANNEL_LMS_RX_CLK              1       // Clock Generator Channel 1 = LMS PLL Reference clock
#define         SDR_CHANNEL_SEPP_RX_CLK             2       // Clock Generator Channel 2 = LMS PLL Reference clock

/** SDR Mainboard class
*   \brief Contains members for device used to monitor and control the
*   board signals and components.
*   Methods to set board configuration signals and to retrieve board status.
*/
class SDR_MAINBOARD
{
    private:

        MAX7310 *ctrl_ = NULL;
        TMP175 *tsense_ = NULL;
        SI5338 *clkgen_ = NULL;

        std::string description_;
        float RFFE_PLL_CLK_in_MHz_[SDR_FP_NUM_FREQ_PLANS];      //!< RF-FE PLL Clock in MHz defined by Frequency Plan
        float RFFE_RX_CLK_in_MHz_[SDR_FP_NUM_FREQ_PLANS];       //!< RF-FE RX Sampling Clock in MHz by Frequency Plan

        SI5338_Frequency_Plan fp_[SDR_FP_NUM_FREQ_PLANS];		//!< local list of SI5338 frequency plans
        void Init_Freq_Plans(void);								//!< initialize frequency plan register values

    public:
        SDR_MAINBOARD();
        virtual ~SDR_MAINBOARD();

        void Initialize_Components();							//!< Initialize on-board component (device register initialization)

        /* ----- Board Description and Infos -----  */
        std::string GetDeviceDescription(void);            		//!< Get SDR device description
        void PrintDeviceInfo(void);                          	//!< Print device info of the different board devices

        /* ----- On-Board Hardware Configuration ----- */
        void Set_RFFE_LNA_Input(unsigned int LNA_number);   	//!< Set the RF-FE input switch to LNA1, LNA2 or LNA3
        unsigned int Get_RFFE_LNA_Input(void);  				//!< Set the RF-FE input switch to LNA1, LNA2 or LNA3
        void Enable_RFFE_RX(void);      						//!< activate RF-FE RX hardware 
        void Disable_RFFE_RX(void);    							//!< deactivate RF-FE RX hardware 
        void Reset_RFFE(void);      							//!< reset the RFFE hardware
        void Enable_RX_LNA(void);       						//!< activate RX LNA hardware 
        void Disable_RX_LNA(void);      						//!< deactivate RX LNA hardware
        unsigned int Check_TempSensor_HW_Alert(void);			//!< check temperature sensor HW alert

        /* ----- On-board Clock Generator ----- */
        // void Upload_Register_Map(std::string filename);
        void Set_ClkGen_Frequency_Plan(unsigned int fp_number);	//!< set clock generator frequency plan (set PLL REF and RX CLK)
        unsigned int Get_ClkGen_Frequency_Plan(void);  			//!< get clock generator frequency plan
        void Enable_RX_Sampling_Clock(void);					//!< enable RX sampling clock
        void Disable_RX_Sampling_Clock(void);					//!< disable RX sampling clock
        void Enable_PLL_Clock(void);							//!< enable PLL reference clock
        void Disable_PLL_Clock(void);							//!< disable PLL reference clock

        /* ---- On-Board Temperature Sensor ----- */
        float Get_Temperature_degC(void);						//!< get on-board temperature in degC

};

/** SDR Mainboard Exception Class
*   \brief This exception type is returned in case of an exception in the SDR board class.
*/
class SDR_MAINBOARD_Exception : public std::exception
{
  private:
    std::string e_;
    
  public:
    SDR_MAINBOARD_Exception(std::string e) : e_(e) {}
    ~SDR_MAINBOARD_Exception() throw () {}
    const char* what() const throw() { return e_.c_str(); }
};

#endif
