/** OPS-SAT SEPP C++ API Library - iADCS-100 Fine ADCS
 *
 * \file SEPP_IADCS_API.h
 *
 * \project  		OPS-SAT SEPP C++ API Library
 *
 * \organisation 	IKS, TU Graz, Inffeldgasse 12/I, 8010 Graz
 * \author 			Reinhard Zeif, MSc, Vasundhara Shiradhonkar, MSc
 * \date 			19.Nov.2020
 *
 * version history
 * \version		0.1		RZ	first draft  														Nov. 2017
 * \version		0.2		RZ	telemetry readout functions added 									Jan. 2018
 * \version		0.3		RZ	pointing mode functions added 										Feb. 2018
 * \version		0.4		RZ	Complete list of commands added to IADCS_100 files (from ICD)		Mar. 2018
 * \version		0.5		RZ	Full update of struct etc. for new Firmware v3						Sep. 2018
 * \version		0.6		RZ	Update to FW deliverd with ICD v3.3.1								Nov. 2018
 * \version		0.7		RZ	Bugfix: Read RW parameters, Standard TM register order
 *							(failure in ICD 3.3.1 - fixed in ICD 3.4 draft)						16.Apr.2019
 * \version		0.9		RZ	Commands and structs added for some remaining cmds of ICD 3.3.1
 * 							Set/Get magnettorquer currents, Get target pointing telemetry		20.Sep.2019
 * \version 	1.0		RZ	SVT3 release with all commands defined in ICD 3.3.1					23.Sep.2019
 * \version		1.1		RZ	iADCS native startracker readout command added						18.Nov.2019
 * \version		1.2		RZ	iADCS native startracker readout command added						18.Nov.2019
 * \version		1.3		RZ	typo fixed in debug message output									21.Apr.2020
 * \version		1.4		RZ	I2C retry count increased to 10										25.May.2020
 * \version		1.5		RZ	Bugfix: Nadir and Earth-Fixed Pointing time value conversion fixed
 * 							to uint64 instead of float64										10.Jul.2020
 * \version		1.6		RZ	Two internally used registers added to standard TM					29.Jul.2020
 * \version		1.7		RZ	Sensor TM attitude quaternion: SF changed to BF						31.Jul.2020
 * \version		1.8		RZ	Attitude TM: bugfix - some parameters were missing/not taken from
 * 							sub-class. Variable EXTERNAL_5V_SUNSENSOR_POWER_SUPPLY_ENABLED
 * 							renamed to EXTERNAL_5V_POWER_SUPPLY_ENABLED because "sunsensor"
 * 							is misleading. The bit does also generate the +5V I2C
 * 							Ref-Voltage for the RWs.											06.Aug.2020
 * \version		1.9		RZ	Bugfix: Offsets for Std-TM internal registers						12.Aug.2020
 * \version		1.10	RZ	Set Epoch Time extended with read back and check functionality		21.Aug.2020
 * \version		1.11	RZ	TLE file parser method added										29.Aug.2020
 * \version		1.12	RZ	Target Pointing Parameter Structures renamed, debug output fixed	03.Sep.2020
 * \version		1.13	RZ	HL pointing commands: epoch time, long, lat value check and
 * 							exceptions added. Special HL inertial pointing command added.
 * 							Initialize_Device method clean-up (not required since iADCs FW
 * 							uses the correct defaults at startup).								20.Sep.2020
 * \version		1.14	RZ	Set/Get Kalman Filter parameter commands added						23.Sep.2020
 * \version		1.15	RZ	Command for inertial pointing parameters added						20.Oct.2020
 * \version		1.16	RZ	Command for inertial operation paramters reworked and changed to
 * 							general set target pointing operation paramters command.			06.Nov.2020
 * \version		1.17	RZ	iADCS device initialization strategy added to allow initialization
 * 							of sensor and actuator parameter from an INI config file.			12.Nov.2020
 * \version		1.18	RZ	Bugfix: Set/Get magnetometer parameters method						19.Nov.2020
 * \version		1.19	VS	Added degree parameter to start targetPointing inertial mode 		16.Dec.2021
 * \version		1.20	VS	Set/Get reaction wheel torque target method							01.Apr.2022
 *
 * Hint:
 * This library contains some iADCS-100 methods that simplify device access and operation.
 * These methods are based on the nominal iADCS-100 commands defined in the device ICD.
 * Some function contain pre-defined default values that are required by the OPS-SAT
 * spacecraft or typical command execution flows.
 * Additionally, all commands from iADCS-100 device ICD can be accessed by using the
 * member "iadcs_100". These functions are implemented and documented in the respective
 * source and header files.
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

#ifndef SEPP_IADCS_API_H_
#define SEPP_IADCS_API_H_

#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <math.h>

#include "SEPP_API_Debug.h"
#include "SEPP_API_Global.h"
#include "IADCS_100.h"
#include "SEPP_INIPARSER.h"


/* -------------------------- General Definitions ----------------------- */

#define SEPP_IADCS_API_I2C_DEVICE_NAME			"i2c-0"			// linux i2c device name in /dev folder

#define SEPP_IADCS_API_VERSION_STRING_BASE		"v1.18, 19.Nov.2020"
#ifdef GIT_VERSION
#define SEPP_IADCS_API_VERSION_STRING	SEPP_IADCS_API_VERSION_STRING_BASE << " " << GIT_VERSION
#else
#define SEPP_IADCS_API_VERSION_STRING	SEPP_IADCS_API_VERSION_STRING_BASE
#endif

/*! \defgroup SEPP_IADCS_API SEPP IADCS C++ API Library
* \{
*/

/* ------------------- Data Structure Definitions ---------------------- */

/** SEPP IADCS API Vector with 3 boolean values x,y,z
* \brief Vector with 3 boolean values for primary coordinate system x,y,z
*/
typedef struct
{
	bool X;
	bool Y;
	bool Z;
} SEPP_IADCS_API_VECTOR3_XYZ_BOOL;

/** SEPP IADCS API Vector with 3 boolean values u,v,w
* \brief Vector with 3 boolean values for secundary coordinate system u,v,w
*/
typedef struct
{
	bool U;
	bool V;
	bool W;
} SEPP_IADCS_API_VECTOR3_UVW_BOOL;

/** SEPP IADCS API Vector with 3 boolean values x,y,z
* \brief Vector with 3 boolean values for primary coordinate system x,y,z
*/
typedef struct
{
	short X;
	short Y;
	short Z;
} SEPP_IADCS_API_VECTOR3_XYZ_SHORT;

/** SEPP IADCS API Vector with 3 boolean values u,v,w
* \brief Vector with 3 boolean values for secundary coordinate system u,v,w
*/
typedef struct
{
	short U;
	short V;
	short W;
} SEPP_IADCS_API_VECTOR3_UVW_SHORT;

/** SEPP IADCS API Vector with 3 unsigned integer values x,y,z
* \brief Vector with 3 unsigned integer values for primary coordinate system x,y,z
*/
typedef struct
{
	unsigned int X;
	unsigned int Y;
	unsigned int Z;
} SEPP_IADCS_API_VECTOR3_XYZ_UINT;

/** SEPP IADCS API Vector with 3 unsigned integer values u,v,w
* \brief Vector with 3 unsigned integer values for secundary coordinate system u,v,w
*/
typedef struct
{
	unsigned int U;
	unsigned int V;
	unsigned int W;
} SEPP_IADCS_API_VECTOR3_UVW_UINT;

/** SEPP IADCS API Vector with 3 signed integer values x,y,z
* \brief Vector with 3 signed integer values for primary coordinate system x,y,z
*/
typedef struct
{
	int X;
	int Y;
	int Z;
} SEPP_IADCS_API_VECTOR3_XYZ_INT;

/** SEPP IADCS API Vector with 3 signed integer values u,v,w
* \brief Vector with 3 signed integer values for secundary coordinate system u,v,w
*/
typedef struct
{
	int U;
	int V;
	int W;
} SEPP_IADCS_API_VECTOR3_UVW_INT;

/** SEPP IADCS API Vector with 3 float values x,y,z
* \brief Vector with 3 float values for primary coordinate system x,y,z
*/
typedef struct
{
	float X;
	float Y;
	float Z;
} SEPP_IADCS_API_VECTOR3_XYZ_FLOAT;

/** SEPP IADCS API Vector with 3 unsigned long long values u,v,w
* \brief Vector with 3 unsigned long long values for secundary coordinate system u,v,w
*/
typedef struct
{
	float U;
	float V;
	float W;
} SEPP_IADCS_API_VECTOR3_UVW_FLOAT;

/** SEPP IADCS API Vector with 3 unsigned long long values x,y,z
* \brief Vector with 3 unsigned long long values for primary coordinate system x,y,z
*/
typedef struct
{
	unsigned long long X;
	unsigned long long Y;
	unsigned long long Z;
} SEPP_IADCS_API_VECTOR3_XYZ_ULONGLONG;



/** SEPP IADCS API 3x3 Matrix float
* \brief 3x3 Matrix with 9 float values
*/
typedef struct
{
	float M11;
	float M12;
	float M13;
	float M21;
	float M22;
	float M23;
	float M31;
	float M32;
	float M33;
} SEPP_IADCS_API_MATRIX3_FLOAT;

/** SEPP IADCS API Quaternion value float 
* \brief Quaternion with q_i, q_j, q_k and q float values
*/
typedef struct
{
	float Q_I;
	float Q_K;
	float Q_J;
	float Q;
} SEPP_IADCS_API_QUATERNION_FLOAT;

/** SEPP IADCS API Star Tracker Quaternion with timestamp
* \brief Star Tracker Quaternion with timestamp
*/
typedef struct
{
	SEPP_IADCS_API_QUATERNION_FLOAT QUATERNION;
	unsigned long long EPOCH_TIME_MSEC;
} SEPP_IADCS_API_STARTRACKER_QUATERNION;


/* ------------------ General data structs for Telemetry --------------------- */

/** SEPP IADCS API Software Version
* \brief Software version consisting of major, minor and patch ID
*/
typedef struct
{
	unsigned char MAJOR;								//!< 1x uint8_t: 	Major version
	unsigned char MINOR;								//!< 1x uint8_t: 	Minor version
	unsigned char PATCH;								//!< 1x uint8_t: 	Patch version
} SEPP_IADCS_API_SW_VERSION;

/** SEPP IADCS API Software Commit ID
* \brief Software commit ID for project and library
*/
typedef struct
{
	unsigned int PROJECT;								//!< 1x uint32_t: 	Project commit id
	unsigned int LIBRARY;								//!< 1x uint32_t: 	Library commit id
} SEPP_IADCS_API_COMMIT_ID;


/* ------------------------- General Enumerations ----------------------- */


enum SEPP_IADCS_API_GYROSCOPES
{
	IADCS_INTERNAL_LOWSPEED_GYRO = 0,
	IADCS_INTERNAL_HIGHSPEED_GYRO = 1,
	IADCS_EXTERNAL_HIGHPERFORMANCE_GYRO = 2
};

enum SEPP_IADCS_API_MAGNETOMETERS
{
	IADCS_INTERNAL_MAGNETOMETER = 0
};

enum SEPP_IADCS_API_REACTIONWHEELS
{
	IADCS_REACTIONWHEEL_X = 0,
	IADCS_REACTIONWHEEL_Y = 1,
	IADCS_REACTIONWHEEL_Z = 2,
	IADCS_REACTIONWHEEL_U = 3,
	IADCS_REACTIONWHEEL_V = 4,
	IADCS_REACTIONWHEEL_W = 5
};

enum SEPP_IADCS_API_MAGNETORQUERS
{
	IADCS_MAGNETORQUER_X = 0,
	IADCS_MAGNETORQUER_Y = 1,
	IADCS_MAGNETORQUER_Z = 2
};

enum SEPP_IADCS_API_SINGLEAXIS_CONTROL_TARGET_AXIS
{
	IADCS_SINGLEAXIS_CONTROL_TARGET_X = 0,
	IADCS_SINGLEAXIS_CONTROL_TARGET_Y = 1,
	IADCS_SINGLEAXIS_CONTROL_TARGET_Z = 2
};

enum SEPP_IADCS_API_TARGET_POINTING_CONTROL_MODES
{
	IADCS_CONTROL_MODE_INERTAL_POINTING_USING_EULER_ROTATION = 0,					//!< 0 - Control mode for inertial pointing by using euler rotation
	IADCS_CONTROL_MODE_FIX_EARTH_TARGET_POINTING_USING_EULER_ROTATION = 1,			//!< 1 - Control mode for fixed earth target pointing by using euler rotation
	IADCS_CONTROL_MODE_TARGET_POINTING_WITH_POLY = 2,								//!< 2 - Control mode for target pointing by using polynomial estimator
	IADCS_CONTROL_MODE_INERTIAL_POINTING_USING_CONST_VELOCITY = 3,					//!< 3 - Control mode for inertial pointing by using constant velocity
	IADCS_CONTROL_MODE_FIXED_EARTH_TARGET_POINTING = 4,								//!< 4 - Control mode for fixed earth target pointing
	IADCS_CONTROL_MODE_EARTH_TARGET_WITH_CONST_VELOCITY = 5,						//!< 5 - Control mode for fixed earth target pointing with const velocity
	IADCS_CONTROL_MODE_NADIR_POINTING = 6											//!< 6 - Control mode for nadir pointing
};

enum SEPP_IADCS_API_TARGET_POINTING_ATTITUDE_DETERMINATION_MODES
{
	IADCS_ATTITUDE_DETERMINATION_USING_MAGNETIC_AND_SUN_DURING_PREALIGNMENT = 0,	//!< 0 - Attitude determination by using magnetic and sun vector data during prealignment
	IADCS_ATTITUDE_DETERMINATION_USING_STARTRACKER_WITH_MAGNETIC_AND_SUN = 1,		//!< 1 - Attitude determination by using the startracker with magnetic and sun vector data
	IADCS_ATTITUDE_DETERMINATION_STARTRACKER_ONLY = 2,								//!< 2 - Attitude determination by using the startracker only
	IADCS_ATTITUDE_DETERMINATION_INVALID = 3										//!< 3 - invalid value
};

enum SEPP_IADCS_API_TARGET_POINTING_GENERAL_MODES
{
	IADCS_NORMAL_MODE = 0,															//!< 0 - iADCS Normal Mode
	IADCS_TEST_MODE = 1,															//!< 1 - iADCS Test Mode
	IADCS_STOP_SATELLITE_ONLY = 2													//!< 2 - iADCS mode to stop satellite only
};

enum SEPP_IADCS_API_CONTROLLER_ACTUATOR_CMD_INTERFACES
{
	IADCS_CTRL_ACTUATOR_CMD_IF_0 = 0,												//!< 0 - Control Output = Speed; RW Cmd Type = Speed
	IADCS_CTRL_ACTUATOR_CMD_IF_1 = 1,												//!< 1 - Control Output = Torque; RW Cmd Type = Torque
	IADCS_CTRL_ACTUATOR_CMD_IF_2 = 2,												//!< 2 - Control Output = Current; RW Cmd Type = Current
	IADCS_CTRL_ACTUATOR_CMD_IF_3 = 3,												//!< 3 - Control Output = Torque; RW Cmd Type = Speed
	IADCS_CTRL_ACTUATOR_CMD_IF_4 = 4,												//!< 4 - Control Output = Torque; RW Cmd Type = Current
	IADCS_CTRL_ACTUATOR_CMD_IF_5 = 5,												//!< 5 - Control Output = Torque; MTQ
	IADCS_CTRL_ACTUATOR_CMD_IF_6 = 6												//!< 6 - Control Output = Dipole; MTQ
};

enum SEPP_IADCS_API_OPERATING_MODE
{
	IADCS_OPERATION_MODE_IDLE = 0,													//!< 0 - idle
	IADCS_OPERATION_MODE_SAFE = 1,													//!< 1 - safe
	IADCS_OPERATION_MODE_MEASUREMENT = 2,											//!< 2 - measurement mode
	IADCS_OPERATION_MODE_DETUMBLING = 3,											//!< 3 - detumbling
	IADCS_OPERATION_MODE_SUNPOINTING = 4,											//!< 4 - sun pointing
	IADCS_OPERATION_MODE_TARGETPOINTING_NADIR = 5,									//!< 5 - target pointing nadir
	IADCS_OPERATION_MODE_TARGETPOINTING_EARTH_FIXED = 6,							//!< 6 - target pointing earth fix
	IADCS_OPERATION_MODE_TARGETPOINTING_EARTH_FIXED_EULER_ROTATION = 7,				//!< 7 - target pointing earth fix euler roation
	IADCS_OPERATION_MODE_TARGETPOINTING_EARTH_FIXED_CONST_VELOCITY = 8,				//!< 8 - target pointing earth fix const velocity
	IADCS_OPERATION_MODE_TARGETPOINTING_WITH_POLY = 9								//!< 9 - target pointing with polynomial prop.
};

enum SEPP_IADCS_API_SUN_EMULATION_MODEL
{
	IADCS_SUN_EMULATION_MODEL_BIMONTLY = 0, 										//!< 0 - bi-montly
	IADCS_SUN_EMULATION_MODEL_VALLADO = 2, 											//!< 2 - Vallado
	IADCS_SUN_EMULATION_MODEL_SVARTVEIT = 3 										//!< 3 - Svartveit
};

enum SEPP_IADCS_API_SYSTEM_REGISTER_TYPE
{
	IADCS_SCHEDULER_REGISTER = 0, 													//!< 0 - Scheduler Register
	IADCS_GENERAL_ERROR_REGISTER = 1, 												//!< 1 - Error Register
	IADCS_STATUS_REGISTER = 2, 														//!< 2 - Status Register
	IADCS_CONTROL_REGISTER = 3, 													//!< 3 - Control Register
	IADCS_HL_MAIN_REGISTER = 4, 													//!< 4 - HL Main Register
	IADCS_HL_SCHEDULER_REGISTER = 5, 												//!< 5 - HL Scheduler Register
	IADCS_HL_MAIN_ERROR_REGISTER = 6  												//!< 6 - HL Main Error Register
};

enum SEPP_IADCS_API_STARTRACKER_OPERATING_MODE
{
	IADCS_STARTRACKER_OPERATING_MODE_DUMPING = 0,									//!< 0 - Dumping mode; St200 sends automatically data
	IADCS_STARTRACKER_OPERATING_MODE_PULLING = 1									//!< 1 - Pulling mode (default mode); St200 send data only on request.
};


/* ----------------------- iADCS registers -------------------- */

/** SEPP IADCS API System Low-Level Register
*/
typedef struct
{
	bool MAINBOARD_POWER_SUPPLY_ENABLED;				//!< 0: Main board power supply enabled
	bool STARTRACKER_POWER_SUPPLY_ENABLED;				//!< 1: Startracker power supply enabled
	bool INTERNAL_REACTIONWHEEL_POWER_SUPPLY_ENABLED;	//!< 2: Internal reaction wheels power supply enabled
	bool EXTERNAL_5V_POWER_SUPPLY_ENABLED;				//!< 3: External +5V power supply enabled (Sun sensor, RW's, ...)
} SEPP_IADCS_API_SYSTEM_LOWLEVEL_DCDC_REGISTER;

/** SEPP IADCS API System Status Register
*/
typedef struct
{
	bool IDLE_MODE;										//!< 1: ADCS in Idle-Mode
	bool SAFE_MODE;										//!< 2: iADCS in Safe-Mode
	bool MEASUREMENT_MODE;								//!< 10: iADCS is in Measurement-Mode
	bool DETUMBLING_MODE;								//!< 11: iADCS is in Detumbling-Mode
	bool SUNPOINTING_MODE;								//!< 12: iADCS is in Sunpointing-Mode
	bool SINGLE_SPINNING_MODE;							//!< 14: iADCS is in Single Spinning Mode
	bool TARGET_POINTING_MODE;							//!< 15: iADCS is in Target Pointing Mode
	bool UNDEFINED_MODE;								//!< 31: iADCS is in undefined Mode
} SEPP_IADCS_API_SYSTEM_STATUS_REGISTER;

/** SEPP IADCS API System Scheduler Register
*/
typedef struct
{
	bool LOWSPEED_GYRO_READING_ENABLE;					//!< 0: Low speed gyro reading enable
	bool HIGHSPEED_GYRO_READING_ENABLE; 				//!< 1: High speed gyro reading enable
	bool HIGHPRECISION_GYRO_READING_ENABLE;				//!< 2: High precision gyro reading enable [is available]
	bool MAGNETOMETER_READING_ENABLE; 					//!< 3: Mems magmetometer reading enable
	bool SUNSENSOR_READING_ENABLE; 						//!< 4: Sunsensor reading enable [not available]
	bool STARTRACKER_READING_ENABLE; 					//!< 5: Star tracker reading enable
	bool POWER_READING_ENABLE; 							//!< 6: Power reading enable
	bool TEMPERATURE_READING_ENABLE; 					//!< 7: Temperature reading enable
	bool REACTIONWHEEL_READING_ENABLE; 					//!< 8: Reaction wheels reading enable
	bool CONTROL_MODE_ENABLE; 							//!< 20: Control Mode enable
	bool ORBIT_PROPAGATION_ENABLE; 						//!< 21: Orbit propagation enable
} SEPP_IADCS_API_SYSTEM_SCHEDULER_REGISTER;

/** SEPP IADCS API System Error Register
*/
typedef struct
{
	bool TIME_INIT_ERROR; 								//!< 0: Time init error
	bool SAT_MOI_INIT_ERROR; 							//!< 1: Satellite mass of inertia init error
	bool CONTROL_MODULE_INIT_ERROR; 					//!< 2: Control module init error
	bool HL_ATTITUDE_CONTROL_MODULE_INIT_ERROR; 		//!< 3: High level attitude control module init error
	bool INFO_FRAME_INIT_ERROR; 						//!< 4: Info frame init error
	bool COMMAND_CRC_ERROR; 							//!< 5: Command CRC error
	bool UNKNOWN_COMMAND_RECEIVED; 						//!< 6: Unknown command received
	bool COMMAND_HANDLING_ERROR; 						//!< 7: Command handling error
														//!< 8: Not used
	bool ORBIT_MODULE_ERROR; 							//!< 9: Orbit module error
	bool CONTROL_MODULE_ERROR; 							//!< 10: Control module error
	bool HL_OPERATION_MODE_ERROR; 						//!< 11: High level operation mode error
	bool ATTITUDE_DETERMINATION_UPDATE_ERROR;			//!< 12: Attitude determination update error
} SEPP_IADCS_API_SYSTEM_ERROR_REGISTER;

/** SEPP IADCS API Sensors Error Register
*/
typedef struct
{
	bool SENSOR_INIT_ERROR; 							//!< 0 Sensor init error
	bool STARTRACKER_INIT_ERROR;						//!< 1 Star tracker init error
	bool GYROS_INIT_ERROR;								//!< 2 Gyros init error
	bool MAGNETOMETER_INIT_ERROR; 						//!< 3 Magnetometer init error
	bool SUN_SENSOR_INIT_ERROR;							//!< 4 Sun sensor init error (only if sun sensors are attached)
	bool STARTRACKER_COMMUNICATION_ERROR; 				//!< 5 Star tracker communication error
	bool STARTRACKER_REPORTS_ERROR;						//!< 6 Star tracker reports error
	bool GYRO_0_ERROR;									//!< 7 Gyro index 0 error
	bool GYRO_1_ERROR; 									//!< 8 Gyro index 1 error
	bool GYRO_2_ERROR; 									//!< 9 Gyro index 2 error
	bool MAGNETOMETER_0_ERROR;							//!< 10 Magnetometer index 0 error
	bool MAGNETOMETER_1_ERROR;							//!< 11 Magnetometer index 1 error (if external sensor is attached)
	bool SUN_SENSOR_0_ERROR;							//!< 12 Sun sensor index 0 error (If optional available)
	bool SUN_SENSOR_1_ERROR;							//!< 13 Sun sensor index 1 error (If optional available)
	bool SUN_SENSOR_2_ERROR;							//!< 14 Sun sensor index 2 error (If optional available)
	bool SUN_SENSOR_3_ERROR;							//!< 15 Sun sensor index 3 error (If optional available)
	bool SUN_SENSOR_4_ERROR;							//!< 16 Sun sensor index 4 error (If optional available)
	bool SUN_SENSOR_5_ERROR;							//!< 17 Sun sensor index 5 error (If optional available)
	bool MAGNETTORQUER_POWER_SENSOR_0_ERROR;			//!< 18 MTQ power sensor index 0 error
	bool STARTRACKER_POWER_SENSOR_1_ERROR;				//!< 19 ST200 power sensor index 1 error
	bool MAINBOARD_POWER_SENSOR_2_ERROR;				//!< 20 Board power sensor index 2 error
	bool INTERNAL_REACTIONWHEELS_POWER_SENSOR_3_ERROR;	//!< 21 Internal reaction wheels power sensor index 3 error
	bool POWER_SENSOR_4_ERROR;							//!< 22 Power sensor index 4 error (not available)
	bool POWER_SENSOR_5_ERROR;							//!< 23 Power sensor index 5 error (not available)
	bool TEMPERATURE_SENSOR_ERROR;						//!< 24 Temperature sensor error
} SEPP_IADCS_API_SENSORS_ERROR_REGISTER;

/** SEPP IADCS API Actuators Error Register
*/
typedef struct
{
	bool ACTUATORS_INIT_ERROR; 							//!< 0 Actuators init error
	bool REACTIONWHEEL_INIT_ERROR; 						//!< 1 Reaction wheels init error
	bool MAGNETTORQUERS_INIT_ERROR; 					//!< 2 Magnetorquers init error
	bool REACTIONWHEEL_0_ERROR; 						//!< 3 Reaction wheel index 0 error
	bool REACTIONWHEEL_1_ERROR; 						//!< 4 Reaction wheel index 1 error
	bool REACTIONWHEEL_2_ERROR; 						//!< 5 Reaction wheel index 2 error
	bool REACTIONWHEEL_3_ERROR; 						//!< 6 Reaction wheel index 3 error
	bool REACTIONWHEEL_4_ERROR; 						//!< 7 Reaction wheel index 4 error
	bool REACTIONWHEEL_5_ERROR; 						//!< 8 Reaction wheel index 5 error
	bool MAGNETTORQUER_0_ERROR; 						//!< 9 Magnetorquer index 0 error
	bool MAGNETTORQUER_1_ERROR; 						//!< 10 Magnetorquer index 1 error
	bool MAGNETTORQUER_2_ERROR; 						//!< 11 Magnetorquer index 2 error
} SEPP_IADCS_API_ACTUATORS_ERROR_REGISTER;

/** SEPP IADCS API System Livelyhood Register
*/
typedef struct
{
	bool CONTROL_PROCESSOR_ALIVE; 						//!< 0: Control processor is alive
	bool LOWSPEED_GYRO_ALIVE;							//!< 1: Low speed gyro is alive
	bool HIGHSPEED_GYRO_ALIVE;							//!< 2: High speed gyro is alive
	bool HIGHPRECISION_GYRO_ALIVE;						//!< 3: High precision gyro is alive
	bool MEMS_MAGNETOMETER_ALIVE;						//!< 4: Mems magnetometer is alive
	bool EXTERNAL_MAGNETOMETER_ALIVE;					//!< 5: External magnetometer is alive [not available]
	bool STARTRACKER_ALIVE;								//!< 6: Star tracker is alive
	bool SUNSENSOR_1_ALIVE;								//!< 7: Sunsensor 1 is alive
	bool SUNSENSOR_2_ALIVE;								//!< 8: Sunsensor 2 is alive
	bool SUNSENSOR_3_ALIVE;								//!< 9: Sunsensor 3 is alive
	bool SUNSENSOR_4_ALIVE;								//!< 10: Sunsensor 4 is alive
	bool SUNSENSOR_5_ALIVE;								//!< 11: Sunsensor 5 is alive
	bool SUNSENSOR_6_ALIVE;								//!< 12: Sunsensor 6 is alive
	bool MAGNETORQUER_CURRENT_SENSOR_ALIVE;				//!< 13: MTQ current sensor is alive
	bool STARTRACKER_CURRENT_SENSOR_ALIVE;				//!< 14: Star tracker current sensor is alive
	bool MAINBOARD_CURRENT_SENSOR_ALIVE;				//!< 15: Main board current sensor is alive
	bool INTERNAL_REACTIONWHEEL_CURRENT_SENSOR_ALIVE;	//!< 16: Internal reaction wheel current sensor is alive
	bool POWER_SENSOR_4_ALIVE;							//!< 17: Additional power sensor is alive (not available)
	bool POWER_SENSOR_5_ALIVE;							//!< 18: Additional power sensor is alive (not available)
	bool MAGNETORQUER_X_ALIVE;							//!< 19: MTQ X is alive
	bool MAGNETORQUER_Y_ALIVE;							//!< 20: MTQ Y is alive
	bool MAGNETORQUER_Z_ALIVE;							//!< 21: MTQ Z is alive
	bool REACTIONWHEEL_X_ALIVE;							//!< 22: Reaction wheel X is alive
	bool REACTIONWHEEL_Y_ALIVE;							//!< 23: Reaction wheel Y is alive
	bool REACTIONWHEEL_Z_ALIVE;							//!< 24: Reaction wheel Z is alive
	bool REACTIONWHEEL_U_ALIVE;							//!< 25: Reaction wheel U is alive (is available)
	bool REACTIONWHEEL_V_ALIVE;							//!< 26: Reaction wheel V is alive (is available)
	bool REACTIONWHEEL_W_ALIVE;							//!< 27: Reaction wheel W is alive (is available)
} SEPP_IADCS_API_SYSTEM_LIVELYHOOD_REGISTER;

/** SEPP IADCS API System HIL Configuration Register
*/
typedef struct
{
	bool LOWSPEED_GYRO_IN_HIL_MODE; 					//!< 0: Low speed gyro is in HIL mode
	bool HIGHSPEED_GYRO_IN_HIL_MODE; 					//!< 1: High speed gyro is in HIL mode
	bool HIGHPRECISION_GYRO_IN_HIL_MODE; 				//!< 2: High precision gyro is in HIL mode [is available]
	bool MEMS_MAGNETOMETER_IN_HIL_MODE; 				//!< 3: Mems magnetometer is in HIL mode
														//!< 4: ?? not defined in ICD
														//!< 5..7: Not used
	bool SUNSENSOR_1_IN_HIL_MODE; 						//!< 8: Sunsensor 1 is in HIL mode [if optional available]
	bool SUNSENSOR_2_IN_HIL_MODE; 						//!< 9: Sunsensor 2 is in HIL mode [if optional available]
	bool SUNSENSOR_3_IN_HIL_MODE; 						//!< 10: Sunsensor 3 is in HIL mode [if optional available]
	bool SUNSENSOR_4_IN_HIL_MODE;						//!< 11: Sunsensor 4 is in HIL mode [if optional available]
	bool SUNSENSOR_5_IN_HIL_MODE; 						//!< 12: Sunsensor 5 is in HIL mode [if optional available]
	bool SUNSENSOR_6_IN_HIL_MODE; 						//!< 13: Sunsensor 6 is in HIL mode [if optional available]
	bool MAGNETORQUER_X_IN_HIL_MODE; 					//!< 14: MTQ X is in HIL mode
	bool MAGNETORQUER_Y_IN_HIL_MODE;					//!< 15: MTQ Y is in HIL mode
	bool MAGNETORQUER_Z_IN_HIL_MODE; 					//!< 16: MTQ Z is in HIL mode
	bool REACTIONWHEEL_X_IN_HIL_MODE; 					//!< 17: Reaction wheel x is in HIL mode
	bool REACTIONWHEEL_Y_IN_HIL_MODE; 					//!< 18: Reaction wheel y is in HIL mode
	bool REACTIONWHEEL_Z_IN_HIL_MODE; 					//!< 19: Reaction wheel z is in HIL mode
	bool REACTIONWHEEL_U_IN_HIL_MODE; 					//!< 20: Reaction wheel u is in HIL mode [is available]
	bool REACTIONWHEEL_V_IN_HIL_MODE; 					//!< 21: Reaction wheel v is in HIL mode [is available]
	bool REACTIONWHEEL_W_IN_HIL_MODE; 					//!< 22: Reaction wheel w is in HIL mode [is available]
	bool STARTRACKER_IN_HIL_MODE; 						//!< 23: Star tracker is in HIL mode
} SEPP_IADCS_API_SYSTEM_HIL_CONFIGURATION_REGISTER;

/** SEPP IADCS API Sat Main Register
*/
typedef struct
{
	bool SUNSENSOR_BY_INTENSITY; 						//!< 0: Using intensity for selection of best sun sensor. By setting this flag the ADCS will use the measured intensity as criterio to select the best measured sun sensor values.
	bool SUNSENSOR_BY_EMULATION_AND_PROPAGATION_ANGLE; 	//!< 1: Using Sun-emulation and propagated angle for selection of best sun sensor. By setting this flag the ADCS will compare the measured sun sensor values with the internal emulated values and select the values set with the best match.
	bool PROPAGATE_ATTITUDE_BY_MAGNETIC_AND_SUN_INFO; 	//!< 2: Using magnetic and sun information to propagate attitude. By setting this flag a attitude quaternion will be calculated by using the measured sun and magnetic sensor values.
	bool PROPAGATE_ATTITUDE_DURING_2_SENSOR_READINGS; 	//!< 3: Propagate attitude during 2 sensors reading. By setting this flag a attitude propagation will be done during 2 measurement, which are widely apart in time.
	bool EMULATE_SUN; 									//!< 4: Emulate sun; An onboard sun emulation will be done by setting this flag.
														//!< 5..7: ?? not defined in ICD
	bool INTERNAL_1;									//!< 8..9: Internal use
	bool INTERNAL_2;
	bool NO_MAGNETORQUER_SUSPEND; 						//!< 10: No MTQ suspend during operation. The magnetorquers will not be suspended during operation.
} SEPP_IADCS_API_SAT_MAIN_REGISTER;

/** SEPP IADCS API Sat Error Register
*/
typedef struct
{
	bool GENERAL;										//!< 0: General Error
	bool SUNSENSOR_BY_INTENSITY;						//!< 1: Error by using intensity criterium for sunsensor-selection
	bool SUNSENSOR_BY_PROPAGATION_ANGLE;				//!< 2: Error by using angle criterium for sunsensor-selection
	bool SUNSENSOR_EVALUATION;							//!< 3: Error by using sun sensor evaluation
	bool ATTITUDE_BY_MAGNETIC_AND_SUN_INFO;				//!< 4: Magnetic/ sun based attitude propagation error
	bool KALMAN_4;										//!< 6: Kalman 4 error
	bool ATTITUDE_PROPAGATION;							//!< 7: Attitude propagation error
	bool SUN_EMULATION;									//!< 8: Sun emulation error
} SEPP_IADCS_API_SAT_ERROR_REGISTER;

/** SEPP IADCS API Sat Scheduler Register
*/
typedef struct
{
														//!< 0-3: must be set to zero
	bool KALMAN_4_FILTER_ACTIVE; 						//!< 4: Kalman 4 filter active. Bet set/unset this flag the kalman filter module is activated/ deactivated
														//!< 5-19: must be set to zero
	bool ENABLE_EXT_SUN_SENSOR_DATA; 					//!< 20: Enable/disable external sun sensor data.
														//!< 21-31: must be set to zero
} SEPP_IADCS_API_SAT_SCHEDULER_REGISTER;

/** SEPP IADCS API Control Status Register
*/
typedef struct
{
	bool IN_IDLE_MODE;									//!< 0: Control module is in idle mode
	bool IN_SINGLE_AXIS_CONTROL_MODE;					//!< 1: Control module is in single axis control mode
	bool IN_ALL_AXIS_CONTROL_MODE;						//!< 2: Control module is in all axis control mode
	bool CONTROL_ERROR;									//!< 31: Control module error
} SEPP_IADCS_API_CONTROL_STATUS_REGISTER;

typedef struct
{
	bool SINGLE_AXIS_INIT;								//!< 0: Single axis init failed.
	bool ALL_AXIS_INIT;									//!< 1: All axis init failed.
	bool SINGLE_AXIS_ANGULAR_VELOCITY_PID;				//!< 2: Single axis control - angular velocity pid - error
	bool SINGLE_AXIS_ANGULAR_VELOCITY_DCT;				//!< 3: Single axis control - angular velocity dct - error
	bool SINGLE_AXIS_ANGLE_PID;							//!< 4: Single axis control - angle pid - error
	bool SINGLE_AXIS_ANGLE_CASCADED_PID;				//!< 5: Single axis control - angle cascading - error
	bool ALL_AXIS_BDOT_PROP;							//!< 6: All axis control - bdot-prop - error
	bool ALL_AXIS_BDOT_MAX;								//!< 7: All axis control - bdot-max - error
	bool ALL_AXIS_SUN_POINTING;							//!< 8: All axis control - sun pointing - error
	bool ALL_AXIS_SINGLE_SPINNING_MODE;					//!< 9: All axis control - single spinning mode - error
	bool ALL_AXIS_TRACKING_MODE;						//!< 10: All axis control - tracking mode - error
	bool SET_IDLE_MODE;									//!< 12: Set idle mode error
	bool SET_REACTIONWHEELS;							//!< 13: Set RW actuators error
	bool SET_MAGNETORQUERS;								//!< 14: Set MTQ actuators error
	bool SINGLE_AXIS_ANGULAR_VELOCITY_CONTROLLER;		//!< 24: Single axis angular velocity controller error
	bool SINGLE_AXIS_ANGLE_CONTROLLER;					//!< 25: Single axis angle controller error
	bool BDOT_STOP;										//!< 26: Bdot Stop error
} SEPP_IADCS_API_CONTROL_ERROR_REGISTER;

/** SEPP IADCS API Single Axis Control Status Register
*/
typedef struct
{
	bool ANGULAR_VELOCITY_PID_IS_ACTIVE; 				//!< 11: Single axis control: Angular velocity PID controller is active
	bool ANGLE_CASCADE_CONTROLLER_IS_ACTIVE; 			//!< 14: Single axis control: Angle cascading controller is active
} SEPP_IADCS_API_SINGLE_AXIS_CONTROL_STATUS_REGISTER;

/** SEPP IADCS API All Axis Control Status Register
*/
typedef struct
{
	bool BDOT_PROP_IS_ACTIVE;							//!< 21: All axis control: Bdot propotional controller is active
	bool BDOT_MAX_IS_ACTIVE; 							//!< 22: All axis control: Bdot max. controller is active
	bool SUN_POINTING_IS_ACTIVE;						//!< 23: All axis control: Sun pointing controller is active
	bool SINGLE_SPINNING_IS_ACTIVE;						//!< 24: All axis control: Single spinning controller is active
	bool TARGET_TRACKING_IS_ACTIVE;						//!< 25: All axis control: Target tracking controller is active
	bool THREE_AXIS_IS_ACTIVE;							//!< 26: All axis control: 3-axes controller is active
	bool BDOT_STOP_IS_ACTIVE;							//!< 27: All axis control: Bdot stop (using RWs) controller is active
} SEPP_IADCS_API_ALL_AXIS_CONTROL_STATUS_REGISTER;


/* ------------ Structures for Register Groups ------------*/

/** SEPP IADCS API All Axis Control Status Register
 * Remark: ICD 3.3.1 does list this but does not list a command to read this data.
*/
typedef struct
{
	SEPP_IADCS_API_CONTROL_STATUS_REGISTER CONTROL_STATUS;								//!< 1x uint32_t:		Main control status register (5.4.3.2.1)
	SEPP_IADCS_API_CONTROL_ERROR_REGISTER CONTROL_ERROR;								//!< 1x uint32_t:		Error register (5.4.3.2.2)
	SEPP_IADCS_API_SINGLE_AXIS_CONTROL_STATUS_REGISTER SINGLE_AXIS_CONTROL_STATUS[3];	//!< 3x uint32: 		Single axis status register[X,Y,Z] (5.4.3.2.3)
	SEPP_IADCS_API_ALL_AXIS_CONTROL_STATUS_REGISTER ALL_AXIS_CONTROL_STATUS; 			//!< 1x uint32_t:		All axis control status register (5.4.3.2.4)
} SEPP_IADCS_API_CONTROL_MODULE_STATUS_REGISTERS;

/** SEPP IADCS API High-Level ADCS Register
*/
typedef struct
{
	SEPP_IADCS_API_SAT_MAIN_REGISTER SAT_MAIN;											//!< 1x uint32_t: 	Sat Main Register (6.1.7.1)
	SEPP_IADCS_API_SAT_ERROR_REGISTER SAT_ERROR;										//!< 1x uint32_t: 	Sat Error Register (6.1.7.2)
	SEPP_IADCS_API_SAT_SCHEDULER_REGISTER SAT_SCHEDULER;								//!< 1x uint32_t: 	Sat Scheduler Register (6.1.7.3)
} SEPP_IADCS_API_HIGHLEVEL_ADCS_REGISTERS;


/* ---------------------- iADCS Telemetry Structures ------------------------- */

/** SEPP IADCS API Info Telemetry
*/
typedef struct
{
	char FRAME_IDENTIFIER[2];															//!< 2x char 			Frame identifier
	unsigned char FRAME_VERSION;														//!< 1x uint8_t 		Frame version
	SEPP_IADCS_API_SW_VERSION SW_VERSION;												//!< 3x uint8_t		Software version
	unsigned char STARTRACKER_TYPE;														//!< 1x uint8_t 		Star tracker type
	unsigned int STARTRACKER_SERIAL_NUMBER;												//!< 1x uint32_t 		Star tracker serial number
	char DEVICE_NAME[9];																//!< 9x char 			Device name
	unsigned int DEVICE_SERIAL_NUMBER;													//!< 1x uint32_t 		Device serial number
	char BUILD_TIMESTAMP[32];															//!< 32x char 		Build time stamp
	SEPP_IADCS_API_COMMIT_ID SW_COMMIT_ID;												//!< 5x uint32_t		Commit ids
	unsigned char DEBUG_LEVEL;															//!< 1x uint8_t 		Debug level
	char COMPILER_NAME[20];																//!< 20x char 		Compiler name
	char COMPILER_VERSION[10];															//!< 10x char 		Compiler version
	char LOW_LEVEL_SW_VERSION[10];														//!< 10x char 		Low level software version
	char LOW_LEVEL_BUILD_TIMESTAMP[32];													//!< 32x char 		Low level build time stamp
} SEPP_IADCS_API_INFO_TELEMETRY;

/** SEPP IADCS API Sensor Telemetry
*/
typedef struct
{
	unsigned long long EPOCH_TIME_MSEC;													//!< 1x int64_t 		Epoch time in [msec]
	unsigned int INDEX_OF_ACTIVE_STARTRACKER;											//!< 1x uint8_t 		Index of active star tracker in star tracker array
	SEPP_IADCS_API_QUATERNION_FLOAT ATTITUDE_QUATERNION_BF;								//!< 4x float 		Measured attitude quaternion of the satellite from active star tracker [qi, qj , qk, q]
	unsigned int INDEX_OF_ACTIVE_GYRO;													//!< 1x uint8_t 		Index of active gyroscope in the gyroscope array. Values of not available sensor are set to zero.
	SEPP_IADCS_API_VECTOR3_XYZ_FLOAT MEASURED_ANGULAR_VELOCITY_GYRO_RADPS[4]; 			//!< 4x [3x float] 	Measured angular velocity vector [wx, wy, wz] of maximum 4 gyroscope in [rad/sec] and body frame.
	unsigned int INDEX_OF_ACTIVE_MAGNETOMETER;											//!< 1x uint8_t 		Index of active magnetormeter in the magnetometer array
	SEPP_IADCS_API_VECTOR3_XYZ_FLOAT MEASURED_MAGNETIC_FIELD_MAGNETOMETER_T[2]; 		//!< 2x [3x float]	Measured magnetic field vector [bx, by, bz] of maximum 2 magnetometer in [T] and body frame. Values of not available sensor are set to zero.
	SEPP_IADCS_API_VECTOR3_XYZ_FLOAT MEASURED_SUN_VECTOR_SUNSENSOR[6];  				//!< 6x [3x float] 	Measured sun vector [svx, svy, svz] of 6 sunsensors. Values of not available sensor are set to zero.
	unsigned int TEMPERATURE_DEGC;														//!< 1x uint8_t 		Temperature in [deg]
} SEPP_IADCS_API_SENSOR_TELEMETRY;

/** SEPP IADCS API Actuator Telemetry
*/
typedef struct
{
	unsigned long long EPOCH_TIME_MSEC;													//!< 1x int64_t 		Epoch time in [msec]
	SEPP_IADCS_API_VECTOR3_XYZ_FLOAT REACTIONWHEEL_TARGET_SPEED_VECTOR_XYZ_RADPS; 		//!< 3x float 		Target speed of 3 internal reaction wheels rw0, rw1, rw2 in [rad/sec]. 
	SEPP_IADCS_API_VECTOR3_UVW_FLOAT REACTIONWHEEL_TARGET_SPEED_VECTOR_UVW_RADPS;		//!< 3x float 		Target speed of 3 external reaction wheels rw3, rw4, rw5 in [rad/sec]. 
	SEPP_IADCS_API_VECTOR3_XYZ_FLOAT REACTIONWHEEL_CURRENT_SPEED_VECTOR_XYZ_RADPS; 		//!< 3x float			Current speed of 3 internal reaction wheels rw0, rw1, rw2 in [rad/sec].
	SEPP_IADCS_API_VECTOR3_UVW_FLOAT REACTIONWHEEL_CURRENT_SPEED_VECTOR_UVW_RADPS; 		//!< 3x float			Current speed of 3 external reaction wheels rw3, rw4, rw5 in [rad/sec]. 
	SEPP_IADCS_API_VECTOR3_XYZ_FLOAT MAGNETORQUERS_TARGET_DIPOLE_MOMENT_VECTOR_AM2; 	//!< 3x float 		Target dipole moment of magnetorquers on 3 axes [mx,my,mz] in [Am2]
	unsigned int MAGNETORQUERS_CURRENT_STATE;											//!< 1x int32_t		Current state of magnetorquers
} SEPP_IADCS_API_ACTUATOR_TELEMETRY;

/** SEPP IADCS API Attitude Telemetry
*/
typedef struct
{
	unsigned long long EPOCH_TIME_MSEC;											//!< 1x int64_t 		Epoch time in [msec]
	SEPP_IADCS_API_QUATERNION_FLOAT ATTITUDE_QUATERNION_BF;						//!< 4x float 		Determinated attitude quaternion in body frame
	SEPP_IADCS_API_VECTOR3_XYZ_FLOAT ANGULAR_VELOCITY_VECTOR_RADPS;				//!< 3x float 		Determinated angular velocity vector [wx, wy, wz] in [rad/sec] and body frame
	unsigned long long EPOCH_TIME_LAST_ATTITUDE_DETERMINATION_MSEC;				//!< 1x int64_t 		Epoch time stamp of last attitude measurement in [msec]
	SEPP_IADCS_API_VECTOR3_XYZ_FLOAT MEASURED_MAGNETIC_FIELD_VECTOR_BF_T;		//!< 3x float 		Measured magnetic field vector [bx, by, bz] from active sensor in [T] and body frame
	unsigned long long EPOCH_TIME_LAST_MAGNETIC_FIELD_MEASUREMENT_MSEC; 		//!< 1x int64_t 		Epoch time stamp of last magnetic field measurement in [msec]
	SEPP_IADCS_API_VECTOR3_XYZ_FLOAT MEASURED_SUN_VECTOR_BF;					//!< 3x float 		Measured sun vector [svx, svy, svz] in body-frame
	unsigned long long EPOCH_TIME_LAST_SUN_VECTOR_MEASUREMENT_MSEC;				//!< 1x int64_t 		Epoch time stamp of last sun vector measurement in [msec]
} SEPP_IADCS_API_ATTITUDE_TELEMETRY;

/** SEPP IADCS API Power Status Telemetry
*/
typedef struct
{
	unsigned long long EPOCH_TIME_MSEC;											//!< 1x int64_t 		Epoch time in [msec]
	float MAGNETTORQUER_POWER_CONSUMPTION_W;									//!< 1x float: 		Power consumption in [W]
	float MAGNETTORQUER_SUPPLY_VOLTAGE_V;										//!< 1x float: 		Voltage in [V ]
	float MAGNETTORQUER_CURRENT_CONSUMPTION_A;									//!< 1x float: 		Current in [A]
	float STARTRACKER_POWER_CONSUMPTION_W;										//!< 1x float:		Power consumption in [W]
	float STARTRACKER_SUPPLY_VOLTAGE_V;											//!< 1x float: 		Voltage in [V ]
	float STARTRACKER_CURRENT_CONSUMPTION_A;									//!< 1x float: 		Current in [A]
	float IADCS_POWER_CONSUMPTION_W;											//!< 1x float: 		Power consumption in [W]
	float IADCS_SUPPLY_VOLTAGE_V;												//!< 1x float: 		Voltage in [V ]
	float IADCS_CURRENT_CONSUMPTION_A;											//!< 1x float: 		Current in [A]
	float REACTIONWHEEL_POWER_CONSUMPTION_W;									//!< 1x float: 		Power consumption in [W]
	float REACTIONWHEEL_SUPPLY_VOLTAGE_V;										//!< 1x float: 		Voltage in [V ]
	float REACTIONWHEEL_CURRENT_CONSUMPTION_A;									//!< 1x float: 		Current in [A]
} SEPP_IADCS_API_POWER_STATUS_TELEMETRY;

/** SEPP IADCS API Standard Telemetry
*/
typedef struct
{
	unsigned long long SYSTEM_TIME_MSEC;										//!< 1x int64_t 		System time in [msec]
	unsigned long long EPOCH_TIME_MSEC;											//!< 1x int64_t 		Epoch time in [msec]
	unsigned int LIVELYHOOD_REGISTER;											//!< 1x uint32_t 		Livelyhood register, indicating detected devices (5.1.6.11)
	unsigned int SYSTEM_STATUS_REGISTER;										//!< 1x uint32_t 		System status register (5.1.6.8)
	unsigned int SYSTEM_SCHEDULER_REGISTER;										//!< 1x uint32_t 		System scheduler register(5.1.6.9)
	unsigned int SYSTEM_ERROR_REGISTER;											//!< 1x uint32_t 		System error register (5.1.6.10)
	unsigned int SENSORS_ERROR_REGISTER;										//!< 1x uint32_t		Sensors error register (5.1.6.11)
	unsigned int ACTUATORS_ERROR_REGISTER;										//!< 1x uint32_t		Actuators error register (5.1.6.12)
	unsigned int CONTROL_MAIN_STATUS;											//!< 1x uint32_t		Main control status register (5.4.3.2.1)
	unsigned int CONTROL_MAIN_ERROR;											//!< 1x uint32_t		Error register (5.4.3.2.2)
	SEPP_IADCS_API_VECTOR3_XYZ_UINT CONTROL_SINGLE_AXIS_STATUS;					//!< 3x uint32			Single axis status register[X, Y,Z] (5.4.3.2.3)
	unsigned int CONTROL_ALL_AXIS_STATUS; 										//!< 1x uint32_t		All axis control status register (5.4.3.2.4)
	unsigned int SAT_MAIN_REGISTER;												//!< 1x uint32_t 		Sat Main Register (5.1.7.1)
	unsigned int SAT_ERROR_REGISTER;											//!< 1x uint32_t 		Sat Error Register (5.1.7.2)
	unsigned int SAT_SCHEDULER_REGISTER;										//!< 1x uint32_t 		Sat Scheduler Register (5.1.7.3)
	unsigned int INTERNAL_USE_1;												//!< 1x uint32_t		Internal Use
	unsigned int INTERNAL_USE_2;												//!< 1x uint32_t		Internal Use
	unsigned int NUMBER_OF_RECEIVED_COMMANDS;									//!< 1x uint32_t 		Number of received commands
	unsigned int NUMBER_OF_FAILED_COMMANDS;										//!< 1x uint32_t 		Number of commands, which are corrupted or not successfully handled.
	unsigned int INTERNAL_USE_3;												//!< 1x uint32_t		Internal Use
} SEPP_IADCS_API_STANDARD_TELEMETRY;

/** SEPP IADCS API Control Module Status
*/
typedef struct
{
	unsigned int CONTROL_STATUS_REG; 											//!< SEPP_IADCS_API_CONTROL_STATUS_REGISTER
	unsigned int CONTROL_ERROR_REG;   											//!< SEPP_IADCS_API_CONTROL_ERROR_REGISTER
	unsigned int THREE_AXIS_CONTROL_REG; 										//!< SEPP_IADCS_API_ALL_AXIS_CONTROL_STATUS_REGISTER
	SEPP_IADCS_API_VECTOR3_XYZ_UINT SINGLE_AXIS_CONTROL_REGISTERS;  			//!< SINGLE_AXIS_CONTROL_REG_X/Y/Z
} SEPP_IADCS_API_CONTROL_MODULE_STATUS;


/* ------------------------ Parameter Structures ---------------------- */

/** SEPP IADCS API Gyroscope Parameters
*/
typedef struct
{
	unsigned char OUTPUT_TYPE; 													//!< 1x uint8_t 		Output Type
	SEPP_IADCS_API_VECTOR3_XYZ_FLOAT SENSOR_STDDEV;								//!< 3x float 			Standard deviation
	SEPP_IADCS_API_VECTOR3_XYZ_FLOAT SENSOR_SENSITIVITY;						//!< 3x float 			Sensitivity Sensor sensitivity vector[sx, sy, sz] in Tesla/digit and sensor frame
	SEPP_IADCS_API_QUATERNION_FLOAT TRANSFORMATION_QUATERNION;					//!< 4x float 			Transformation quaternion
	unsigned long long UPDATE_INTERVAL_MSEC;									//!< 1x int64_t 		Update interval Sensor update interval in [msec]
	unsigned char SENSOR_CORRECTION_ENABLE;										//!< uint8_t 			Sensor correction
	SEPP_IADCS_API_MATRIX3_FLOAT CORRECTION_SCALE;								//!< 9x float 			Correction scale matrix [3x3] Sg
	SEPP_IADCS_API_VECTOR3_XYZ_FLOAT CORRECTION_OFFSET;							//!< 3x float 			Correction offset
	unsigned int BIAS_COUNTER;													//!< 1x uint32_t 		Bias counter
	unsigned char AVG_FILTER_ENABLE;											//!< 1x uint8_t 		Averaging filter enable flag
	unsigned int AVG_FILTER_COUNTER;											//!< 1x uint32_t 		Averaging filter
	float AVG_FILTER_CRITERION; 												//!< 1x float 			Sensor values criterion for averaging filter Measurement data is only used by the filter, if the deviation between the current value and the last one is below the max. allowed deviation in [rad/sec]
} SEPP_IADCS_API_GYROSCOPE_PARAMETERS;

/** SEPP IADCS API Magnetometer Parameters
*/
typedef struct
{
	SEPP_IADCS_API_VECTOR3_XYZ_FLOAT SENSOR_STDDEV;
	SEPP_IADCS_API_VECTOR3_XYZ_FLOAT SENSOR_SENSITIVITY;
	SEPP_IADCS_API_QUATERNION_FLOAT TRANSFORMATION_QUATERNION;
	unsigned long long UPDATE_INTERVAL_MSEC;
	unsigned char SENSOR_CORRECTION_ENABLE;
	SEPP_IADCS_API_MATRIX3_FLOAT CORRECTION_SCALE;
	SEPP_IADCS_API_VECTOR3_XYZ_FLOAT CORRECTION_OFFSET;
	unsigned char AVG_FILTER_ENABLE;
	unsigned int AVG_FILTER_COUNTER;
	float AVG_FILTER_CRITERION;
	unsigned char MOVING_AVG_FILTER_ENABLE;
	float MOVING_AVG_FILTER_GAIN;
	float MOVING_AVG_FILTER_CRITERION;
} SEPP_IADCS_API_MAGNETOMETER_PARAMETERS;

/** SEPP IADCS API Single Reaction Wheel Parameters
*/
typedef struct
{
	float MOMENT_OF_INERTIA_KGM2;
	float MAX_SPEED_RADPS;
	float MAX_TORQUE_NM;
	float MOTOR_CONSTANT;
	unsigned long long MAX_VALID_TIME_SPEED_MEASUREMENT;
} SEPP_IADCS_API_SINGLE_REACTIONWHEEL_PARAMETERS;

/** SEPP IADCS API Reaction Wheel Array Parameters
*/
typedef struct
{
	unsigned int NUMBER_OF_WHEELS;														//!< Number of reaction wheels
	unsigned int CONTROL_MODE;															//!< Control Mode
	float MOMENT_OF_INERTIA_KGM2;														//!< Moment of Inertia
	float MAX_SPEED_RADPS;																//!< Maximum Speed (default: 1047.2)
	float MAX_TORQUE_NM;																//!< Maximum Torque
	float MOTOR_CONSTANT;																//!< Motor Constand (default: 0)
	unsigned long long UPDATE_INTERVAL_MSEC;											//!< Update Interval
	unsigned long long MAX_VALID_TIME_SPEED_MEASUREMENT;								//!< Maximum valid time of speed measurement (default: 1000)
	SEPP_IADCS_API_VECTOR3_XYZ_FLOAT TORQUE_DISTRIBUTION_MATRIX[6]; 					//!< 3x6 matrix for 6 wheels, order: M00-M05,M10-M16,..,M60-M66
} SEPP_IADCS_API_REACTIONWHEEL_ARRAY_PARAMETERS;

/** SEPP IADCS API Magnetorquer Parameters
*/
typedef struct
{
	SEPP_IADCS_API_VECTOR3_XYZ_FLOAT CONV_FACTORS_DIPOLE_MOMENT_TO_CURRENT;				//!< Conversion factors between dipole moment and MTQ current
	SEPP_IADCS_API_VECTOR3_XYZ_FLOAT MAX_DIPOLE_MOMENT_AM2;								//!< Maximum Dipole Moment
	SEPP_IADCS_API_MATRIX3_FLOAT ORIENTATION_TO_BF;										//!< Orientaton of MTQs to body frame
	SEPP_IADCS_API_VECTOR3_XYZ_ULONGLONG RELAX_TIME_MSEC;								//!< Relax time
} SEPP_IADCS_API_MAGNETORQUER_PARAMETERS;

/** SEPP IADCS API PID Controller Parameters
*/
typedef struct
{
	float K_P;																			//!< 0..3 1x float			kp Propotional gain
	float K_I;																			//!< 4..7 1x float			ki Integrator gain
	float K_D;																			//!< 8..11 1x float 		kd Diff. gain
	float K_AW;																			//!< 12..15 1x float 		kaw Anti-windup gain
	float TH_AW;																		//!< 16..19 1x float 		thraw Anti-windup threshold
	SEPP_IADCS_API_CONTROLLER_ACTUATOR_CMD_INTERFACES CTRL_ACTUATOR_CMD_IF; 			//!< 20..23 1x int32_t		cmdtype Controller - actuator command interface 6.4.3.3
	unsigned long long UPDATE_INTERVAL_MSEC;											//!< 24..31 1x int64_t		tupdate Update interval in [msec]
} SEPP_IADCS_API_PID_CONTROLLER_PARAMETERS;

/** SEPP IADCS API Cascaded PID Controller Parameters
*/
typedef struct
{
	float K_P_IN;																		//!< 0..3 1x float			kp PID-In- Propotional gain
	float K_I_IN;																		//!< 4..7 1x float			ki PID-In- Integrator gain
	float K_D_IN;																		//!< 8..11 1x float 		kd PID-In- Diff. gain
	float K_AW_IN;																		//!< 12..15 1x float 		kaw PID-In- Anti-windup gain
	float TH_AW_IN;																		//!< 16..19 1x float 		thraw PID-In- Anti-windup threshold
	float K_P_OUT;																		//!< 20..23 1x float 		kp PID-Out- Propotional gain
	float K_I_OUT;																		//!< 24..27 1x float 		ki PID-Out- Integrator gain
	float K_D_OUT;																		//!< 28..31 1x float 		kd PID-Out- Diff. gain
	float K_AW_OUT;																		//!< 32..35 1x float 		kaw PID-Out- Anti-windup gain
	float TH_AW_OUT;																	//!< 36..39 1x float 		thraw PID-Out- Anti-windup threshold
	SEPP_IADCS_API_CONTROLLER_ACTUATOR_CMD_INTERFACES CTRL_ACTUATOR_CMD_IF;				//!< 40..43 1x int32_t		cmdtype Controller - actuator command interface 6.4.3.3
	unsigned long long UPDATE_INTERVAL_MSEC;											//!< 44..51 1x int64_t		tupdate Update interval in [msec]
} SEPP_IADCS_API_CASCADED_PID_CONTROLLER_PARAMETERS;

/** SEPP IADCS API BDot Controller Parameters
*/
typedef struct
{
	float K;																			//!< 0..3 1x float 			k Control gain:
																						//							k < 0: Detumbling
																						//							k > 0: Momentum build up
	SEPP_IADCS_API_CONTROLLER_ACTUATOR_CMD_INTERFACES CTRL_ACTUATOR_CMD_IF; 			//!< 4..7 1x int32_t 		cmdtype Controller - actuator command interface 6.4.3.3
	unsigned long long UPDATE_INTERVAL_MSEC;											//!< 8..15 1x int64_t		tupdate Update interval in [msec]
} SEPP_IADCS_API_BDOT_CONTROLLER_PARAMETERS;

/** SEPP IADCS API Sun Pointing Controller Parameters
*/
typedef struct
{
	float K_P;																			//!< 0..3 1x float 			kp Controller feedback gain affecting the behavior of the control error.
	SEPP_IADCS_API_MATRIX3_FLOAT K_V;													//!< 4..39 9x float 		kv kv is an [3x3] symmetric damping matrix affecting the behavior of the angular velocity
	SEPP_IADCS_API_VECTOR3_XYZ_FLOAT MAX_TORQUE;										//!< 40..51 3x float 		trqmax Maximum available torques on 3 axes [trqmax,x, trqmax,y, trqmax,z] in Nm and body frame. This values are usually related to the maximum available torquers of the actuators.
	SEPP_IADCS_API_CONTROLLER_ACTUATOR_CMD_INTERFACES CTRL_ACTUATOR_CMD_IF;				//!< 52..55 1x int32_t 		cmdtype Controller - actuator command interface 6.4.3.3
	unsigned long long UPDATE_INTERVAL_MSEC;											//!< 56..63 1x int64_t t	update Update interval in [msec]
} SEPP_IADCS_API_SUN_POINTING_CONTROLLER_PARAMETERS;

/** SEPP IADCS API Sliding Controller Parameters
*/
typedef struct
{
	float K_1; 																			//!< 0..3 1x float 			k1 Sliding gain k1
	float K_2; 																			//!< 4..7 1x float 			k2 Sliding gain k2
	SEPP_IADCS_API_CONTROLLER_ACTUATOR_CMD_INTERFACES CTRL_ACTUATOR_CMD_IF; 			//!< 8..11 1x int32_t 		cmdtype Controller - actuator command interface 6.4.3.3
	unsigned long long UPDATE_INTERVAL_MSEC; 											//!< 12..19 1x int64_t 		tupdate Update interval in [msec]
} SEPP_IADCS_API_SLIDING_CONTROLLER_PARAMETERS;

/** SEPP IADCS API Detumbling Mode Parameters
*/
typedef struct
{
	unsigned long long START_EPOCH_TIME_MSEC;  											//!< 0..7 int64_t 			tstart Start time as epoch time in [msec]
	unsigned long long STOP_EPOCH_TIME_MSEC;											//!< 8..15 int64_t 			tstop Stop time as epoch time in [msec]
} SEPP_IADCS_API_DETUMBLING_MODE_PARAMETERS;

/** SEPP IADCS API Sun Pointing Mode Parameters
*/
typedef struct
{
	SEPP_IADCS_API_VECTOR3_XYZ_FLOAT TARGET_VECTOR_BF;									//!< 0..11 3x float 		vtgt,b Target vector [vtgt,x, vtgt,y, vtgt,z ] as unit vector in body frame
	unsigned long long START_EPOCH_TIME_MSEC;											//!< 12..19 int64_t 		tstart Start time as epoch time in [msec]
	unsigned long long STOP_EPOCH_TIME_MSEC;											//!< 20..27 int64_t 		tstop Stop time as epoch time in [msec]
} SEPP_IADCS_API_SUN_POINTING_MODE_PARAMETERS;

/** SEPP IADCS API Target Pointing Tolerance Parameters
*/
typedef struct
{
	float PREALIGNMENT_ANGLE_TOLERANCE_RAD;												//!< Target Pointing CV angle tolerance in rad
	float PREALIGNMENT_ANGLE_TOLERANCE_PERCENT;											//!< Target Pointing CV angle tolerance in percent
	float PREALIGNMENT_ANGULAR_VELOCITY_TOLERANCE_RADPS;								//!< Target Pointing CV angular velocity tolerance in rad\/s
	float PREALIGNMENT_TARGET_THRESHOLD_RAD;											//!< Target Pointing CV pre-alignment tolerance in rad
} SEPP_IADCS_API_TARGET_POINTING_TOLERANCE_PARAMETERS;

/** SEPP IADCS API Earth Target Pointing Const Velocity Mode Parameters
*/
typedef struct
{
	SEPP_IADCS_API_TARGET_POINTING_ATTITUDE_DETERMINATION_MODES DETERMINATION_MODE;		//!< 0..3 1x int32_t 		modeatt_det Attitude determination mode 6.5.4.7.6
	SEPP_IADCS_API_VECTOR3_XYZ_FLOAT LOS_VECTOR_BF; 									//!< 4..15 3x float 		vlos Line of sight vector [vlos,x, vlos,y, vlos,z] as unit vector in body frame
	SEPP_IADCS_API_VECTOR3_XYZ_FLOAT FLIGHT_VECTOR_BF; 									//!< 16..27 3x float 		vvel Flight vector [vvel,x, vvel,y, vvel,z ] as unit vector in body frame
	float START_LATITUDE_RAD; 															//!< 28..35 2x float 		latstart; latend Start latitude, End latitude [rad] in WGS84 frame
	float STOP_LATITUDE_RAD;
	float START_LONGITUDE_RAD; 															//!< 36..43 2x float 		lonstart; lonend Stop latitude, longitude [rad] WGS84 frame
	float STOP_LONGITUDE_RAD;
	unsigned long long START_EPOCH_TIME_MSEC; 											//!< 44..51 1x int64_t 		tstart Start time as epoch time in [msec]
	unsigned long long STOP_EPOCH_TIME_MSEC; 											//!< 52..59 1x int64_t 		tstop Stop time as epoch time in [msec]
	unsigned long long OFFSET_TIME_MSEC; 												//!< 60..67 1x int64_t 		toffset Offset time in [msec] (optional)
	unsigned long long UPDATE_INTERVAL_MSEC; 											//!< 68..75 1x int64_t 		tupdate Update interval in [msec]
	SEPP_IADCS_API_TARGET_POINTING_TOLERANCE_PARAMETERS TOLERANCE_PARAMETERS; 			//!< 76..91 4x float 		Tolerance parameters, see 6.5.4.7.8
} SEPP_IADCS_API_TARGET_POINTING_CONST_VELOCITY_MODE_PARAMETERS;

/** SEPP IADCS API Earth Target Pointing Fixed Mode Parameters
*/
typedef struct
{
	SEPP_IADCS_API_TARGET_POINTING_ATTITUDE_DETERMINATION_MODES DETERMINATION_MODE;		//!< 0..3 1x int32_t 		modeatt_det Attitude determination mode 6.5.4.7.6
	SEPP_IADCS_API_VECTOR3_XYZ_FLOAT LOS_VECTOR_BF;										//!< 4..15 3x float 		vlos Line of sight vector [vlos,x, vlos,y, vlos,z] as unit vector in body frame
	SEPP_IADCS_API_VECTOR3_XYZ_FLOAT FLIGHT_VECTOR_BF;									//!< 16..27 3x float 		vvel Flight vector [vvel,x, vvel,y, vvel,z ] as unit vector in body frame
	float TARGET_LATITUDE_RAD;															//!< 28..35 2x float 		lat; lon Target latitude, lon lontitude in [rad] in WGS84 frame
	float TARGET_LONGITUDE_RAD;
	unsigned long long OFFSET_TIME_MSEC; 												//!< 36..43 1x int64_t 		toffset Offset time in [msec] (optional)
	unsigned long long UPDATE_INTERVAL_MSEC;											//!< 44..51 1x int64_t 		tupdate Update interval in [msec]
	SEPP_IADCS_API_TARGET_POINTING_TOLERANCE_PARAMETERS TOLERANCE_PARAMETERS;			//!< 52..67 4x float 		Tolerance parameters, see 6.5.4.7.8
} SEPP_IADCS_API_TARGET_POINTING_FIXED_MODE_PARAMETERS;

/** SEPP IADCS API Earth Target Pointing Nadir Mode Parameters
*/
typedef struct
{
	SEPP_IADCS_API_TARGET_POINTING_ATTITUDE_DETERMINATION_MODES DETERMINATION_MODE; 	//!< 0..3 1x int32_t 		modeatt_det Attitude determination mode 6.5.4.7.6
	SEPP_IADCS_API_VECTOR3_XYZ_FLOAT LOS_VECTOR_BF; 									//!< 4..15 3x float 		vlos Line of sight vector [vlos,x, vlos,y, vlos,z] as unit vector in body frame
	SEPP_IADCS_API_VECTOR3_XYZ_FLOAT FLIGHT_VECTOR_BF; 									//!< 16..27 3x float 		vvel Flight vector [vvel,x, vvel,y, vvel,z ] as unit vector in body frame
	unsigned long long OFFSET_TIME_MSEC; 												//!< 28..35 1x int64_t 		toffset Offset time in [msec] (optional)
	unsigned long long UPDATE_INTERVAL_MSEC; 											//!< 36..43 1x int64_t 		tupdate Update interval in [msec]
	SEPP_IADCS_API_TARGET_POINTING_TOLERANCE_PARAMETERS TOLERANCE_PARAMETERS;			//44..59 4x float 			Tolerance parameters, see 6.5.4.7.8
} SEPP_IADCS_API_TARGET_POINTING_NADIR_MODE_PARAMETERS;

/** SEPP IADCS API Star Tracker Event List
*/
typedef struct
{
	uint8_t HEADER; 																	// reduced event list header
	uint8_t NUM_EVENT; 																	// max 23
	int32_t XISF[23] = {0}; 															// [px/100] x-position of event (image sensor frame)
	int32_t YISF[23] = {0};																// [px/100] y-position of event (image sensor frame)
} SEPP_IADCS_API_STARTRACKER_EVENT_LIST;

/** SEPP IADCS API Orbit TLE Data
*/
typedef struct
{
	unsigned char TLE_1[70];													//!< TLE line 1
	unsigned char TLE_2[70];													//!< TLE line 2
	unsigned long long UPDATE_INTERVAL_MSEC;									//!< update interval of orbit propagator in [msec];
} SEPP_IADCS_API_ORBIT_TLE_DATA;

/** SEPP IADCS API Orbit Propagation Data
*/
typedef struct
{
	SEPP_IADCS_API_VECTOR3_XYZ_FLOAT POSITION;									//!< Propagation Data Position
	SEPP_IADCS_API_VECTOR3_XYZ_FLOAT VELOCITY;									//!< Propagation Data Velocity
	double julian_date;															//!< Julian Date
} SEPP_IADCS_API_ORBIT_PROPAGATION_DATA;

/** SEPP IADCS API Gyro Values
*/
typedef struct
{
	SEPP_IADCS_API_VECTOR3_XYZ_FLOAT ANGULAR_VELOCITY_BF;						//!< Angular velocity vector [wx, wy, wz] in [rad/s] in body frame
	unsigned long long TIMESTAMP_MSEC;											//!< time stamp of last value reading [msec]. This parameter is used for the GET function only.
} SEPP_IADCS_API_GYRO_VALUES;

/** SEPP IADCS API Magnetometer Values
*/
typedef struct
{
	SEPP_IADCS_API_VECTOR3_XYZ_FLOAT MAGN_FLUX_DENSITY_BF;						//!< Measured magnetic field vector [bx, by, bz] in [T] in body frame
	unsigned long long TIMESTAMP_MSEC;											//!< time stamp of last value reading [msec]. This parameter is used for the GET function only.
} SEPP_IADCS_API_MAGNETOMETER_VALUES;

/** SEPP IADCS API Sun Sensor Parameters
*/
typedef struct
{
	SEPP_IADCS_API_VECTOR3_XYZ_FLOAT SUN_VECTOR;								//!< Sun vector (with unit length)
	float INTENSITY;															//!< Sun intensity
} SEPP_IADCS_API_SUNSENSOR_PARAMETERS;

/** SEPP IADCS API Sun Sensor All Sensors Parameters
*/
typedef struct
{
	SEPP_IADCS_API_SUNSENSOR_PARAMETERS SENSOR_PARAMETERS[6];					//!< Sun Sensor parameters for all 6 sensors
} SEPP_IADCS_API_SUNSENSOR_PARAMETERS_ALL_SENSORS;							

/** SEPP IADCS API Reaction Wheel Speeds
*/
typedef struct
{
	SEPP_IADCS_API_VECTOR3_XYZ_FLOAT INTERNAL;									//!< speed of internal wheels x,y,z [rad/s]
	SEPP_IADCS_API_VECTOR3_UVW_FLOAT EXTERNAL;									//!< speed of external wheels u,v,w [rad/s]
} SEPP_IADCS_API_REACTIONWHEEL_SPEEDS;

/** SEPP IADCS API Reaction Wheel Torque
*/
typedef struct
{
	SEPP_IADCS_API_VECTOR3_XYZ_FLOAT TORQUE_TARGET;									//!< speed of internal wheels x,y,z [rad/s]
} SEPP_IADCS_API_REACTIONWHEEL_TORQUE;

/* ------------------------------ Target Pointing Telemetry Structures ----------------------------*/

//! IADCS-100 Target Pointing Target Parameters Telemetry
typedef struct
{
	unsigned long long CURRENT_EPOCH_TIME_MSEC;									//!<	0..7 		1x int64_t 		Current Epoch time in [msec]
	unsigned long long START_EPOCH_TIME_MSEC;									//!<	8..15 		1x int64_t 		Start time as epoch time in [msec]
	unsigned long long STOP_EPOCH_TIME_MSEC;									//!<	16..23 		1x int64_t 		Stop time as epoch time in [msec]
	unsigned long long OFFSET_TIME_MSEC;										//!<	24..31 		1x int64_t		Offset time in [msec] (optional)
	SEPP_IADCS_API_TARGET_POINTING_CONTROL_MODES CONTROL_MODE;					//!<	32..35 		1x int32_t 		Control mode (see 5.5.4.5.5)
	SEPP_IADCS_API_MATRIX3_FLOAT ROTATION_MATRIX;								//!<	36..71 		9x float 		(Internal use)
																				//!< 							Rotation matrix (Desired Body Frame Attitude wrt Defined Reference Frame)
	float POLYNOMIAL_COEFF[28];													//!<	72..183 	28x float 		(Optional) Polynomial coefficients
																				//!<								(Degree max = 6)
																				//!<								These coefficients are used to calculate/update the target quaternion during pointing mode.
	SEPP_IADCS_API_QUATERNION_FLOAT START_QUATERNION;							//!<	184..199 	4x float 		Start quatertion
	SEPP_IADCS_API_QUATERNION_FLOAT END_QUATERNION;								//!<	200..215 	4x float 		End quaternion
	float START_LATITUDE_RAD;													//!<	216..223 	2x float 		Start lat, lon in [rad] and WGS84 frame
	float START_LONGITUDE_RAD;
	float STOP_LATITUDE_RAD;													//!<	224..231 	2x float 		End lat, lon in [rad] and WGS84 frame
	float STOP_LONGITUDE_RAD;
} SEPP_IADCS_API_TARGET_POINTING_TARGET_PARAMETERS_TELEMETRY;

//! IADCS-100 Target Pointing Operation Parameters Telemetry
typedef struct
{
	SEPP_IADCS_API_TARGET_POINTING_GENERAL_MODES GENERAL_MODE;							//!<	0-3 		1x int32_t 		Mode: (see 5.5.4.5.7)
	SEPP_IADCS_API_TARGET_POINTING_ATTITUDE_DETERMINATION_MODES DETERMINATION_MODE;		//!<	4-7 		1x int32_t 		Attitude Determination Mode (see 5.5.4.5.6)
	SEPP_IADCS_API_TARGET_POINTING_CONTROL_MODES CONTROL_MODE;							//!<	8-11 		1x int32_t 		Control mode (see 5.5.4.5.5)
	SEPP_IADCS_API_TARGET_POINTING_TOLERANCE_PARAMETERS TOLERANCE_PARAMETERS;			//!<	12-27 		4x float 		Tolerance parameters, see 5.5.4.5.8
	unsigned long long START_EPOCH_TIME_MSEC;											//!<	28-35 		1x int64_t 		Start time as epoch time in [msec]
	float TARGET_LATITUDE_RAD;															//!<	36-39 		1x float 		Target latitude in [rad] and WGS84 frame
	float TARGET_LONGITUDE_RAD;															//!<	40-43 		1x float 		Target lontitude in [rad] and WGS84 frame
	unsigned long long UPDATE_INTERVAL_MSEC; 											//!<	44-51 		1x int64_t 		Update interval [msec]
} SEPP_IADCS_API_TARGET_POINTING_OPERATION_PARAMETERS_TELEMETRY;

//! IADCS-100 Target Pointing Operation Data Telemetry
typedef struct
{
	unsigned long long CURRENT_EPOCH_TIME_MSEC;									//!<	0..7 		1x int64_t 		Current epoch time in [msec]
	unsigned long long NEXT_ACTION_EPOCH_TIME_MSEC;								//!<	8..15 		1x int64_t 		Epoch time of next action/ calclation [msec]
	unsigned int MAIN_REGISTER;													//!<	16..19 		1x uint32_t 	Main register (Optional)
	unsigned char CURRENT_FSM_STATE;											//!<	20 			1x uint8_t 		Current state of the state machine (internal use only)
	unsigned char STATE_TARGET;													//!<	21 			1x uint8_t 		State target
	unsigned char PREALIGNMENT_IS_READY;										//!<	22 			1x uint8_t 		[Pre alignment] Is ready?
	unsigned char PREALIGNMENT_CURRENT_ROTATION_AXIS;							//!<	23 			1x uint8_t 		[Pre alignment] Current rotation axis
	SEPP_IADCS_API_VECTOR3_XYZ_SHORT PREALIGNMENT_ACTIVE_FLAG;					//!<	24..26 		3x uint8_t 		[Pre alignment] Active flags for 3 axis
	SEPP_IADCS_API_VECTOR3_XYZ_FLOAT PREALIGNMENT_TARGET_ROTATION_ANGLE;		//!<	27..38 		3x float 		[Pre alignment] Target rotation angles in [rad]
	SEPP_IADCS_API_VECTOR3_XYZ_FLOAT PREALIGNMENT_ROTATION_ANGLE;				//!<	39..50 		3x float 		[Pre alignment] Rotation angles in [rad]
	SEPP_IADCS_API_VECTOR3_XYZ_FLOAT ANGULAR_VELOCITY_BF;						//!<	51..62 		3x float 		Angular velocity in b-frame in [rad/sec]
	SEPP_IADCS_API_QUATERNION_FLOAT TARGET_QUATERNION;							//!<	63..78 		4x float 		Calculated target quaternion
	SEPP_IADCS_API_QUATERNION_FLOAT ATTITUDE_QUATERNION;						//!<	79..94 		4x float 		Current attitude quaternion
	SEPP_IADCS_API_VECTOR3_XYZ_FLOAT TARGET_REACTIONWHEEL_SPEED_XYZ_RADPS;		//!<	95..118 	6x float 		Target RW speeds in [rad/sec]
	SEPP_IADCS_API_VECTOR3_UVW_FLOAT TARGET_REACTIONWHEEL_SPEED_UVW_RADPS;
	SEPP_IADCS_API_VECTOR3_XYZ_FLOAT CURRENT_REACTIONWHEEL_SPEED_XYZ_RADPS;		//!<	119..142 	6x float 		Current RW speeds in [rad/sec]
	SEPP_IADCS_API_VECTOR3_UVW_FLOAT CURRENT_REACTIONWHEEL_SPEED_UVW_RADPS;
} SEPP_IADCS_API_TARGET_POINTING_OPERATION_DATA_TELEMETRY;

/* ----------- User-specific/Special High-Level Command Structures ------------- */

//! IADCS-100 Target Pointing Inertial Parameters
typedef struct
{

	SEPP_IADCS_API_TARGET_POINTING_ATTITUDE_DETERMINATION_MODES DETERMINATION_MODE;		//!<	0..3 		1x int32_t 		Attitude Determination Mode ( Section 5.5.4.5.6)
	SEPP_IADCS_API_VECTOR3_XYZ_FLOAT LOS_VECTOR_BF;										//!<	4..15 		3x float 		Line of sight vector in body frame
	SEPP_IADCS_API_VECTOR3_XYZ_FLOAT FLIGHT_VECTOR_BF;									//!<	16..27 		3x float 		Flight vector in body frame
	unsigned int DEGREE;																//!<	28..31 		1x int32_t 		Degree of qpolynomial
	SEPP_IADCS_API_QUATERNION_FLOAT BASE_QUATERNION;									//!<	32..47 		4x float 		Base Quaternion
																						//!<	48..143 	24x float 		(set to zero)
	unsigned long long START_EPOCH_TIME_MSEC;											//!<	144..151 	1x int64_t 		Start time as unix epoch in [msec]
	unsigned long long STOP_EPOCH_TIME_MSEC;											//!<	152..159 	1x int64_t 		Stop time as unix epoch in [msec]
	unsigned long long OFFSET_TIME_MSEC; 												//!<	160..167 	1x int64_t 		Offset time in [msec] (optional)
	unsigned long long UPDATE_INTERVAL_MSEC;											//!<	168..175 	1x int64_t 		Update Interval in [msec]
} SEPP_IADCS_API_TARGET_POINTING_INERTIAL_MODE_PARAMETERS;

//! IADCS-100 Kalman Filter Parameters
typedef struct
{
	double BIAS_PROCESS_VARIANCE;									//!<	0..7		1x double		bias process variance = 1.000000000 x 10^-16 (default)
	double ATTITUDE_PROCESS_VARIANCE;								//!<	8..15		1x double		attitude process variance = 2.741556778 x 10^-9 (default)
	double SUN_MAG_MEASUREMENT_VARIANCE;							//!<	16..23		1x double		sun/mag measurement variance = 5.288496871 x 10^-5 (default)
	double STS_MEASUREMENT_VARIANCE;								//!<	24..32		1x double		sts measurement variance = 1.504283555 x 10^-7 (default)
	bool DISABLE_BIAS;												//!<	33			1x uint8_t		disable bias, disable bias = 1, enable bias = 0 (default)
} SEPP_IADCS_API_KALMAN_FILTER_PARAMETERS;

//! IADCS-100 Target Pointing Operation Parameters
typedef struct
{
	unsigned long long UPDATE_INTERVAL_MSEC;
	float ANGLE_TOLERANCE_RAD;
	float ANGLE_TOLERANCE_PERC;
	float SPEED_TOLERANCE_RADPS;
	SEPP_IADCS_API_VECTOR3_XYZ_FLOAT ANGLES_RAD;
	unsigned long long WAIT_TIME_MSEC;
} SEPP_IADCS_API_TARGET_POINTING_OPERATION_PARAMETERS;


/* ---------------------- Definitions for Helper Methods ---------------- */

typedef struct
{
	int status;											//!< status for validity check
	char tle1[71];										//!< TLE line 1
	char tle2[71];										//!< TLE line 2
} SEPP_IADCS_API_TLE_PARSER_DATA;


/* ----------------------- End of global definitions -------------------- */

/** SEPP C++ API Library - IADCS Device Class
 * \brief Contains members for device address and general information.
 * Set and Get functions for device address, description and interface type.
 * Methods to generate correct message sequences for write and read commands.
*/
class SEPP_IADCS_API
{
	private:

	public:
		SEPP_IADCS_API();
		virtual ~SEPP_IADCS_API();


		/* All commands defined in the iADCS-100 device ICD can be accessed by using this member.
		 * The functions are implemented and documented in the respective "IADCS_100" source and header files.
		 * Hint:
		 * User-specific methods can be defined below to add some custom commands that implement procedures,
		 * high-level concepts or command that use already some static parameters for OPS-SAT mission.
		 */
		IADCS_100 *iadcs_100;            // IADCS_100 device object

		/* ---------------------- Initialization Methods --------------------------- */
		void Initialize_Device(std::string ini_filename);													//!< Initialize iADCS with user-specific configuration settings (INI file)

		/* -------------- Generic Device Info ---------------- */
		void Print_Info(void);


		/* ------------ 5. Data Interface Commands ----------- */
		/* ---------- 5.1 General Commands ------------------- */
		/* -------- 5.1.1 System Low-Level Commands ---------- */
		void Set_DCDC_Configuration(SEPP_IADCS_API_SYSTEM_LOWLEVEL_DCDC_REGISTER dcdc_reg);					//!< Set low-level DC-DC converter enable states

		/* --------------- 5.1.2 System Commands -------------- */

		/* -------------- Epoch and System Time -------------- */
		bool Set_Epoch_Time(unsigned long long iadcs_epoch_time_msec);										//!> Set iADCS time in seconds since epoch 01.01.1970
		unsigned long long Get_Epoch_Time(void);															//!> Get iADCS time in seconds since epoch 01.01.1970
		unsigned long long Get_System_Time(void);															//!< Get system	time from startup in msec


		/* ------------- Configuration and Setup ------------- */
		void Set_Power_Update_Interval(unsigned long long interval_msec);									//!> Set iADCS Power TM update interval
		unsigned long long Get_Power_Update_Interval(void);													//!> Get iADCS Power TM update interval
		void Set_Temperature_Update_Interval(unsigned long long interval_msec);								//!> Set iADCS Temperature TM update interval
		unsigned long long Get_Temperature_Update_Interval(void);											//!> Get iADCS Temperature TM update interval
		void Set_Active_Gyroscope(SEPP_IADCS_API_GYROSCOPES gyro_index);									//!> Set active Gyroscope
		void Set_Active_Magnetometer(SEPP_IADCS_API_MAGNETOMETERS magnetometer_index);						//!> Set active Magnetometer

		/* ------------- Device Telemetry Methods ----------- */
		SEPP_IADCS_API_INFO_TELEMETRY Get_Info_Telemetry(void);												//!< Get iADCS info telemetry
		SEPP_IADCS_API_STANDARD_TELEMETRY Get_Standard_Telemetry(void);										//!< Get iADCS standard telemetry
		SEPP_IADCS_API_SENSOR_TELEMETRY Get_Sensor_Telemetry(void);											//!< Get iADCS sensor telemetry
		SEPP_IADCS_API_ACTUATOR_TELEMETRY Get_Actuator_Telemetry(void);										//!< Get iADCS actuator telemetry
		SEPP_IADCS_API_ATTITUDE_TELEMETRY Get_Attitude_Telemetry(void);										//!< Get iADCS attitude telemetry
		SEPP_IADCS_API_POWER_STATUS_TELEMETRY Get_Power_Status_Telemetry(void);								//!< Get iADCS power status telemetry

		/* ------------------- Register Methods -------------- */
		void Set_System_Scheduler_Register(SEPP_IADCS_API_SYSTEM_SCHEDULER_REGISTER scheduler_reg);			//!< Set System Scheduler Register
		SEPP_IADCS_API_SYSTEM_SCHEDULER_REGISTER Get_System_Scheduler_Register(void);						//!< Get iADCS system scheduler register parameters
		void Clear_System_Error_Register(void);																//!< Clear System Error register

		// Additional custom methods
		SEPP_IADCS_API_SYSTEM_STATUS_REGISTER Get_System_Status_Register(void);								//!< Get iADCS system status register parameters
		SEPP_IADCS_API_SYSTEM_ERROR_REGISTER Get_System_Error_Register(void);								//!< Get iADCS system error register parameters
		SEPP_IADCS_API_SENSORS_ERROR_REGISTER Get_Sensors_Error_Register(void);								//!< Get iADCS sensors error register parameters
		SEPP_IADCS_API_ACTUATORS_ERROR_REGISTER Get_Actuators_Error_Register(void);							//!< Get iADCS actuators error register parameters
		SEPP_IADCS_API_SYSTEM_LIVELYHOOD_REGISTER Get_System_Livelyhood_Register(void);  					//!< Get iADCS Livelyhood register parameters
		SEPP_IADCS_API_SAT_MAIN_REGISTER Get_Sat_Main_Register(void);										//!< Get iADCS sat main register parameters
		SEPP_IADCS_API_SAT_ERROR_REGISTER Get_Sat_Error_Register(void);										//!< Get iADCS error register parameters
		SEPP_IADCS_API_SAT_SCHEDULER_REGISTER Get_Sat_Scheduler_Register(void);								//!< Get iADCS sat scheduler register parameters

		/* ------------ 5.1.3 Startracker Commands ----------- */
		void Set_Startracker_Quaternion(SEPP_IADCS_API_STARTRACKER_QUATERNION quat);						//!< Set Startracker Quaternion
		SEPP_IADCS_API_STARTRACKER_QUATERNION Get_Startracker_Quaternion(void);								//!< Get Startracker Quaternion
		SEPP_IADCS_API_STARTRACKER_EVENT_LIST Get_Startracker_Event_List(void);								//!< Get star list data from ST200
		void Set_Startracker_Operation_Mode(SEPP_IADCS_API_STARTRACKER_OPERATING_MODE mode);				//!< Set Startracker operation mode
		SEPP_IADCS_API_STARTRACKER_OPERATING_MODE Get_Startracker_Operation_Mode(void);						//!< Get Startracker operation mode
		void Send_Startracker_Native_Command(unsigned char *native_command, unsigned int number_of_bytes);	//!< Send Startracker native command
		unsigned int Check_For_Startracker_Native_Reply(void); 												//!< Check whether a reply of a native command was sent.
		void Read_Startracker_Native_Reply(unsigned char *native_reply, unsigned int number_of_bytes);		//!< Read Startracker reply
	
		/* --------------- 5.1.4 Orbit Commands -------------- */
		unsigned char Init_Orbit_Module(SEPP_IADCS_API_ORBIT_TLE_DATA tle_data);							//!< Initialize Orbit Propagator with TLE data
		SEPP_IADCS_API_ORBIT_PROPAGATION_DATA Get_Orbit_Data(void);											//!< Get Orbit Data from Propagator

		/* ------------------ 5.1.5 HIL commands ------------- */
		void Set_HIL_Configuration(SEPP_IADCS_API_SYSTEM_HIL_CONFIGURATION_REGISTER reg_value);				//!< Set HIL configuration to set certain devices in HIL-mode, that values can be set from outside.
		SEPP_IADCS_API_SYSTEM_HIL_CONFIGURATION_REGISTER Get_HIL_Configuration(void);						//!< Get HIL configuration

		/* ------------- 5.2 Sensor Command Interface ----------- */

		/* ------------- 5.2.1 Gyroscope ------------------------ */
		/* ------------- 5.2.1.1 Gyroscope Commands ------------- */
		void Set_Gyro_Values(SEPP_IADCS_API_GYROSCOPES gyro_index, SEPP_IADCS_API_VECTOR3_XYZ_FLOAT axis_rate_SF_radps);		//!< Set Gyro Values
		SEPP_IADCS_API_GYRO_VALUES Get_Gyro_Values(SEPP_IADCS_API_GYROSCOPES gyro_index);										//!< Get iADCS Gyroscope Rate
		void Set_Gyro_Parameters(SEPP_IADCS_API_GYROSCOPES gyro_index, SEPP_IADCS_API_GYROSCOPE_PARAMETERS params);				//!< Set Gyro Parameters
		SEPP_IADCS_API_GYROSCOPE_PARAMETERS Get_Gyro_Parameters(SEPP_IADCS_API_GYROSCOPES gyro_index);							//!< Get Gyro Parameters
		void Set_Gyro_Update_Interval(SEPP_IADCS_API_GYROSCOPES gyro_index, unsigned long long interval_msec);					//!< Set Gyro Update Interval in msec
		unsigned long long Get_Gyro_Update_Interval(SEPP_IADCS_API_GYROSCOPES gyro_index);										//!< Get Gyro Update Interval in msec
		void Enable_Gyro_Correction(SEPP_IADCS_API_GYROSCOPES gyro_index);														//!< Enable Gyro Correction
		void Disable_Gyro_Correction(SEPP_IADCS_API_GYROSCOPES gyro_index);														//!< Disable Gyro Correction
		void Start_Gyro_Bias_Calculation(SEPP_IADCS_API_GYROSCOPES gyro_index);													//!< Start Gyro Zero Bias Calculation
		void Set_Gyro_Bias_Value(SEPP_IADCS_API_GYROSCOPES gyro_index, SEPP_IADCS_API_VECTOR3_XYZ_FLOAT gyro_bias_radps);		//!< Set Gyro Bias Value
		SEPP_IADCS_API_VECTOR3_XYZ_FLOAT Get_Gyro_Bias_Value(SEPP_IADCS_API_GYROSCOPES gyro_index);								//!< Get Gyro Bias Value (after calculation or set)
		void Enable_Gyro_Bias_Removement(SEPP_IADCS_API_GYROSCOPES gyro_index);													//!< Enable Gyro Bias Removement
		void Disable_Gyro_Bias_Removement(SEPP_IADCS_API_GYROSCOPES gyro_index);												//!< Disable Gyro Bias Removement
		void Enable_Gyro_Averaging_Filter(SEPP_IADCS_API_GYROSCOPES gyro_index);												//!< Enable Gyro Averaging Filter
		void Disable_Gyro_Averaging_Filter(SEPP_IADCS_API_GYROSCOPES gyro_index);												//!< Disable Gyro Averaging Filter

		/* ------------- 5.2.2 Magnetometers ------------------------ */
		/* ------------- 5.2.2.1 Magnetometer Commands ------------- */
		void Set_Magnetometer_Values(SEPP_IADCS_API_MAGNETOMETERS magnetometer_index, SEPP_IADCS_API_VECTOR3_XYZ_FLOAT magn_flux_density_T);	//!< Set Magnetometer Values
		SEPP_IADCS_API_MAGNETOMETER_VALUES Get_Magnetometer_Values(SEPP_IADCS_API_MAGNETOMETERS magnetometer_index);							//!< Get iADCS Gyroscope Rate
		void Set_Magnetometer_Parameters(SEPP_IADCS_API_MAGNETOMETERS magnetometer_index, SEPP_IADCS_API_MAGNETOMETER_PARAMETERS params);
		SEPP_IADCS_API_MAGNETOMETER_PARAMETERS Get_Magnetometer_Parameters(SEPP_IADCS_API_MAGNETOMETERS magnetometer_index);
		void Set_Magnetometer_Update_Interval(SEPP_IADCS_API_MAGNETOMETERS magnetometer_index, unsigned long long interval_msec);
		unsigned long long Get_Magnetometer_Update_Interval(SEPP_IADCS_API_MAGNETOMETERS magnetometer_index);
		void Enable_Magnetometer_Correction(SEPP_IADCS_API_MAGNETOMETERS magnetometer_index);
		void Disable_Magnetometer_Correction(SEPP_IADCS_API_MAGNETOMETERS magnetometer_index);
		void Enable_Magnetometer_Averaging_Filter(SEPP_IADCS_API_MAGNETOMETERS magnetometer_index);
		void Disable_Magnetometer_Averaging_Filter(SEPP_IADCS_API_MAGNETOMETERS magnetometer_index);
		void Enable_Magnetometer_Moving_Averaging_Filter(SEPP_IADCS_API_MAGNETOMETERS magnetometer_index);
		void Disable_Magnetometer_Moving_Averaging_Filter(SEPP_IADCS_API_MAGNETOMETERS magnetometer_index);

		/* ------------- 5.2.3 Sun Sensors ------------------------ */
		/* ------------- 5.2.3.1 Sun Sensor Commands -------------- */
		void Set_Sun_Vector(SEPP_IADCS_API_VECTOR3_XYZ_FLOAT sun_vector_SF, unsigned long long epoch_timestamp_msec);				//!< Set Sun Vector


		/* ------------- 5.3 Actuator Command Interface ----------- */

		/* ------------- 5.3.1 Reaction Wheels ------------------------ */
		/* ------------- 5.3.1.1 Reaction Wheel Commands ------------- */
		void Set_ReactionWheel_Speed(SEPP_IADCS_API_REACTIONWHEELS wheel_index, float speed_radps);
		float Get_ReactionWheel_Speed(SEPP_IADCS_API_REACTIONWHEELS wheel_index);
		void Set_ReactionWheel_Parameters(SEPP_IADCS_API_REACTIONWHEELS wheel_index, SEPP_IADCS_API_SINGLE_REACTIONWHEEL_PARAMETERS params);
		SEPP_IADCS_API_SINGLE_REACTIONWHEEL_PARAMETERS Get_ReactionWheel_Parameters(SEPP_IADCS_API_REACTIONWHEELS wheel_index);
		void Set_ReactionWheel_All_Update_Interval(unsigned long long interval_msec);
		unsigned long long Get_ReactionWheel_All_Update_Interval(void);
		void Enable_ReactionWheel_Idle_Mode(SEPP_IADCS_API_REACTIONWHEELS wheel_index);
		void Set_ReactionWheel_All_Speeds(SEPP_IADCS_API_REACTIONWHEEL_SPEEDS speed_radps);
		SEPP_IADCS_API_REACTIONWHEEL_SPEEDS Get_ReactionWheel_All_Speeds(void);
		void Set_ReactionWheel_All_Parameters(SEPP_IADCS_API_REACTIONWHEEL_ARRAY_PARAMETERS params);
		SEPP_IADCS_API_REACTIONWHEEL_ARRAY_PARAMETERS Get_ReactionWheel_All_Parameters(void);
		void Enable_ReactionWheel_All_Idle_Mode(void);
		void Set_ReactionWheel_Torque_Target(SEPP_IADCS_API_REACTIONWHEEL_TORQUE torque_Nm);
		SEPP_IADCS_API_REACTIONWHEEL_TORQUE Get_ReactionWheel_Torque_Target(void);

		/* ------------ 5.3.2 Magnettorquers ------------------ */
		/* ------------ 5.3.2.1 Device Description ------------ */
		/* ------------ 5.3.2.2 Magnettorquer Commands -------- */
		void Set_Magnettorquer_All_Dipole_Moments(SEPP_IADCS_API_VECTOR3_XYZ_FLOAT moment_Am);
		SEPP_IADCS_API_VECTOR3_XYZ_FLOAT Get_Magnettorquer_All_Dipole_Moments(void);
		void Suspend_All_Magnettorquers(void);
		void Resume_All_Magnettorquers(void);
		void Set_Magnettorquer_Parameters(SEPP_IADCS_API_MAGNETORQUER_PARAMETERS params);
		SEPP_IADCS_API_MAGNETORQUER_PARAMETERS Get_Magnettorquer_Parameters(void);
		void Set_Magnettorquer_All_Currents(SEPP_IADCS_API_VECTOR3_XYZ_FLOAT moment_Am);
		SEPP_IADCS_API_VECTOR3_XYZ_FLOAT Get_Magnettorquer_All_Currents(void);


		/* ----------- 5.4. Control Command Interface ----------- */

		/* ----------- 5.4.1 Single Axis Control ---------------- */
		/* ----------- 5.4.1.2 Angular Velocity Control --------- */
		void Set_SingleAxis_AngularVelocity_Controller_Parameters(SEPP_IADCS_API_SINGLEAXIS_CONTROL_TARGET_AXIS axis, SEPP_IADCS_API_PID_CONTROLLER_PARAMETERS params);
		SEPP_IADCS_API_PID_CONTROLLER_PARAMETERS Get_SingleAxis_AngularVelocity_Controller_Parameters(SEPP_IADCS_API_SINGLEAXIS_CONTROL_TARGET_AXIS axis);
		void Start_SingleAxis_AngularVelocity_Controller(SEPP_IADCS_API_SINGLEAXIS_CONTROL_TARGET_AXIS axis, float angular_velocity_radps);
		void Stop_SingleAxis_AngularVelocity_Controller(SEPP_IADCS_API_SINGLEAXIS_CONTROL_TARGET_AXIS axis);

		/* ----------- 5.4.1.3 Angle Step Control --------- */
		void Set_SingleAxis_AngleStep_Controller_Parameters(SEPP_IADCS_API_SINGLEAXIS_CONTROL_TARGET_AXIS axis, SEPP_IADCS_API_CASCADED_PID_CONTROLLER_PARAMETERS params);
		SEPP_IADCS_API_CASCADED_PID_CONTROLLER_PARAMETERS Get_SingleAxis_AngleStep_Controller_Parameters(SEPP_IADCS_API_SINGLEAXIS_CONTROL_TARGET_AXIS axis);
		void Start_SingleAxis_AngleStep_Controller(SEPP_IADCS_API_SINGLEAXIS_CONTROL_TARGET_AXIS axis, float angle_step_rad);
		void Stop_SingleAxis_AngleStep_Controller(SEPP_IADCS_API_SINGLEAXIS_CONTROL_TARGET_AXIS axis);

		/* ----------- 5.4.2 Three Axis Control ---------------- */
		/* ----------- 5.4.2.1 B-Dot Control --------- */
		void Set_ThreeAxis_BDot_Controller_Max_Parameters(SEPP_IADCS_API_BDOT_CONTROLLER_PARAMETERS params);
		SEPP_IADCS_API_BDOT_CONTROLLER_PARAMETERS Get_ThreeAxis_BDot_Controller_Max_Parameters(void);
		void Set_ThreeAxis_BDot_Controller_Proportional_Parameters(SEPP_IADCS_API_BDOT_CONTROLLER_PARAMETERS params);
		SEPP_IADCS_API_BDOT_CONTROLLER_PARAMETERS Get_ThreeAxis_BDot_Controller_Proportional_Parameters(void);

		/* ----------- 5.4.2.2 Sun Pointing Control --------- */
		void Set_ThreeAxis_SunPointing_Controller_Parameters(SEPP_IADCS_API_SUN_POINTING_CONTROLLER_PARAMETERS params);
		SEPP_IADCS_API_SUN_POINTING_CONTROLLER_PARAMETERS Get_ThreeAxis_SunPointing_Controller_Parameters(void);

		/* ----------- 5.4.2.3 Sliding Control --------- */
		void Set_ThreeAxis_Sliding_Controller_Parameters(SEPP_IADCS_API_SLIDING_CONTROLLER_PARAMETERS params);
		SEPP_IADCS_API_SLIDING_CONTROLLER_PARAMETERS Get_ThreeAxis_Sliding_Controller_Parameters(void);


		/* ----------- 5.5 Operation Mode Command Interface ---------------- */

		/* ----------- 5.5.1 Basic Modes ---------------- */
		void Set_Operation_Mode_Idle(void);														//!< Triggers set mode idle command execution
		void Set_Operation_Mode_Safe(void);														//!< Triggers set mode safe command execution
		void Set_Operation_Mode_Measurement(void);												//!< Triggers set mode measurement command execution

		/* ----------- 5.5.2 Detumbling Mode ---------------- */
		void Start_Operation_Mode_Detumbling(SEPP_IADCS_API_DETUMBLING_MODE_PARAMETERS config);
		void Stop_Operation_Mode_Detumbling(void);

		/* ----------- 5.5.3 Active Sun Pointing Mode ---------------- */
		void Start_Operation_Mode_Sun_Pointing(SEPP_IADCS_API_SUN_POINTING_MODE_PARAMETERS config);
		void Stop_Operation_Mode_Sun_Pointing(void);

		/* ----------- 5.5.4 Payload Target Modes ---------------- */

		/* --- 5.5.4.2 Target Pointing - Earth Const. Velocity Commands ---- */
		void Start_Target_Pointing_Earth_Const_Velocity_Mode(SEPP_IADCS_API_TARGET_POINTING_CONST_VELOCITY_MODE_PARAMETERS config);
		void Stop_Target_Pointing_Earth_Const_Velocity_Mode(void);

		/* --- 5.5.4.3 Target Pointing - Earth Fix Commands ---- */
		void Start_Target_Pointing_Earth_Fix_Mode(SEPP_IADCS_API_TARGET_POINTING_FIXED_MODE_PARAMETERS config);
		void Stop_Target_Pointing_Earth_Fix_Mode(void);

		/* ----- 5.5.4.4 Target Pointing - Nadir Commands ------ */
		void Start_Target_Pointing_Nadir_Mode(SEPP_IADCS_API_TARGET_POINTING_NADIR_MODE_PARAMETERS config);
		void Stop_Target_Pointing_Nadir_Mode(void);

		/* ----------- 5.5.4.5 Target Pointing - Telemetry & Identfiers ------ */
		/* ----------- 5.5.4.5.1 Target Pointing - Telemetry Commands ------ */
		SEPP_IADCS_API_TARGET_POINTING_TARGET_PARAMETERS_TELEMETRY Get_Target_Pointing_Target_Parameters_Telemetry(void);
		SEPP_IADCS_API_TARGET_POINTING_OPERATION_PARAMETERS_TELEMETRY Get_Target_Pointing_Operation_Parameters_Telemetry(void);
		SEPP_IADCS_API_TARGET_POINTING_OPERATION_DATA_TELEMETRY Get_Target_Pointing_Operation_Data_Telemetry(void);


		/* ---------------- User-specific/Special High-Level Methods -------------------- */
		void Start_Target_Pointing_Inertial_Mode(SEPP_IADCS_API_TARGET_POINTING_INERTIAL_MODE_PARAMETERS config);
		void Stop_Target_Pointing_Inertial_Mode(void);
		void Set_Kalman_Filter_Parameters(SEPP_IADCS_API_KALMAN_FILTER_PARAMETERS params);
		SEPP_IADCS_API_KALMAN_FILTER_PARAMETERS Get_Kalman_Filter_Parameters();
		void Set_Target_Pointing_Operation_Parameters(SEPP_IADCS_API_TARGET_POINTING_OPERATION_PARAMETERS params);

		/* --------------------- Some Helper Methods --------------------- */
		float rpm_to_radps(float rpm_value);						//!< Convert rounds/min to radians/sec
		float radps_to_rpm(float radps_value);						//!< Convert radians/sec to rounds/min

		SEPP_IADCS_API_TLE_PARSER_DATA parse_TLE_file(char *filename);						//!< Read lines from TLE orbit parameter file
};

/** SEPP C++ API Library - IADCS Device Exception
 */
class SEPP_IADCS_API_Exception : public std::exception
{
  private:
      std::string e_;
    
  public:
      SEPP_IADCS_API_Exception(std::string e) : e_(e) {}
      ~SEPP_IADCS_API_Exception() throw () {}
      const char* what() const throw() { return e_.c_str(); }
};

/** \} */ // End of Group

#endif

