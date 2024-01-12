/** OPS-SAT SEPP SDR Middleware API
*
* \project  		OPS-SAT
* \Organisation  	IKS, TU Graz, Inffeldgasse 12/I, 8010 Graz
* \Author 			Reinhard Zeif, MSc
* \Date 			08.Dec.2020
*
* 
* \version      0.1		first draft															Oct.2015
* \version		0.2		EM test    															Nov.2017
* \version		0.3		Further testing and bugfixing										Dec.2017
* \version 		0.4		Receive_IQ_Sample_Data fix: 
*						BUSY flag instead of IRQ used to detect end of data transfer		29.May.2020
* \version		0.5		Description added to Receive_IQ_Samples method						08.Dec.2020
*
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
*/


#ifndef SEPP_SDR_FPGA_MW_H_
#define SEPP_SDR_FPGA_MW_H_

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

#include "SEPP_API_Global.h"
#include "SEPP_API_Debug.h"

#include "sepp_msgdma_dispatcher.h"
#include "sepp_mm_regmap.h"
#include "memcopy.h"


/* ------------------ SDR Modular SG DMA Definitions ------------------- */
// SEPP SDR mSGDMA dispatcher base address. The dispatcher is part of the FPGA and
// accessed via H2F bridge.
// The address is a global SEPP address that is defined in "SEPP_API_Global.h"
#define SEPP_SDR_FPGA_MW_MSGDMA_DISPATCHER_CSR_BASE 			(SEPP_GLOBAL_SDR_MSGDMA_DISPATCHER_CSR_BASE)
#define SEPP_SDR_FPGA_MW_MSGDMA_DISPATCHER_DESCRIPTOR_BASE 		(SEPP_GLOBAL_SDR_MSGDMA_DISPATCHER_DESCRIPTOR_BASE)

// Base, size and end HPS RAM address of region where MSGDMA block can write sample data
// The address is a global SEPP address that is defined in "SEPP_API_Global.h"
#define SEPP_SDR_FPGA_MW_MSGDMA_WRITEBUFFER_RAM_BASE			(SEPP_GLOBAL_HPS_RAM_RESERVED_REGION_SDR_MSGDMA_BASE - SEPP_GLOBAL_AXI_H2F_BRIDGE_BASE)
#define SEPP_SDR_FPGA_MW_MSGDMA_WRITEBUFFER_RAM_SIZE			(SEPP_GLOBAL_HPS_RAM_RESERVED_REGION_SDR_MSGDMA_SIZE)
#define SEPP_SDR_FPGA_MW_MSGDMA_WRITEBUFFER_RAM_END				(SEPP_GLOBAL_HPS_RAM_RESERVED_REGION_SDR_MSGDMA_END)

/* Define Sample Buffer size and number of Buffers */
#define SEPP_SDR_FPGA_MW_SAMPLE_BUFFER_MAXSIZE 				(SEPP_GLOBAL_HPS_RAM_RESERVED_REGION_SDR_MSGDMA_SIZE)

/* Parameter Error Checking */
#if (SEPP_SDR_FPGA_MW_SAMPLE_BUFFER_MAXSIZE > SEPP_SDR_FPGA_MW_MSGDMA_WRITEBUFFER_RAM_SIZE)
  #error There is not enough memory for the SEPP SDR FPGA MW parameters you have set, either reduce 'NUMBER_OF_BUFFERS' or 'MAXIMUM_BUFFER_SIZE'
#endif
/* ---------------------------------------------------------------------- */

/* -------------- SDR Avalon MM Slave RegMap Definitions ---------------- */
// SEPP SDR RegMap Base Address. RegMap is part of FPGA and accessed via H2F bridge.
// The address is a global SEPP address that is defined in "SEPP_API_Global.h"
#define SEPP_SDR_FPGA_MW_AVMMS_CSR_BASE 						(SEPP_GLOBAL_SDR_AVMMS_REGMAP_BASE)
#define SEPP_SDR_FPGA_MW_AVMMS_CSR_SIZE							(SEPP_GLOBAL_SDR_AVMMS_REGMAP_SIZE)


// RegMap 32-bit registers, offset per register has to be 4 because of 32-bit
#define SEPP_SDR_FPGA_MW_AVMMS_CSR_CONTROL_REG                             (0x00) // register 1, byte offset 4
#define SEPP_SDR_FPGA_MW_AVMMS_CSR_STATUS_REG                              (0x04) // register 0, byte offset 0

// masks for the status register bits
#define SEPP_SDR_FPGA_MW_AVMMS_CSR_STATUS_BUSY_MASK                        (1)
#define SEPP_SDR_FPGA_MW_AVMMS_CSR_STATUS_BUSY_OFFSET                      (0)
#define SEPP_SDR_FPGA_MW_AVMMS_CSR_STATUS_TBC_MASK                         (1<<1)
#define SEPP_SDR_FPGA_MW_AVMMS_CSR_STATUS_TBC_OFFSET                       (1)

// masks for the control register bits
#define SEPP_SDR_FPGA_MW_AVMMS_CSR_CONTROL_RX_ENABLE_MASK                  (1)
#define SEPP_SDR_FPGA_MW_AVMMS_CSR_CONTROL_RX_ENABLE_OFFSET                (0)
#define SEPP_SDR_FPGA_MW_AVMMS_CSR_CONTROL_CLEAR_BUFFERS_MASK              (1<<1)
#define SEPP_SDR_FPGA_MW_AVMMS_CSR_CONTROL_CLEAR_BUFFERS_OFFSET            (1)


/* ---------------------------------------------------------------------- */


/* ------------------- Structure and Enum Definitions ------------------- */
typedef struct sdr_iq_data
{
	unsigned short *i_data;
	unsigned short *q_data;
	unsigned int number_of_samples;
} SDR_FPGA_MW_I_AND_Q_DATA;

/* ---------------------------------------------------------------------- */

/** SEPP_SDR FPGA middleware (MW) device class.
*   Contains members for device address and general information. 
*   Set and Get functions for device address, description and interface type.
*   Methods to generate correct message sequences for write and read commands.
*/
class SDR_FPGA_MW
{
    private:
        SEPP_MSGDMA_DISPATCHER *sdr_dispatcher_;            // mSGDMA dispatcher FPGA core
        SEPP_MM_REGMAP *sdr_csr_;		// SDR Control and Status register


    public:
        SDR_FPGA_MW();
        virtual ~SDR_FPGA_MW();

        void Enable_Data_Streaming(void);		// enable SDR streaming, start Parallel-IO to Avalon-ST conversion
        void Disable_Data_Streaming(void);	// disable SDR streaming, stop Parallel-IO to Avalon-ST conversion

        void Clear_Receiver_Buffers(void);		// clear FPGA internal FIFO and MSGDMA buffers

        /* ---------- FPGA specific RegMap methods ------------- */

        void Receive_IQ_Sample_Data(unsigned int *samples, unsigned int number_of_samples, unsigned int timeout = 10);
        SDR_FPGA_MW_I_AND_Q_DATA Split_IQ_Samples_Into_I_and_Q(unsigned int *iq_data, unsigned int number_of_samples);

        void PrintInfo(void);

};

class SDR_FPGA_MW_Exception : public std::exception
{
  private:
	std::string e_;

  public:
	SDR_FPGA_MW_Exception(std::string e) : e_(e) {}
	~SDR_FPGA_MW_Exception() throw () {}
	const char* what() const throw() { return e_.c_str(); }
};

#endif // SEPP_SDR_FPGA_MW_H_

