/** LMS6002 Device Library
 *
 * \file LMS.h
 *
 * \project    OPS-SAT Project
 * \copyright  Graz University of Technology, IKS
 * \author     Reinhard Zeif, Fabian Golser
 * \date       31.May.2020
 *
 * \version		1.0		Initial release 									15.Nov.2019
 * \version 	1.1		"METHODEND" and return commands fixed to correct failures with
 * 						indentation of debug output 						27.May.2020
 * \version		1.2		LMS calibration check lock method reworked 			28.May.2020
 * \version		1.3		CapAutoTune_S debug/error output reworked			29.May.2020
 * \version		1.4		Method added to save/restore RegMap parameters,
 * 						MemCopy Method updated, some minor fixes			31.May.2020
 * \version		1.5		Bugfix VCO Capacitance Tuning						08.May.2021
 *
 * \note Codebase provided by Lime Microsystems (www.limemicro.com)
 *
 * \brief LMS main class with specific device commands.
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

#ifndef LMS_H_
#define LMS_H_


#include <typeinfo>
#include <string.h>
#include <map>
#include <iomanip>
#include <bitset>
#include <iostream>
#include <unistd.h>
#include <math.h>


#include "LMS_Definitions.h"
#include "SEPP_API_Debug.h"
#include "LMS_Parameter.h"
#include "LMS_RegisterMap.h"
#include "sepp_spi_dev.h"


/* General SDR Definitions */
#define			LMS_HW_VERSION								"1.0.1"
#define			LMS_SW_API_VERSION							"1.5"

enum eLMS_CHANNEL
{
	RX_CHANNEL,
	TX_CHANNEL
};

/** LMS6002 Device Class
 * \brief LMS6002 Device Class with register map and device specific methods
 */
class LMS : public SEPP_SPI_DEV
{
  public:
	/* --------------- Constructors -----------------*/
    LMS();
    LMS(double refFreqPLL_in_MHz);
    LMS(double refFreqPLL_in_MHz, std::string spi_dev_name, unsigned int mode, unsigned int data_rate_in_Hz);
    
    /* ---------------- Destructor ------------------*/
    virtual ~LMS();

    /* ----------------- General ------------------- */
    void Connect(std::string spi_dev_name, unsigned int mode, unsigned int data_rate_in_Hz);
    void initializeRegistersToDefaults(void);	//!< Initialize LMS Registers (default values and LMS specific updates).
    void PrintInfo(void);

    void Enable(eLMS_CHANNEL channel);
    void Disable(eLMS_CHANNEL channel);
    int CheckForDevice(void);

    void loadRegisterMapFromConfigFile(const char *LMSConfig_Filename);
    void saveRegisterMapToConfigFile(const char *LMSConfig_Filename, bool binary = true);

    double getPLLReferenceFrequency();
    void setPLLReferenceFrequency(double refFreqPLL_in_MHz);

    eLMS_PARAMETER string2LMSParameter(const char *str);

    /* ----------------- Receiver ------------------- */
    double setRXCarrierFrequency(double frequencyInGHz);
    double getRXCarrierFrequency();
    
    // set/get RX LNA
    void setActiveLNA(unsigned int LNA_number);
    unsigned int getActiveLNA(void);

    // set/get RX gain
    void setRXLNAGain(unsigned int gain_mode);
    void setRXVGA1Gain(unsigned int gain_code);
    void setRXVGA2Gain(unsigned int gain_code);
    unsigned int getRXLNAGain(void);
    unsigned int getRXVGA1Gain(void);
    unsigned int getRXVGA2Gain(void);

    // set/get LPF bandwidth
    void setRXLPFBandwidth(unsigned int bandwidth_code);
    unsigned int getRXLPFBandwidth(void);

    /* ----------------- Transmitter ------------------- */
    double setTXCarrierFrequency(double frequencyInGHz);
    double getTXCarrierFrequency();
    void setTXVGA1Gain(unsigned int gain_code);

    /* ----------------- Calibration ------------------- */
    void AutoCalibration();
    void CalibrateTx();
    void CalibrateRx();
    void CalibrateLPFCore();

private:

    LMS_RegisterMap *registerMap_;
    /**
     * \brief frequencyMap_ is a map containing the frequency table from the
     * datasheet page 7: Tx: 0x15, Rx: 0x25 7-2 FREQSEL[5:0].
     * It is used to calculate the PLL frequency.
     */
    std::map<double, unsigned> PLLFrequencyMap_;

    double refFreqPLL_in_MHz_;  	// Frequency of PLL reference clock input signal
    unsigned int clock_enable_;
    unsigned rgrDecode_, chbPwrPllMods_, rgrMODE_, cmbVCOCAP_, rgrSELVCO_, rgrFRANGE_, DesFreq_MHz_;

    void setRegister(const unsigned char address, const unsigned char value);
    unsigned char getRegister(const unsigned char address);
    void setParameter(const eLMS_PARAMETER parameter, const unsigned long value);
    unsigned long getParameter(const eLMS_PARAMETER parameter);

    void initPLLFrequencyMap();
    
    unsigned int GetVTuneState(const bool Rx);
    int CapAutoTune_S(const bool Rx);
    bool VCOSelection(const bool Rx);
    bool CapIncrement(const bool Rx, unsigned int UntilState);
    bool CapDecrement(const bool Rx, unsigned int UntilState);

    void EnableSPIClocksByMask(const unsigned int mask);
    void StoreClockEnableParameter(void);
    void RestoreClockEnableParameter(void);

    void computePLLFrequencyParameters(double frequencyInGHz, unsigned *nint, unsigned *nfrac, unsigned *frange);

    void printMap();

    void DCResetCalibration(eLMS_MODULE module);
    void DCLoadValues(eLMS_MODULE module, unsigned value);
    void DCCalibration(eLMS_MODULE module);
    void DCStartCalibration(eLMS_MODULE module);
    bool DCCalibrationCheckLock(eLMS_PARAMETER param_lock, eLMS_PARAMETER param_calib_done, eLMS_PARAMETER dc_reg_val);
    void SetPLLTo320MHz();
    void RestoreAfterSetPLLTo320MHz();

    int getModuleAddress(eLMS_MODULE module);
    eLMS_MODULE string2LMSModule(const char *str);
};

/** LMS6002 Device Exception Class
* \brief This exception type is returned in case of an exception in the LMS device class.
*/
class LMS_Exception : public std::exception
{
  private:
	std::string e_;

  public:
	LMS_Exception(std::string e) : e_(e) { }
	~LMS_Exception() throw () {}
	const char* what() const throw() { return e_.c_str(); }
};


#endif // LMS_H
