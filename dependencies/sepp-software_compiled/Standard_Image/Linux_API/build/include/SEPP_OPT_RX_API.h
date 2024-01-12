/** OPS-SAT SEPP C++ API Library - Optical Receiver
 * 
 * \file SEPP_OPT_RX_API.h
 *
 * \brief The class can be used to access the Optical-RX connected to the SEPP.
 * The library contains the device specific commands.
 *
 * \project  			OPS-SAT SEPP C++ API Library
 *
 * \organisation	Graz University of Technology
 * \author     		Reinhard Zeif, MSc
 * \date       		16. Nov. 2021
 *
 * \version    		0.1		initial release								Apr. 2019
 * \version    		0.2		tested with EM								Apr. 2019
 * \version    		0.3		tested with FM								Apr. 2019
 * \version			1.0		Commissioning version in-orbit  			Dec. 2019
 * \version			1.1		Print_Info debug output added with
 * 							version string								29.May.2020
 * \version			1.2		Shared Memory Erase and Read methods updated and extended
 * 							with some debug output, #defines added		16.Nov.2021
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
 **/


#ifndef SEPP_OPT_RX_API_H_
#define SEPP_OPT_RX_API_H_

#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>


#include "optrx_mainboard.h"
#include "optrx_mcu.h"

#include "SEPP_API_Debug.h"


/*! \defgroup SEPP_OPT_RX_API SEPP OPT-RX C++ API Library
* \{
*/

/* -------------------------- General Definitions ----------------------- */

#define SEPP_OPTRX_API_VERSION_STRING		"v1.2, 16.Nov.2021"
#ifdef GIT_VERSION
#define SEPP_IADCS_API_VERSION_STRING	SEPP_IADCS_API_VERSION_STRING_BASE << " " << GIT_VERSION
#else
#define SEPP_IADCS_API_VERSION_STRING	SEPP_IADCS_API_VERSION_STRING_BASE
#endif

/* ----------------------- End of global definitions -------------------- */

/** SEPP C++ API Library - OPT-RX device class
* 	\brief Contains members for device address and general information. 
*		Set and Get functions for device address, description and interface type.
* 	Methods to generate correct message sequences for write and read commands.
*/
class SEPP_OPT_RX_API
{
	private:

		OPTRX_MAINBOARD *mb_;            // OPT-RX mainboard
		
	public:
		SEPP_OPT_RX_API();
		virtual ~SEPP_OPT_RX_API();

		/* -------------- Generic Device Info ---------------- */
        void Print_Info(void);

		/* ----- Temperature Sensor methods ----- */
		float Get_Temperature_degC(void);
		
		/* ----- Board configuration methods ----- */
	    void Set_SharedMemory_IF_Switch(unsigned int MCU_or_External);
	    unsigned int Get_SharedMemory_IF_Switch(void);

	    /* ----- MCU monitor and control methods ----- */
		//void Enable_RX_Detector(void);
		//void Disable_RX_Detector(void);
		//unsigned int Get_RX_Detector_Reference(void);
		//void Set_RX_Detector_Reference(unsigned int ref_sel);

	    /* ------ Shared Memory access methods ----- */
		unsigned int Read_SharedMemory(unsigned char *buffer, unsigned int number_of_bytes);
		unsigned int Erase_SharedMemory_SubSector_4KB(unsigned int subsector_address, unsigned int number_of_subsectors);

};

/** SEPP C++ API Library - OPT-RX Device Exception
*/
class SEPP_OPT_RX_API_Exception : public std::exception
{
  private:
	  std::string e_;
	
  public:
	  SEPP_OPT_RX_API_Exception(std::string e) : e_(e) {}
	  ~SEPP_OPT_RX_API_Exception() throw () {}
	  const char* what() const throw() { return e_.c_str(); }
};

/** \} */ // End of Group

#endif
