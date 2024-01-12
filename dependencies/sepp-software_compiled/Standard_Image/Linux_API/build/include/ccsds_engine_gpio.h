/** CCSDS Engine GPIO interface
 *
 * \file ccsds_engine_gpio.h
 *
 * Generic Class for SEPP CCSDS-Engine Monitor & Control Interface
 *
 * This library can be used to monitor and control the CCSDS-Engine from SEPP SoC user-space software.
*
* \project  		OPS-SAT SEPP C++ API Library
*
* \organisation IKS, TU Graz, Inffeldgasse 12/I, 8010 Graz
 * \date       5. Feb. 2019
 * \author     Reinhard Zeif, MSc
 *
 * \version    1.0
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
 
 
#ifndef SEPP_CCSDS_ENGINE_CONTROL_H
#define SEPP_CCSDS_ENGINE_CONTROL_H

#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "sepp_gpio.h"
#include "SEPP_API_Debug.h"


/* CCSDS RF Band Definitions */
#define 	CCSDS_Engine_GPIO_RF_BAND_S						0					//!< CCSDS Engine GPIO band definition is S-Band
#define 	CCSDS_Engine_GPIO_RF_BAND_X						1					//!< CCSDS Engine GPIO band definition is X-Band

/* General FPGA PIO direction definitions */
#define		CCSDS_Engine_GPIO_DIRECTION_INPUT				1				//!< CCSDS Engine GPIO direction is input
#define		CCSDS_Engine_GPIO_DIRECTION_OUTPUT				0			//!< CCSDS Engine GPIO direction is output
/* General FPGA PIO pin level definitions */
#define		CCSDS_Engine_GPIO_PIN_LOW						0						//!< CCSDS Engine GPIO voltage level is low
#define		CCSDS_Engine_GPIO_PIN_HIGH						1				//!< CCSDS Engine GPIO voltage level is high


/* --------------------------- GPIO pin numbers ----------------------------------*/
#define		CCSDS_Engine_GPIO_PINNUMBER_BAND_CTRL			416		//!< PIO pin number in /sys/class/gpio directory for BAND_CONTROL output (input at engine)
#define		CCSDS_Engine_GPIO_PINNUMBER_BAND_STATE			415		//!< PIO pin number in /sys/class/gpio directory for BAND_STATE input (output at engine)
#define		CCSDS_Engine_GPIO_PINNUMBER_BYPASS_CTRL_UL		417		//!< PIO pin number in /sys/class/gpio directory for BYPASS_CTRL_UL output (input at engine)
#define		CCSDS_Engine_GPIO_PINNUMBER_BYPASS_CTRL_DL		418		//!< PIO pin number in /sys/class/gpio directory for BYPASS_CTRL_DL output (input at engine)
#define		CCSDS_Engine_GPIO_PINNUMBER_BYPASS_STATE_UL		419		//!< PIO pin number in /sys/class/gpio directory for BYPASS_STATE_UL input (output at engine)
#define		CCSDS_Engine_GPIO_PINNUMBER_BYPASS_STATE_DL		420		//!< PIO pin number in /sys/class/gpio directory for BYPASS_STATE_DL input (output at engine)

/** CCSDS Engine GPIO Device Class
*/
class CCSDS_Engine_GPIO
{

  public:
    CCSDS_Engine_GPIO(void);				
    virtual ~CCSDS_Engine_GPIO(void);

    void PrintDeviceInfo();

    void Init(void);
    void Set_RF_Band_Control(unsigned int rf_band);
    unsigned int Get_RF_Band_State(void);
    void Set_Downlink_Bypass_Ctrl(unsigned int high_or_low);
    void Set_Uplink_Bypass_Ctrl(unsigned int high_or_low);
    unsigned int Get_Downlink_Bypass_State(void);
    unsigned int Get_Uplink_Bypass_State(void);


  private:
    SEPP_GPIO *gpio_band_ctrl_ = NULL;
    SEPP_GPIO *gpio_band_state_ = NULL;
    SEPP_GPIO *gpio_bypass_ctrl_ul_ = NULL;
    SEPP_GPIO *gpio_bypass_ctrl_dl_ = NULL;
    SEPP_GPIO *gpio_bypass_state_ul_ = NULL;
    SEPP_GPIO *gpio_bypass_state_dl_ = NULL;

};

/** CCSDS Engine Exception Class
*/
class CCSDS_Engine_GPIO_Exception : public std::exception
{
  private:
	std::string e_;
	
  public:
	CCSDS_Engine_GPIO_Exception(std::string e) : e_(e) {}
	~CCSDS_Engine_GPIO_Exception() throw () {}
	const char* what() const throw() { return e_.c_str(); }
};


#endif
