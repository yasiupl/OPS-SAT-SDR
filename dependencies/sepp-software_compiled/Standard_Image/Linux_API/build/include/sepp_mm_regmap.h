/** OPS-SAT SEPP C++ API CORE Library - SEPP memory-mapped register map
 *
 * \file SEPP_MM_RegMap.h
 *
 * \brief Generic Class for SEPP memory-mapped register map.
 *
 * \project  		OPS-SAT SEPP C++ API Library
 *
 * \organisation IKS, TU Graz, Inffeldgasse 12/I, 8010 Graz
 * \author 			Reinhard Zeif, MSc
 * \Date 			18. Dec 2017
 *
 * \version	 	0.1  first draft						Oct.2015
 * \version		0.2	 EM test    						Nov.2017
 * \version		0.3	 Further testing and bugfixing		Dec. 2017
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
 
 
#ifndef SEPP_MM_REGMAP_H
#define SEPP_MM_REGMAP_H

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

#include "SEPP_API_Debug.h"


/* SEPP Memory-Mapped Register Map Class
 * The class is used to map external physical IO registers to the virtual
 * address space.
 */
class SEPP_MM_REGMAP
{
  public:
	  SEPP_MM_REGMAP(unsigned int base_address, unsigned int size_in_bytes);
    virtual ~SEPP_MM_REGMAP(void);
    
    void PrintInfo(void);
    
    unsigned int Read32(unsigned int offset_in_bytes);					//<! Read 32-bit register
    unsigned short Read16(unsigned short offset_in_bytes);					//<! Read 16-bit register
    unsigned char Read8(unsigned char offset_in_bytes);					//<! Read 8-bit register

    void Write32(unsigned int offset_in_bytes, unsigned int value);		//<! Write 32-bit register
    void Write16(unsigned short offset_in_bytes, unsigned short value);		//<! Write 16-bit register
    void Write8(unsigned char offset_in_bytes, unsigned char value);		//<! Write 16-bit register
    
  private:
    unsigned int base_address_;
    unsigned int size_in_bytes_;

    void *pMMRegMap_;  //!< memory mapped pointer to registers
    
    size_t page_size_; 
    int fd_;
};


class SEPP_MM_REGMAP_Exception : public std::exception
{
  private:
	std::string e_;
	
  public:
	SEPP_MM_REGMAP_Exception(std::string e) : e_(e) {}
	~SEPP_MM_REGMAP_Exception() throw () {}
	const char* what() const throw() { return e_.c_str(); }
};


#endif
