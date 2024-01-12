/** OPS-SAT SEPP C++ API Library - Global Definitions
*
* \file SEPP_API_Global.h
*
* SEPP API library Global Definitions
*
* \project  		OPS-SAT SEPP C++ API Library
*
* \organisation IKS, TU Graz, Inffeldgasse 12/I, 8010 Graz
* \author 			Reinhard Zeif, MSc
* \date 			13.Nov.2020
*
* \version 			0.1  	first draft																	Dec.2017
* \version			0.2		update global fpga offset and base addresses								Jan. 2018
* \version			0.3		FPGA gpio updated															Oct. 2018
* \version			0.4		Path and file definitions added to allow device initialization from
* 							INI configuration files. Definition for iADCS INI added.					13.Nov.2020
* 
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


#ifndef SEPP_API_GLOBAL_H_
#define SEPP_API_GLOBAL_H_

/* ----------- DEVICE CONFIGURATION INI FILE DEFINITIONS --------- */

// default path to the SEPP API initialization/calibration INI configuratin files
#define SEPP_API_CONFIG_FILE_PATH									"/etc/sepp_api_config/"

// default files for device configuration/initialization (with initialization parameters)
#define SEPP_IADCS_API_DEVICE_INITIALIZATION_CONFIG_FILE			"sepp_iadcs_dev_init.ini"


/* ---------- SEPP HPS/FPGA BRIDGE DEFINITIONS ---------- */

#define		SEPP_GLOBAL_LW_H2F_BRIDGE_BASE							0xFF200000		// FPGA Slaves Accessed Via Lightweight HPS2FPGA AXI Bridge
#define		SEPP_GLOBAL_AXI_H2F_BRIDGE_BASE							0xC0000000 		// FPGA Slaves Accessed Via HPS2FPGA AXI Bridge


/* -------- SEPP LW H2F BRDIGE FPGA MODULE OFFSET + BASES ------ */
// System ID Module
#define		SEPP_GLOBAL_SYSTEM_ID_OFFSET							0x0000			// defined in QSys
#define		SEPP_GLOBAL_SYSTEM_ID_SIZE								0x0007			
#define		SEPP_GLOBAL_SYSTEM_ID_BASE								(SEPP_GLOBAL_LW_H2F_BRIDGE_BASE + SEPP_GLOBAL_SYSTEM_ID_OFFSET)
// FPGA GPIO
#define		SEPP_GLOBAL_FPGA_GPIO_0_OFFSET							0x1000			// defined in QSys
#define		SEPP_GLOBAL_FPGA_GPIO_0_SIZE							0x0020			// 8 x 32 bit = 32 byte
#define		SEPP_GLOBAL_FPGA_GPIO_0_BASE							(SEPP_GLOBAL_LW_H2F_BRIDGE_BASE + SEPP_GLOBAL_FPGA_GPIO_0_OFFSET)
// SDR Avalon MM Slave RegMap Port
#define		SEPP_GLOBAL_SDR_AVMMS_REGMAP_OFFSET						0x2000			// Avalon MM Slave RegMap for SDR Control/Status Registers, defined in QSys
#define		SEPP_GLOBAL_SDR_AVMMS_REGMAP_SIZE						0x0008			// reg-map size in bytes: 2 x 32 bit = 8 byte
#define		SEPP_GLOBAL_SDR_AVMMS_REGMAP_BASE						(SEPP_GLOBAL_LW_H2F_BRIDGE_BASE + SEPP_GLOBAL_SDR_AVMMS_REGMAP_OFFSET)
// SDR MSGDMA Dispatcher CSR Port
#define		SEPP_GLOBAL_SDR_MSGDMA_DISPATCHER_CSR_OFFSET			0x3000			// defined in QSys
#define		SEPP_GLOBAL_SDR_MSGDMA_DISPATCHER_CSR_SIZE				0x0020			// 8 x 32 bit = 32 bytes
#define		SEPP_GLOBAL_SDR_MSGDMA_DISPATCHER_CSR_BASE				(SEPP_GLOBAL_LW_H2F_BRIDGE_BASE + SEPP_GLOBAL_SDR_MSGDMA_DISPATCHER_CSR_OFFSET)
// SDR MSGDMA Dispatcher Descriptor Port
#define		SEPP_GLOBAL_SDR_MSGDMA_DISPATCHER_DESCRIPTOR_OFFSET		0x4000			// defined in QSys
#define		SEPP_GLOBAL_SDR_MSGDMA_DISPATCHER_DESCRIPTOR_SIZE		0x0010			// 4 x 32 bit = 16 byte
#define		SEPP_GLOBAL_SDR_MSGDMA_DISPATCHER_DESCRIPTOR_BASE		(SEPP_GLOBAL_LW_H2F_BRIDGE_BASE + SEPP_GLOBAL_SDR_MSGDMA_DISPATCHER_DESCRIPTOR_OFFSET)



/* ---------- SEPP HPS RAM MEMORY DEFINITIONS ----------- */
/* 1 MB = 1 Megabyte = 1024 * 1 KB = 1,048,576 bytes
 * 1 GB = 0x40000000 bytes = 1073741824 byte = 1024 MB
 * The SEPP mainboard has by default 1GB = 1024 MB of HPD RAM memory.
 * The lower 1000 MB are used for Linux while the upper memory
 * segment of 24 MB is reserved for memory-mapped I/O with FPGA.
 * Hence, bootloader parameter bootargs environment varialbe contains
 * mem=1000M memmap=24M$1000M entry to reserve upper 24 MB.
 * The linux memory of 1000 MB = 0x3E800000 = 1048576000 byte.
 * As a consequence, the RAM reserved region starts at address
 * 0x3E800000 since non-reserved address range goes from
 * 0 to 0x3E800000-1 = 0x3E7FFFFF.
 */

// Reserved HPS RAM Region and Size
// 64 MB of upper HPS RAM (FPGA RAM connected over AXI H2F bridge) is currently reserved
#define		SEPP_GLOBAL_HPS_RAM_RESERVED_REGION_SIZE	( 64 << 20 )						// Size of reserved region is 64 Megabyte
#define		SEPP_GLOBAL_HPS_RAM_RESERVED_REGION_BASE	( SEPP_GLOBAL_AXI_H2F_BRIDGE_BASE + 0x20000000 - SEPP_GLOBAL_HPS_RAM_RESERVED_REGION_SIZE )		// Base address of reserved memory region used for data transfer between FPGA and HPS

// SDR FPGA Module SGDMA writer used the following region an size to transfer samples from FPGA to HPS RAM
#define		SEPP_GLOBAL_HPS_RAM_RESERVED_REGION_SDR_MSGDMA_OFFSET		0x00000000		// Offset from HPS RAM base address where SEPP SDR MSGDMA core can start to write bytes to
#define		SEPP_GLOBAL_HPS_RAM_RESERVED_REGION_SDR_MSGDMA_SIZE		SEPP_GLOBAL_HPS_RAM_RESERVED_REGION_SIZE		// Size of region where SDR mSGDMA can write data to = full reserved region
#define		SEPP_GLOBAL_HPS_RAM_RESERVED_REGION_SDR_MSGDMA_BASE		(SEPP_GLOBAL_HPS_RAM_RESERVED_REGION_BASE + SEPP_GLOBAL_HPS_RAM_RESERVED_REGION_SDR_MSGDMA_OFFSET)  // base address in RAM where SDR mSGDMA core can write data bytes
#define		SEPP_GLOBAL_HPS_RAM_RESERVED_REGION_SDR_MSGDMA_END		(SEPP_GLOBAL_HPS_RAM_RESERVED_REGION_SDR_MSGDMA_BASE+SEPP_GLOBAL_HPS_RAM_RESERVED_REGION_SDR_MSGDMA_SIZE-1)		// End is base + size - 1, because address index starts from 0 (index = 0 to SIZE-1) = 1048575 bytes

/* IMPORTANT: last memory segment address must not go above 0x40000000 !!! */

/* just as a hint: */
// #define			SEPP_MB_HPS_RAM_RESERVED_REGION_NEXT_OFFSET 		(SEPP_MB_HPS_RAM_RESERVED_REGION_SDR_MSGDMA_BASE+SEPP_MB_HPS_RAM_RESERVED_REGION_SDR_MSGDMA_SIZE)		// placeholder so save next offset value

/* ----------------------------------------------------- */


#endif
