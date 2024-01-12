/*! \file    bst_ims100_tools.h
    \brief   Defines helper functions for IMS100 operation
    \author  [BST]

    Copyright: Berlin Space Technologies GmbH
*/

#ifndef _BST_IMS100_TOOLS_H_
#define _BST_IMS100_TOOLS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <bst_ims100_def.h>

// ************************************************************************************************
// ************************************************************************************************
// ************************************************************************************************
// print functions

/**
 * @brief   Return human readable string of standard telemetry
 * @details Necessary string length is TBD bytes including string terminating character
 *
 * ...
 */
bst_ret_t bst_ims100_tools_print_tele_std(char* buf, const int n, const bst_ims100_tele_std_t* tele);

/**
 * @brief   Return human readable string of imaging config
 * @details Necessary string length is TBD bytes including string terminating character
 * 
 * ...
 */
bst_ret_t bst_ims100_tools_print_img_config(char* buf, const int n, const bst_ims100_img_config_t* img_config);

// ************************************************************************************************
// ************************************************************************************************
// ************************************************************************************************

/**
 * @brief   Calculates frame rate from imaging configuration and number of skipped images
 * @details Maximum readout rate with SEPP is TBD bytes/seconds
 * 
 * ...
 */
bst_ret_t bst_ims100_tools_calc_frame_rate(float* rate, const bst_ims100_img_config_t* img_config, const uint8_t n_skip);

#ifdef __cplusplus
}
#endif

#endif
