/** SI5338 IC Device Library
 * 
 * \file SI5338.h
 *
 * SI5338 clock generator library for I2C device communication.
 * The library contains the device register map and commands for register access.
 * 
 * \project    OPS-SAT Project
 *
 * \copyright  Graz University of Technology
 * \date       15. June 2016
 * \author     Reinhard Zeif, MSc
 * \version    1.0
 *
 *(C) 2018 Graz University of Technology / IKS. All rights reserved.
 * Your use of TU Graz design tools, logic functions and other 
 * software and tools and any output files any of the foregoing 
 * (including device programming or simulation files), and any associated 
 * documentation or information are expressly subject to the terms and 
 * conditions of the applicable license agreement, including, without 
 * limitation, that your use is for the sole purpose of programming the 
 * OPS-SAT processing platform.
 *
 **/
 
#ifndef SI5338_H_
#define SI5338_H_

#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <errno.h>
#include <sys/ioctl.h>

#include "sepp_i2c_dev.h"


/* General Device Definitions for SI5338 */
#define			SI5338_MAX_OUTPUT_CHANNELS				4

#define			SI5338_FP_IN_REG_COUNT					3			// Frequency Plan: register count in Input config group
#define			SI5338_FP_IN_REG_START					28
#define			SI5338_FP_OUT_REG_COUNT					4			// Frequency Plan: register count in Output config group
#define			SI5338_FP_OUT_REG_START					31
#define			SI5338_FP_PLL_REG_COUNT					3			// Frequency Plan: register count in PLL config group
#define			SI5338_FP_PLL_REG_START					48
#define			SI5338_FP_MSx_REG_COUNT					10			// Frequency Plan: register count in MultiSynth groups MSx (P1,P2,P3)
#define			SI5338_FP_MS0_REG_START					53
#define			SI5338_FP_MS1_REG_START					64
#define			SI5338_FP_MS2_REG_START					75
#define			SI5338_FP_MS3_REG_START					86
#define			SI5338_FP_MSN_REG_START					97


/* Register Map Definitions for SI5338 */
#define			SI5338_REG_IN_MUX1						28
#define			SI5338_MASK_IN_MUX1_P2DIV_IN			0x20
#define			SI5338_MASK_IN_MUX1_P1DIV_IN			0x1C
#define			SI5338_MASK_IN_MUX1_XTAL_FREQ			0x03
#define			SI5338_REG_IN_MUX2						29
#define			SI5338_MASK_IN_MUX2_PFD_IN_REF			0xE0
#define			SI5338_MASK_IN_MUX2_P1DIV_IN			0x18
#define			SI5338_MASK_IN_MUX2_P1DIV				0x07
#define			SI5338_REG_IN_MUX3						30
#define			SI5338_MASK_IN_MUX3_PFD_IN_FB			0xE0
#define			SI5338_MASK_IN_MUX3_P2DIV_IN			0x18
#define			SI5338_MASK_IN_MUX3_P2DIV				0x07

#define			SI5338_REG_DRVx_VDDO					35
#define			SI5338_MASK_DRV3_VDDO					0xC0
#define			SI5338_MASK_DRV2_VDDO					0x30
#define			SI5338_MASK_DRV1_VDDO					0x0C
#define			SI5338_MASK_DRV0_VDDO					0x03

#define			SI5338_REG_DRV0_INV_FMT					36
#define			SI5338_REG_DRV1_INV_FMT					37
#define			SI5338_REG_DRV2_INV_FMT					38
#define			SI5338_REG_DRV3_INV_FMT					39
#define			SI5338_MASK_DRVx_INV					0x18
#define			SI5338_MASK_DRVx_FMT					0x07

#define			SI5338_REG_DRVx_TRIM1					40
#define			SI5338_MASK_DRVx_TRIM1_DRV0_TRIM		0x1F
#define			SI5338_MASK_DRVx_TRIM1_DRV1_TRIM		0xE0
#define			SI5338_REG_DRVx_TRIM2					41
#define			SI5338_MASK_DRVx_TRIM2_DRV1_TRIM		0x03
#define			SI5338_MASK_DRVx_TRIM2_DRV2_TRIM		0x7C
#define			SI5338_REG_DRVx_TRIM3					42
#define			SI5338_MASK_DRVx_TRIM3_DRV2_TRIM		0x1F

#define			SI5338_REG_PLL_CFG1						48
#define			SI5338_REG_PLL_CFG2						49
#define			SI5338_MASK_PLL_CFG2_FCAL_OVRD_EN		0x80
#define			SI5338_MASK_PLL_CFG2_VCO_GAIN			0x70
#define			SI5338_MASK_PLL_CFG2_RSEL				0xC0
#define			SI5338_MASK_PLL_CFG2_BWSEL				0x03
#define			SI5338_REG_PLL_CFG3						50
#define			SI5338_REG_PLL_CFG4						51

#define			SI5338_REG_CLK0_DISST_MS0_PHSTEP		110
#define			SI5338_REG_CLK1_DISST_MS1_PHSTEP		114
#define			SI5338_REG_CLK2_DISST_MS2_PHSTEP		118
#define			SI5338_REG_CLK3_DISST_MS3_PHSTEP		122
#define			SI5338_MASK_CLKx_DISST					0xC0
#define			SI5338_MASK_MSx_PHSTEP					0x3F

#define			SI5338_REG_LOS_LOL						218
#define			SI5338_MASK_PLL_LOL						0x10
#define			SI5338_MASK_LOS_FDBK					0x08
#define			SI5338_MASK_LOS_CLKIN					0x04
#define			SI5338_MASK_SYS_CAL						0x01

#define			SI5338_REG_OEB							230
#define			SI5338_MASK_OEB_ALL						0x10
#define			SI5338_MASK_OEB_3						0x08
#define			SI5338_MASK_OEB_2						0x04
#define			SI5338_MASK_OEB_1						0x02
#define			SI5338_MASK_OEB_0						0x01

#define			SI5338_REG_DIS_LOL						241
#define			SI5338_MASK_DIS_LOL						0x80
#define			SI5338_MASK_DIS_LOL_0x65				0x3F

#define			SI5338_REG_SOFT_RESET					246
#define			SI5338_MASK_SOFT_RESET					0x02



unsigned int const SI5538_Reg_WriteMask[351] =
{
0x00,0x00,0x00,0x00,0x00,0x00,0x1D,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x1F,0x1F,0x1F,0x1F,
0xFF,0x7F,0x3F,0x00,0x00,0xFF,0xFF,0x3F,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0x3F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0x3F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0x3F,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0x3F,0x00,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xBF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x0F,
0x0F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x0F,0x0F,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0x0F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0x0F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x80,0x02,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x0F,
0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0x0F,0x00,0x00,0x00,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0x0F,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x0F,0x00,0x00,
0x00
};


/** Frequency Plan Structure
 * The Frequency Plan Structure should be used to write a selected frequency plan
 * to the device. The frequency plan contains the necessary registers to set the
 * output frequency of all 4 output channels.
 * Hint: The easiest way to get these registers values is the use of the SiLabs
 * 		ClockBuilder GUI software. After configuration the register map can be exported
 * 		to a RegisterMap text file. These exported register values should be used to
 * 		populate the frequency plan structure with correct register values.
 * Important: The correct register addresses have to be used to guarantee proper function.
 * 		Please see SI5338 Reference Manual for more details.
 *
 * Register List (do not change):
 * 1) IN_CFG (input configuration): 28,29,30
 * 2) OUT_CFG (output configuration): 31,32,33,34
 * 3) PLL_CFG (PLL configuration): 48,49,50
 * 4) MS0 (MultiSynth0): 53,54,55,56,57,58,59,60,61,62
 * 5) MS1 (MultiSynth1): 64,65,66,67,68,69,70,71,72,73
 * 6) MS2 (MultiSynth1): 75,76,77,78,79,80,81,82,83,84
 * 7) MS3 (MultiSynth1): 86,87,88,89,90,91,92,93,94,95
 * 8) MSN (MultiSynthN): 97,98,99,100,101,102,103,104,105,106
 *
 */
typedef struct SI5338_Frequency_Plan{
	// Input Config
	// index 0-2 is register: 28,29,30
	unsigned int IN_CFG[SI5338_FP_IN_REG_COUNT];
	// Output Config
	// index 0-3 is register: 31,32,33,34
	unsigned int OUT_CFG[SI5338_FP_OUT_REG_COUNT];
	// PLL Config
	// index 0-2 is register: 48,49,50
	unsigned int PLL_CFG[SI5338_FP_PLL_REG_COUNT];
	// MultiSynth 0
	// index 0-9 is register: 53,54,55,56,57,58,59,60,61,62
	unsigned int MS0[SI5338_FP_MSx_REG_COUNT];
	// MultiSynth 1
	// index 0-9 is register: 64,65,66,67,68,69,70,71,72,73
	unsigned int MS1[SI5338_FP_MSx_REG_COUNT];
	// MultiSynth 2
	// index 0-9 is register: 75,76,77,78,79,80,81,82,83,84
	unsigned int MS2[SI5338_FP_MSx_REG_COUNT];
	// MultiSynth 3
	// index 0-9 is register: 86,87,88,89,90,91,92,93,94,95
	unsigned int MS3[SI5338_FP_MSx_REG_COUNT];
	// MultiSynth N
	// index 0-9 is register: 97,98,99,100,101,102,103,104,105,106
	unsigned int MSN[SI5338_FP_MSx_REG_COUNT];
} SI5338_Frequency_Plan;



/** SI5338 device class.
* 	Contains members for device address and general information. 
*	  Set and Get functions for device address, description and interface type.
* 	Methods to generate correct message sequences for write and read commands.
*/

class SI5338 : public SEPP_I2C_DEV
{
	private:

		// SEPP_I2C_DEV *i2c_dev_;       // i2c device file handler
		float REFCLK_Frequency_in_MHz_; 	// frequency of the reference clock in MHz from external XTAl, OSC or FBCLK
		float MAX_Frequency_in_MHz_;	// maximum device output frequency in MHz defined by device type

		void Set_Page(unsigned int page);
		unsigned int Get_Page(void);

		void WriteRegisterByte(unsigned int reg_addr, unsigned int reg_value);
		unsigned int ReadRegisterByte(unsigned int reg_addr);
		
		void Write_Frequency_Plan_Registers(SI5338_Frequency_Plan fp);
		void Read_Frequency_Plan_Registers(SI5338_Frequency_Plan &fp);

	public:
		SI5338();
		SI5338(float REFCLK_Frequency_in_MHz, float MAX_Frequency_in_MHz);
		SI5338(int i2c_address, std::string i2c_device_name, float REFCLK_Frequency_in_MHz, float MAX_Frequency_in_MHz);
		virtual ~SI5338();

		void Connect(int i2c_address, std::string i2c_device_name);
		void Disconnect();

		float Get_REFCLK_Frequency(void);					// Get reference clock frequency in MHz
		unsigned int Check_Status(void);				// Check the status of the device
		void PrintDeviceInfo();                               //!< Print device summary / info
		
		/* public Methods for Device Configuration */
		
   
		/* ----- Custom user functions for device ----- */

		void Print_Frequency_Plan(SI5338_Frequency_Plan fp);

		// void Enable_Clock_Phase_Offset(unsigned int channel);
		void Enable_Output(unsigned int channel);
		void Disable_Output(unsigned int channel);

		void Set_Frequency_Plan(SI5338_Frequency_Plan freq_plan);
		void Set_REFCLK_Input(unsigned int refclk_input, unsigned int Xtal_Freq_Range);
		void Set_Phase_Offset(unsigned int channel, unsigned int delay_in_ps, unsigned int stepsize_in_ps, unsigned int inc_or_dec);
		void Set_Output_DisableState(unsigned int channel, unsigned int disable_state);
		void Set_Output_SignalType(unsigned int channel, unsigned int signal_type);
		void Set_Output_Voltage(unsigned int channel, unsigned int voltage_level);
		void Set_Output_TrimValue(unsigned int channel, unsigned int trim_value);

		SI5338_Frequency_Plan Get_Frequency_Plan(void);
		unsigned int Get_REFCLK_Input(void);

};

/** SI5338 exception class.
* 	This exception type is returned in case of an exception in the SI5338 device class.
*/
class SI5338_Exception : public std::exception
{
  private:
	std::string e_;
	
  public:
	SI5338_Exception(std::string e) : e_(e) {}
	~SI5338_Exception() throw () {}
	const char* what() const throw() { return e_.c_str(); }
};

#endif
