/** OPS-SAT SEPP C++ API Library - Debug Functionality
 *  
 * \file SEPP_API_Debug.h
 *
 * SEPP API library for debugging.
 * The library contains some defintions, macros and methods for source code debugging.
 * 
 * \project 		OPS-SAT SEPP C++ API Library
 *
 * \copyright		Graz University of Technology
 * \date 		16. October 2017
 * \author 		Reinhard Zeif, MSc (RZ), Fabian Golser, Max Henkel
 *
 * \version  		0.1		Intial release					16. Oct. 2017
 * \version		1.0		Hex code extension, some bubfixes		25. Jan. 2018
 * \version		1.1		Debug output configurable at runtime		19. Nov. 2021
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
 **/




#ifndef SEPP_API_DEBUG_H
#define SEPP_API_DEBUG_H

#include <typeinfo>
#include <iostream>
#include <iomanip>
#include <bitset>

/** ------------------------ DEFINITION TEMPLATE FOR API's ------------------------
*
* The following lines should be used in the API libraries to define the DEBUG LEVEL
* SEPP_API_DEBUGGING sub-system (uncomment to enable debug output)
*
*
* // 1. Global DEBUGGING on/off switch
* ifndef ENABLE_SEPP_API_DEBUGGING
* 	#define 	ENABLE_SEPP_API_DEBUGGING		 			// enable global debugging
* #endif
* #ifndef ENABLE_SEPP_API_DEBUG_COLORS
*	#define 	ENABLE_SEPP_API_DEBUG_COLORS				// enable debug text colors
* #endif
*
* // 2. Global debug level on/off switches
* #ifdef ENABLE_SEPP_API_DEBUGGING
*	#define 	USEERROR            		// enable errors
*	#define 	USEWARNING          		// enable warnings
* 	#define 	USEDEBUG            		// enable debug general
*	#define 	USEDEBUGCONSTRUCTOR 		// enable debug constructors
* 	#define 	USEDEBUGMETHOD      		// enable debug methods
*	#define	USEDEBUGREGISTER    			// enable debug write/read register
*	#define 	USEINDENTATION				// enable debug indentation
*	#define 	USEINDENTATIONLINES			// enable debug indentation with corresponding lines
* #endif
*
* -------------------------------------------------------------------------------- */


/*---------------- global DEBUGGING on/off switch -----------------------------*/

#ifndef ENABLE_SEPP_API_DEBUGGING
	#define 	ENABLE_SEPP_API_DEBUGGING		 			//!< enable global debugging
#endif
#ifndef ENABLE_SEPP_API_DEBUG_COLORS
	#define 	ENABLE_SEPP_API_DEBUG_COLORS				//!< enable debug text colors
#endif

// DEBUGGING sub-system (uncomment to enable debug output)
#ifdef ENABLE_SEPP_API_DEBUGGING
	#ifndef SEPP_API_DEBUG_USE_ERROR
		#define 	SEPP_API_DEBUG_USE_ERROR            		//!< enable errors
	#endif
	#ifndef SEPP_API_DEBUG_USE_WARNING
		#define 	SEPP_API_DEBUG_USE_WARNING          		//!< enable warnings
	#endif
	#ifndef SEPP_API_DEBUG_USE_DEBUG
		#define 	SEPP_API_DEBUG_USE_DEBUG            		//!< enable debug general
	#endif
	#ifndef SEPP_API_DEBUG_USE_DEBUGCONSTRUCTOR
		#define 	SEPP_API_DEBUG_USE_DEBUGCONSTRUCTOR 		//!< enable debug constructors
	#endif
	#ifndef SEPP_API_DEBUG_USE_DEBUGMETHOD
		//#define 	SEPP_API_DEBUG_USE_DEBUGMETHOD      	//!< enable debug methods
	#endif
	#ifndef SEPP_API_DEBUG_USE_DEBUGREGISTER
		//#define	SEPP_API_DEBUG_USE_DEBUGREGISTER    		//!< enable debug write/read register
	#endif
	#ifndef SEPP_API_DEBUG_USE_INDENTATION
		#define	SEPP_API_DEBUG_USE_INDENTATION					//!< enable debug indentation
	#endif
	#ifndef SEPP_API_DEBUG_USE_INDENTATIONLINES
		#define	SEPP_API_DEBUG_USE_INDENTATIONLINES			//!< enable debug indentation with corresponding lines
	#endif
#endif

/*-----------------------------------------------------------------------------*/


// hex representation for debug output
#ifndef HEX
	#define 	HEX(x) 			"0x" << std::internal << std::setfill('0') << std::setw(2) << std::hex << (x) << std::dec << std::setw(0) << std::setfill(' ') << ""  //!< Print 8-bit HEX value with 0x plus 2 characters
#endif
#ifndef HEX2
	#define 	HEX2(x) 		(HEX(x))  //!< Print 8-bit HEX value with 0x plus 2 characters
#endif
#ifndef HEX4
	#define 	HEX4(x) 		"0x" << std::internal << std::setfill('0') << std::setw(4) << std::hex << (x) << std::dec << std::setw(0) << std::setfill(' ') << ""  //!< Print 16-bit HEX value with 0x plus 4 characters
#endif
#ifndef HEX8
	#define 	HEX8(x) 		"0x" << std::internal << std::setfill('0') << std::setw(8) << std::hex << (x) << std::dec << std::setw(0) << std::setfill(' ') << ""  //!< Print 32-bit HEX value with 0x plus 8 characters
#endif
#ifndef HEX16
	#define 	HEX16(x) 		"0x" << std::internal << std::setfill('0') << std::setw(16) << std::hex << (x) << std::dec << std::setw(0) << std::setfill(' ') << ""  //!< Print HEX 64-bit value with 0x plus 16 characters
#endif

//!< Debug output indention
#ifdef SEPP_API_DEBUG_USE_INDENTATION
  #define INDENT_INC SEPP_API_Debug::incrementIndent();		//!< Increment Indentation
  #define INDENT_DEC SEPP_API_Debug::decrementIndent();		//!< Decrement Indentation
#else
  #define INDENT_INC                                   //!< Use Indentation incremenation
  #define INDENT_DEC                                   //!< Use Indentation decrementation
#endif

#define DOUT(x) SEPP_API_Debug::dout(x)

//!< Debug output colors
#ifdef ENABLE_SEPP_API_DEBUG_COLORS
  // all the colors
  #define BLACK   		30				//!< color code for black
  #define RED     		31				//!< color code for red
  #define GREEN   		32				//!< color code for green
  #define YELLOW  		33				//!< color code for yellow
  #define BLUE    		34				//!< color code for blue
  #define MAGENTA 		35				//!< color code for magenta
  #define CYAN    		36				//!< color code for cyan
  #define WHITE   		37				//!< color code for white
  // background colors
  #define BG_BLACK   	40				//!< color code for background black
  #define BG_RED     	41				//!< color code for background red
  #define BG_GREEN   	42				//!< color code for background green
  #define BG_YELLOW  	43				//!< color code for background yellow
  #define BG_BLUE    	44				//!< color code for background blue
  #define BG_MAGENTA 	45				//!< color code for background magenta
  #define BG_CYAN    	46				//!< color code for background cyan
  #define BG_WHITE   	47				//!< color code for background white
  // general color strings
  #define COLOR_PRE(x) "\033[" << x << "m"	//!< Linux Color Code Prefix
  #define COLOR_POST "\033[0m"							//!< Linux Color Code Postfix
#else
  #define COLOR_PRE(x)  ""
  #define COLOR_POST 	""
#endif

//!< Debug error message output
#define 	ERROR 							(std::string)"\n '-> File:      " + __FILE__ + "\n '-> Function:  " + __PRETTY_FUNCTION__

//!< error message output functionality
#ifdef SEPP_API_DEBUG_USE_ERROR
  #define ERRORMSG(x) do { DOUT(SEPP_API_Debug::Error) << COLOR_PRE(RED) << std::dec << x << COLOR_POST << \
    std::endl; } while (0)
#else
  #define ERRORMSG(x)
#endif

//!< warning message output functionality
#ifdef SEPP_API_DEBUG_USE_WARNING
  #define WARNING(x) do { DOUT(SEPP_API_Debug::Warning) << COLOR_PRE(YELLOW) << std::dec << \
    "WARNING: " << x << COLOR_POST << std::endl; }  while (0)
#else
  #define WARNING(x)
#endif

//!< debugging functionality general
#ifdef SEPP_API_DEBUG_USE_DEBUG
  #define DEBUG(x) do { DOUT(SEPP_API_Debug::Debug) << COLOR_PRE(GREEN) << std::dec << x << COLOR_POST << \
    std::endl; } while (0)
  #define DEBUG_NOLN(x) do { DOUT(SEPP_API_Debug::Debug) << COLOR_PRE(GREEN) << std::dec << x << COLOR_POST; \
   } while (0)
#else
  #define DEBUG(x)
  #define DEBUG_NOLN(x)
#endif

//!< debugging functionality constructors and destructors
#ifdef SEPP_API_DEBUG_USE_DEBUGCONSTRUCTOR
  #define CONSTRUCTOR(x) do { DOUT(SEPP_API_Debug::Constructor) << COLOR_PRE(CYAN) << std::dec << " --> " << \
    typeid(this).name() << " constructor: " << x << COLOR_POST << std::endl; } while (0)
  #define DESTRUCTOR(x) do { DOUT(SEPP_API_Debug::Destructor) << COLOR_PRE(CYAN) << std::dec << " <-- " << \
    typeid(this).name() << " destructor: " << x << COLOR_POST << std::endl; } while (0)
#else
  #define CONSTRUCTOR(x)
  #define DESTRUCTOR(x)
#endif

//!< debugging functionality start/end method
#ifdef SEPP_API_DEBUG_USE_DEBUGMETHOD
  #define METHOD(x) do { DOUT(SEPP_API_Debug::Method) << COLOR_PRE(MAGENTA) << std::dec << __PRETTY_FUNCTION__ \
    << " " << x << COLOR_POST << std::endl; INDENT_INC } while (0)
  #define METHODEND(x) do { INDENT_DEC DOUT(SEPP_API_Debug::Method) << COLOR_PRE(MAGENTA) << std::dec << __PRETTY_FUNCTION__\
    << " ENDS " << x << COLOR_POST << std::endl; } while (0)
#else
  #define METHOD(x)
  #define METHODEND(x)
#endif

//!< debugging functionality write/read register
#ifdef SEPP_API_DEBUG_USE_DEBUGREGISTER
  #define REGISTER(x) do { DOUT(SEPP_API_Debug::Register) << std::dec << x << std::endl; } while (0)
#else
  #define REGISTER(x)
#endif



/** SEPP C++ API Library - Debug Class
*/
class SEPP_API_Debug
{
  public:
    enum Level  { Method, Constructor, Destructor, Debug, Warning, Error, Register};
    enum Indent { IndentNone, IndentNormal, IndentWithLines };

    static void setIndent(enum Indent indent);
    static enum Indent getIndent();
    static void printIndent();
    static void printIndentWithLines();
    static void incrementIndent();
    static void decrementIndent();

    static void setLevel(enum Level level);
    static void clearLevel(enum Level level);
    static void clearAllLevel();

    static int getLevel();

    static std::ostream &dout(enum Level level = Level::Debug);

  private:
    static int indentation_;
    static enum Indent indent_;
    static unsigned int level_;
};

#endif // SEPP_API_DEBUG_H

