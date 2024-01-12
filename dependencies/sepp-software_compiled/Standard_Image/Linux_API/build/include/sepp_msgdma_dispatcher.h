/** OPS-SAT SEPP C++ API CORE Library - SEPP Altera MSGDMA Dispatcher Library
 *
 * \file sepp_msgdma_dispatcher.h
 *
 * \brief Generic Class for SEPP memory mapped registers
 * The memory mapping can be used to access memory mapped registers in the FPGA portion of the SEPP SoC.
 *
 * \project  		OPS-SAT SEPP C++ API Library
 *
 * \organisation IKS, TU Graz, Inffeldgasse 12/I, 8010 Graz
 * \author 			Reinhard Zeif, MSc
 * \date			19.Oct.2020
 *
 * \version 		0.1		first draft												Oct.2015
 * \version 		0.2		EM test													Nov.2017
 * \version 		0.3		Further testing and bugfixing							Dec. 2017
 * \version			0.4		Debug output added 										Jul.2019
 * \version			0.5		METHODEND debug output fixed in Get_Status method		29.May.2020
 * \version			1.0		Typos fixed, doxygen docu added							19.Oct.2020
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


#ifndef SEPP_MSGDMA_DISPATCHER_H_
#define SEPP_MSGDMA_DISPATCHER_H_

#include <iostream>
#include <string.h>
#include <iomanip>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <math.h>

#include "altera/csr_regs.h"
#include "altera/descriptor_regs.h"
#include "altera/response_regs.h"

#include "sepp_mm_regmap.h"
#include "SEPP_API_Debug.h"


/* ------------ Definitions ------------- */
//#define 	HEX8(x) 			"0x" << std::internal << std::setfill('0') << std::setw(8) << std::hex << (x) << std::dec << std::setw(0) << std::setfill(' ') << ""

#define STANDARD_DESCRIPTOR_SIZE 		16
#define EXTENDED_DESCRIPTOR_SIZE 		32
#define RESPONSE_SIZE 					8

#define msgdma_standard_descriptor_packed __attribute__ ((packed, aligned(STANDARD_DESCRIPTOR_SIZE)))
#define msgdma_extended_descriptor_packed __attribute__ ((packed, aligned(EXTENDED_DESCRIPTOR_SIZE)))
#define msgdma_response_packed __attribute__ ((packed, aligned(RESPONSE_SIZE)))


/* ------------ Structures ------------- */

// use this structure if you haven't enabled the enhanced features
typedef struct {
  unsigned int read_address;
  unsigned int write_address;
  unsigned int transfer_length;
  unsigned int control;
} msgdma_standard_descriptor_packed msgdma_standard_descriptor;


// use ths structure if you have enabled the enhanced features (only the elements enabled in hardware will be used)
typedef struct {
  unsigned int read_address_low;
  unsigned int write_address_low;
  unsigned int transfer_length;
  unsigned short sequence_number;
  unsigned char  read_burst_count;
  unsigned char  write_burst_count;
  unsigned short read_stride;
  unsigned short write_stride;
  unsigned int read_address_high;
  unsigned int write_address_high;
  unsigned int control;
} msgdma_extended_descriptor_packed msgdma_extended_descriptor;


// this struct should only be used if response information is enabled
typedef struct {
  unsigned int actual_bytes_transferred;
  unsigned char error;
  unsigned char early_termination;
} msgdma_response_packed msgdma_response;


/* ------------ Enumerators ------------- */
enum SEPP_MSGDMA_DESCRIPTOR_TYPE
{
	ST_TO_MM = 0,   // for a msgdma writer in FPGA that writes to DDR RAM
	MM_TO_ST = 1,	// for a msgdma reader in FPGA that reads from DDR RAM
	MM_TO_MM = 2	// for a msgdma reader and msgdma writer both in FPGA that read and write from and to DDR RAM
};


/** SEPP_MSGDMA_DISPATCHER altera modular scatter-gather DMA device class.
*   Contains members for device address and general information. 
*   Set and Get functions for device address, description and interface type.
*   Methods to generate correct message sequences for write and read commands.
*/
class SEPP_MSGDMA_DISPATCHER
{
    private:
		std::string description_;			//!< Description of the device, e.g. device name and purpose
		std::string interface_type_;		//!< Hardware interface type used for communication, e.g. I2C or SPI
		unsigned int interface_status_;   	//!< status of interface: 1 = connected, 0 = disconnected

		unsigned int csr_base_address_;				// page aligned base address for Dispatcher CSR port registers
		unsigned int descriptor_base_address_;		// page aligned base address for Dispatcher Descriptor port registers
		unsigned int response_base_address_;		// page aligned base address fore Dispatcher Response port registers
		bool mm_response_port_is_used_;				// flag if memory-mapped response port is used: true = used, false = not used

		/* ------ Memory-Mapped Register Access --------- */
		SEPP_MM_REGMAP *csr_port_;
		SEPP_MM_REGMAP *descriptor_port_;
		SEPP_MM_REGMAP *response_port_;

	
    public:
        SEPP_MSGDMA_DISPATCHER(unsigned int csr_base, unsigned int descriptor_base);
        SEPP_MSGDMA_DISPATCHER(unsigned int csr_base, unsigned int descriptor_base, unsigned int response_base);
        virtual ~SEPP_MSGDMA_DISPATCHER();
 
        void PrintInfo(void);

        // Construct a mSGDMA dispatcher standard or extended descriptor
        msgdma_standard_descriptor Construct_Std_ST_to_MM_Descriptor(unsigned int write_address, unsigned int length, unsigned int control);
        msgdma_standard_descriptor Construct_Std_MM_to_ST_Descriptor(unsigned int read_address, unsigned int length, unsigned int control);
        msgdma_standard_descriptor Construct_Std_MM_to_MM_Descriptor(unsigned int read_address, unsigned int write_address, unsigned int length, unsigned int control);
        msgdma_extended_descriptor Construct_Ext_ST_to_MM_Descriptor(unsigned int write_address, unsigned int length, unsigned int control, unsigned short sequence_number, unsigned char write_burst_count, unsigned short write_stride);
        msgdma_extended_descriptor Construct_Ext_MM_to_ST_Descriptor(unsigned int read_address, unsigned int length, unsigned int control, unsigned short sequence_number, unsigned char read_burst_count, unsigned short read_stride);
        msgdma_extended_descriptor Construct_Ext_MM_to_MM_Descriptor(unsigned int read_address, unsigned int write_address, unsigned int length, unsigned int control, unsigned short sequence_number, unsigned char read_burst_count, unsigned char write_burst_count, unsigned short read_stride, unsigned short write_stride);

        // Write descriptor to dispatcher
        void Write_Std_Descriptor_to_Dispatcher(msgdma_standard_descriptor descriptor);
        void Write_Ext_Descriptor_to_Dispatcher(msgdma_extended_descriptor descriptor);

        // response reading function
        msgdma_response Read_MM_Response(void);

        /* ------ Methods for Dispatcher Control and Status Registers (CSR) ------- */
        unsigned int Get_Status(void);								// Read CSR status register value
        void Set_Status(unsigned int value);						// Write CSR status register value
        unsigned int Get_Control(void);								// Read CSR control register value
        void Set_Control(unsigned int value);						// Write CSR status register value
        unsigned int Get_Read_Descriptor_Buffer_Fill_Level(void);
        unsigned int Get_Write_Descriptor_Buffer_Fill_Level(void);
        unsigned int Get_Response_Buffer_Fill_Level(void);
        unsigned int Get_Read_Sequence_Number(void);
        unsigned int Get_Write_Sequence_Number(void);

        /* ------------------- CSR Status Register Flag Methods  ------------------ */
        unsigned int Get_Busy_Flag(void);							// Read CSR status register busy flag
        unsigned int Get_Descriptor_Buffer_Empty_Flag(void);		// Read CSR status register descriptor buffer empty flag
        unsigned int Get_Descriptor_Buffer_Full_Flag(void);			// Read CSR status register descriptor buffer full flag
        unsigned int Get_Response_Buffer_Empty_Flag(void);			// Read CSR status register response buffer empty flag
        unsigned int Get_Response_Buffer_Full_Flag(void);			// Read CSR status register response buffer full flag
        unsigned int Get_Stopped_Flag(void);						// Read CSR status register stopped flag
        unsigned int Get_Resetting_Flag(void);						// Read CSR status register resetting flag
        unsigned int Get_Stopped_On_Error_Flag(void);				// Read CSR status register stopped on error flag
        unsigned int Get_Stopped_On_Early_Termination_Flag(void);	// Read CSR status register stopped on early termination flag

        unsigned int Get_IRQ_Flag(void);							// Read CSR status register interrupt request flag
        void Clear_IRQ_Flag(void);									// Clear CSR status register interrupt request flag

        /* ---------------------- CSR Control Register Methods -------------------- */
        void Stop_Dispatcher(void);									// Set control stop flag to stop dispatcher
        void Start_Dispatcher(void);								// Clear control stop flag to start dispatcher
        void Reset_Dispatcher(void);
        void Enable_Stop_On_Error(void);
        void Disable_Stop_On_Error(void);
        void Enable_Stop_on_Early_Termination(void);
        void Disable_Stop_On_Early_termination(void);
        void Enable_Global_Interrupt_Mask(void);
        void Disable_Global_Interrupt_Mask(void);
        void Stop_Descriptors(void);
        void Start_Descriptors(void);
 
};

class SEPP_MSGDMA_DISPATCHER_Exception : public std::exception
{
  private:
	std::string e_;

  public:
	SEPP_MSGDMA_DISPATCHER_Exception(std::string e) : e_(e) {}
	~SEPP_MSGDMA_DISPATCHER_Exception() throw () {}
	const char* what() const throw() { return e_.c_str(); }
};

#endif // SEPP_MSGDMA_DISPATCHER_H

