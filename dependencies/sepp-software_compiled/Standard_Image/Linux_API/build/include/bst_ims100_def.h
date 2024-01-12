/*! \file    bst_ims100_def.h
    \brief   Standard definitions for IMS100 operation
    \author  [BST]

    Copyright: Berlin Space Technologies GmbH
*/

#ifndef _BST_IMS100_DEF_H_
#define _BST_IMS100_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include <stdlib.h>
#include <stdbool.h>

// ************************************************************************************************
// ************************************************************************************************
// ************************************************************************************************

#ifndef _BST_H_

/**
 * BST Standard Return Type
 */
typedef enum
{
    BST_RETURN_SUCCESS = 0,   ///< Indicates that the function performed as expected.
    BST_RETURN_FAILURE = 1    ///< Indicates that an error occured during function call.
} bst_ret_t;

#endif

#define BST_IMS100_VERBOSE_ERROR 1
#define BST_IMS100_VERBOSE_INFO  2

#ifndef BST_IMS100_VERBOSE
#define BST_IMS100_VERBOSE 0
#endif

#if BST_IMS100_VERBOSE >= BST_IMS100_VERBOSE_ERROR
#define IMS100_PRINT_ERROR(...) {fprintf(stderr, "-E- [%s] ", __FUNCTION__); fprintf(stderr, __VA_ARGS__);}
#else
#define IMS100_PRINT_ERROR(...) {}
#endif

#if BST_IMS100_VERBOSE >= BST_IMS100_VERBOSE_INFO
#define IMS100_PRINT_INFO(...) {printf("-I- [%s] ", __FUNCTION__); printf(__VA_ARGS__);}
#else
#define IMS100_PRINT_INFO(...) {}
#endif

/**
 * Standard Telemetry Struct
 */
typedef struct
{
    char    version[128];          ///< Version string as provided by the device - cmd: "show version".
    uint8_t status;                ///< Status byte    as provided by the device - cmd: "show status".
    int8_t  temp;                  ///< Temperature in °C                        - cmd: "show temperature raw".
} bst_ims100_tele_std_t;



/**
 * Standard Imaging Configuration Struct
 */
typedef struct
{
    uint16_t t_exp;                ///< Exposure time in milli-seconds; value 1 to 10,000 ms  [default is 100]

    uint16_t row_start;            ///< Start row;    value 0..1943                      [default is    0]
    uint16_t row_end;              ///< End   row;    value 0..1943; value >= row_start  [default is 1943]
    uint16_t col_start;            ///< Start column; value 0..2047                      [default is    0]
    uint16_t col_end;              ///< End   column; value 0..2047; value >= col_start  [default is 2047]

    uint8_t g_green;               ///< Gain of green channel; value 1...255  [default is TBD]
    uint8_t g_red;                 ///< Gain of red   channel; value 1...255  [default is TBD]
    uint8_t g_blue;                ///< Gain of blue  channel; value 1...255  [default is TBD]

} bst_ims100_img_config_t;

/**
 * Color type 
 */
typedef enum
{
    BST_IMS100_COLOR_BW  = 0,
    BST_IMS100_COLOR_RGB
} bst_ims100_img_color_t;

/**
 * IMS100 Image Attributes
 */
typedef struct
{
    uint32_t width;
    uint32_t height;
    uint32_t offx;
    uint32_t offy;
    double   timestamp;
    uint32_t t_exp;
} bst_ims100_img_attr_t;

/**
 * Structure representing an image shot by the IMS100
 */
typedef struct
{
    bst_ims100_img_attr_t   attr;   ///< image attributes
    uint16_t              * data;   ///< pixel buffer
    size_t                  data_n; ///< size of pixel buffer in uint16_t
} bst_ims100_img_t;

// ************************************************************************************************
// ************************************************************************************************
// ************************************************************************************************

/**
 * @brief      Set Config struct to default values
 *             t_exp     = 100
 *             row_start = 0
 *             row_end   = 1943
 *             col_start = 0
 *             col_end   = 2047
 *             g_green   = 1
 *             g_red     = 1
 *             g_blue    = 1
 * 
 *
 * @param[out] config  The config struct to be filled
 */
void bst_ims100_img_config_default(bst_ims100_img_config_t * config);

#ifdef __cplusplus
}
#endif

#endif
