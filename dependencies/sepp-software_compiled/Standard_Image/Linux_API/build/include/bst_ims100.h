/*! \file    bst_ims100.h
    \brief   Defines access functions for IMS100 operation
    \author  [BST]

    Copyright: Berlin Space Technologies GmbH

    Streaming mode example:
    @code

    bst_ims100_start_continuous(n, n_skip);

    while(bst_ims100_continuous_running() == true)
    {
        bst_ims100_img_t img;
        if(bst_ims100_get_new_image(&img) == true)
        {
            // process img ...
        }
        
        if(stop_condition == true)
        {
            bst_ims100_stop_continuous();
        }
    }

    @endcode

*/
#ifndef _BST_IMS100_H_
#define _BST_IMS100_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <bst_ims100_def.h>

#define  BST_IMS100_BLOCKSIZE     4096

/**
 * must be defined for exporting png files
 */
#ifdef BST_HAS_LIBPNG
#include <png.h>
// Do not evaluate by SWIG preprocessor
#ifndef SWIG
#if PNG_LIBPNG_VER < 10606
#error "libpng version >= 1.6.6 needed"
#endif
#endif

#endif

// ************************************************************************************************
// ************************************************************************************************
// ************************************************************************************************
// IMS100 High Level Access

/**
 * @brief   Init IMS100 device
 * @details Performs:
 *          - opens USB com          port (\ref bst_ims100_usb_open)
 *          - opens USB mass-storage port (\ref bst_ims100_massstorage_open)
 *          - send "\r\n" initialising command
 *          - configures watch-dog
 *          This function assumes that the device is in a "after power cycle" state.
 *          Meaning this function will fail if the device has already received commands
 *          after the last power cycle.
 * 
 * @param[in]  portname     Name of device port,         e.g. /dev/ttyACM0
 * @param[in]  imgdevice    Name of mass storage device, e.g. /dev/sdb
 * @param[in]  do_watchdog  0 - to disable watchdog; 1 - to enable watchdog; other values trigger error [default is 0]
 * @return     \ref bst_ret_t
 */
bst_ret_t bst_ims100_init(const char * portname, const char * imgdevice, const int do_watchdog);

/**
 * @brief   Closes USB ports to IMS100
 * @details Performs:
 *          - closes USB com          port (\ref bst_ims100_usb_close)
 *          - closes USB mass-storage port (\ref bst_ims100_massstorage_close)
 *
 * @return  \ref bst_ret_t
 */
bst_ret_t bst_ims100_done(void);

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

/**
 * @brief   Get standard telemetry
 * @details Version, Status, Temperature
 * 
 * @param[out]  tele \ref bst_ims100_tele_std_t
 * @return           \ref bst_ret_t
 */
bst_ret_t bst_ims100_get_tele_std(bst_ims100_tele_std_t* tele);

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
// Imaging configuration

/**
 * @brief   Set imaging configuration
 * @details Exposure time, image size and channel gains
 *          Calls:
 *          - bst_ims100_set_exp_time()
 *          - bst_ims100_set_img_size()
 *          - bst_ims100_set_img_crop()
 *          - bst_ims100_reconfigure()
 *          - bst_ims100_set_gain()
 * 
 * @param[in]  img_config  \ref bst_ims100_img_config_t
 * @return                 \ref bst_ret_t
 */
bst_ret_t bst_ims100_set_img_config(const bst_ims100_img_config_t* img_config);

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

/**
 * @brief   Capture n images and stores them into given pixel buffer.
 * @details Returns after capturing of n images or when buffer is full.
 *
 * Max. allowed images are 26 as IMS100 has only an internal ring buffer of 26 images.
 * Resets IMS100 after operation (clears ring buffer; resets internal address).
 *
 * Function provides the primary simple image capturing:
 * 1) Enables imaging mode on IMS100
 * 2) Waits until IMS100 has stored n images
 * 3) Retrieves all images from the device
 *
 * @param[in]   n         Number of images to capture: 1..26
 * @param[in]   n_skip    Number of images to skip (determines frame rate)
 * @return                \ref bst_ret_t
 */
bst_ret_t bst_ims100_get_img_n(bst_ims100_img_t * imgs, const uint16_t n, const uint8_t n_skip);

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

/**
 * @brief   Capture n images
 * Max. allowed images are 26 as IMS100 has only an internal ring buffer of 26 images.
 * Resets IMS100 after operation (clears ring buffer; resets internal address).
 *
 * Function provides the primary simple image capturing:
 * 1) Enables imaging mode on IMS100
 * 2) Waits until IMS100 has stored n images
 *
 * @param[in]   n         Number of images to capture: 1..26
 * @param[in]   n_skip    Number of images to skip (determines frame rate)
 * @return                \ref bst_ret_t
 */
bst_ret_t bst_ims100_shoot_img_n(const uint16_t n, const uint8_t n_skip);

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

/**
 * @brief      Triggers continous imaging mode on IMS100
 *
 * @param[in]   n         Number of images to capture: 1..26
 * @param[in]   n_skip    Number of images to skip (determines frame rate)
 *
 * @return     \ref bst_ret_t
 */
bst_ret_t bst_ims100_start_continuous(const uint16_t n, const uint8_t n_skip);

/**
 * @brief      Stops imaging mode on IMS100
 *
 * @return     \ref bst_ret_t
 */
bst_ret_t bst_ims100_stop_continuous(void);

/**
 * @brief      Checks if continuous imaging mode is still running
 *
 * @return     true in case of running imaging mode otherwise false
 */
bool bst_ims100_continuous_running(void);

/**
 * @brief      Checks for new image shot in continuous imaging mode
 *             and stores it in img in case a new image is shot
 *
 * @param[out] img   Out-parameter for new image
 * @return           true in case new image was written to img
 */
bool bst_ims100_get_new_image(bst_ims100_img_t * img);

/**
 * @brief      This function converts a raw in_img into a rgb out_img
 *             by debayering the swapped raw img.
 *             out_img's data buffer will be allocate by this function
 *             and should be freed by the caller afterwards
 *
 * @param[in]  in_img   raw input image
 * @param      out_img  The out image in rgb format
 * @param[in]  g_red    red   gain
 * @param[in]  g_green  green gain
 * @param[in]  g_blue   blue  gain
 * @param[in]  bitdepth desired bitdepth of the output image
 * 
 * @return     \ref bst_ret_t
 */
bst_ret_t bst_ims100_img_debayer(const bst_ims100_img_t * in_img, uint8_t * out_img,
                                    uint16_t g_red, uint16_t g_green, uint16_t g_blue, const uint8_t bitdepth);

#ifdef BST_HAS_LIBPNG

/**
 * @brief      Stores a bst_ims100_img_t as png file.
 * 
 *             Known issue: missing interpolation at the image edges
 *             will cause corrupted edges in the png.
 *
 * @param[in]  img       The bst_ims100_img_t to store
 * @param[in]  filename  The filename
 * @param[in]  bitdepth  The desired bitdepth for png file (valid values are 8 and 16)
 * @param[in]  color     The effective color of img. This is a device specific parameter.
 *                       Valid values are BST_IMS100_COLOR_BW and BST_IMS100_COLOR_RGB.
 *                       \ref bst_ims100_img_color_t
 *
 * @return     \ref bst_ret_t
 */
bst_ret_t bst_ims100_write_png(const bst_ims100_img_t * img, const char * filename,
                               const uint8_t bitdepth, const bst_ims100_img_color_t color);
#endif  // #ifdef BST_HAS_LIBPNG
// ************************************************************************************************
// ************************************************************************************************
// ************************************************************************************************
// IMS100 Low Level Access

/**
 * @brief   Open USB port to IMS100
 * @details baud rate: 115200
 *          data bits: 8
 *          parity:    none
 *          stop bits: 1,
 *          time-out:  2000 msec (total time-out)
 *
 * @param[in]  portname  Name of device port, e.g. /dev/ttyACM0
 * @return     \ref bst_ret_t
 */
bst_ret_t bst_ims100_usb_open(const char * portname);

/**
 * @brief   Closes USB port to IMS100
 * @return  \ref bst_ret_t
 */
bst_ret_t bst_ims100_usb_close(void);

/**
 * @brief      Connect to the IMS100 massstorage
 *
 * @param[in]  imgdevice  Device name, e.g. /dev/sdb
 *
 * @return     \ref bst_ret_t
 */
bst_ret_t bst_ims100_massstorage_open(const char * imgdevice);

/**
 * @brief      Closes the IMS100 massstorage connection
 *
 * @return     \ref bst_ret_t
 */
bst_ret_t bst_ims100_massstorage_close(void);

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
// Imaging configuration

/**
 * @brief   Set exposure time
 * @details Performs:
 *          - sets exposure time
 *
 * Note: Reconfiguration is required afterwards to affect
 *       image capturing
 * 
 * @param[in] t_exp  Exposure time in milli-seconds; value 1 to 10,000 ms  [default is 100]
 * @return           \ref bst_ret_t
 */
bst_ret_t bst_ims100_set_exp_time(const uint16_t   t_exp);

/**
 * @brief      Reconfigures the IMS100
 *
 * @return     \ref bst_ret_t
 */
bst_ret_t bst_ims100_reconfigure(void);

/**
 * @brief   Set image size
 * 
 * @param[in]  row_start  Start row;    value 0..1943                      [default is    0]
 * @param[in]  row_end    End   row;    value 0..1943; value >= row_start  [default is 1943]
 * @param[in]  col_start  Start column; value 0..2047                      [default is    0]
 * @param[in]  col_end    End   column; value 0..2047; value >= col_start  [default is 2047]
 * @return                \ref bst_ret_t
 */
bst_ret_t bst_ims100_set_img_size(const uint16_t row_start, const uint16_t row_end,
                                  const uint16_t col_start, const uint16_t col_end);

/**
 * @brief   Set color channel gains
 * 
 * @param[in]  g_green  Gain of green channel; value 1...255  [default is TBD]
 * @param[in]  g_red    Gain of red   channel; value 1...255  [default is TBD]
 * @param[in]  g_blue   Gain of blue  channel; value 1...255  [default is TBD]
 * @return              \ref bst_ret_t
 */
bst_ret_t bst_ims100_set_gain(const uint8_t g_green, const uint8_t g_red, const uint8_t g_blue);

#ifdef __cplusplus
}
#endif

#endif
