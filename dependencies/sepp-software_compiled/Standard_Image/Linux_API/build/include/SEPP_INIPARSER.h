/** OPS-SAT SEPP C++ API EXT Library - INI Parser
 *
 * \file SEPP_INIPARSER.h
 *
 * \project  		OPS-SAT SEPP C++ API Library
 *
 * \organisation 	IKS, TU Graz, Inffeldgasse 12/I, 8010 Graz
 * \author 			Reinhard Zeif, MSc
 * \date 			08.Dec.2020
 *
 * version history
 * \version		0.1		first draft  												01.Sep.2020
 * \version		0.2		parse_line method reworked with C++ push_back() and back()
 * 						methods instead [idx]. Multi-Line value parsing fixed		12.Sep.2020
 * \version		0.3		get_value method for unsigned char parameters added			12.Nov.2020
 * \version		0.4		get_value method for comma-seperated vector parameters added
 * 						to allow implementation of config loops						08.Dec.2020
 *
 *
 * Code reference: This C/C++ library is partially based on the C-code "iniparser"
 * library from N. Devillard (MIT licence).
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

#ifndef SEPP_INIPARSER_H_
#define SEPP_INIPARSER_H_

/*---------------------------------------------------------------------------
                                Includes
 ---------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <stdarg.h>
#include <iostream>
#include <fstream>

#include "sepp_iniparser_dictionary.h"


/**
 * This enum stores the status for each parsed line (internal use only).
 */
typedef enum {
    LINE_UNPROCESSED,
    LINE_ERROR,
    LINE_EMPTY,
    LINE_COMMENT,
    LINE_SECTION,
    LINE_VALUE
} SEPP_INIPARSER_LINE_STATUS;

/*
 * The following #include is necessary on many Unixes but not Linux.
 * It is not needed for Windows platforms.
 * Uncomment it if needed.
 */
/* #include <unistd.h> */

/*---------------------------- Defines -------------------------------------*/
#define SEPP_INIPARSER_ASCIILINESZ         (1024)
#define SEPP_INIPARSER_INI_INVALID_KEY     ((char*)-1)

/*--------------------------------------------------------------------------*/

class SEPP_INIPARSER
{
	private:
		SEPP_INIPARSER_DICTIONARY *dict_;

		std::string strlwc(std::string in);
		std::string strstrip(std::string in);
		SEPP_INIPARSER_LINE_STATUS parse_line(std::string input_line, std::string &section, std::string &key, std::string &value);

	public:
		SEPP_INIPARSER();
		virtual ~SEPP_INIPARSER();

		/* --------- Load and Free INI dictionary ---------- */
		int load_dict(const char * ini_filename);
		void free_dict();

		/* -------------- Dump dictionary data -------------- */
		void dump(FILE * f);
		void dump_ini(FILE * f);
		void dumpsection_ini(std::string s, FILE * f);

		/* ------------- get section info/data -------------- */
		std::string get_section_name(int n);
		int get_number_of_sections();
		int get_number_of_section_keys(std::string s);
		//const char ** getseckeys(const char * s, const char ** keys);

		/* ----------- get value from INI file -------------- */
		int get_value(std::string key, std::string &value);
		int get_value(std::string key, int *value);
		int get_value(std::string key, unsigned int *value);
		int get_value(std::string key, long int *value);
		int get_value(std::string key, unsigned long long *value);
		int get_value(std::string key, float *value);
		int get_value(std::string key, double *value);
		int get_value(std::string key, bool *value);
		int get_value(std::string key, unsigned char *value);
		int get_value(std::string key, std::vector<unsigned int> *value);
		int get_value(std::string key, std::vector<double> *value);

		/* --------- methods for key-value entries ---------- */
		int find_entry(std::string key);
		int set_entry(std::string key, std::string val);
		int unset_entry(std::string key);

};

class SEPP_INIPARSER_Exception : public std::exception
{
	private:
		std::string e_;

	public:
		SEPP_INIPARSER_Exception(std::string e) : e_(e) {}
		~SEPP_INIPARSER_Exception() throw () {}
		const char* what() const throw() { return e_.c_str(); }
};


#endif
