/** OPS-SAT SEPP C++ API EXT Library - INI Parser Dictionary
 *
 * \file sepp_iniparser_dictionary.h
 *
 * \project  		OPS-SAT SEPP C++ API Library
 *
 * \organisation 	IKS, TU Graz, Inffeldgasse 12/I, 8010 Graz
 * \author 			Reinhard Zeif, MSc
 * \date 			29.Aug.2020
 *
 * version history
 * \version		0.1		first draft  												01.Sep.2020
 *
 *
 * Code reference: This C/C++ library is partially based on the C-code "iniparser"
 * library from N. Devillard (MIT licence).
 * This module implements a simple list of string/string associations. This
 * object is useful to store informations retrieved from a configuration file.
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

#ifndef SEPP_INIPARSER_DICTIONARY_H_
#define SEPP_INIPARSER_DICTIONARY_H_

/*---------------------------- Includes ------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>


/*---------------------------- Defines -------------------------------------*/
/** Maximum value size for integers and doubles. */
#define SEPP_INIPARSER_MAXVALSZ    			1024

/** Minimal allocated number of entries in a sepp_iniparser_dictionary */
#define SEPP_INIPARSER_DICTMINSZ   			128

/** Invalid key token */
#define SEPP_INIPARSER_DICT_INVALID_KEY    ((char*)-1)

/** Dictionary entry */
typedef struct
{
	std::string value ;   	/** string value */
	std::string key ;   	/** string key */
	unsigned int hash ;  	/** hash value for key */
} SEPP_INIPARSER_DICT_ENTRY;


/** SEPP C++ API EXT Library - SEPP INI Parser Dictionary
 * This object contains a list of string/string associations. Each
 * association is identified by a unique string key. Looking up values
 * in the sepp_iniparser_dictionary is speeded up by the use of a (hopefully collision-free)
 * hash function.
 */
class SEPP_INIPARSER_DICTIONARY
{
	private:
		std::vector<SEPP_INIPARSER_DICT_ENTRY> entries_;

		unsigned calc_hash(std::string key);

	public:
		SEPP_INIPARSER_DICTIONARY();
		virtual ~SEPP_INIPARSER_DICTIONARY();

		SEPP_INIPARSER_DICT_ENTRY get_entry(unsigned int index);
		int unset_entry(std::string key);
		int get_value(std::string key, std::string& val);
		int set_value(std::string key, std::string val);
		int delete_all();
		int dump(FILE * out);
		int size();
};

/** SEPP C++ API CORE Library - SEPP I2C Device Exception
*/
class SEPP_INIPARSER_DICTIONARY_Exception : public std::exception
{
	private:
		std::string e_;

	public:
		SEPP_INIPARSER_DICTIONARY_Exception(std::string e) : e_(e) {}
		~SEPP_INIPARSER_DICTIONARY_Exception() throw () {}
		const char* what() const throw() { return e_.c_str(); }
};

#endif
