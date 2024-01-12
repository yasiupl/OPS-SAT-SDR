/** LMS6002 Device Library - Config File
 *
 * \file LMS_ConfigFile.h
 *
 * \project    OPS-SAT Project
 * \copyright  Graz University of Technology, IKS
 * \author     Reinhard Zeif, Fabian Golser
 * \date       15. Nov. 2017
 * \version    1.0
 *
 * \brief This class is used to restore register values from or
 * store register values to LMS chip configuration files.
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

#ifndef LMS_CONFIGFILE_H
#define LMS_CONFIGFILE_H


#include <iostream>
#include <fstream>
#include <string>

#include "LMS_Definitions.h"
#include "SEPP_API_Debug.h"

/** LMS6002 Config File Class
 * \brief The LMS configuration file class used to store/recall device configurations
 */
class LMS_ConfigFile
{
	public:
		LMS_ConfigFile();
		virtual ~LMS_ConfigFile();

		const static bool checkFileExistence(const char* filename);			//!< check if file exists
		void open(const char *filename = "sepp_sdr_config_file.bin");	//!< open config file
		void close(void);												//!< close config file

		void write(char *values, unsigned int number_of_bytes, unsigned long address);			//!< write specific value from config file
		void read(char *values, unsigned int number_of_bytes, unsigned long address);  				//!< read specific value from config file

		const char* getFilename();										//!< get config file name

	private:
		std::fstream file_;

		const char* file_name_;		//!< config file name
		unsigned file_size_;  		//!< number of registers/bytes used in config file
		//bool binary_;				//!< enable/disable usage of binary file format
};

/** LMS6002 Configuration File Exception Class
* \brief This exception type is returned in case of an exception in the LMS ConfigFile class.
*/
class LMS_CONFIGFILE_Exception : public std::exception
{
  private:
	std::string e_;

  public:
	LMS_CONFIGFILE_Exception(std::string e) : e_(e) {}
	~LMS_CONFIGFILE_Exception() throw () {}
	const char* what() const throw() { return e_.c_str(); }
};


#endif // LMS_CONFIGFILE_H
