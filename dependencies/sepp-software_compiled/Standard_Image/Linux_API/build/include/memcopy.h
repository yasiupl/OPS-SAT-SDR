/** OPS-SAT SEPP SDR Middleware API - Memory Access
*
* \project  		OPS-SAT
* \Organisation  	IKS, TU Graz, Inffeldgasse 12/I, 8010 Graz
* \Author 			Reinhard Zeif, MSc
* \Date 			31.May.2020
*
*
* \version      0.1		first draft									Oct.2015
* \version		0.2		EM test    									Nov.2017
* \version		0.3		Further testing and bugfixing				Dec.2017
* \version		0.4		return added if memory is not accessible	31.May.2020
*
*
* (C) 2015-2020 Graz University of Technology / IKS. All rights reserved.
* Your use of TU Graz design tools, logic functions and other 
* software and tools and any output files any of the foregoing 
* (including device programming or simulation files), and any associated 
* documentation or information are expressly subject to the terms and 
* conditions of the applicable license agreement, including, without 
* limitation, that your use is for the sole purpose of programming the 
* OPS-SAT processing platform.
*
*/

#ifndef MEMCOPY   /* Include guard */
#define MEMCOPY

// Filehandler header

using namespace std;

#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "SEPP_API_Debug.h"

int memwrite(unsigned int* buffer, unsigned int address, unsigned int size_in_bytes);

int memread(unsigned int* buffer, unsigned int address, unsigned int size_in_bytes);

#endif
