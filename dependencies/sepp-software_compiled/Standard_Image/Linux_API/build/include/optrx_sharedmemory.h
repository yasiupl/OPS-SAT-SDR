/** OPT-RX Shared Memory Device Library
 *
 * \file optrx_sharedmemory.h
 *
 * Generic Class for Optical-RX shared memory SPI commands and parameters.
 *
 * \project 		OPS-SAT
 * \organisation	Graz University of Technology
 * \author     		Reinhard Zeif, MSc
 * \date       		29.Nov.2021
 *
 * \brief The class can be used to access the Optical-RX shared memory for
 * read and erase operations.
 *
 * Revision History:
 * \version 0.1		Initial release with read and erase methods
 * \version	0.2		Erase method reworked with extended flag checking			07.Dec.2020
 * \version	0.3		Erase and Write methods reviewed, updated and extended
 * 					with some debug output										16.Nov.2021
 * \version 0.4		Erase and Read extended with auto-recognition of 3-Byte or
 * 					4-Byte addressing mode										29.Nov.2021
 * 					
 * \copyright (C) 2021 Graz University of Technology / IKS. All rights reserved.
 * Your use of TU Graz design tools, logic functions and other
 * software and tools and any output files any of the foregoing
 * (including device programming or simulation files), and any associated
 * documentation or information are expressly subject to the terms and
 * conditions of the applicable license agreement, including, without
 * limitation, that your use is for the sole purpose of programming the
 * OPS-SAT processing platform.
 */

#ifndef OPTRX_SHAREDMEMORY_H_
#define OPTRX_SHAREDMEMORY_H_

#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <cstring>

#include "sepp_spi_dev.h"
#include "SEPP_API_Debug.h"

/** --------------- Definitions for Shared Memory ------------------------- */

// Memory SPI Command Codes
#define		OPTRX_SHAREDMEMORY_CMD_READ_3B							0x03  	// READ with 3-byte address
#define		OPTRX_SHAREDMEMORY_CMD_READ_4B							0x13	// READ with 4-byte address
#define		OPTRX_SHAREDMEMORY_CMD_ERASE_3B_SUBSECTOR_4K			0x20  	// ERASE 4K SubSector with 3-byte address
#define		OPTRX_SHAREDMEMORY_CMD_ERASE_4B_SUBSECTOR_4K			0x21	// ERASE 4K SubSector with 4-byte address
#define		OPTRX_SHAREDMEMORY_CMD_WRITE_ENABLE						0x06  	// WRITE_ENABLE command

#define		OPTRX_SHAREDMEMORY_CMD_READ_FLAG_STATUS_REG				0x70  	// READ_FLAG_STATUS_REGISTER command
#define		OPTRX_SHAREDMEMORY_MASK_FLAG_STATUS_REG_PECTRL_STATUS			0x80	// bit 7: Program Erase Controller status bit
#define		OPTRX_SHAREDMEMORY_MASK_FLAG_STATUS_REG_ERASESUSPEND_STATUS		0x40	// bit 6: Erase suspended status bit
#define		OPTRX_SHAREDMEMORY_MASK_FLAG_STATUS_REG_ERASE_ERROR				0x20	// bit 5: Erase error bit
#define		OPTRX_SHAREDMEMORY_MASK_FLAG_STATUS_REG_PROGRAM_ERROR			0x10	// bit 4: Program error bit
#define		OPTRX_SHAREDMEMORY_MASK_FLAG_STATUS_REG_RESERVED				0x08	// bit 3: Reserved
#define		OPTRX_SHAREDMEMORY_MASK_FLAG_STATUS_REG_PROGRAM_SUSPEND_STATUS	0x04	// bit 2: Program suspend status bit
#define		OPTRX_SHAREDMEMORY_MASK_FLAG_STATUS_REG_PROTECTION_ERROR		0x02	// bit 1: Protection error bit
#define		OPTRX_SHAREDMEMORY_MASK_FLAG_STATUS_REG_ADDRESSING_STATUS		0x01	// bit 0: Addressing 3B/4B status bit

#define		OPTRX_SHAREDMEMORY_CMD_READ_STATUS_REG					0x05  	// READ_STATUS_REGISTER command
#define		OPTRX_SHAREDMEMORY_MASK_STATUS_REG_STATUSREGWRITEENABLE_CONTROL	0x80	// bit 7: Status register write enable/disable
#define		OPTRX_SHAREDMEMORY_MASK_STATUS_REG_TOPBOTTOM_CONTROL			0x20	// bit 5: protected memory area defined by the block protect bits starts from the top or bottom of the memory array
#define		OPTRX_SHAREDMEMORY_MASK_STATUS_REG_BLOCKPROTECT_CONTROL			0x5C	// bit 6,4,3,2: Block Protection
#define		OPTRX_SHAREDMEMORY_MASK_STATUS_REG_WRITEENABLELATCH_CONTROL		0x02	// bit 1: Write Latch Enable
#define		OPTRX_SHAREDMEMORY_MASK_STATUS_REG_WRITEINPROGRESS_STATUS		0x01	// bit 0: Write in Progress status bit

// Memory Specific
#define		OPTRX_SHAREDMEMORY_4K_SUBSECTOR_SIZE					0x1000		// 4kByte Subsector size in Bytes
#define		OPTRX_SHAREDMEMORY_MAX_4K_SUBSECTORS_3B					16384		// Max. number of 4kByte subsectors accessible with 3-Byte address
#define		OPTRX_SHAREDMEMORY_MAX_4K_SUBSECTORS_4B					32768		// Max. number of 4kByte subsectors accessible with 4-Byte address, limited by 1Gbit memory size


/** OPT-RX Shared-Memory Device Class
* 	\brief Contains members for device identification and access check.
* 	Methods to generate correct message sequences for read and erase commands.
*/
class OPTRX_SHAREDMEMORY: public SEPP_SPI_DEV
{
	private:
		unsigned int Check_Memory_4Byte_Address_Mode();		//!< Check Memory Address Mode

	public:
		OPTRX_SHAREDMEMORY();
		virtual ~OPTRX_SHAREDMEMORY();

		/* public Methods for Device Configuration */
		void Connect(std::string spi_dev_name, unsigned int mode, unsigned int data_rate_in_Hz);
		void Disconnect();

		void PrintDeviceInfo();                     		//!< Print device summary / info

		unsigned int Check_Memory_Access(void);																				//!< Check if Memory Access is working
		unsigned int Read_Memory(unsigned char *data_buffer, unsigned int number_of_bytes); 										//!< Read Memory
		unsigned int Erase_Memory_4KB_SubSectors(unsigned int subsector_address, unsigned int number_of_subsectors);		//!< Erase Memory

};

/** OPT-RX Shared-Memory Exception Class
*/
class OPTRX_SHAREDMEMORY_Exception : public std::exception
{
  private:
	std::string e_;

  public:
	OPTRX_SHAREDMEMORY_Exception(std::string e) : e_(e) {}
	~OPTRX_SHAREDMEMORY_Exception() throw () {}
	const char* what() const throw() { return e_.c_str(); }
};

#endif /* OPTRX_SHAREDMEMORY_H_ */
