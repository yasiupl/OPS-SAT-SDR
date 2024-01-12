/** OPS-SAT SEPP C++ API CORE Library - SEPP HPS GPIO Device
 *
 * \file sepp_gpio.h
 *
 * \brief Generic library for SEPP HPS general purpose IO (GPIO) .
 * The class can be used to access HPS GPIO pins on the SEPP SoC.
 *
 * \project  		OPS-SAT SEPP C++ API Library
 * \Organisation  	IKS, TU Graz, Inffeldgasse 12/I, 8010 Graz
 * \Author 			Reinhard Zeif, MSc
 * \Date 				18. Dec 2017
 * \version			0.3
 *
 * \version   	0.1  first draft						Oct.2015
 * \version			0.2	 EM test    						Nov.2017
 * \version			0.3	 Further testing and bugfixing		Dec. 2017
 *
 * \copyright (C) 2015-2019 Graz University of Technology / IKS. All rights reserved.
 * Your use of TU Graz design tools, logic functions and other 
 * software and tools and any output files any of the foregoing 
 * (including device programming or simulation files), and any associated 
 * documentation or information are expressly subject to the terms and 
 * conditions of the applicable license agreement, including, without 
 * limitation, that your use is for the sole purpose of programming the 
 * OPS-SAT processing platform.
 *
 */
 
 
#ifndef SEPP_GPIO_H_
#define SEPP_GPIO_H_


#define		SEPP_GPIO_OUTPUT				0
#define		SEPP_GPIO_INPUT					1
#define		SEPP_GPIO_HIGH					1
#define		SEPP_GPIO_LOW						0

#define 	SEPP_GPIO_MAX_PATH_LENGTH			250
#define		SEPP_GPIO_MAX_BUF_LENGTH			250


/** SEPP C++ API CORE Library - SEPP HPS GPIO Device Class
* 	\brief This class contains members and methods for GPIO access from Linux user-space. 
*	  The methods are used to read/write signal levels and direction of SEPP GPIOs.
*/
class SEPP_GPIO
{
	public:
		SEPP_GPIO(char * gpio_dev_name, unsigned int gpio_nr);       //!< Constructor
		virtual ~SEPP_GPIO(void);                               //!< Destructor
    
		// GPIO Configuration Methods
		void Export(void);
		void Unexport(void);
		void Set_Direction(int in_or_out);
		void Set_Level(int high_or_low);
		int Get_Direction(void);
		int Get_Level(void);
    
		// GPIO linux kernel device name methods
		void Set_DevName(char * gpio_dev_name);	      //!< set Linux device name, "/sys/class/<gpio_dev_name>"
		char* Get_DevName(void);
   
		void Print_Info(void);

	private:
		unsigned int gpio_nr_;                  // GPIO pin number
		char gpio_dev_name_[SEPP_GPIO_MAX_PATH_LENGTH+1];     // local member for device path, "/sys/class/<gpio_dev_name>"

};


/** SEPP C++ API CORE Library - SEPP GPIO Device Exception
* 	\brief This exception type is returned in case of an exception in the GPIO device class.
*/
class SEPP_GPIO_Exception : public std::exception
{
  private:
	std::string e_;
	
  public:
	SEPP_GPIO_Exception(std::string e) : e_(e) {}
	~SEPP_GPIO_Exception() throw () {}
	const char* what() const throw() { return e_.c_str(); }
};

/** SEPP C++ API CORE Library - SEPP GPIO Device EXPORT Exception
* 	\brief This exception type is returned in case of an exception while exporting pins with the GPIO device class.
*   The exception can occur if a pin is already exported or can't be exported.
*/
class SEPP_GPIO_EXPORT_Exception : public std::exception
{
  private:
	std::string e_;
  public:
	SEPP_GPIO_EXPORT_Exception(std::string e) : e_(e) {}
	~SEPP_GPIO_EXPORT_Exception() throw () {}
	const char* what() const throw() { return e_.c_str(); }
};

#endif
