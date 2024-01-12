/** IADCS-100 Device Library
 * 
 * \file IADCS_100.h
 *
 * IADCS_100 library for I2C device communication.
 * The library contains the device register map and commands for register access.
 * 
 * \project     OPS-SAT Project
 *
 * \copyright   Graz University of Technology
 * \author      Reinhard Zeif, MSc (RZ), Manuel Kubicka, MSc (MC), Vasundhara Shiradhonkar, MSc (VS)
 * \date        19.Nov.2020
 *
 * version history:
 * \version		0.1		MC	Initial release of general set/get methods							19.Oct.2017
 * \version		0.2		RZ	IADCS_I2C_DEV changed to SEPP_I2C_DEV and some coding updates 		21.Oct.2017
 * \version		0.3		RZ	Update and bugfix for operation modes 								15.Mar.2018
 * \version		0.4		RZ	Update to new iADCS firmware with 2-byte commands and
 *							little endian NMO 2-byte value										21.Jul.2018
 * \version		0.5		RZ	New release and update for ICD v3.2	firmware						10.Oct.2018
 * \version		0.6		RZ	Update of livelyhood data length bug, additional command
 *							that is used to set a external sun vector							11.Dec.2018
 * \version		0.7		RZ 	review and fine tuning of some methods								07.Apr.2019
 * \version		0.8		RZ	Bugfix: Read RW parameters, Standard TM register order				16.Apr.2019
 * \version		0.9		RZ	Commands and structs added for some remaining cmds of ICD 3.3.1
 * 							Set/Get magnettorquer currents, Get target pointing telemetry		20.Sep.2019
 * \version 	1.0		RZ	SVT3 release with all commands defined in ICD 3.3.1					23.Sep.2019
 * \version		1.1		RZ	Bugfix: ICD v3.3.1 - fix wrong command codes for IDLE and SAFE mode
 *							commands. Safe and Idle commands fixed.								01.Oct.2019
 * \version		1.2		RZ	iADCS native startracker readout command added						18.Nov.2019
 * \version		1.3		RZ	typo fixed in debug message output									21.Apr.2020
 * \version		1.4		RZ	I2C retry count increased to 10										25.May.2020
 * \version		1.5		RZ	Bugfix: Nadir and Earth-Fixed Pointing time value conversion fixed
 * 							to uint64 instead of float64										10.Jul.2020
 * \version		1.6		RZ	Two internally used registers added to standard TM					29.Jul.2020
 * \version		1.7		RZ	Sensor TM attitude quaternion: SF changed to BF						31.Jul.2020
 * \version		1.8		RZ	Bugfix: Offsets for Std-TM internal registers						12.Aug.2020
 * \version		1.9		RZ	Bugfix: Debug output fixed in start target pointing fixed method	07.Sep.2020
 * \version		1.10	RZ	Target pointing inertial mode command added (special tgt mode)		20.Sep.2020
 * \version		1.11	RZ	Set/Get Kalman filter methods and parameters added					23.Sep.2020
 * \version		1.12	RZ	Command for inertial pointing parameters added						20.Oct.2020
 * \version		1.13	RZ	Command for inertial operation paramters reworked and changed to
 *							general set target pointing operation paramters command.			06.Nov.2020
 * \version		1.14	RZ	Bugfix: Set/Get magnetometer parameters method						19.Nov.2020
 * \version		1.15	VS	Added degree parameter to start targetPointing inertial mode 		16.Dec.2021
 * \version		1.16	VS	Set/Get reaction wheel torque target method							01.Apr.2022
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
 **/
 
#ifndef IADCS_100_H_
#define IADCS_100_H_

#include <iostream>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <math.h>

#include "sepp_i2c_dev.h"
#include "SEPP_API_Debug.h"


/* ---------------------------- General iADCS-100 Definitions --------------------------- */
#define 		IADCS_100_I2C_ADDR							0x0A	// I2C device address
#define			IADCS_100_I2C_WAIT_WR_RD_USEC				100000 	// wait time between write and read = 100000 us = 100 ms (ICD: t_wait > 20 ms)

#define 		IADCS_100_CRC_NUMBER_OF_RETRIES				10		// Number of retries if CRC failure is detected
#define 		IADCS_100_CRC_RETRY_DELAY_USEC				300000	// Delay between retries caused by CRC failure = 300000 us = 300 ms

/* -------------------------------------------------------------------------------------- */



/*! -------------------- iADCS-100 Command Codes (C1,C2) --------------------- */
//! According to iADCS_TN01_02_V03.3.1_ICD_OPSSAT (15.Nov.2018)
//! System Low Level Commands
#define		IADCS_100_CMD_SETDCDCCONFIGURATION								0x0101
//! System Commands
#define		IADCS_100_CMD_SETEPOCHTIME										0x0210
#define		IADCS_100_CMD_GETEPOCHTIME										0x0211
#define		IADCS_100_CMD_GETSYSTEMTIME										0x0212
#define		IADCS_100_CMD_GETINFOFRAME										0x0220
#define		IADCS_100_CMD_GETSTANDARDTELEMETRY								0x0221
#define		IADCS_100_CMD_GETSENSORTELEMETRY								0x0222
#define		IADCS_100_CMD_GETACTUATORTELEMETRY								0x0223
#define		IADCS_100_CMD_GETATTITUDETELEMETRY								0x0224
#define		IADCS_100_CMD_GETPOWERTELEMETRY									0x0225
#define		IADCS_100_CMD_GETSYSTEMSCHEDULERREGISTER						0x0228
#define		IADCS_100_CMD_SETSYSTEMSCHEDULERREGISTER						0x0229
#define		IADCS_100_CMD_CLEARSYSTEMERRORREGISTER							0x0230
#define		IADCS_100_CMD_SETACTIVEGYROSCOPE								0x0231
#define		IADCS_100_CMD_SETACTIVEMAGNETOMETER								0x0232
#define		IADCS_100_CMD_SETTEMPERATUREUPDATEINTERVAL						0x0234
#define		IADCS_100_CMD_GETTEMPERATUREUPDATEINTERVAL						0x0235
#define		IADCS_100_CMD_SETPOWERREADINGUPDATEINTERVAL						0x0236
#define		IADCS_100_CMD_GETPOWERREADINGUPDATEINTERVAL						0x0237
//! Star tracker ST200 Commands
#define		IADCS_100_CMD_SETSTARTRACKERQUATERNION							0xAB01
#define		IADCS_100_CMD_GETSTARTRACKERQUATERNION							0xAB02
#define		IADCS_100_CMD_GETSTARTRACKEREVENTLIST							0xAB04
#define		IADCS_100_CMD_SETSTARTRACKEROPERATIONMODE						0xAB11
#define		IADCS_100_CMD_GETSTARTRACKEROPERATIONMODE						0xAB12
#define		IADCS_100_CMD_SENDSTARTRACKERNATIVECOMMAND						0xAB20
#define		IADCS_100_CMD_CHECKFORREPLYONSTARTRACKERNATIVECOMMANDS			0xAB21
//! Orbit Commands
#define		IADCS_100_CMD_INITORBITMODULEWITHTLEDATA						0xAC01
#define		IADCS_100_CMD_GETORBITDATA										0xAC02
//! HIL Commands
#define		IADCS_100_CMD_SETHILCONFIGURATION								0xAD01
#define		IADCS_100_CMD_GETHILCONFIGURATION								0xAD02
//! Gyroscope Commands
#define		IADCS_100_CMD_GYROSETSENSORVALUE								0xA101
#define		IADCS_100_CMD_GYROGETSENSORVALUE								0xA102
#define		IADCS_100_CMD_GYROSETSENSORPARAMETER							0xA103
#define		IADCS_100_CMD_GYROGETSENSORPARAMETER							0xA104
#define		IADCS_100_CMD_GYROSETSENSORUPDATEINTERVAL						0xA105
#define		IADCS_100_CMD_GYROGETSENSORUPDATEINTERVAL						0xA106
#define		IADCS_100_CMD_GYROENABLESENSORCORRECTION						0xA107
#define		IADCS_100_CMD_GYRODISABLESENSORCORRECTION						0xA108
#define		IADCS_100_CMD_GYROSTARTSENSORZEROBIASCALCULATION				0xA109
#define		IADCS_100_CMD_GYROSETSENSORZEROBIASVALUE						0xA10A
#define		IADCS_100_CMD_GYROGETSENSORZEROBIASVALUE						0xA10B
#define		IADCS_100_CMD_GYROENABLESENSORBIASREMOVEMENT					0xA10C
#define		IADCS_100_CMD_GYRODISABLESENSORBIASREMOVEMENT					0xA10D
#define		IADCS_100_CMD_GYROENABLESENSORAVERAGINGFILTER					0xA10E
#define		IADCS_100_CMD_GYRODISABLESENSORAVERAGINGFILTER					0xA10F
//! Magnetometer Commands
#define		IADCS_100_CMD_MAGNETOMETERSETSENSORVALUE						0xA201
#define		IADCS_100_CMD_MAGNETOMETERGETSENSORVALUE						0xA202
#define		IADCS_100_CMD_MAGNETOMETERSETSENSORPARAMETER					0xA203
#define		IADCS_100_CMD_MAGNETOMETERGETSENSORPARAMETER					0xA204
#define		IADCS_100_CMD_MAGNETOMETERSETSENSORUPDATEINTERVAL				0xA205
#define		IADCS_100_CMD_MAGNETOMETERGETSENSORUPDATEINTERVAL				0xA206
#define		IADCS_100_CMD_MAGNETOMETERENABLESENSORCORRECTION				0xA207
#define		IADCS_100_CMD_MAGNETOMETERDISABLESENSORCORRECTION				0xA208
#define		IADCS_100_CMD_MAGNETOMETERENABLESENSORAVERAGINGFILTER			0xA209
#define		IADCS_100_CMD_MAGNETOMETERDISABLESENSORAVERAGINGFILTER			0xA20A
#define		IADCS_100_CMD_MAGNETOMETERENABLESENSORMOVINGAVERAGEFILTER		0xA20B
#define		IADCS_100_CMD_MAGNETOMETERDISABLESENSORMOVINGAVERAGEFILTER		0xA20C
//! Sun Sensor Commands
#define		IADCS_100_CMD_SUNSENSORSETSUNVECTOR								0xA330
//! Reaction Wheel Array Commands
#define		IADCS_100_CMD_REACTIONWHEELSETWHEELSPEED						0xB001
#define		IADCS_100_CMD_REACTIONWHEELGETWHEELSPEED						0xB002
#define		IADCS_100_CMD_REACTIONWHEELSETWHEELPARAMETER					0xB003
#define		IADCS_100_CMD_REACTIONWHEELGETWHEELPARAMETER					0xB004
#define		IADCS_100_CMD_REACTIONWHEELSETUPDATEINTERVALALLWHEELS			0xB005
#define		IADCS_100_CMD_REACTIONWHEELGETUPDATEINTERVALALLWHEELS			0xB006
#define		IADCS_100_CMD_REACTIONWHEELSETSINGLEWHEELINIDLEMODE				0xB007
#define		IADCS_100_CMD_REACTIONWHEELSETWHEELSPEEDSFORALLWHEELS			0xB008
#define		IADCS_100_CMD_REACTIONWHEELGETWHEELSPEEDSFORALLWHEELS			0xB009
#define		IADCS_100_CMD_REACTIONWHEELSETPARAMETERSFORALLWHEELS			0xB00A
#define		IADCS_100_CMD_REACTIONWHEELGETPARAMETERSFORALLWHEELS			0xB00B
#define		IADCS_100_CMD_REACTIONWHEELSETALLWHEELSINIDLEMODE				0xB00C
#define		IADCS_100_CMD_REACTIONWHEELSETTORQUETARGET						0xB00D
#define		IADCS_100_CMD_REACTIONWHEELGETTORQUETARGET						0xB00E
//! Magnettorquer Commands
#define		IADCS_100_CMD_MAGNETTORQUERSETDIPOLEMOMENTS						0xB101
#define		IADCS_100_CMD_MAGNETTORQUERGETDIPOLEMOMENTS						0xB102
#define		IADCS_100_CMD_MAGNETTORQUERSUSPEND								0xB103
#define		IADCS_100_CMD_MAGNETTORQUERRESUME								0xB104
#define		IADCS_100_CMD_MAGNETTORQUERSETALLPARAMETERS						0XB105
#define		IADCS_100_CMD_MAGNETTORQUERGETALLPARAMETERS						0xB106
#define		IADCS_100_CMD_MAGNETTORQUERSETCURRENTS							0xB107
#define		IADCS_100_CMD_MAGNETTORQUERGETCURRENTS							0xB108
//! Angular Velocity Control Commands
#define		IADCS_100_CMD_AVCSETPIDPARAMETER								0xC011
#define		IADCS_100_CMD_AVCGETPIDPARAMETER								0xC012
#define		IADCS_100_CMD_AVCSTARTPIDCONTROLLER								0xC013
#define		IADCS_100_CMD_AVCSTOPPIDCONTROLLER								0xC014
//! Angle Step Control Commands
#define		IADCS_100_CMD_ASCSETCASCADINGPARAMETER							0xC031
#define		IADCS_100_CMD_ASCGETCASCADINGPARAMETER							0xC032
#define		IADCS_100_CMD_ASCSTARTCASCADINGCONTROLLER						0xC033
#define		IADCS_100_CMD_ASCSTOPCASCADINGCONTROLLER						0xC034
//! BDOT Controller Commands
#define		IADCS_100_CMD_BDOTSETBDOTMAXPARAMETERS							0xC051
#define		IADCS_100_CMD_BDOTGETBDOTMAXPARAMETERS							0xC052
#define		IADCS_100_CMD_BDOTSETBDOTPROPORTIONALPARAMETERS					0xC053
#define		IADCS_100_CMD_BDOTGETBDOTPROPORTIONALPARAMETERS					0xC054
//! Sun Pointing Controller Commands
#define		IADCS_100_CMD_SPCSETCONTROLLERPARAMETERS						0xC061
#define		IADCS_100_CMD_SPCGETCONTROLLERPARAMETERS						0xC062
//! Sliding Control Commands
#define		IADCS_100_CMD_SCSETCONTROLLERPARAMETERS							0xC081
#define		IADCS_100_CMD_SCGETCONTROLLERPARAMETERS							0xC082
//! Basic Mode Commands
#define		IADCS_100_CMD_SETIDLEMODE										0xC102  // bugfix to ICD v3.3.1: IDLE = 0xC102 (instead of 0xC103)
#define		IADCS_100_CMD_SETSAFEMODE										0xC103  // bugfix to ICD v3.3.1: SAFE = 0xC103 (instead of 0xC102)
#define		IADCS_100_CMD_SETMEASUREMENTMODE								0xC104
#define		IADCS_100_CMD_STARTDETUMBLINGMODE								0xC111
#define		IADCS_100_CMD_STOPDETUMBLINGMODE								0xC101
//! Sun Pointing Mode Commands
#define		IADCS_100_CMD_STARTSUNPOINTINGMODE								0xC121
#define		IADCS_100_CMD_STOPSUNPOINTINGMODE								0xC101
//! Target Pointing Mode Commands
#define		IADCS_100_CMD_STARTEARTHTARGETPOINTINGMODECONSTANTVELOCITY		0xC154
#define		IADCS_100_CMD_STOPEARTHTARGETPOINTINGMODECONSTANTVELOCITY		0xC101
#define		IADCS_100_CMD_STARTEARTHTARGETPOINTINGMODEFIX					0xC153
#define		IADCS_100_CMD_STOPEARTHTARGETPOINTINGMODEFIX					0xC101
#define		IADCS_100_CMD_STARTNADIRPOINTING								0xC152
#define		IADCS_100_CMD_STOPNADIRPOINTING									0xC101
#define		IADCS_100_CMD_GETEARTHTARGETPOINTINGTARGETPARAMETERS			0xC160
#define		IADCS_100_CMD_GETEARTHTARGETPOINTINGOPERATIONPARAMETERS			0xC161
#define		IADCS_100_CMD_GETEARTHTARGETPOINTINGOPERATIONDATA				0xC162
//! Special Extra Commands
#define		IADCS_100_CMD_STARTINERTIALPOINTING								0xC151
#define		IADCS_100_CMD_STOPINERTIALPOINTING								0xC101
#define		IADCS_100_CMD_GETKALMANFILTERPARAMETERS							0xC311
#define		IADCS_100_CMD_SETKALMANFILTERPARAMETERS							0xC312
#define		IADCS_100_CMD_SETTARGETPOINTINGOPERATIONPARAMETERS				0xC163


/* ----------------------------  Limit Definitions -------------------------- */
#define 	IADCS_100_NUM_GYROS							3					//!< Number of available gyros
#define		IADCS_100_NUM_MAGNETOMETERS					1					//!< Number of available magnetometers
#define		IADCS_100_NUM_MAGNETORQUERS					3					//!< Number of available magnettorquers
#define		IADCS_100_NUM_REACTIONWHEELS				6					//!< Number of available reaction wheels


/* -------------------------  iADCS-100 CRC Settings ------------------------ */
// For tests use this page: http://crccalc.com/ and look at row "CRC-8"
#define 	CRC8_POLY_CCITT								0x07 				//!< CRC8 Polynomial: x^8 + x^2 + x^1 + x^0
#define		CRC8_START_CCITT							0x00 				//!< CRC8 Initialization value: 0


/* ---------------------- General iADCS-100 Structures ---------------------- */


/** \struct	IADCS_100_Vector3_ushort
 * \brief 	IADCS-100 Vector with 3 unsigned short values x,y,z 
 */
typedef struct
{
	unsigned short x;
	unsigned short y;
	unsigned short z;
} IADCS_100_Vector3_ushort;

/** \struct	IADCS_100_Vector3_uint
 * \brief 	IADCS-100 Vector with 3 unsigned integer values x,y,z 
 */
 typedef struct
{
	unsigned int x;
	unsigned int y;
	unsigned int z;
} IADCS_100_Vector3_uint;

/** \struct	IADCS_100_Vector3_int
 * \brief 	IADCS-100 Vector with 3 signed integer values x,y,z 
 */
typedef struct
{
	int x;
	int y;
	int z;
} IADCS_100_Vector3_int;

/** \struct	IADCS_100_Vector3_float
 * \brief 	IADCS-100  Vector with 3 float values x,y,z 
 */
typedef struct
{
	float x;
	float y;
	float z;
} IADCS_100_Vector3_float;

/** \struct	IADCS_100_Vector6_float
 * \brief 	IADCS-100 Vector with 6 float values for two coordinate system x,y,z and u,v,w 
 */
typedef struct
{
	float x;
	float y;
	float z;
	float u;
	float v;
	float w;
} IADCS_100_Vector6_float;

/** \struct	IADCS_100_Matrix3_float
 * \brief 	IADCS-100 Matrix with 3x3 float values
 */
typedef struct
{
	float m11;
	float m12;
	float m13;
	float m21;
	float m22;
	float m23;
	float m31;
	float m32;
	float m33;
} IADCS_100_Matrix3_float;


/* ------------------------------ General Telemetry Structures ----------------------------*/

 
/** \struct IADCS_100_Info_Telemetry
 *	\brief  IADCS-100 Info Telemetry 
 */
typedef struct
{
	char FRAME_IDENTIFIER[2];							//! 0..1 	2x char 		Frame identifier
	unsigned char FRAME_VERSION;						//! 2 		1x uint8_t 		Frame version
														//! 3..5 	3x uint8_t		Software version
	unsigned char SW_VERSION_MAJOR;						//! 1x uint8_t: 	Major version
	unsigned char SW_VERSION_MINOR;						//! 1x uint8_t: 	Minor version
	unsigned char SW_VERSION_PATCH;						//! 1x uint8_t: 	Patch version
	unsigned char STARTRACKER_TYPE;						//! 6: 		1x uint8_t 		Star tracker type
	unsigned int STARTRACKER_SERIAL_NUMBER;				//! 7..10 	1x uint32_t 	Star tracker serial number
	char DEVICE_NAME[9];								//! 11..19 	9x char 		Device name
	unsigned int DEVICE_SERIAL_NUMBER;					//! 20..23 	1x uint32_t 	Device serial number
	char BUILD_TIMESTAMP[32];							//! 24..55 	32x char 		Build time stamp
														//! 56..75 	5x uint32_t		Commit ids
	unsigned int SW_COMMIT_ID_PROJECT;					//! 1x uint32_t: 	Project commit id
														//! 1x uint32_t: 	Internal use
														//! 1x uint32_t: 	Internal use
	unsigned int SW_COMMIT_ID_LIBRARY;					//! 1x uint32_t: 	Library commit id
														//! 1x uint32_t: 	Internal use
	unsigned char DEBUG_LEVEL;							//! 76: 		1x uint8_t 		Debug level
	char COMPILER_NAME[20];								//! 77..96 	20x char 		Compiler name
	char COMPILER_VERSION[10];							//! 97..106 	10x char 		Compiler version
	char LOW_LEVEL_SW_VERSION[10];						//! 107..116 10x char 		Low level software version
	char LOW_LEVEL_BUILD_TIMESTAMP[32];					//! 117..148 32x char 		Low level build time stamp
} IADCS_100_Info_Telemetry;

//! IADCS-100 Standard Telemetry 
typedef struct
{
	unsigned long long SYSTEM_TIME_MSEC;				// 0..7 	1x int64_t 		System time in [msec]
	unsigned long long EPOCH_TIME_MSEC;					// 8..15 	1x int64_t 		Epoch time in [msec]
	unsigned int LIVELYHOOD_REGISTER;					// 16..19 	1x uint32_t 	Livelyhood register, indicating detected devices (6.1.6.11)
	unsigned int SYSTEM_STATUS_REGISTER;				// 20..23 	1x uint32_t 	System status register (5.1.6.13)
	unsigned int SYSTEM_SCHEDULER_REGISTER;				// 24..27	1x uint32_t 	System scheduler register (5.1.6.10)
	unsigned int SYSTEM_ERROR_REGISTER;					// 28..31	1x uint32_t		System error register (5.1.11)
	unsigned int SENSORS_ERROR_REGISTER;				// 32..35	1x uint32_t		Sensors error register (5.1.11)
	unsigned int ACTUATORS_ERROR_REGISTER;				// 36..39 	1x uint32_t		Actuators error register (5.1.6.12)
														// 40..63 	6x uint32_t 	Control module registers
	unsigned int CONTROL_MAIN_STATUS;					// 			1x uint32_t:	Main control status register (5.4.3.2.1)
	unsigned int CONTROL_MAIN_ERROR;					// 			1x uint32_t:	Error register (5.4.3.2.2)
	unsigned int CONTROL_SINGLE_AXIS_STATUS_X;			// 			3x uint32:		Single axis status register[X, Y,Z] (5.4.3.2.3)
	unsigned int CONTROL_SINGLE_AXIS_STATUS_Y;			//
	unsigned int CONTROL_SINGLE_AXIS_STATUS_Z;			//
	unsigned int CONTROL_ALL_AXIS_STATUS; 				// 			1x uint32_t:	All axis control status register (5.4.3.2.4)
														// 64..83 	5x uint32_t		High level ADCS registers
	unsigned int SAT_MAIN_REGISTER;						// 			1x uint32_t: 	Sat Main Register (6.1.7.1)
	unsigned int SAT_ERROR_REGISTER;					// 			1x uint32_t: 	Sat Error Register (6.1.7.2)
	unsigned int SAT_SCHEDULER_REGISTER;				// 			1x uint32_t: 	Sat Scheduler Register (6.1.7.3)
	unsigned int INTERNAL_USE_1;						// 			2x uint32_t: 	internal use
	unsigned int INTERNAL_USE_2;
	unsigned int NUMBER_OF_RECEIVED_COMMANDS;			// 84..87 	1x uint32_t 	Number of received commands
	unsigned int NUMBER_OF_FAILED_COMMANDS;				// 88..91 	1x uint32_t 	Number of commands, which are corrupted or not successfully handled.
	unsigned int INTERNAL_USE_3;						// 92..95	1x uint32_t		internal use only
} IADCS_100_Standard_Telemetry;

//! IADCS-100 Sensor Telemetry 
typedef struct
{
	unsigned long long EPOCH_TIME_MSEC;					// 0..7: 	1x int64_t 		Epoch time in [msec]
	unsigned int INDEX_OF_ACTIVE_STARTRACKER;			// 8:		1x uint8_t 		Index of active star tracker in star tracker array
														// 9..24: 	4x float 		Measured attitude quaternion of the satellite from active star tracker [qi, qj , qk, q]
	float QUATERNION_ATTITUDE_1_BF;									// Quaternion 1
	float QUATERNION_ATTITUDE_2_BF;									// Quaternion 2
	float QUATERNION_ATTITUDE_3_BF;									// Quaternion 3
	float QUATERNION_ATTITUDE_4_BF;									// Quaternion 4
	unsigned char INDEX_OF_ACTIVE_GYRO;					// 25 		1x uint8_t 		Index of active gyroscope in the gyroscope array
														// 26..73 	4x [3x float] 	Measured angular velocity vector [wx, wy, wz] of maximum 4 gyroscope in [rad/sec] and body frame.
														// 							Values of not available sensor are set to zero.
	float MEASURED_ANGULAR_VELOCITY_GYRO_1_X_RADPS;
	float MEASURED_ANGULAR_VELOCITY_GYRO_1_Y_RADPS;
	float MEASURED_ANGULAR_VELOCITY_GYRO_1_Z_RADPS;
	float MEASURED_ANGULAR_VELOCITY_GYRO_2_X_RADPS;
	float MEASURED_ANGULAR_VELOCITY_GYRO_2_Y_RADPS;
	float MEASURED_ANGULAR_VELOCITY_GYRO_2_Z_RADPS;
	float MEASURED_ANGULAR_VELOCITY_GYRO_3_X_RADPS;
	float MEASURED_ANGULAR_VELOCITY_GYRO_3_Y_RADPS;
	float MEASURED_ANGULAR_VELOCITY_GYRO_3_Z_RADPS;
	float MEASURED_ANGULAR_VELOCITY_GYRO_4_X_RADPS;
	float MEASURED_ANGULAR_VELOCITY_GYRO_4_Y_RADPS;
	float MEASURED_ANGULAR_VELOCITY_GYRO_4_Z_RADPS;
	unsigned char INDEX_OF_ACTIVE_MAGNETOMETER;			// 74: 		1x uint8_t 		Index of active magnetormeter in the magnetometer array
														// 75..98 	2x [3x float]	Measured magnetic field vector [bx, by, bz] of maximum 2 magnetometer in [T] and body frame.
														//							Values of not available sensor are set to zero.
	float MEASURED_MAGNETIC_FIELD_MAGNETOMETER_1_X_T;
	float MEASURED_MAGNETIC_FIELD_MAGNETOMETER_1_Y_T;
	float MEASURED_MAGNETIC_FIELD_MAGNETOMETER_1_Z_T;
	float MEASURED_MAGNETIC_FIELD_MAGNETOMETER_2_X_T;
	float MEASURED_MAGNETIC_FIELD_MAGNETOMETER_2_Y_T;
	float MEASURED_MAGNETIC_FIELD_MAGNETOMETER_2_Z_T;
														// 99..170 	6x [3x float] 	Measured sun vector [svx, svy, svz] of 6 sunsensors.
														// 							Values of not available sensor are set to zero.
	float MEASURED_SUN_VECTOR_SUNSENSOR_1_X;
	float MEASURED_SUN_VECTOR_SUNSENSOR_1_Y;
	float MEASURED_SUN_VECTOR_SUNSENSOR_1_Z;
	float MEASURED_SUN_VECTOR_SUNSENSOR_2_X;
	float MEASURED_SUN_VECTOR_SUNSENSOR_2_Y;
	float MEASURED_SUN_VECTOR_SUNSENSOR_2_Z;
	float MEASURED_SUN_VECTOR_SUNSENSOR_3_X;
	float MEASURED_SUN_VECTOR_SUNSENSOR_3_Y;
	float MEASURED_SUN_VECTOR_SUNSENSOR_3_Z;
	float MEASURED_SUN_VECTOR_SUNSENSOR_4_X;
	float MEASURED_SUN_VECTOR_SUNSENSOR_4_Y;
	float MEASURED_SUN_VECTOR_SUNSENSOR_4_Z;
	float MEASURED_SUN_VECTOR_SUNSENSOR_5_X;
	float MEASURED_SUN_VECTOR_SUNSENSOR_5_Y;
	float MEASURED_SUN_VECTOR_SUNSENSOR_5_Z;
	float MEASURED_SUN_VECTOR_SUNSENSOR_6_X;
	float MEASURED_SUN_VECTOR_SUNSENSOR_6_Y;
	float MEASURED_SUN_VECTOR_SUNSENSOR_6_Z;
	float TEMPERATURE_DEGC;								// 171 		1x float 		Temperature in [deg]
} IADCS_100_Sensor_Telemetry;

//! IADCS-100 Actuator Telemetry 
typedef struct
{
	unsigned long long EPOCH_TIME_MSEC;					// 0..7 	1x int64_t 		Epoch time in [msec]
														// 8..31 	6x float 		Target speed of max. 6 reaction wheels rw0, rw1, ...rw5 in [rad/sec].
														// 							Values of not available wheels are set to zero.
	float REACTIONWHEEL_TARGET_SPEED_X_RADPS;
	float REACTIONWHEEL_TARGET_SPEED_Y_RADPS;
	float REACTIONWHEEL_TARGET_SPEED_Z_RADPS;
	float REACTIONWHEEL_TARGET_SPEED_U_RADPS;
	float REACTIONWHEEL_TARGET_SPEED_V_RADPS;
	float REACTIONWHEEL_TARGET_SPEED_W_RADPS;
														// 32..55 	6x float		Current speed of max. 6 reaction wheels rw0, rw1, ...rw5 in [rad/sec].
														// 							Values of not available wheels are set to zero.
	float REACTIONWHEEL_CURRENT_SPEED_X_RADPS;
	float REACTIONWHEEL_CURRENT_SPEED_Y_RADPS;
	float REACTIONWHEEL_CURRENT_SPEED_Z_RADPS;
	float REACTIONWHEEL_CURRENT_SPEED_U_RADPS;
	float REACTIONWHEEL_CURRENT_SPEED_V_RADPS;
	float REACTIONWHEEL_CURRENT_SPEED_W_RADPS;
														// 56..67 	3x float 		Target dipole moment of magnetorquers on 3 axes [mx,my,mz] in [Am2]
	float MAGNETORQUERS_TARGET_DIPOLE_MOMENT_X_AM2;
	float MAGNETORQUERS_TARGET_DIPOLE_MOMENT_Y_AM2;
	float MAGNETORQUERS_TARGET_DIPOLE_MOMENT_Z_AM2;
	unsigned int MAGNETORQUERS_CURRENT_STATE;			// 68..71 	1x int32_t		Current state of magnetorquers
} IADCS_100_Actuator_Telemetry;

//! IADCS-100 Attitude Telemetry 
typedef struct
{
	unsigned long long EPOCH_TIME_MSEC;					// 0..7 	1x int64_t 		Epoch time in [msec]
														// 8..23 	4x float 		Determinated attitude quaternion in body frame
	float QUATERNION_ATTITUDE_1_BF;						// Quaternion Attitude 1
	float QUATERNION_ATTITUDE_2_BF;						// Quaternion Attitude 2
	float QUATERNION_ATTITUDE_3_BF;						// Quaternion Attitude 3
	float QUATERNION_ATTITUDE_4_BF;						// Quaternion Attitude 4
														// 24..35 	3x float 		Determinated angular velocity vector [wx, wy, wz] in [rad/sec] and body frame
	float ANGULAR_VELOCITY_X_RADPS;						// Angular Velocity X [rad/sec]
	float ANGULAR_VELOCITY_Y_RADPS;						// Angular Velocity Y [rad/sec]
	float ANGULAR_VELOCITY_Z_RADPS;						// Angular Velocity Z [rad/sec]
														// 36..43 	1x int64_t 		Epoch time stamp of last attitude determination in [msec]
	unsigned long long EPOCH_TIME_LAST_ATTITUDE_DETERMINATION_MSEC;
														// 44..55 	3x float 		Measured magnetic field vector [bx, by, bz] from active sensor in [T] and body frame
	float MEASURED_MAGNETIC_FIELD_X_BF_T;
	float MEASURED_MAGNETIC_FIELD_Y_BF_T;
	float MEASURED_MAGNETIC_FIELD_Z_BF_T;
														// 56..63 	1x int64_t 		Epoch time stamp of last magnetic field measurement in [msec]
	unsigned long long EPOCH_TIME_LAST_MAGNETIC_FIELD_MEASUREMENT_MSEC;
														// 64..75 	3x float 		Measured sun vector [svx, svy, svz] in body-frame
	float MEASURED_SUN_VECTOR_X_BF;
	float MEASURED_SUN_VECTOR_Y_BF;
	float MEASURED_SUN_VECTOR_Z_BF;
														// 76..83 	1x int64_t 	Epoch time stamp of last sun vector measurement in [msec]
	unsigned long long EPOCH_TIME_LAST_SUN_VECTOR_MEASUREMENT_MSEC;
} IADCS_100_Attitude_Telemetry;

//! IADCS-100 Power Telemetry 
typedef struct
{
	unsigned long long EPOCH_TIME_MSEC;					// 0..7 	1x int64_t 	Epoch time in [msec]
														// 8..19 	3x float	Power telemetry of all magnetorquers
	float MAGNETTORQUER_POWER_CONSUMPTION_W;			// 			1x float	Power consumption in [W]
	float MAGNETTORQUER_SUPPLY_VOLTAGE_V;				// 			1x float	Voltage in [V ]
	float MAGNETTORQUER_CURRENT_CONSUMPTION_A;			// 			1x float	Current in [A]
														// 20..31 	3x float 	PPower telemetry of star tracker
	float STARTRACKER_POWER_CONSUMPTION_W;				// 			1x float	Power consumption in [W]
	float STARTRACKER_SUPPLY_VOLTAGE_V;					// 			1x float	Voltage in [V ]
	float STARTRACKER_CURRENT_CONSUMPTION_A;			// 			1x float	Current in [A]
														// 32..43 	3x float	Power telemetry of iADCS main board
	float IADCS_POWER_CONSUMPTION_W;					// 			1x float 	Power consumption in [W]
	float IADCS_SUPPLY_VOLTAGE_V;						// 			1x float 	Voltage in [V ]
	float IADCS_CURRENT_CONSUMPTION_A;					// 			1x float 	Current in [A]
														// 44..55 	3x float	Power telemetry of all internal reaction wheels
	float REACTIONWHEEL_POWER_CONSUMPTION_W;			// 			1x float	Power consumption in [W]
	float REACTIONWHEEL_SUPPLY_VOLTAGE_V;				// 			1x float 	Voltage in [V ]
	float REACTIONWHEEL_CURRENT_CONSUMPTION_A;			// 			1x float 	Current in [A]
} IADCS_100_Power_Telemetry;

//! IADCS-100 Target Pointing Target Parameter Telemetry 
typedef struct
{
	unsigned long long EPOCH_TIME_MSEC;					// 0..7 		1x int64_t 		Epoch time in [msec]
	unsigned long long START_TIME_AS_EPOCH_TIME_MSEC;	// 8..15 		1x int64_t 		Start time as epoch time in [msec]
	unsigned long long STOP_TIME_AS_EPOCH_TIME_MSEC;	// 16..23 		1x int64_t 		Stop time as epoch time in [msec]
	unsigned long long OFFSET_TIME_MSEC;				// 24..31 		1x int64_t 		Offset time in [msec] (optional)
	unsigned int CONTROL_MODE;							// 32..35 		1x int32_t 		Control mode (see 6.5.4.7.5)
														// 36..71 		9x float 		(Internal use) Rotation matrix (Desired Body Frame Attitude wrt Defined Reference Frame)
	float ROTATION_MATRIX_BF_TO_RF_M11;					// 3x3 matrix
	float ROTATION_MATRIX_BF_TO_RF_M12;
	float ROTATION_MATRIX_BF_TO_RF_M13;
	float ROTATION_MATRIX_BF_TO_RF_M21;
	float ROTATION_MATRIX_BF_TO_RF_M22;
	float ROTATION_MATRIX_BF_TO_RF_M23;
	float ROTATION_MATRIX_BF_TO_RF_M31;
	float ROTATION_MATRIX_BF_TO_RF_M32;
	float ROTATION_MATRIX_BF_TO_RF_M33;
	float POLY_COEFF[28];								// 72..183 		28x float 		Polynomial coefficients (Degree max = 6)
	float START_QUATERNION_1;							// 184..199 	4x float 		Start quatertion
	float START_QUATERNION_2;
	float START_QUATERNION_3;
	float START_QUATERNION_4;
	float STOP_QUATERNION_1;							// 200..215 	4x float 		End quaternion
	float STOP_QUATERNION_2;
	float STOP_QUATERNION_3;
	float STOP_QUATERNION_4;
	float START_LATITUDE_WGS84_RAD;						// 216..223 	2x float 		Start lat, lon in [rad] and WGS84 frame
	float START_LONGITUDE_WGS84_RAD;
	float STOP_LATITUDE_WGS84_RAD;						// 224..231 	2x float 		End lat, lon in [rad] and WGS84 frame
	float STOP_LONGITUDE_WGS84_RAD;
} IADCS_100_Target_Pointing_Target_Parameter_Telemetry;

//! IADCS-100 Target Pointing Parameter Telemetry 
typedef struct
{
	int GENERAL_MODE;									// 0-3 			1x int32_t 		Mode: General Mode Identifier (see 6.5.4.7.7)
	int ATTITUDE_DETERMINATION_MODE;					// 4-7 			1x int32_t 		Attitude Determination Mode (see 6.5.4.7.6)
	int CONTROL_MODE;									// 8-11 		1x int32_t 		Control mode (see 6.5.4.7.5)
	float PREALIGNMENT_ANGLE_TOLERANCE_RAD;				// 12-27 		4x float 		Tolerance parameters, see 6.5.4.7.8
	float PREALIGNMENT_ANGLE_TOLERANCE_PER;
	float PREALIGNMENT_ANGULAR_VELOCITY_TOLERANCE_RADPS;
	float TARGET_THRESHOLD_RAD;
	unsigned long long START_TIME_AS_EPOCH_TIME_MSEC;	// 28-35 		1x int64_t 		Start time as epoch time in [msec]
	float TARGET_LATITUDE_WGS84_RAD;					// 36-39 		1x float 		Target latitude in [rad] and WGS84 frame
	float TARGET_LONGITUDE_WGS84_RAD;					// 40-43 		1x float 		Target longitude in [rad] and WGS84 frame
	unsigned long long UPDATE_INTERVAL_MSEC;			// 44-51 		1x int64_t 		Update interval [msec]
} IADCS_100_Target_Pointing_Parameter_Telemetry;

//! IADCS-100 Target Pointing Data Telemetry 
typedef struct
{
	unsigned long long CURRENT_EPOCH_TIME_MSEC;			// 0..7 		1x int64_t 		Current epoch time in [msec]
	unsigned long long EPOCH_TIME_OF_NEXT_ACTION_MSEC;	// 8..15 		1x int64_t 		Epoch time of nex action/ calclation [msec]
	unsigned int MAIN_REGISTER;							// 16..19 		1x uint32_t 	Main register (Optional)
	unsigned char CURRENT_STATE;						// 20 			1x uint8_t 		Current state of the state machine
	unsigned char TARGET_STATE;							// 21 			1x uint8_t 		State target
	unsigned char PREALIGNMENT_IS_READY;				// 22 			1x uint8_t 		[Pre alignment] Is ready?
	unsigned char PREALIGNMENT_CURRENT_ROTATION_AXIS;	// 23 			1x uint8_t 		[Pre alignment] Current rotation axis
	unsigned char PREALIGNMENT_ACTIVE_FLAGS;			// 24..26 		3x uint8_t 		[Pre alignment] Active flags for 3 axis
	float PREALIGNMENT_TARGET_ROTATION_ANGLE_X_RAD;		// 27..38 		3x float 		[Pre alignment] Target rotation angles in[rad]
	float PREALIGNMENT_TARGET_ROTATION_ANGLE_Y_RAD;
	float PREALIGNMENT_TARGET_ROTATION_ANGLE_Z_RAD;
	float PREALIGNMENT_ROTATION_ANGLE_X_RAD;			// 39..50 		3x float 		[Pre alignment] Rotation angles in[rad]
	float PREALIGNMENT_ROTATION_ANGLE_Y_RAD;
	float PREALIGNMENT_ROTATION_ANGLE_Z_RAD;
	float ANGULAR_VELOCITY_BF_RADPS;					// 51..62 		3x float 		Angular velocity in b-frame in[rad/sec]
	float TARGET_QUATERNION_1;							// 63..78 		4x float 		Calculated target quatinion
	float TARGET_QUATERNION_2;
	float TARGET_QUATERNION_3;
	float TARGET_QUATERNION_4;
	float ATTITUDE_QUATERNION_1;						// 79..94 		4x float 		Current attitude quaternion
	float ATTITUDE_QUATERNION_2;
	float ATTITUDE_QUATERNION_3;
	float ATTITUDE_QUATERNION_4;
	float REACTIONWHEEL_TARGET_SPEED_X_RADPS;			// 95..118 		6x float 		Target RW speeds in[rad/sec]
	float REACTIONWHEEL_TARGET_SPEED_Y_RADPS;
	float REACTIONWHEEL_TARGET_SPEED_Z_RADPS;
	float REACTIONWHEEL_TARGET_SPEED_U_RADPS;
	float REACTIONWHEEL_TARGET_SPEED_V_RADPS;
	float REACTIONWHEEL_TARGET_SPEED_W_RADPS;
	float REACTIONWHEEL_CURRENT_SPEED_X_RADPS;			// 119..142 	6x float 		Current RW speeds in[rad/sec]
	float REACTIONWHEEL_CURRENT_SPEED_Y_RADPS;
	float REACTIONWHEEL_CURRENT_SPEED_Z_RADPS;
	float REACTIONWHEEL_CURRENT_SPEED_U_RADPS;
	float REACTIONWHEEL_CURRENT_SPEED_V_RADPS;
	float REACTIONWHEEL_CURRENT_SPEED_W_RADPS;
} IADCS_100_Target_Pointing_Data_Telemetry;


/* ------------- IADCS-100 command parameters ----------*/

//! IADCS-100 star tracker quaternion with epoch time
typedef struct
{
	float Q_1;
	float Q_2;
	float Q_3;
	float Q_4;
	unsigned long long EPOCH_TIME_MSEC;
} IADCS_100_Startracker_Quaternion;

/** \struct IADCS-100 Quaternion
 *  \bried IADCS-100 Quaternion with Q1,Q2,Q3,Q4 parameters.
 */
typedef struct
{
	float Q_1;
	float Q_2;
	float Q_3;
	float Q_4;
} IADCS_100_Quaternion;

//! Reduced Start tracker Event LIST
typedef struct
{
	uint8_t HEADER; 																	// reduced event list header
	uint8_t NUM_EVENT; 																	// max 23
	int32_t XISF[23] = {0}; 															// [px/100] x-position of event (image sensor frame)
	int32_t YISF[23] = {0};																// [px/100] y-position of event (image sensor frame)
} IADCS_100_REDUCED_STARTRACKER_EVENT_LIST;

//! IADCS-100 Orbit TLE data
typedef struct
{
	char TLE_1[70];
	char TLE_2[70];
	unsigned long long UPDATE_INTERVAL;
} IADCS_100_Orbit_TLE_Data;

//! IADCS-100 Orbit Propagation Data
typedef struct
{
	IADCS_100_Vector3_float position;
	IADCS_100_Vector3_float velocity;
	double julian_date;
} IADCS_100_Orbit_Propagation_Data;

//! IADCS-100 Sun Vector with timestamp
typedef struct
{
	IADCS_100_Vector3_float vector_bf;
	unsigned long long timestamp;
} IADCS_100_Sun_Vector;

//! IADCS-100 Sun Sensor Parameters Single
typedef struct
{
	IADCS_100_Vector3_float sun_vector;
	float intensity;
} IADCS_100_SunSensor_Parameters;

//! IADCS-100 Sun Sensor Parameters All Sensors
typedef struct
{
	IADCS_100_SunSensor_Parameters sensor_1;
	IADCS_100_SunSensor_Parameters sensor_2;
	IADCS_100_SunSensor_Parameters sensor_3;
	IADCS_100_SunSensor_Parameters sensor_4;
	IADCS_100_SunSensor_Parameters sensor_5;
	IADCS_100_SunSensor_Parameters sensor_6;
} IADCS_100_SunSensor_Parameters_All_Sensors;

//! IADCS-100 Gyroscope Parameters 
typedef struct
{
	unsigned char OUTPUT_TYPE; 						// 0 			1x uint8_t 		Output Type
	IADCS_100_Vector3_float SENSOR_STDDEV;			// 1..12 		3x float 		Standard deviation
	IADCS_100_Vector3_float SENSOR_SENSITIVITY;		// 13..24 		3x float 		Sensitivity Sensor sensitivity vector[sx, sy, sz] in
	IADCS_100_Quaternion QUATERNION;				// 25..40 		4x float 		Transformation quaternion
	unsigned long long UPDATE_INTERVAL_MSEC;		// 41..48 		1x int64_t 		Update interval Sensor update interval in [msec]
	unsigned char SENSOR_CORRECTION_ENABLE;			// 49 			1x uint8_t 		Sensor correction
	IADCS_100_Matrix3_float CORRECTION_SCALE;		// 50..85 		9x float 		Correction scale matrix [3x3] Sg
	IADCS_100_Vector3_float CORRECTION_OFFSET;		// 86..97 		3x float 		Correction offset
	unsigned int BIAS_COUNTER;						// 98..101 		1x uint32_t 	Bias counter
	unsigned char AVG_FILTER_ENABLE;				// 102 			1x uint8_t 		Averaging filter enable flag
	unsigned int AVG_FILTER_COUNTER;				// 103..106		1x uint32_t 	Averaging filter
	float AVG_FILTER_CRITERION; 					// 107..110		1x float 		Sensor values criterion for averaging filter Measurement data is only used by the filter, if the deviation between the current value and the last one is below the max. allowed deviation in [rad/sec]
} IADCS_100_Gyroscope_Parameters;

//! IADCS-100 Magnetometer Parameters 
typedef struct
{
	IADCS_100_Vector3_float SENSOR_STDDEV;
	IADCS_100_Vector3_float SENSOR_SENSITIVITY;
	IADCS_100_Quaternion QUATERNION;
	unsigned long long UPDATE_INTERVAL_MSEC;
	unsigned char SENSOR_CORRECTION_ENABLE;
	IADCS_100_Matrix3_float CORRECTION_SCALE;
	IADCS_100_Vector3_float CORRECTION_OFFSET;
	unsigned char AVG_FILTER_ENABLE;
	unsigned int AVG_FILTER_COUNTER;
	float AVG_FILTER_CRITERION;
	unsigned char MOVING_AVG_FILTER_ENABLE;
	float MOVING_AVG_FILTER_GAIN;
	float MOVING_AVG_FILTER_CRITERION;
} IADCS_100_Magnetometer_Parameters;

//! IADCS-100 Single Reaction Wheel Parameters 
typedef struct
{
	float MOMENT_OF_INERTIA;
	float MAX_SPEED;
	float MAX_TORQUE;
	float MOTOR_CONSTANT;
	unsigned long long MAX_VALID_TIME_SPEED_MEASUREMENT;
} IADCS_100_Single_Reactionwheel_Parameters;

//! IADCS-100 Reaction Wheel Array Parameters 
typedef struct
{
	unsigned int NUMBER_OF_WHEELS;
	unsigned int CONTROL_MODE;
	float MOMENT_OF_INERTIA;
	float MAX_SPEED;
	float MAX_TORQUE;
	float MOTOR_CONSTANT;
	unsigned long long UPDATE_INTERVAL;
	unsigned long long MAX_VALID_TIME_SPEED_MEASUREMENT;
	float TORQUE_DISTRIBUTION_MATRIX_M00; // 3x6 matrix for 6 wheels, order: M00-M05,M10-M15,M20-M26
	float TORQUE_DISTRIBUTION_MATRIX_M01;
	float TORQUE_DISTRIBUTION_MATRIX_M02;
	float TORQUE_DISTRIBUTION_MATRIX_M03;
	float TORQUE_DISTRIBUTION_MATRIX_M04;
	float TORQUE_DISTRIBUTION_MATRIX_M05;
	float TORQUE_DISTRIBUTION_MATRIX_M10;
	float TORQUE_DISTRIBUTION_MATRIX_M11;
	float TORQUE_DISTRIBUTION_MATRIX_M12;
	float TORQUE_DISTRIBUTION_MATRIX_M13;
	float TORQUE_DISTRIBUTION_MATRIX_M14;
	float TORQUE_DISTRIBUTION_MATRIX_M15;
	float TORQUE_DISTRIBUTION_MATRIX_M20;
	float TORQUE_DISTRIBUTION_MATRIX_M21;
	float TORQUE_DISTRIBUTION_MATRIX_M22;
	float TORQUE_DISTRIBUTION_MATRIX_M23;
	float TORQUE_DISTRIBUTION_MATRIX_M24;
	float TORQUE_DISTRIBUTION_MATRIX_M25;
} IADCS_100_Reactionwheel_Array_Parameters;

//! IADCS-100 Magnetorquer Parameters 
typedef struct
{
	IADCS_100_Vector3_float CONV_FACTORS_DIPOLE_MOMENT_TO_CURRENT;
	IADCS_100_Vector3_float MAX_DIPOLE_MOMENT;
	IADCS_100_Matrix3_float ORIENTATION_TO_BF;
	unsigned long long RELAX_TIME_X;
	unsigned long long RELAX_TIME_Y;
	unsigned long long RELAX_TIME_Z;
} IADCS_100_Magnetorquer_Parameters;

//! IADCS-100 PID Controller Parameters 
typedef struct
{
	float K_P;									//!	0..3 		1x float 		kp Propotional gain
	float K_I;									//!	4..7 		1x float 		ki Integrator gain
	float K_D;									//!	8..11 		1x float 		kd Diff. gain
	float K_AW;									//!	12..15 		1x float 		kaw Anti-windup gain
	float TH_AW;								//!	16..19 		1x float 		thraw Anti-windup threshold
	unsigned int CTRL_ACTUATOR_CMD_IF; 			//	20..23 		1x int32_t 		cmdtype Controller - actuator command interface 6.4.3.3
	unsigned long long UPDATE_INTERVAL;			//	24..31 		1x int64_t 		tupdate Update interval in [msec]
} IADCS_100_PID_Controller_Parameters;

//! IADCS-100 Cascaded PID Controller Parameters 
typedef struct
{
	float K_P_IN;								//!	0..3 		1x float 		kp PID-In- Propotional gain
	float K_I_IN;								//!	4..7 		1x float 		ki PID-In- Integrator gain
	float K_D_IN;								//!	8..11 		1x float 		kd PID-In- Diff. gain
	float K_AW_IN;								//!	12..15 		1x float 		kaw PID-In- Anti-windup gain
	float TH_AW_IN;								//!	16..19 		1x float 		thraw PID-In- Anti-windup threshold
	float K_P_OUT;								//!	20..23		1x float 		kp PID-Out- Propotional gain
	float K_I_OUT;								//!	24..27		1x float 		ki PID-Out- Integrator gain
	float K_D_OUT;								//!	28..31		1x float 		kd PID-Out- Diff. gain
	float K_AW_OUT;								//!	32..35		1x float 		kaw PID-Out- Anti-windup gain
	float TH_AW_OUT;							//!	36..39		1x float 		thraw PID-Out- Anti-windup threshold
	unsigned int CTRL_ACTUATOR_CMD_IF;			//!	40..43		1x int32_t 		cmdtype Controller - actuator command interface 6.4.3.3
	unsigned long long UPDATE_INTERVAL;			//!	44..51		1x int64_t 		tupdate Update interval in [msec]
} IADCS_100_Cascaded_PID_Controller_Parameters;

//! IADCS-100 BDot Controller Parameters 
typedef struct
{
	float K;									//!	0..3 		1x float 		k Control gain:
												//!		k < 0: Detumbling
												//!		k > 0: Momentum build up
	unsigned int CTRL_ACTUATOR_CMD_IF; 			//!	4..7 		1x int32_t 		cmdtype Controller - actuator command interface 6.4.3.3
	unsigned long long UPDATE_INTERVAL;			//!	8..15 		1x int64_t 		tupdate Update interval in [msec]
} IADCS_100_BDot_Controller_Parameters;

//! IADCS-100 Sun Pointing Controller Parameters 
typedef struct
{
	float K_P;									//!	0..3 		1x float 		kp Controller feedback gain affecting the behavior of the control error.
	IADCS_100_Matrix3_float K_V;				//!	4..39 		9x float 		kv kv is an [3x3] symmetric damping matrix affecting the behavior of the angular velocity
	IADCS_100_Vector3_float MAX_TORQUE;			//!	40..51 		3x float 		trqmax Maximum available torques on 3 axes [trqmax,x, trqmax,y, trqmax,z] in Nm and body frame. This values are usually related to the maximum available torquers of the actuators.
	unsigned int CTRL_ACTUATOR_CMD_IF;			//!	52..55 		1x int32_t 		cmdtype Controller - actuator command interface 6.4.3.3
	unsigned long long UPDATE_INTERVAL;			//!	56..63 		1x int64_t 		tupdate Update interval in [msec]
} IADCS_100_SunPointing_Controller_Parameters;

//! IADCS-100 Sliding Controller Parameters 
typedef struct
{
	float K_1; 									//!	0..3 		1x float 		k1 Sliding gain k1
	float K_2; 									//!	4..7 		1x float 		k2 Sliding gain k2
	unsigned int CTRL_ACTUATOR_CMD_IF; 			//!	8..11 		1x int32_t 		cmdtype Controller - actuator command interface 6.4.3.3
	unsigned long long UPDATE_INTERVAL; 		//!	12..19 		1x int64_t 		tupdate Update interval in [msec]
} IADCS_100_Sliding_Controller_Parameters;

//! IADCS-100 Detumblind Mode Parameters 
typedef struct
{
	unsigned long long START_EPOCH_TIME;  		//! 0..7 		int64_t 		tstart Start time as epoch time in [msec]
	unsigned long long STOP_EPOCH_TIME;			//! 8..15 		int64_t 		tstop Stop time as epoch time in [msec]
} IADCS_100_Detumbling_Mode_Parameters;

//! IADCS-100 Sun Pointing Mode Parameters 
typedef struct
{
	IADCS_100_Vector3_float TARGET_VECTOR_BF;	//!	0..11 		3x float 		vtgt,b Target vector [vtgt,x, vtgt,y, vtgt,z ] as unit vector in body frame
	unsigned long long START_EPOCH_TIME;		//!	12..19 		int64_t 		tstart Start time as epoch time in [msec]
	unsigned long long STOP_EPOCH_TIME;			//!	20..27 		int64_t 		tstop Stop time as epoch time in [msec]
} IADCS_100_SunPointing_Mode_Parameters;

//! IADCS-100 Target Pointing Tolerance Parameters 
typedef struct
{
	float PREALIGNMENT_ANGLE_TOLERANCE_RAD;
	float PREALIGNMENT_ANGLE_TOLERANCE_PERCENT;
	float PREALIGNMENT_ANGULAR_VELOCITY_TOLERANCE_RADPS;
	float PREALIGNMENT_TARGET_THRESHOLD_RAD;
} IADCS_100_TargetPointing_Tolerance_Parameters;

//! IADCS-100 Target Pointing Constant Velocity Parameters 
typedef struct
{
	unsigned int DETERMINATION_MODE; 										//! 0..3 		1x int32_t 		modeatt_det Attitude determination mode 5.5.4.5.6
	IADCS_100_Vector3_float LOS_VECTOR_BF; 									//! 4..15 		3x float 		vlos Line of sight vector [vlos,x, vlos,y, vlos,z] as unit vector in body frame
	IADCS_100_Vector3_float FLIGHT_VECTOR_BF; 								//! 16..27 		3x float 		vvel Flight vector [vvel,x, vvel,y, vvel,z ] as unit vector in body frame
	float START_LATITUDE; 													//! 28..35 		2x float 		latstart; latend Start latitude, End latitude [rad] in WGS84 frame
	float STOP_LATITUDE;
	float START_LONGITUDE; 													//! 36..43 		2x float 		lonstart; lonend Stop latitude, longitude [rad] WGS84 frame
	float STOP_LONGITUDE;
	unsigned long long START_EPOCH_TIME; 									//! 44..51 		1x int64_t 		tstart Start time as epoch time in [msec]
	unsigned long long STOP_EPOCH_TIME; 									//! 52..59 		1x int64_t 		tstop Stop time as epoch time in [msec]
	unsigned long long OFFSET_TIME; 										//! 60..67 		1x int64_t 		toffset Offset time in [msec] (optional)
	unsigned long long UPDATE_INTERVAL; 									//! 68..75 		1x int64_t 		tupdate Update interval in [msec]
	IADCS_100_TargetPointing_Tolerance_Parameters TOLERANCE_PARAMETERS; 	//! 76..91 		4x float 		Tolerance parameters, see 5.5.4.5.8
} IADCS_100_Earth_TargetPointing_Const_Velocity_Parameters;

//! IADCS-100 Target Pointing Fixed Parameters 
typedef struct
{
	unsigned int DETERMINATION_MODE;										//!	0..3 		1x int32_t 		modeatt_det Attitude determination mode 5.5.4.5.6
	IADCS_100_Vector3_float LOS_VECTOR_BF;									//!	4..15 		3x float 		vlos Line of sight vector [vlos,x, vlos,y, vlos,z] as unit vector in body frame
	IADCS_100_Vector3_float FLIGHT_VECTOR_BF;								//!	16..27 		3x float 		vvel Flight vector [vvel,x, vvel,y, vvel,z ] as unit vector in body frame
	float TARGET_LATITUDE;													//!	28..35 		2x float 		lat; lon Target latitude, lon lontitude in [rad] in WGS84 frame
	float TARGET_LONGITUDE;
	unsigned long long OFFSET_TIME; 										//!	36..43 		1x int64_t 		toffset Offset time in [msec] (optional)
	unsigned long long UPDATE_INTERVAL;										//!	44..51 		1x int64_t 		tupdate Update interval in [msec]
	IADCS_100_TargetPointing_Tolerance_Parameters TOLERANCE_PARAMETERS;		//!	52..67 		4x float 		Tolerance parameters, see 5.5.4.5.8
} IADCS_100_Earth_TargetPointing_Fixed_Parameters;

//! IADCS-100 Nadir Pointing Parameters 
typedef struct
{
	unsigned int DETERMINATION_MODE; 										//! 0..3 		1x int32_t 		modeatt_det Attitude determination mode 5.5.4.5.6
	IADCS_100_Vector3_float LOS_VECTOR_BF; 									//! 4..15 		3x float 		vlos Line of sight vector [vlos,x, vlos,y, vlos,z] as unit vector in body frame
	IADCS_100_Vector3_float FLIGHT_VECTOR_BF; 								//! 16..27 		3x float 		vvel Flight vector [vvel,x, vvel,y, vvel,z ] as unit vector in body frame
	unsigned long long OFFSET_TIME; 										//! 28..35 		1x int64_t 		toffset Offset time in [msec] (optional)
	unsigned long long UPDATE_INTERVAL; 									//! 36..43 		1x int64_t 		tupdate Update interval in [msec]
	IADCS_100_TargetPointing_Tolerance_Parameters TOLERANCE_PARAMETERS;		//! 44..59 		4x float 		Tolerance parameters, see 5.5.4.5.8
} IADCS_100_NadirPointing_Parameters;


/* ------------------------------ Target Pointing Telemetry Structures ----------------------------*/

//! IADCS-100 Target Pointing Target Parameters Telemetry
typedef struct
{
	unsigned long long CURRENT_EPOCH_TIME_MSEC;								//!	0..7 		1x int64_t 		Current Epoch time in [msec]
	unsigned long long START_EPOCH_TIME;									//!	8..15 		1x int64_t 		Start time as epoch time in [msec]
	unsigned long long STOP_EPOCH_TIME;										//!	16..23 		1x int64_t 		Stop time as epoch time in [msec]
	unsigned long long OFFSET_TIME_MSEC;									//!	24..31 		1x int64_t		Offset time in [msec] (optional)
	unsigned int CONTROL_MODE;												//!	32..35 		1x int32_t 		Control mode (see 5.5.4.5.5)
	IADCS_100_Matrix3_float ROTATION_MATRIX;								//!	36..71 		9x float 		(Internal use)
																			//! 							Rotation matrix (Desired Body Frame Attitude wrt Defined Reference Frame)
	float POLYNOMIAL_COEFF[28];												//!	72..183 	28x float 		(Optional) Polynomial coefficients
																			//!								(Degree max = 6)
																			//!								These coefficients are used to calculate/update the target quaternion during pointing mode.
	IADCS_100_Quaternion START_QUATERNION;									//!	184..199 	4x float 		Start quatertion
	IADCS_100_Quaternion END_QUATERNION;									//!	200..215 	4x float 		End quaternion
	float START_LATITUDE;													//!	216..223 	2x float 		Start lat, lon in [rad] and WGS84 frame
	float START_LONGITUDE;
	float STOP_LATITUDE;													//!	224..231 	2x float 		End lat, lon in [rad] and WGS84 frame
	float STOP_LONGITUDE;
} IADCS_100_TargetPointing_Target_Parameters_Telemetry;

//! IADCS-100 Target Pointing Operation Parameters Telemetry
typedef struct
{
	unsigned int GENERAL_MODE;												//!	0-3 		1x int32_t 		Mode: (see 5.5.4.5.7)
	unsigned int DETERMINATION_MODE;										//!	4-7 		1x int32_t 		Attitude Determination Mode (see 5.5.4.5.6)
	unsigned int CONTROL_MODE;												//!	8-11 		1x int32_t 		Control mode (see 5.5.4.5.5)
	IADCS_100_TargetPointing_Tolerance_Parameters TOLERANCE_PARAMETERS;		//!	12-27 		4x float 		Tolerance parameters, see 5.5.4.5.8
	unsigned long long START_EPOCH_TIME;									//!	28-35 		1x int64_t 		Start time as epoch time in [msec]
	float TARGET_LATITUDE;													//!	36-39 		1x float 		Target latitude in [rad] and WGS84 frame
	float TARGET_LONGITUDE;													//!	40-43 		1x float 		Target lontitude in [rad] and WGS84 frame
	unsigned long long UPDATE_INTERVAL; 									//!	44-51 		1x int64_t 		Update interval [msec]
} IADCS_100_TargetPointing_Operation_Parameters_Telemetry;

//! IADCS-100 Target Pointing Operation Data Telemetry
typedef struct
{
	unsigned long long CURRENT_EPOCH_TIME_MSEC;								//!	0..7 		1x int64_t 		Current epoch time in [msec]
	unsigned long long NEXT_ACTION_EPOCH_TIME_MSEC;							//!	8..15 		1x int64_t 		Epoch time of next action/ calclation [msec]
	unsigned int MAIN_REGISTER;												//!	16..19 		1x uint32_t 	Main register (Optional)
	unsigned char CURRENT_STATE;											//!	20 			1x uint8_t 		Current state of the state machine (internal use only)
	unsigned char STATE_TARGET;												//!	21 			1x uint8_t 		State target
																			//!								0 - Point startracker in dark target
																			//!								1 - Final target pointing
	unsigned char PREALIGNMENT_IS_READY;									//!	22 			1x uint8_t 		[Pre alignment] Is ready?
	unsigned char PREALIGNMENT_CURRENT_ROTATION_AXIS;						//!	23 			1x uint8_t 		[Pre alignment] Current rotation axis
	IADCS_100_Vector3_ushort PREALIGNMENT_ACTIVE_FLAG;						//!	24..26 		3x uint8_t 		[Pre alignment] Active flags for 3 axis
	IADCS_100_Vector3_float PREALIGNMENT_TARGET_ROTATION_ANGLE;				//!	27..38 		3x float 		[Pre alignment] Target rotation angles in[rad]
	IADCS_100_Vector3_float PREALIGNMENT_ROTATION_ANGLE;					//!	39..50 		3x float 		[Pre alignment] Rotation angles in[rad]
	IADCS_100_Vector3_float ANGULAR_VELOCITY_BF;							//!	51..62 		3x float 		Angular velocity in b-frame in[rad/sec]
	IADCS_100_Quaternion TARGET_QUATERNION;									//!	63..78 		4x float 		Calculated target quaternion
	IADCS_100_Quaternion ATTITUDE_QUATERNION;								//!	79..94 		4x float 		Current attitude quaternion
	IADCS_100_Vector6_float TARGET_RW_SPEED;								//!	95..118 	6x float 		Target RW speeds in[rad/sec]
	IADCS_100_Vector6_float CURRENT_RW_SPEED;								//!	119..142 	6x float 		Current RW speeds in[rad/sec]
} IADCS_100_TargetPointing_Operation_Data_Telemetry;

/* ---------------------------- Special Command Structures --------------------------- */

//! IADCS-100 Target Pointing Inertial Parameters
typedef struct
{
	unsigned int DETERMINATION_MODE;										//!<	0..3 		1x int32_t 		Attitude Determination Mode ( Section 5.5.4.5.6)
	IADCS_100_Vector3_float LOS_VECTOR_BF;									//!<	4..15 		3x float 		Line of sight vector in body frame
	IADCS_100_Vector3_float FLIGHT_VECTOR_BF;								//!<	16..27 		3x float 		Flight vector in body frame
	unsigned int DEGREE;													//!<	28..31 		1x int32_t 		Degree
	IADCS_100_Quaternion BASE_QUATERNION;									//!<	32..47 		4x float 		Base Quaternion when degree is 0
																			//!<	48..143 	24x float 		Used for base quaternion when degree is 6
	unsigned long long START_EPOCH_TIME;									//!<	144..151 	1x int64_t 		Start time as unix epoch in [msec]
	unsigned long long STOP_EPOCH_TIME;										//!<	152..159 	1x int64_t 		Stop time as unix epoch in [msec]
	unsigned long long OFFSET_TIME; 										//!<	160..167 	1x int64_t 		Offset time in [msec] (optional)
	unsigned long long UPDATE_INTERVAL;										//!<	168..175 	1x int64_t 		Update Interval in [msec]
} IADCS_100_TargetPointing_Inertial_Parameters;

//! IADCS-100 Kalman Filter Parameters
typedef struct
{
	double BIAS_PROCESS_VARIANCE;											//!<	0..7		1x double		bias process variance = 1.000000000 x 10^-16 (default)
	double ATTITUDE_PROCESS_VARIANCE;										//!<	8..15		1x double		attitude process variance = 2.741556778 x 10^-9 (default)
	double SUN_MAG_MEASUREMENT_VARIANCE;									//!<	16..23		1x double		sun/mag measurement variance = 5.288496871 x 10^-5 (default)
	double STS_MEASUREMENT_VARIANCE;										//!<	24..32		1x double		sts measurement variance = 1.504283555 x 10^-7 (default)
	unsigned char DISABLE_BIAS;												//!<	33			1x uint8_t		disable bias, disable bias = 1, enable bias = 0 (default)
} IADCS_100_KalmanFilter_Parameters;

//! IADCS-100 Target Pointing Operation Parameters
typedef struct
{
	unsigned long long UPDATE_INTERVAL;
	float ANGLE_TOLERANCE_RAD;
	float ANGLE_TOLERANCE_PERC;
	float SPEED_TOLERANCE_RADPS;
	IADCS_100_Vector3_float ANGLES;
	unsigned long long WAIT_TIME;
} IADCS_100_TargetPointing_Operation_Parameters;

/* ------------------------------ Class Definition ----------------------------*/

/** IADCS-100 device class
* 	\brief Contains members for device address and general information. 
*	  Set and Get functions for device address, description and interface type.
* 	Methods to generate correct message sequences for write and read commands.
*/
class IADCS_100 : public SEPP_I2C_DEV
{
	private:
		uint16_t crc_tabccitt[256];
		uint8_t crc8_tabccitt[256];

		/* CRC check methods */
		uint8_t crc8_ccitt( const unsigned char *input_str, unsigned int num_bytes );
		uint8_t crc8_ccitt_generic(const unsigned char *data , unsigned int n, unsigned char start_value);
		void init_crc8ccitt_table( void );
		bool check_message_crc8(unsigned char *message, unsigned int num_bytes);

		unsigned char Check_Command_Errors(void);

		/* ----------- Byte Array Conversion Helper ---------- */
		unsigned short ToUInt16(unsigned char *byte_array);
		void FromUInt16(unsigned char *byte_array, unsigned int value);
		unsigned int ToUInt32(unsigned char *byte_array);
		int32_t ToInt32_ST(unsigned char *byte_array);
		void FromUInt32(unsigned char *byte_array, unsigned int value);
		unsigned long long ToUInt64(unsigned char *byte_array);
		void FromUInt64(unsigned char *byte_array, unsigned long long value);
		float ToFloat32(unsigned char *byte_array);
		void FromFloat32(unsigned char *byte_array, float value);
		double ToFloat64(unsigned char *byte_array);
		void FromFloat64(unsigned char *byte_array, double value);

		/* ----------------------- Misc ---------------------- */
		bool isLittleEndian(void);

	public:
		IADCS_100();
		IADCS_100(unsigned int i2c_address, std::string i2c_device_name);
		virtual ~IADCS_100();

		/* public Methods for Device Configuration */
		void Connect(unsigned int i2c_address, std::string i2c_device_name);
		void Disconnect();
		void PrintDeviceInfo();                               			//!< Print device summary / info
   
		/* -------- Generic I2C Write and Read Methods ---------- */
		void IADCS_Generic_Set(unsigned short cmd_id, unsigned char *data_bytes, unsigned int num_data_bytes);
		void IADCS_Generic_Get(unsigned short cmd_id, unsigned char *data_bytes, unsigned int num_data_bytes, unsigned char *response_bytes, unsigned int num_response_bytes);
		void IADCS_Startracker_Native_Get(unsigned char *response_bytes, unsigned int num_response_bytes);

		/* ------------ 5. Data Interface Commands -------------- */
		/* --------- 5.1.1 System Low Level Commands ------------ */
		void Set_DCDC_Configuration(unsigned int register_value);		//!< Set dcdc power state for iADCS components

		/* ---------------5.1.2 System Commands ----------------- */

		/* -------------- Epoch and System Time ----------------- */
		void Set_Epoch_Time(unsigned long long epoch_time_msec);					//!< Set iADCS epoch time in msec
		void Get_Epoch_Time(unsigned long long *epoch_time_msec);				//!< Get iADCS epoch time in msec
		void Get_System_Time(unsigned long long *system_time_msec);				//!< Get system	time from startup in msec

		/* ------------- Configuration and Setup ---------------- */
		void Set_Power_Update_Interval(unsigned long long interval_msec);
		void Get_Power_Update_Interval(unsigned long long *interval_msec);
		void Set_Temperature_Update_Interval(unsigned long long interval_msec);
		void Get_Temperature_Update_Interval(unsigned long long *interval_msec);
		void Set_Active_Gyroscope(unsigned char gyro_index);
		void Set_Active_Magnetometer(unsigned char magnetometer_index);

		/* ------------- Device Telemetry Methods --------------. */
		void Get_Info_Frame(IADCS_100_Info_Telemetry *info_tm);							//!< Get info frame telemetry
		void Get_Standard_Telemetry(IADCS_100_Standard_Telemetry *std_tm);				//!< Get iADCS standard telemetry
		void Get_Sensor_Telemetry(IADCS_100_Sensor_Telemetry *sensor_tm);				//!< Get iADCS sensor telemetry
		void Get_Actuator_Telemetry(IADCS_100_Actuator_Telemetry *actuator_tm);			//!< Get iADCS actuator telemetry
		void Get_Attitude_Telemetry(IADCS_100_Attitude_Telemetry *attitude_tm);			//!< Get iADCS attitude telemetry
		void Get_Power_Telemetry(IADCS_100_Power_Telemetry *power_tm);					//!< Get iADCS power status telemetry

		/* ------------------- Register Methods ----------------- */
		void Get_System_Scheduler_Register(unsigned int *scheduler_reg);
		void Set_System_Scheduler_Register(unsigned int scheduler_reg);
		void Clear_System_Error_Register(void);

		/* ------------ 5.1.3 Startracker Commands -------------- */
		void Set_Startracker_Quaternion(IADCS_100_Startracker_Quaternion quat);
		void Get_Startracker_Quaternion(IADCS_100_Startracker_Quaternion *quat);
		void Get_Startracker_Event_List(IADCS_100_REDUCED_STARTRACKER_EVENT_LIST *event_list);										//!< Get star list data from ST200 [AD1]
		void Set_Startracker_Operation_Mode(unsigned char mode);
		void Get_Startracker_Operation_Mode(unsigned char *mode);
		void Send_Startracker_Native_Command(unsigned char *native, unsigned int number_of_bytes);
		void Check_Startracker_Native_Reply(unsigned int *number_of_bytes); 							//!< Check whether a reply of a native command was sent.
		void Read_Startracker_Native_Reply(unsigned char *reply, unsigned int number_of_bytes);   		//!< Get/Read Native Reply Command

		/* --------------- 5.1.4 Orbit Commands ----------------- */
		void Init_Orbit_Module(IADCS_100_Orbit_TLE_Data tle_data, unsigned char *success_flag);
		void Get_Orbit_Data(IADCS_100_Orbit_Propagation_Data *prop_data);

		/* ------------------ 5.1.5 HIL commands ---------------- */
		void Set_HIL_Configuration(unsigned int reg_value);			//!< Set HIL configuration to set certain devices in HIL-mode, that values can be set from outside.
		void Get_HIL_Configuration(unsigned int *reg_value);		//!< Get HIL configuration


		/* ------------- 5.2 Sensor Command Interface ----------- */

		/* ------------- 5.2.1 Gyroscope ------------------------ */
		/* ------------- 5.2.1.1 Gyroscope Commands ------------- */
		void Set_Gyro_Values(unsigned char gyro_index, IADCS_100_Vector3_float axis_rate_SF_radps);
		void Get_Gyro_Values(unsigned char gyro_index, IADCS_100_Vector3_float *axis_rate_SF_radps, unsigned long long *timestamp_msec);	//!< Get iADCS Gyroscope Rate
		void Set_Gyro_Parameters(unsigned char gyro_index, IADCS_100_Gyroscope_Parameters params);
		void Get_Gyro_Parameters(unsigned char gyro_index, IADCS_100_Gyroscope_Parameters *params);
		void Set_Gyro_Update_Interval(unsigned char gyro_index, unsigned long long interval_msec);
		void Get_Gyro_Update_Interval(unsigned char gyro_index, unsigned long long *interval_msec);
		void Enable_Gyro_Correction(unsigned char gyro_index);
		void Disable_Gyro_Correction(unsigned char gyro_index);
		void Start_Gyro_Bias_Calculation(unsigned char gyro_index);			//!< Start Gyro Zero Bias Calculation
		void Set_Gyro_Bias_Value(unsigned char gyro_index, IADCS_100_Vector3_float gyro_bias_radps);
		void Get_Gyro_Bias_Value(unsigned char gyro_index, IADCS_100_Vector3_float *gyro_bias_radps);	//!< Get iADCS Gyroscope Bias
		void Enable_Gyro_Bias_Removement(unsigned char gyro_index);
		void Disable_Gyro_Bias_Removement(unsigned char gyro_index);
		void Enable_Gyro_Averaging_Filter(unsigned char gyro_index);
		void Disable_Gyro_Averaging_Filter(unsigned char gyro_index);

		/* ------------- 5.2.2 Magnetometers -------------------- */
		/* ------------- 5.2.2.1 Magnetometer Commands ---------- */
		void Set_Magnetometer_Values(unsigned char magnetometer_index, IADCS_100_Vector3_float magn_flux_density_T);
		void Get_Magnetometer_Values(unsigned char magnetometer_index, IADCS_100_Vector3_float *magn_flux_density_T, unsigned long long *timestamp_msec);	//!< Get iADCS Gyroscope Rate
		void Set_Magnetometer_Parameters(unsigned char magnetometer_index, IADCS_100_Magnetometer_Parameters params);
		void Get_Magnetometer_Parameters(unsigned char magnetometer_index, IADCS_100_Magnetometer_Parameters *params);
		void Set_Magnetometer_Update_Interval(unsigned char magnetometer_index, unsigned long long interval_msec);
		void Get_Magnetometer_Update_Interval(unsigned char magnetometer_index, unsigned long long *interval_msec);
		void Enable_Magnetometer_Correction(unsigned char magnetometer_index);
		void Disable_Magnetometer_Correction(unsigned char magnetometer_index);
		void Enable_Magnetometer_Averaging_Filter(unsigned char magnetometer_index);
		void Disable_Magnetometer_Averaging_Filter(unsigned char magnetometer_index);
		void Enable_Magnetometer_Moving_Averaging_Filter(unsigned char magnetometer_index);
		void Disable_Magnetometer_Moving_Averaging_Filter(unsigned char magnetometer_index);


		/* ------------- 5.2.3 Sun Sensors ---------------------- */
		/* ------------- 5.2.3.1 Sun Sensor Commands ------------ */
		void Set_Sun_Vector(IADCS_100_Vector3_float sun_vector_SF, unsigned long long epoch_timestamp_msec);


		/* ------------- 5.3 Actuator Command Interface --------- */

		/* ------------- 5.3.1 Reaction Wheels ------------------ */
		/* ------------- 5.3.1.1 Reaction Wheel Commands -------- */
		void Set_ReactionWheel_Speed(unsigned char wheel_index, float speed_radps);
		void Get_ReactionWheel_Speed(unsigned char wheel_index, float *speed_radps);
		void Set_ReactionWheel_Parameters(unsigned char wheel_index, IADCS_100_Single_Reactionwheel_Parameters params);
		void Get_ReactionWheel_Parameters(unsigned char wheel_index, IADCS_100_Single_Reactionwheel_Parameters *params);
		void Set_ReactionWheel_All_Update_Interval(unsigned long long interval_msec);
		void Get_ReactionWheel_All_Update_Interval(unsigned long long *interval_msec);
		void Enable_ReactionWheel_Idle_Mode(unsigned char wheel_index);
		void Set_ReactionWheel_All_Speeds(IADCS_100_Vector6_float speed_radps);
		void Get_ReactionWheel_All_Speeds(IADCS_100_Vector6_float *speed_radps);
		void Set_ReactionWheel_All_Parameters(IADCS_100_Reactionwheel_Array_Parameters params);
		void Get_ReactionWheel_All_Parameters(IADCS_100_Reactionwheel_Array_Parameters *params);
		void Enable_ReactionWheel_All_Idle_Mode();
		void Set_ReactionWheel_Torque(IADCS_100_Vector3_float torque_Nm);
		void Get_ReactionWheel_Torque(IADCS_100_Vector3_float *torque_Nm);

		/* ------------ 5.3.2 Magnettorquers -------------------- */
		/* ------------ 5.3.2.1 Magnettorquer Commands ---------- */
		void Set_Magnettorquer_All_Dipole_Moments(IADCS_100_Vector3_float moment_Am);
		void Get_Magnettorquer_All_Dipole_Moments(IADCS_100_Vector3_float *moment_Am);
		void Suspend_All_Magnettorquers(void);
		void Resume_All_Magnettorquers(void);
		void Set_Magnettorquer_Parameters(IADCS_100_Magnetorquer_Parameters params);
		void Get_Magnettorquer_Parameters(IADCS_100_Magnetorquer_Parameters *params);
		void Set_Magnettorquer_All_Currents(IADCS_100_Vector3_float target_current_A);
		void Get_Magnettorquer_All_Currents(IADCS_100_Vector3_float *target_current_A);


		/* ----------- 5.4. Control Command Interface ----------- */

		/* ----------- 5.4.1 Single Axis Control ---------------- */
		/* ----------- 5.4.1.2 Angular Velocity Control --------- */
		void Set_SingleAxis_AngularVelocity_Controller_Parameters(unsigned char axis, IADCS_100_PID_Controller_Parameters params);
		void Get_SingleAxis_AngularVelocity_Controller_Parameters(unsigned char axis, IADCS_100_PID_Controller_Parameters *params);
		void Start_SingleAxis_AngularVelocity_Controller(unsigned char axis, float angular_velocity_radps);
		void Stop_SingleAxis_AngularVelocity_Controller(unsigned char axis);

		/* ----------- 5.4.1.3 Angle Step Control --------------- */
		void Set_SingleAxis_AngleStep_Controller_Parameters(unsigned char axis, IADCS_100_Cascaded_PID_Controller_Parameters params);
		void Get_SingleAxis_AngleStep_Controller_Parameters(unsigned char axis, IADCS_100_Cascaded_PID_Controller_Parameters *params);
		void Start_SingleAxis_AngleStep_Controller(unsigned char axis, float angle_step_rad);
		void Stop_SingleAxis_AngleStep_Controller(unsigned char axis);

		/* ----------- 5.4.2 Three Axis Control ---------------- */
		/* ----------- 5.4.2.1 B-Dot Control ------------------- */
		void Set_ThreeAxis_BDot_Controller_Max_Parameters(IADCS_100_BDot_Controller_Parameters params);
		void Get_ThreeAxis_BDot_Controller_Max_Parameters(IADCS_100_BDot_Controller_Parameters *params);
		void Set_ThreeAxis_BDot_Controller_Proportional_Parameters(IADCS_100_BDot_Controller_Parameters params);
		void Get_ThreeAxis_BDot_Controller_Proportional_Parameters(IADCS_100_BDot_Controller_Parameters *params);

		/* ----------- 5.4.2.2 Sun Pointing Control ------------ */
		void Set_ThreeAxis_SunPointing_Controller_Parameters(IADCS_100_SunPointing_Controller_Parameters params);
		void Get_ThreeAxis_SunPointing_Controller_Parameters(IADCS_100_SunPointing_Controller_Parameters *params);

		/* ----------- 5.4.2.3 Sliding Control ----------------- */
		void Set_ThreeAxis_Sliding_Controller_Parameters(IADCS_100_Sliding_Controller_Parameters params);
		void Get_ThreeAxis_Sliding_Controller_Parameters(IADCS_100_Sliding_Controller_Parameters *params);


		/* ----------- 5.5 Operation Mode Command Interface --------- */

		/* ----------- 5.5.1 Basic Modes ---------------------------- */
		void Set_OperationMode_Safe(void);														//!< Triggers set mode idle command execution
		void Set_OperationMode_Idle(void);														//!< Triggers set mode idle command execution
		void Set_OperationMode_Measurement(void);												//!< Triggers set mode measurement command execution

		/* ----------- 5.5.2 Detumbling Mode ---------------- */
		void Start_OperationMode_Detumbling(IADCS_100_Detumbling_Mode_Parameters config);
		void Stop_OperationMode_Detumbling(void);

		/* ----------- 5.5.3 Active Sun Pointing Mode ---------------- */
		void Start_OperationMode_Sun_Pointing(IADCS_100_SunPointing_Mode_Parameters config);
		void Stop_OperationMode_Sun_Pointing(void);

		/* ----------- 5.5.4 Payload Target Modes ---------------- */

		/* ----------- 5.5.4.2 Target Pointing - Earth Const. Velocity Commands ---- */
		void Start_TargetPointing_Earth_Const_Velocity_Mode(IADCS_100_Earth_TargetPointing_Const_Velocity_Parameters params);
		void Stop_TargetPointing_Earth_Const_Velocity_Mode(void);

		/* ----------- 5.5.4.3 Target Pointing - Earth Fix Commands ---- */
		void Start_TargetPointing_Earth_Fix_Mode(IADCS_100_Earth_TargetPointing_Fixed_Parameters params);
		void Stop_TargetPointing_Earth_Fix_Mode(void);

		/* ----------- 5.5.4.4 Target Pointing - Nadir Commands ------ */
		void Start_TargetPointing_Nadir_Mode(IADCS_100_NadirPointing_Parameters params);
		void Stop_TargetPointing_Nadir_Mode(void);

		/* ----------- 5.5.4.5 Target Pointing - Telemetry & Identfiers ------ */
		/* ----------- 5.5.4.5.1 Target Pointing - Telemetry Commands ------ */
		void Get_TargetPointing_Target_Parameters_Telemetry(IADCS_100_TargetPointing_Target_Parameters_Telemetry *target_tm);
		void Get_TargetPointing_Operation_Parameters_Telemetry(IADCS_100_TargetPointing_Operation_Parameters_Telemetry *operation_tm);
		void Get_TargetPointing_Operation_Data_Telemetry(IADCS_100_TargetPointing_Operation_Data_Telemetry *data_tm);

		/* ----------- Special Commands (extra) ------------- */
		void Start_TargetPointing_Inertial_Mode(IADCS_100_TargetPointing_Inertial_Parameters params);
		void Stop_TargetPointing_Inertial_Mode(void);
		void Set_KalmanFilter_Parameters(IADCS_100_KalmanFilter_Parameters params);
		void Get_KalmanFilter_Parameters(IADCS_100_KalmanFilter_Parameters *params);
		void Set_TargetPointing_Operation_Parameters(IADCS_100_TargetPointing_Operation_Parameters params);

};

/** IADCS-100 Device Exception class
* 	\brief This exception type is returned in case of an exception in the IADCS_100 device class.
*/
class IADCS_100_Exception : public std::exception
{
  private:
	std::string e_;
	
  public:
	IADCS_100_Exception(std::string e) : e_(e) {}
	~IADCS_100_Exception() throw () {}
	const char* what() const throw() { return e_.c_str(); }
};

#endif
