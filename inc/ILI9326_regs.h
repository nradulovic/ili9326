/*************************************************************************************************
 * This file is part of ILI9326
 *
 * Copyright (C) 2011, 2012 - Nenad Radulovic
 *
 * ILI9326 is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * ILI9326 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ILI9326; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA  02110-1301  USA
 *
 * web site:    http://blueskynet.dyndns-server.com
 * e-mail  :    blueskyniss@gmail.com
 *************************************************************************************************/


/*********************************************************************************************//**
 * @file
 *
 * @author  	nenad
 *
 * @brief       Registri ILI9326 kontrolera.
 *
 * ------------------------------------------------------------------------------------------------
 *
 * @addtogroup  ILI9326_intf
 *
 ****************************************************************************************//** @{ */


#ifndef ILI9326_REGS_H_
#define ILI9326_REGS_H_


/*************************************************************************************************
 * INCLUDE FILES
 *************************************************************************************************/

/*-----------------------------------------------------------------------------------------------*
 * Module dependencies
 *-----------------------------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------------------------*
 * Module configuration and adapter
 *-----------------------------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------------------------*
 * EXTERNS
 *-----------------------------------------------------------------------------------*//** @cond */

#ifdef ILI9326_REGS_H_VAR
# define ILI9326_REGS_H_EXT
#else
# define ILI9326_REGS_H_EXT extern
#endif


/** @endcond*//***********************************************************************************
 * DEFINES
 *************************************************************************************************/


/*************************************************************************************************
 * MACRO's
 *************************************************************************************************/


/*************************************************************************************************
 * DATA TYPES
 *************************************************************************************************/

#define I_LCD_DRV_OUTPUT_CTRL           0x0001
#define I_LCD_DRIVING_WAVE_CTRL         0x0002
#define I_ENTRY_MODE                    0x0003
#define I_OUTLINE_SHARPENING_CTRL       0x0006
#define I_DISP_CTRL_1                   0x0007
#define I_DISP_CTRL_2                   0x0008
#define I_DISP_CTRL_3                   0x0009
#define I_LOW_PWR_CTRL                  0x000b
#define I_RGB_DISP_INTF_CTRL_1          0x000c
#define I_RGB_DISP_INTF_CTRL_2          0x000f
#define I_PANEL_INTF_CTRL_1             0x0010
#define I_PANEL_INTF_CTRL_2             0x0011
#define I_PANEL_INTF_CTRL_3             0x0012
#define I_PANEL_INTF_CTRL_4             0x0020
#define I_PANEL_INTF_CTRL_5             0x0021
#define I_PANEL_INTF_CTRL_6             0x0022
#define I_FRAME_MARKER_POS              0x0090
#define I_PWR_CTRL_1                    0x0100
#define I_PWR_CTRL_2                    0x0101
#define I_PWR_CTRL_3                    0x0102
#define I_PWR_CTRL_4                    0x0103
#define I_PWR_CTRL_5                    0x0107
#define I_GRAM_ADDR_H_SET               0x0200
#define I_GRAM_ADDR_V_SET               0x0201
#define I_GRAM_WR_DATA                  0x0202
#define I_FRAME_RATE_COLOR_CTRL         0x020b
#define I_H_ADDR_START_POS              0x0210
#define I_H_ADDR_END_POS                0x0211
#define I_V_ADDR_START_POS              0x0212
#define I_V_ADDR_END_POS                0x0213
#define I_UID_CODE                      0x0280
#define I_VCOMH                         0x0281
#define I_MPT_VCM_PROG                  0x0290
#define I_MTP_VCM_STAT_EN               0x0291
#define I_MTP_PROG_KEY                  0x0295
#define I_GAMMA_CTRL_1                  0x0300
#define I_GAMMA_CTRL_2                  0x0301
#define I_GAMMA_CTRL_3                  0x0302
#define I_GAMMA_CTRL_4                  0x0305
#define I_GAMMA_CTRL_5                  0x0306
#define I_GAMMA_CTRL_6                  0x0307
#define I_GAMMA_CTRL_7                  0x0308
#define I_GAMMA_CTRL_8                  0x0309
#define I_GAMMA_CTRL_9                  0x030c
#define I_GAMMA_CTRL_10                 0x030d
#define I_BASE_IMG_DISP_CTRL_1          0x0400
#define I_BASE_IMG_DISP_CTRL_2          0x0401
#define I_BASE_IMG_DISP_CTRL_3          0x0404
#define I_PART_IMG_1_DISP_POS           0x0500
#define I_PART_IMG_1_AREA_START         0x0501
#define I_PART_IMG_1_AREA_END           0x0502
#define I_PART_IMG_2_DISP_POS           0x0503
#define I_PART_IMG_2_AREA_START         0x0504
#define I_PART_IMG_2_AREA_END           0x0505

#define INDEX_WR                        0x0
#define STAT_RD                         0x1
#define REG_WR                          0x2
#define REG_RD                          0x3

#define BIT_SS                          ((uint16_t)(1U << 8))
#define BIT_LINE_INVERSION              ((uint16_t)(1U << 8))
#define BIT_SWAP_RGB                    ((uint16_t)(1U << 12))
#define BITS_ORIENT_0                   ((uint16_t)((1U << 5) | (1U << 4)))
#define BITS_ORIENT_90                  ((uint16_t)((1U << 5) | (1U << 3)))
#define BITS_ORIENT_180                 ((uint16_t)0)
#define BITS_ORIENT_270                 ((uint16_t)((1U << 4) | (1U << 3)))
#define BIT_SAP                         ((uint16_t)(1U << 12))
#define BITS_AP(val)                                                            \
    ((uint16_t)((val) << 4))
#define BIT_APE                         ((uint16_t)(1U << 7))
#define BITS_BT(val)                                                            \
    ((uint16_t)((val) << 8))
#define BITS_DC0(val)                                                           \
    ((uint16_t)((val) << 4))
#define BITS_DC1(val)                                                           \
    ((uint16_t)((val) << 8))
#define BITS_VC(val)                                                            \
    ((uint16_t)(val))
#define BIT_VREG1R(val)                                                         \
    ((uint16_t)((val) << 7))
#define BIT_PSON                        ((uint16_t)(1U << 5))
#define BIT_PON                         ((uint16_t)(1U << 4))
#define BITS_VRH(val)                                                           \
    ((uint16_t)(val))
#define BIT_UNDOC102_1                  ((uint16_t)(1U << 8))
#define BIT_VCOMG                       ((uint16_t)(1U << 13))
#define BITS_VDV(val)                                                           \
    ((uint16_t)((val) << 8))
#define BIT_GS                          ((uint16_t)(1U << 15))
#define BITS_NL(val)                                                            \
    ((uint16_t)((val) << 8))
#define BITS_SCN(val)                                                           \
    ((uint16_t)(val))
#define BIT_NDL                         ((uint16_t)(1U << 2))
#define BIT_VLE                         ((uint16_t)(1U << 1))
#define BIT_REV                         ((uint16_t)(1U))
#define BITS_NOWI(val)                                                          \
    ((uint16_t)((val) << 8))
#define BITS_DIVI(val)                                                          \
    ((uint16_t)((val) << 8))
#define BITS_RTNI(val)                                                          \
    ((uint16_t)(val))
#define BITS_DIVE(val)                                                          \
    ((uint16_t)((val) << 8))
#define BITS_RTNE(val)                                                          \
    ((uint16_t)(val))
#define BITS_PTDE(val)                                                          \
    ((uint16_t)((val) << 12))
#define BIT_BASEE                       ((uint16_t)(1U << 8))
#define BIT_BGR                         ((uint16_t)(1U << 12))
#define BIT_VON                         ((uint16_t)(1U << 6))
#define BIT_GON                         ((uint16_t)(1U << 5))
#define BIT_DTE                         ((uint16_t)(1U << 4))
#define BITS_D(val)                                                             \
    ((uint16_t)(val))
#define BITS_IDIR(val)                                                          \
    ((uint16_t)((val) << 4))
#define BIT_AM                          ((uint16_t)(1U << 3))


/*************************************************************************************************
 * GLOBAL VARIABLES
 *************************************************************************************************/


/*************************************************************************************************
 * FUNCTION PROTOTYPES
 *************************************************************************************************/


/*************************************************************************************************
 * CONFIGURATION ERRORS
 *************************************************************************************//** @cond */


/** @endcond *//** @} *//*************************************************************************
 * END of ILI9326_regs.h
 *************************************************************************************************/
#endif /* ILI9326_REGS_H_ */
