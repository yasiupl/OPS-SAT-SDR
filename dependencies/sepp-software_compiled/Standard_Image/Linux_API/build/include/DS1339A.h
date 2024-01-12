/** DS1339A IC Device Library
 *
 * \file DS1339A.h
 *
 * DS1339A Real-Time Clock (RTC) library for I2C device communication.
 * The library contains the device register map and commands for register access.
 *
 * \project    OPS-SAT Project
 *
 * \copyright  Graz University of Technology
 * \date       15. Sept 2016
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

#ifndef DS1339A_h
#define DS1339A_h

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

/* Register Map and Masks for DS1339A */

/* DS1339A SECONDS register - Read/Write */
#define         DS1339A_REG_SECOND		            0x00		// Seconds Register
#define         DS1339A_MASK_SEC_10		        0x70		// bit 6:4 - 10 Seconds,Seconds decade value.
#define         DS1339A_MASK_SEC_1		        0x0F		// bit 3:0 - Seconds,Seconds unit position.
/* DS1339A MINUTES register - Read/Write */
#define         DS1339A_REG_MINUTE		            0x01		// Minutes Register
#define         DS1339A_MASK_MIN_10		        0x70		// bit 6:4 - 10 Minutes,Minutes decade value.
#define         DS1339A_MASK_MIN_1		        0x0F		// bit 3:0 - Minutes,Minutes unit position.
/* DS1339A HOURS register - Read/Write */
#define         DS1339A_REG_HOUR		          0x02		// Hours Register
#define         DS1339A_MASK_HOUR_12H24H		  0x40		// bit 6 - 12H/24H mode select,0 = 24H mode, 1 = 12H mode.
#define         DS1339A_MASK_HOUR_AMPM_20		0x20		// bit 5 - AM/PM or 20 Hour,0 = AM or hour value is smaller than 20, 1 = PM or hour value greater or equal 20.
#define         DS1339A_MASK_HOUR_10		      0x10		// bit 4 - 10 Hour, Hour decade value.
#define         DS1339A_MASK_HOUR_1		      0x0F		// bit 3:0 - Hours,Hours unit position.
/* DS1339A DAY register - Read/Write */
#define         DS1339A_REG_DAY		            0x03		// Day Register
#define         DS1339A_MASK_DAY_1		        0x07		// bit 2:0 - Day, Day of the week value.
/* DS1339A DATE register - Read/Write */
#define         DS1339A_REG_DATE		          0x04		// Date Register
#define         DS1339A_MASK_DATE_10		      0x30		// bit 5:4 - 10 Date, Date decade value.
#define         DS1339A_MASK_DATE_1		        0x0F		// bit 3:0 - Date, Date unit position.
/* DS1339A MONTH register - Read/Write */
#define         DS1339A_REG_MONTH		          0x05		// Month Register
#define         DS1339A_MASK_CENTURY		      0x80		// bit 7 - Century
#define         DS1339A_MASK_MONTH_10		      0x10		// bit 4 - 10 Month,Month decade value.
#define         DS1339A_MASK_MONTH_1		      0x0F		// bit 3:0 - Month,Month unit position.
/* DS1339A YEAR register - Read/Write */
#define         DS1339A_REG_YEAR		          0x06		// Year Register
#define         DS1339A_MASK_YEAR_10		      0xF0		// bit 7:4 - 10 Year,Year decade value.
#define         DS1339A_MASK_YEAR_1		        0x0F		// bit 3:0 - Hours
/* DS1339A ALARM1 SECONDS register - Read/Write */
#define         DS1339A_REG_A1_SEC		        0x07		// Alarm 1 Seconds Register
#define         DS1339A_MASK_A1_M1		        0x80		// bit 7 - A1M1 bit, Alarm 1 Mask 1 bit.
#define         DS1339A_MASK_A1_SEC_10		    0x70		// bit 6:4 - 10 Seconds,Seconds decade value.
#define         DS1339A_MASK_A1_SEC_1		      0x0F		// bit 3:0 - Seconds,Seconds unit position.
/* DS1339A ALARM1 MINUTES register - Read/Write */
#define         DS1339A_REG_A1_MIN		        0x08		// Alarm 1 Minutes Register
#define         DS1339A_MASK_A1_M2		        0x80		// bit 7 - A1M2 bit,Alarm 1 Mask 2 bit.
#define         DS1339A_MASK_A1_MIN_10		    0x70		// bit 6:4 - 10 Minutes,Minutes decade value.
#define         DS1339A_MASK_A1_MIN_1		      0x0F		// bit 3:0 - Minutes,Minutes unit position.
/* DS1339A ALARM1 HOURS register - Read/Write */
#define         DS1339A_REG_A1_HOURS		      0x09		// Alarm 1 Hours Register
#define         DS1339A_MASK_A1_M3		        0x80		// bit 7 - A1M3 bit,Alarm 1 Mask 3 Bit.
#define         LTC4222_MASK_A1_HOURS_12H24H		0x40		// bit 6 - 12H/24H mode select,0 = 24H mode, 1 = 12H mode.
#define         DS1339A_MASK_A1_HOURS_AMPM_20		0x20		// bit 5 - AM/PM or 20 Hour,0 = AM or hour value is smaller than 20, 1 = PM or hour value greater or equal 20.
#define         DS1339A_MASK_A1_HOURS_10		  0x10		// bit 4 - 10 Hour, Hour decade value.
#define         DS1339A_MASK_A1_HOURS_1		    0x0F		// bit 3:0 - Hours,Hours unit position.
/* DS1339A ALARM1 DAY-DATE register - Read/Write */
#define         DS1339A_REG_A1_DYDT		        0x0A		// Alarm 1 Day-Date Register
#define         DS1339A_MASK_A1_M4		        0x80		// bit 7 - A1M4 bit,Alarm 1 Mask 4 bit.
#define         LTC4222_MASK_A1_DY_EN		      0x40		// bit 6 - Day/Date mode select,0 = Date, 1 = Day.
#define         DS1339A_MASK_A1_DATE_10		    0x30		// bit 5:4 - 10 Date, Date decade value.
#define         DS1339A_MASK_A1_DAY_DATE_1		0x0F		// bit 3:0 - Day/Date,Day/Date unit position.
/* DS1339A ALARM2 MINUTES register - Read/Write */
#define         DS1339A_REG_A2_MIN		        0x0B		// Alarm 2 Minutes Register
#define         DS1339A_MASK_A2_M2		        0x80		// bit 7 - A2M2 bit,Alarm 2 Mask 2 bit.
#define         DS1339A_MASK_A2_MIN_10		    0x70		// bit 6:4 - 10 Minutes,Minutes decade value.
#define         DS1339A_MASK_A2_MIN_1		      0x0F		// bit 3:0 - Minutes,Minutes unit position.
/* DS1339A ALARM2 HOURS register - Read/Write */
#define         DS1339A_REG_A2_HOURS		      0x0C		// Alarm 2 Hours Register
#define         DS1339A_MASK_A2_M3		        0x80		// bit 7 - A2M3 bit,Alarm 2 Mask 3 bit.
#define         DS1339A_MASK_A2_HOURS_12H24H		0x40		// bit 6 - 12H/24H mode select,0 = 24H mode, 1 = 12H mode.
#define         DS1339A_MASK_A2_HOURS_AMPM_20		0x20		// bit 5 - AM/PM or 20 Hour,0 = AM or hour value is smaller than 20, 1 = PM or hour value greater or equal 20.
#define         DS1339A_MASK_A2_HOURS_10		0x10		// bit 4 - 10 Hour, Hour decade value.
#define         DS1339A_MASK_A2_HOURS_1		0x0F		// bit 3:0 - Hours,Hours unit position.
/* DS1339A ALARM2 DAY-DATE register - Read/Write */
#define         DS1339A_REG_A2_DYDT		        0x0D		// Day-Date Register
#define         DS1339A_MASK_A2_M4		        0x80		// bit 7 - A2M4 flag
#define         DS1339A_MASK_A2_DY_EN		      0x40		// bit 6 - Day/Date mode select,0 = Date, 1 = Day.
#define         DS1339A_MASK_A2_DATE_10		    0x30		// bit 5:4 - 10 Date, Date decade value.
#define         DS1339A_MASK_A2_DAY_DATE_1		0x0F		// bit 3:0 - Day/Date,Day/Date unit position.
/* DS1339A CONTROL register - Read/Write */
#define         DS1339A_REG_CTRL		          0x0E		// Control Register
#define         DS1339A_MASK_CTRL_EOSC		    0x80		// bit 7 - EOSC Enable Oscillator,0 = oscillator enabled (default), 1 = oscillator disabled.
#define         DS1339A_MASK_CTRL_BBSQI		    0x20		// bit 5 - BBSQI Battery-Backed Square-Wave Interrupt,1 = enable SQW/INTn output while powerd by Vbackup, 0 = disable SQW/INT while powered by Vbackup (default).
#define         DS1339A_MASK_CTRL_RS		      0x18		// bit 4:3 - RS2:1 Rate Select Bits,If INTCN = 0: Control Value for the frequence of square-wave signal generated at SQW/INTn output.If INTCN = 1: Not used
#define         DS1339A_MASK_CTRL_INTCN		    0x04		// bit 2 - INTCN Interrupt Control,1 = match between time and Alarm1/2 activates SQW/INTn output if alarm is enabled, 0 = square wave is generated on SQW/INTn pin (default).
#define         DS1339A_MASK_A2IE		          0x02		// bit 1 - A2IE Alarm 2 Interrupt Enable,1 = SQW/INTn signal is activated if A2F bit in the status register is set (wehin INTCN=1), 0 = A2F bit does not initiate an interrupt signal (default).
#define         DS1339A_MASK_A1IE		          0x01		// bit 0 - A1IE Alarm 1 Interrupt Enable,1 = SQW/INTn signal is activated if A1F bit in the status register is set (wehin INTCN=1), 0 = A1F bit does not initiate an interrupt signal (default).
/* DS1339A STATUS register - Read/Write */
#define         DS1339A_REG_STATUS		        0x0F		// Status Register
#define         DS1339A_MASK_STATUS_OSF		    0x80		// bit 7 - OSF Oscillator Stop Flag,Set to 1 if: first time power applied, VCC/Vbat insufficient, EOSC bit set to 1 or external crystal influences
#define         DS1339A_MASK_STATUS_A2F		    0x02		// bit 1 - A2F Alarm 2 Flag,0 = Alarm 2 condition not matched, 1 = Alarm 2 condition matched.
#define         DS1339A_MASK_STATUS_A1F		    0x01		// bit 0 - A1F Alarm 1 Flag,0 = Alarm 1 condition not matched, 1 = Alarm 1 condition matched.
/* DS1339A TRICKLE CHARGER register - Read/Write */
#define         DS1339A_REG_TCHRG		          0x10		// Trickle Charger Register
#define         DS1339A_MASK_TCHRG_TCS		    0xF0		// bit 7:4 - TCS3:0
#define         DS1339A_MASK_TCHRG_DS		      0x0C		// bit 3:2 - DS1:0
#define         DS1339A_MASK_TCHRG_ROUT		    0x03		// bit 1:0 - ROUT1:0


typedef struct
{
  unsigned char second;
  unsigned char minute;
  unsigned char hour;
  unsigned char day;
  unsigned char date;
  unsigned char month;
  unsigned char year;
} DS1339A_DateTime;


/** DS1339A device class.
* 	Contains members for device address and general information. 
*	Set and Get functions for device address, description and interface type.
* 	Methods to generate correct message sequences for write and read commands.
*/
class DS1339A : public SEPP_I2C_DEV
{
	private:
		

		/* private Methods */
		unsigned int Verify_DateTime_Values(DS1339A_DateTime dt);
		void Init_Registers(void);

		void WriteRegisterByte(unsigned int reg_addr, unsigned int reg_value);
		unsigned int ReadRegisterByte(unsigned int reg_addr);

	public:
		DS1339A();
		DS1339A(unsigned int i2c_address, std::string i2c_device_name);
		~DS1339A();

		void Connect(unsigned int i2c_address, std::string i2c_device_name);
		void Disconnect();

		void PrintDeviceInfo();                               //!< Print device summary / info
		
		/* User functions */
		void Clear_Status_Flags(void);
		unsigned int Get_Oscillator_Stop_Flag(void);
		void Set_Control_Register(unsigned int reg_value);
		unsigned int Get_Control_Register(void);
		void Set_Date_Time(DS1339A_DateTime date_time);
		DS1339A_DateTime Get_Date_Time(void);

};

/** DS1339A exception class.
* 	This exception type is returned in case of an exception in the DS1339A device class.
*/
class DS1339A_Exception : public std::exception
{
  private:
	std::string e_;

  public:
	DS1339A_Exception(std::string e) : e_(e) {}
	~DS1339A_Exception() throw () {}
	const char* what() const throw() { return e_.c_str(); }
};

#endif
