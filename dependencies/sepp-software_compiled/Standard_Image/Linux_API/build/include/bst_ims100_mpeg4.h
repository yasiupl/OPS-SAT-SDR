/*! \file    bst_ims100_mpeg4.h
    \brief   Writing mpeg4 video files based on bst_ims100_img_t
    \author  [BST]

    Copyright: Berlin Space Technologies GmbH
    See main.c for the minimal usage example.
*/

#ifndef _BST_IMS100_MPEG4_H_
#define _BST_IMS100_MPEG4_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <bst_ims100_def.h>

/**
 * @brief      Set up the context for mpeg4 video writing. Define video dimensions by setting
 *             the appropriate parameters in the bst_ims100_img_attr_t struct.
 *
 * @param[in]  filename  The video filename
 * @param[in]  attr      The initial attributes defining the video dimensions height and width.
 *
 * @return     \ref bst_ret_t
 */
bst_ret_t bst_ims100_mpeg4_init(const char * filename, const bst_ims100_img_attr_t * attr);

/**
 * @brief      Append a new bst_ims100_img_t.
 *             Needs to be called after successfully calling bst_ims100_mpeg4_init.
 *
 * @param[in]  frame  The new image to be added.
 *
 * @return     \ref bst_ret_t
 */
bst_ret_t bst_ims100_mpeg4_add_frame(const bst_ims100_img_t * frame);

/**
 * @brief      Completes a running video capturing.
 *             Needs to be called after successfully calling bst_ims100_mpeg4_init.
 *
 * @return     \ref bst_ret_t
 */
bst_ret_t bst_ims100_mpeg4_done(void);

/**
 * @brief      Sets the color type which is assumed as the input color type.
 *
 * @param[in]  color_type  The color type
 *
 * @return     \ref bst_ret_t
 */
bst_ret_t bst_ims100_mpeg4_set_in_colortype(bst_ims100_img_color_t color_type);

#ifdef __cplusplus
}
#endif

#endif
