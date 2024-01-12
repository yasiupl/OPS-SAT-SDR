/** OPS-SAT SEPP C++ API Library - SEPP Software Defined Radio (SDR)
*
* \file SEPP_SDR_API.h
*
* SEPP Software Defined Radio API Library.
*
* \project			OPS-SAT SEPP C++ API Library
*
* \organisation		IKS, TU Graz, Inffeldgasse 12/I, 8010 Graz
* \author			Reinhard Zeif, MSc
* \date				07.Dec.2020
*
* \version 			0.1  	first draft														Dec.2017
* \version			0.2		EM Test															Jan. 2018
* \version			0.3		FM Test															Oct. 2018
* \version			1.0		SDR Commissioning version in-orbit								08.May.2020
* \version			1.1		Constructor clean-up, Print_Info methods reworked				26.May.2020
* \version 			1.2		"METHODEND" and return commands fixed to correct failures with
* 							indentation of debug output, msgdma debug output added,
* 							LMS check lock method reworked									28.May.2020
* \version			1.3		Get_RX_Sampling_Frequency method fixed: return and subcommand call
* 							updated because METHODEND was called too early					29.May.2020
* \version			1.4		FPGA_MW reworked, LMS methods extended for parameter save/restore,
* 							minor reworks and fixes											31.May.2020
* \version			1.5		Helper method added to convert enumerators to MHz value			07.Dec.2020
*
*
* \copyright (C) 2017-2020 Graz University of Technology / IKS. All rights reserved.
* Your use of TU Graz design tools, logic functions and other 
* software and tools and any output files any of the foregoing 
* (including device programming or simulation files), and any associated 
* documentation or information are expressly subject to the terms and 
* conditions of the applicable license agreement, including, without 
* limitation, that your use is for the sole purpose of programming the 
* OPS-SAT processing platform.
*
*/

#ifndef SEPP_SDR_API_H_
#define SEPP_SDR_API_H_


#include <typeinfo>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <math.h>

#include "SEPP_API_Debug.h"
#include "LMS.h"
#include "sdr_mainboard.h"
#include "sdr_fpga_mw.h"

/*! \defgroup SEPP_SDR_API SEPP SDR C++ API Library
* \{
*/


/********************** GENERAL Definitions *************************/

#define SEPP_SDR_API_VERSION_STRING_BASE	"v1.5, 07.Dec.2020"
#ifdef GIT_VERSION
#define SEPP_SDR_API_VERSION_STRING   SEPP_SDR_API_VERSION_STRING_BASE << " " << GIT_VERSION
#else
#define SEPP_SDR_API_VERSION_STRING   SEPP_SDR_API_VERSION_STRING_BASE
#endif

/********************************************************************/


/* SDR RFFE RX Sampling Frequency Enumerator
 * A predefined set of frequencies is available that can be
 * used for data sampling with RF front end.
 */
enum eSDR_RFFE_RX_SAMPLING_FREQ
{
	RFFE_RX_SAMPLING_1M5 = 0,       // 1.5 MHz
	RFFE_RX_SAMPLING_1M75 = 1,      // 1.75 MHz
	RFFE_RX_SAMPLING_2M5 = 2,       // 2.5 MHz
	RFFE_RX_SAMPLING_3M = 3,        // 3.0 MHz
	RFFE_RX_SAMPLING_3M84 = 4,      // 4.0 MHz
	RFFE_RX_SAMPLING_5M = 5,        // 5.0 MHz
	RFFE_RX_SAMPLING_5M5 = 6,       // 5.5 MHz
	RFFE_RX_SAMPLING_6M = 7,        // 6.0 MHz
	RFFE_RX_SAMPLING_7M = 8,        // 7.0 MHz
	RFFE_RX_SAMPLING_8M75 = 9,      // 8.75 MHz
	RFFE_RX_SAMPLING_10M = 10,      // 10.0 MHz (default)
	RFFE_RX_SAMPLING_12M = 11,      // 12.0 MHz
	RFFE_RX_SAMPLING_14M = 12,      // 14.0 MHz
	RFFE_RX_SAMPLING_20M = 13,      // 20.0 MHz
	RFFE_RX_SAMPLING_24M = 14,      // 24.0 MHz
	RFFE_RX_SAMPLING_28M = 15,      // 28.0 MHz
	RFFE_RX_SAMPLING_32M = 16,      // 32.0 MHz
	RFFE_RX_SAMPLING_36M = 17,      // 36.0 MHz
	RFFE_RX_SAMPLING_40M = 18,      // 40.0 MHz
	RFFE_RX_SAMPLING_60M = 19,      // 60.0 MHz
	RFFE_RX_SAMPLING_76M8 = 20,     // 76.8 MHz
	RFFE_RX_SAMPLING_80M = 21       // 80.0 MHz (max)

};

/* SDR RX RFFE Low-Pass Filter Bandwidth Enumerator
 * A predefined set of RX baseband low-pass filter bandwidths
 * is available that can be used to limit the frequency range
 * of the baseband signal received with RF front end.
 */
enum eSDR_RFFE_RX_LPF_BW
{
	RFFE_RX_LPFBW_14M = 0,          // 14.0 MHz (default)
	RFFE_RX_LPFBW_10M = 1,          // 10.0 MHz
	RFFE_RX_LPFBW_7M = 2,           // 7.0 MHz
	RFFE_RX_LPFBW_6M = 3,           // 6.0 MHz
	RFFE_RX_LPFBW_5M = 4,           // 5.0 MHz
	RFFE_RX_LPFBW_4M375 = 5,        // 4.375 MHz
	RFFE_RX_LPFBW_3M5 = 6,          // 3.5 MHz
	RFFE_RX_LPFBW_3M = 7,           // 3.0 MHz
	RFFE_RX_LPFBW_2M75 = 8,         // 2.75 MHz
	RFFE_RX_LPFBW_2M5 = 9,          // 2.5 MHz
	RFFE_RX_LPFBW_1M92 = 10,        // 1.92 MHz
	RFFE_RX_LPFBW_1M5 = 11,         // 1.5 MHz
	RFFE_RX_LPFBW_1M375 = 12,       // 1.375 MHz
	RFFE_RX_LPFBW_1M25 = 13,        // 1.25 MHz
	RFFE_RX_LPFBW_0M875 = 14,       // 0.875 MHz
	RFFE_RX_LPFBW_0M75 = 15         // 0.75 MHz
};

enum eSDR_RFFE_INPUT
{
	RFFE_INPUT_1 = 1,		// lower band
	RFFE_INPUT_2 = 2,		// upper band
	RFFE_INPUT_3 = 3,		// broadband input
	RFFE_INPUT_DISCONNECTED = 4  // disconnected
};


/** SEPP C++ API Library - SEPP SDR Device Class
 * Contains members for device address and general information.
 * Set and Get functions for device address, description and interface type.
 * Methods to generate correct message sequences for write and read commands.
*/
class SEPP_SDR_API
{
	private:
		SDR_MAINBOARD *mb_;				//!< bus expander used for PCB configuration
		LMS *lms_;								//!< LMS SPI monitor and control interface (API)
		SDR_FPGA_MW *fpga_mw_;		//!< FPGA middleware with mSGDMA and RegMap for SDR

	public:
		SEPP_SDR_API();
		virtual ~SEPP_SDR_API();

		void Print_Info(void);

		/* -------- LMS register initialization -------- */
		void Init_RFFE_Registers(void);														//!< Initialize RF frontend registers to their default values

		/* --------- Temperature Sensor methods -------- */
		float Get_Temperature_degC(void);													//!< Get on-board temperature in degC

		/* ------------- RFFE configuration ------------ */
		void Set_RF_Frontend_Input(eSDR_RFFE_INPUT channel);								//!< Set the RF frontend Input (LNA 1 to 3)
		eSDR_RFFE_INPUT Get_RF_Frontend_Input(void);										//!< Get the RF frontend Input (LNA 1 to 3)
		void Reset_RF_Fronend(void);														//!< Reset the RF frontend module
		void Enable_Receiver(void);															//!< Enable the Receiver (via GPIO)
		void Disable_Receiver(void);														//!< Disable the Receiver (via GPIO)
		void Enable_RX_Sampling_Clock(void);												//!< Enable the RX Sampling Clock (clock generator)
		void Disable_RX_Sampling_Clock(void);												//!< Disable the RX Sampling Clock (clock generator)
		void Calibrate_RF_Frontend(void);													//!< Calibrate the RF Frontend (via SPI)

		/* ------------------- RFFE Gain --------------- */
		void Set_RX_Gain_in_dB(unsigned int gain_in_dB);									//!< Set RX Gain in dB (full chain)
		unsigned int Get_RX_Gain_in_dB(void);												//!< Get RX Gain in dB (full chain)

		/* ------------- RFFE Frequencies  ------------- */
		double Set_RX_Carrier_Frequency_in_GHz(double freq_in_GHz);							//!< Set RX Carrier Frequency in GHz
		double Get_RX_Carrier_Frequency_in_GHz(void);										//!< Get RX Carrier Frequency in GHz
		void Set_RX_Sampling_Frequency(eSDR_RFFE_RX_SAMPLING_FREQ sampling_freq_code);		//!< Set RX Sampling Frequency (clock generator)
		eSDR_RFFE_RX_SAMPLING_FREQ Get_RX_Sampling_Frequency(void);							//!< Get RX Sampling Frequency (clock generator)
		void Set_RXLPF_Bandwidth(eSDR_RFFE_RX_LPF_BW lfp_bandwidth_code);					//!< Set RX Low-Pass Filter Bandwidth
		eSDR_RFFE_RX_LPF_BW Get_RXLPF_Bandwidth(void);										//!< Get RX Low-Pass Filter Bandwidth

		/* ----------- FPGA Middleware ----------------- */
		void Receive_IQ_Samples(unsigned int *samples, unsigned int number_of_samples);		//!< Receive IQ Samples (via FPGA DMA writer)

		/* -------------- Helper methods ----------------*/
		float rxsf_to_MHz(eSDR_RFFE_RX_SAMPLING_FREQ fs_cfg);								//!< Get RX sampling frequency in MHz from enumerator
		float rxlpfbw_to_MHz(eSDR_RFFE_RX_LPF_BW lpfbw_cfg);								//!< Get RX LPF bandwidth in MHz from enumerator

};

/** SEPP C++ API Library - SEPP SDR Device Exception
*
*/
class SEPP_SDR_API_Exception : public std::exception
{
  private:
      std::string e_;
    
  public:
      SEPP_SDR_API_Exception(std::string e) : e_(e) {}
      ~SEPP_SDR_API_Exception() throw () {}
      const char* what() const throw() { return e_.c_str(); }
};

/** \} */ // End of Group

#endif
