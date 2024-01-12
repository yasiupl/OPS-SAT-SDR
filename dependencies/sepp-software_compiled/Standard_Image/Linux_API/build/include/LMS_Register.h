/** LMS6002 Device Library - Registers
 *
 * \file LMS_Register.h
 *
 * \project    OPS-SAT Project
 * \copyright  Graz University of Technology, IKS
 * \author     Reinhard Zeif, Fabian Golser
 * \date       15. Nov. 2017
 * \version    1.0
 *
 * \note Codebase provided by Lime Microsystems (www.limemicro.com)
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

#ifndef LMS_REGMAP_REGISTER_H_
#define LMS_REGMAP_REGISTER_H_

#include "LMS_Definitions.h"
#include "SEPP_API_Debug.h"

/** LMS6002 Register-Map Register Class
 * \brief LMS6002 Device Register used to control and monitor the device
 */
class LMS_RegMap_Register
{
  public:

	LMS_RegMap_Register();
	LMS_RegMap_Register(unsigned char val, unsigned char defVal, unsigned char mask);
	LMS_RegMap_Register(const LMS_RegMap_Register &p2);

    virtual ~LMS_RegMap_Register();

    LMS_RegMap_Register &operator=(const LMS_RegMap_Register &p2);

    unsigned char value;
    unsigned char defaultValue;
    unsigned char mask;
};



#endif /* LMS_REGMAP_REGISTER_H_ */
