/** LMS6002 Device Library - Parameters
 *
 * \file LMS_Parameter.h
 *
 * \project    OPS-SAT Project
 * \copyright  Graz University of Technology, IKS
 * \author     Reinhard Zeif, Fabian Golser
 * \date       15. Nov. 2017
 * \version    1.0
 *
 * \note Codebase provided by Lime Microsystems (www.limemicro.com)
 *
 * \brief Enumerations for all chip modules and all register parameters.
 * A parameter can consist of one ore more registers and a defined number
 * of bit segments.
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

#ifndef LMS_REGMAP_PARAMETER_H
#define LMS_REGMAP_PARAMETER_H

// #include <string>
#include <string.h>

#include "LMS_Definitions.h"
#include "SEPP_API_Debug.h"

/** LMS6002 Register Piece Structure
 * \brief LMS6002 Registers consist of one or more register pieces. 
 * The piece is represented by a bit range of a register with a defined address.
 */
struct LMS_RegPiece
{
  unsigned int addr;
  unsigned int msb;
  unsigned int lsb;
};

/** LMS6002 Register-Map Parameter Class
 * \brief LMS6002 Register Map Parameter Class used to read/write parameters that are located in one or more registers 
 */
class LMS_RegMap_Parameter
{
  public:
	LMS_RegMap_Parameter();
	LMS_RegMap_Parameter(unsigned msb, unsigned lsb, unsigned long addr, unsigned long defVal, const char* nameID);
	LMS_RegMap_Parameter(unsigned msb, unsigned lsb, unsigned long addr, unsigned long defVal, const char* nameID, int extremeValue);
	LMS_RegMap_Parameter(unsigned msb, unsigned lsb, unsigned long addr, unsigned long defVal, const char* nameID, int extremeValue, bool do_not_throw);
	LMS_RegMap_Parameter(const LMS_RegPiece *MSBtoLSB, unsigned pCount, unsigned long defVal, const char* nameID);
	LMS_RegMap_Parameter(const LMS_RegPiece *MSBtoLSB, unsigned pCount, unsigned long defVal, const char* nameID, int extremeValue);
	LMS_RegMap_Parameter(const LMS_RegPiece *MSBtoLSB, unsigned pCount, unsigned long defVal, const char* nameID, int extremeValue, bool do_not_throw);
	LMS_RegMap_Parameter(const LMS_RegMap_Parameter &p2);

    virtual ~LMS_RegMap_Parameter();

    LMS_RegMap_Parameter &operator=(const LMS_RegMap_Parameter &p2);

    void checkValueRange(const unsigned long value, bool do_not_throw = false);

  // private:

    std::string name_;
    unsigned char bitsCount_;
    unsigned char piecesCount_;
    unsigned long defaultValue_;

    // Register segments/pieces used by the parameter
    LMS_RegPiece *register_pieces; //first index lsb, further msb

    /**
     * \brief extremeValue_ if positive defines the maximum allowed value, if
     * negative the minimum allowed value. Default value is 0xFF, which is the
     * maximum possible value of a 8-bit register anyway.
     * If extremeValue_ is zero, the parameter is readonly and can not be
     * written. This is considered in the SetParameterValue() method.
     * If a parameter consists of more than one registers and the extremeValue_
     * is 0xFF it will not be considered. Only bitsCount checks count than.
     * If the range of bitsCount is narrower this value an exception is thrown.
     */
    int extremeValue_;

    bool do_not_throw_;

};


#endif // LMS_REGMAP_PARAMETER_H


