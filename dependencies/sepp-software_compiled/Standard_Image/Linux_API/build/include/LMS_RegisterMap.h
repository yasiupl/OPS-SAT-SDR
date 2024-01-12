/** LMS6002 Device Library - Register Map
 *
 * \file LMS_RegisterMap.h
 *
 * \project    OPS-SAT Project
 * \copyright  Graz University of Technology, IKS
 * \author     Reinhard Zeif, Fabian Golser
 * \date       15. Nov. 2017
 * \version    1.0
 *
 * \note Codebase provided by Lime Microsystems (www.limemicro.com)
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

#ifndef LMS_REGISTERSMAP_H
#define LMS_REGISTERSMAP_H


#include <map>
#include <vector>
#include <iostream>
#include <iomanip>      // field width in cout
#include <sstream>
#include <typeinfo>
#include <iomanip>
#include <bitset>
#include <unistd.h>

#include "LMS_Definitions.h"
#include "SEPP_API_Debug.h"
#include "sepp_spi_dev.h"
#include "LMS_Parameter.h"
#include "LMS_Register.h"
#include "LMS_ConfigFile.h"


/** LMS6002dr2 modules enumerator
*/
enum eLMS_MODULE
{
  UNDEFINED_MODULE = -1,
  TOP,
  TXPLL,
  RXPLL,
  TXLPF,
  TXRF,
  RXLPF,
  RXVGA2,
  RXFE,
  ADDC,
  LMS_MODULE_COUNT
};

/** LMS6002dr2 parameters enumerator
*/
enum eLMS_PARAMETER
{
  LMS_PARAM_UNDEFINED = 0,
  //TOP module
  DC_REGVAL_TOP,
  RCCAL_LPFCAL,
  DC_LOCK_TOP,
  DC_CLBR_DONE_TOP,
  DC_UD_TOP,
  DC_CNTVAL_TOP,
  DC_START_CLBR_TOP,
  DC_LOAD_TOP,
  DC_SRESET_TOP,
  DC_ADDR_TOP,
  VER,
  REV,
  DECODE_TOP,
  SRESET,
  EN_TOP,
  STXEN,
  SRXEN,
  TFWMODE,
  CLKSEL_LPFCAL,
  PD_CLKLPFCAL,
  ENF_EN_CAL_LPFCAL,
  RST_CAL_LPFCAL,
  EN_CAL_LPFCAL,
  FORCE_CODE_CAL_LPFCAL,
  BWC_LPFCAL,
  BBBYP,
  LBEN_LPFIN,
  LBEN_VGA2IN,
  LBEN_OPIN,
  LBRFEN,
  RXOUTSW,
  CLK_EN,
  CLK_EN_6,
  CLK_EN_5,
  CLK_EN_4,
  CLK_EN_3,
  CLK_EN_2,
  CLK_EN_1,
  CLK_EN_0,
  FDDTDD,
  TDDMOD,
  PDXCOBUF,
  SLFBXCOBUF,
  BYPXCOBUF,
  MASK,
  PD_1_TOP,
  PD_0_TOP,

  //RxPLL
  NINT_RXPLL,
  NFRAC_RXPLL,
  DITHEN_RXPLL,
  DITHN_RXPLL,
  EN_RXPLL,
  AUTOBYP_RXPLL,
  DECODE_RXPLL,
  MODE_RXPLL,
  SELVCO_RXPLL,
  FRANGE_RXPLL,
  FREQSEL_RXPLL,
  SELOUT_RXPLL,
  EN_PFD_UP_RXPLL,
  OEN_TSTD_SX_RXPLL,
  PASSEN_TSTOD_SD_RXPLL,
  ICHP_RXPLL,
  BYPVCOREG_RXPLL,
  PDVCOREG_RXPLL,
  FSTVCOBG_RXPLL,
  OFFUP_RXPLL,
  VOVCOREG_RXPLL,
  OFFDOWN_RXPLL,
  VCOCAP_RXPLL,
  VTUNE_H_RXPLL,
  VTUNE_L_RXPLL,
  BCODE_RXPLL,
  ACODE_RXPLL,
  PD_VCOCOMP_SX_RXPLL,
  ENLOBUF_RXPLL,
  ENLAMP_RXPLL,
  TRI_RXPLL,
  POL_RXPLL,
  PFDPD_RXPLL,
  ENFEEDDIV_RXPLL,
  PFDCLKP_RXPLL,
  BCLKSEL_RXPLL,
  BINSEL_RXPLL,
  BSTART_RXPLL,
  BSTATE_RXPLL,
  BSIG_RXPLL,

  //TXPLL
  NINT_TXPLL,
  NFRAC_TXPLL,
  DITHEN_TXPLL,
  DITHN_TXPLL,
  EN_TXPLL,
  AUTOBYP_TXPLL,
  DECODE_TXPLL,
  MODE_TXPLL,
  SELVCO_TXPLL,
  FRANGE_TXPLL,
  FREQSEL_TXPLL,
  SELOUT_TXPLL,
  EN_PFD_UP_TXPLL,
  OEN_TSTD_SX_TXPLL,
  PASSEN_TSTOD_SD_TXPLL,
  ICHP_TXPLL,
  BYPVCOREG_TXPLL,
  PDVCOREG_TXPLL,
  FSTVCOBG_TXPLL,
  OFFUP_TXPLL,
  VOVCOREG_TXPLL,
  OFFDOWN_TXPLL,
  VCOCAP_TXPLL,
  VTUNE_H_TXPLL,
  VTUNE_L_TXPLL,
  BCODE_TXPLL,
  ACODE_TXPLL,
  PD_VCOCOMP_SX_TXPLL,
  ENLOBUF_TXPLL,
  ENLAMP_TXPLL,
  TRI_TXPLL,
  POL_TXPLL,
  PFDPD_TXPLL,
  ENFEEDDIV_TXPLL,
  PFDCLKP_TXPLL,
  BCLKSEL_TXPLL,
  BINSEL_TXPLL,
  BSTART_TXPLL,
  BSTATE_TXPLL,
  BSIG_TXPLL,

  //TX LPF
  DC_REGVAL_TXLPF,
  DC_LOCK_TXLPF,
  DC_CLBR_DONE_TXLPF,
  DC_UD_TXLPF,
  DC_CNTVAL_TXLPF,
  DC_START_CLBR_TXLPF,
  DC_LOAD_TXLPF,
  DC_SRESET_TXLPF,
  DC_ADDR_TXLPF,
  BWC_LPF_TXLPF,
  EN_TXLPF,
  DECODE_TXLPF,
  BYP_EN_LPF_TXLPF,
  DCO_DACCAL_TXLPF,
  TX_DACBUF_PD_TXLPF,
  RCCAL_LPF_TXLPF,
  PD_DCOCMP_LPF_TXLPF,
  PD_DCODAC_LPF_TXLPF,
  PD_DCOREF_LPF_TXLPF,
  PD_FIL_LPF_TXLPF,

  //RXLPF,ADC,DAC
  DC_REGVAL_RXLPF,
  DC_LOCK_RXLPF,
  DC_CLBR_DONE_RXLPF,
  DC_UD_RXLPF,
  DC_CNTVAL_RXLPF,
  DC_START_CLBR_RXLPF,
  DC_LOAD_RXLPF,
  DC_SRESET_RXLPF,
  DC_ADDR_RXLPF,
  BWC_LPF_RXLPF,
  EN_RXLPF,
  DECODE_RXLPF,
  BYP_EN_LPF_RXLPF,
  DCO_DACCAL_RXLPF,

  RCCAL_LPF_RXLPF,
  PD_DCOCMP_LPF_RXLPF,
  PD_DCODAC_LPF_RXLPF,
  PD_DCOREF_LPF_RXLPF,
  PD_FIL_LPF_RXLPF,

    EN_ADC_DAC,
    DECODE_ADC_DAC,
    TX_CTRL1_64,
    TX_CTRL1_3,
    TX_CTRL1_10,

    RX_CTRL1_76,
    RX_CTRL1_54,
    RX_CTRL1_30,

    RX_CTRL2_65, // hint: failure in datasheet - there it's called RX_CTRL2_76
    RX_CTRL2_43, // hint: failure in datasheet - there it's called RX_CTRL2_54
    RX_CTRL2_21, // hint: failure in datasheet - there it's called RX_CTRL2_43
    RX_CTRL2_0,

    MISC_CTRL_9,
    MISC_CTRL_8,
    MISC_CTRL_7,
    MISC_CTRL_6,
    MISC_CTRL_5,
    RX_CTRL3_7,
    RX_CTRL3_10,

    RX_CTRL4_76,
    RX_CTRL4_54,
    RX_CTRL4_32,
    RX_CTRL4_10,

    RX_CTRL5_76,
    RX_CTRL5_54,
    RX_CTRL5_32,
    RX_CTRL5_10,

    REF_CTRL0_74,
    REF_CTRL0_30,

    REF_CTRL1_76,
    REF_CTRL1_54,
    REF_CTRL1_30,

    MISC_CTRL_4,
    MISC_CTRL_3,
    MISC_CTRL_2,
    MISC_CTRL_1,
    MISC_CTRL_0,

  //TX RF
  EN_TXRF,
  DECODE_TXRF,
  TXVGA1GAIN,
  TXVGA1DC_I,
  TXVGA1DC_Q,
  PA_EN,
  PD_DRVAUX,
  VGA2GAIN_TXVGA2,
  ENVD_2, //need to merge into one
  ENVD_10,
  PKDBW,
  LOOPBBEN,
  FST_PKDET,
  FST_TXHFBIAS,
  ICT_TXLOBUF,
  VBCAS_TXDRV,
  ICT_TXMIX,
  ICT_TXDRV,
  PW_VGA1_I,
  PW_VGA1_Q,
  PD_TXDRV,
  PD_TXLOBUF,
  PD_TXMIX,
  TXVGA1GAINT,
  G_TXVGA2,
  PD_PKDET,
  DC_REGVAL_RXVGA2,
  DC_LOCK_RXVGA2,
  DC_CLBR_DONE_RXVGA2,
  DC_UD_RXVGA2,
  DC_CNTVAL_RXVGA2,
  DC_START_CLBR_RXVGA2,
  DC_LOAD_RXVGA2,
  DC_SRESET_RXVGA2,
  DC_ADDR_RXVGA2,
  VCM_RXVGA2,
  EN_RXVGA2,
  DECODE_RXVGA2,
  VGA2GAIN_RXVGA2,
  PD_9,
  PD_8,
  PD_7,
  PD_6,
  PD_5,
  PD_4,
  PD_3,
  PD_2,
  PD_1,
  PD_0,
  VGA2GAINB,
  VGA2GAINA,
  DECODE_RXFE,
  EN_RXFE,
  IN1SEL_MIX_RXFE,
  DCOFF_I_RXFE,
  INLOAD_LNA_RXFE,
  DCOFF_Q_RXFE,
  XLOAD_LNA_RXFE,
  IP2TRIM_I_RXFE,
  IP2TRIM_Q_RXFE,
  G_LNA_RXFE,
  LNASEL_RXFE,
  CBE_LNA_RXFE,
  RFB_TIA_RXFE,
  CFB_TIA_RXFE,
  RDLEXT_LNA_RXFE,
  RDLINT_LNA_RXFE,
  ICT_MIX_RXFE,
  ICT_LNA_RXFE,
  ICT_TIA_RXFE,
  ICT_MXLOB_RXFE,
  LOBN_MIX_RXFE,
  RINEN_MIX_RXFE,
  G_FINE_LNA3_RXFE,
  PD_TIA_RXFE,
  PD_MXLOB_RXFE,
  PD_MIX_RXFE,
  PD_LNA_RXFE,

  LMS_PARAMETER_COUNT
};


/** LMS6002 Register-Map Class
 * \brief LMS6002 Register Map with registers/parameters for device monitoring and control 
 */
class LMS_RegisterMap
{
  public:

	LMS_RegisterMap(SEPP_SPI_DEV *spi_dev);
    virtual ~LMS_RegisterMap();

    std::map<eLMS_PARAMETER, LMS_RegMap_Parameter> m_parameterList;

    void SetRegistersToDefault();

    bool SetParameterValue(const eLMS_PARAMETER param, unsigned long value);
    bool SetRegisterValue(const unsigned char address, const unsigned char value);
    unsigned long GetParameterValue(const eLMS_PARAMETER param);
    unsigned char GetRegisterValue(const unsigned char address);

    const std::map<eLMS_PARAMETER, unsigned long> readAllParameters();
    void writeAllParameters(const std::map<eLMS_PARAMETER, unsigned long> registerMapParameters);
    const std::map<unsigned char, unsigned char> readAllRegisters();
    void writeAllRegisters(std::map<unsigned char, unsigned char>);

    std::map<unsigned char, unsigned char> readRegisterMapFromConfigFile(LMS_ConfigFile *config);
	void writeRegisterMapToConfigFile(LMS_ConfigFile *config, std::map<unsigned char, unsigned char> register_map);

    eLMS_PARAMETER string2Parameter(const char *str);
    eLMS_MODULE string2LMSModule(const char *str);
    int getModuleAddress(eLMS_MODULE);
    int getCalibrationGeneralDCArrayPosition(eLMS_MODULE module);

  private:

    std::map<unsigned char, LMS_RegMap_Register> m_registerList;
    SEPP_SPI_DEV *spi_;

    void initParameterList(void);
    void initRegisterList(void);
    unsigned long makeMask(unsigned msb, unsigned lsb);

};


/** LMS6002 Register-Map Exception Class
* \brief This exception type is returned in case of an exception in the LMS RegisterMap class.
*/
class LMS_REGMAP_Exception : public std::exception
{
  private:
	std::string e_;

  public:
	LMS_REGMAP_Exception(std::string e) : e_(e) {}
	~LMS_REGMAP_Exception() throw () {}
	const char* what() const throw() { return e_.c_str(); }
};

#endif // LMS_REGISTERSMAP_H
