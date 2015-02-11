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
 * @brief       Interfejs ILI9326_lld (Low Level Driver).
 *
 *              U ovoj datoteci se nalaze osnovne funkcije za crtanje i
 *              manipulaciju displej kontrolera.
 *
 * ------------------------------------------------------------------------------------------------
 *
 * @addtogroup  ILI9326_intf
 *
 ****************************************************************************************//** @{ */


#ifndef ILI9326_LLD_H_
#define ILI9326_LLD_H_


/*************************************************************************************************
 * INCLUDE FILES
 *************************************************************************************************/

/*-----------------------------------------------------------------------------------------------*
 * Module dependencies
 *-----------------------------------------------------------------------------------------------*/
#include <stdint.h>
#include "ILI9326_regs.h"


/*-----------------------------------------------------------------------------------------------*
 * Module configuration and adapter
 *-----------------------------------------------------------------------------------------------*/

#include "ILI9326_cfg.h"
#include "lpc17xx_clkpwr.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_ssp.h"
#include "lpc17xx_gpio.h"
#include "wait_api.h"


/*-----------------------------------------------------------------------------------------------*
 * EXTERNS
 *-----------------------------------------------------------------------------------*//** @cond */

#ifdef ILI9326_LLD_H_VAR
# define ILI9326_LLD_H_EXT
#else
# define ILI9326_LLD_H_EXT extern
#endif


/** @endcond*//***********************************************************************************
 * DEFINES
 *************************************************************************************************/


/*************************************************************************************************
 * MACRO's
 *************************************************************************************************/


/*-----------------------------------------------------------------------------------------------*
 * C++ zastita - pocetak
 *-----------------------------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C"
{
#endif


/*************************************************************************************************
 * DATA TYPES
 *************************************************************************************************/

typedef enum lcdOrient {
    I_ORIENT_NORMAL,
    I_ORIENT_0,
    I_ORIENT_90,
    I_ORIENT_180,
    I_ORIENT_270,
} lcdOrient_T;


/*************************************************************************************************
 * GLOBAL VARIABLES
 *************************************************************************************************/


/*************************************************************************************************
 * FUNCTION PROTOTYPES
 *************************************************************************************************/

/*-----------------------------------------------------------------------------------------------*/
/**
 * @brief       Crta horizontalnu liniju.
 *
 * @param       x                       x koordinata,
 * @param       y                       y koordinata,
 * @param       length                  duzina linije,
 * @param       color                   boja linije.
 */
/*-----------------------------------------------------------------------------------------------*/
void I_drawHLine(
    uint16_t x,
    uint16_t y,
    uint16_t length,
    uint16_t color);

/*-----------------------------------------------------------------------------------------------*/
/**
 * @brief       Crta vertikalnu liniju.
 *
 * @param       x                       x koordinata,
 * @param       y                       y koordinata,
 * @param       length                  duzina linije,
 * @param       color                   boja piksela.
 */
/*-----------------------------------------------------------------------------------------------*/
void I_drawVLine(
    uint16_t x,
    uint16_t y,
    uint16_t length,
    uint16_t color);

/*-----------------------------------------------------------------------------------------------*/
/**
 * @brief       Postavlja jedan piksel.
 *
 * @param       xCord                   x koordinata,
 * @param       yCord                   y koordinata,
 * @param       color                   boja piksela.
 */
/*-----------------------------------------------------------------------------------------------*/
void I_drawPoint(
    uint16_t xCord,
    uint16_t yCord,
    uint16_t color);

/*-----------------------------------------------------------------------------------------------*/
/**
 * @brief       Vraca maksimalnu X koordinatu za trenutnu orijentaciju
 *
 * @return
 */
/*-----------------------------------------------------------------------------------------------*/
uint16_t I_getMaxX(
    void);

/*-----------------------------------------------------------------------------------------------*/
/**
 * @brief       Vraca maksimalnu Y koordinatu za trenutnu orijentaciju
 *
 * @return
 */
/*-----------------------------------------------------------------------------------------------*/
uint16_t I_getMaxY(
    void);

/*-----------------------------------------------------------------------------------------------*/
/**
* @brief       Izvrsava inicijalizaciju displej kontrolera i postavljanje na
*              podrazumevane vrednosti.
*
*              Funkcija je radjena po power-on algoritmu koji je dat na
*              120 str. ILI9326 datasheet-a.
*              Display-on sekvenca je data na 118 str.
*/
/*-----------------------------------------------------------------------------------------------*/
void I_resetDevice(
    void);

/*-----------------------------------------------------------------------------------------------*/
/**
 * @brief       Postavlja prozor iscrtavanja na ceo LCD displej.
 */
/*-----------------------------------------------------------------------------------------------*/
void I_windowReset(
    void);

/*-----------------------------------------------------------------------------------------------*/
/**
 * @brief       Postavlja prozor na zadate koordinate.
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 */
/*-----------------------------------------------------------------------------------------------*/
void I_windowSet(
    uint16_t x1,
    uint16_t y1,
    uint16_t x2,
    uint16_t y2);

/*-----------------------------------------------------------------------------------------------*/
/**
 * @brief       Postavlja orijentaciju LCD displeja
 *
 * @param       orientation             Orijentacija
 * @arg         I_ORIENT_NORMAL         - postavlja orijentaciju na podrazumevanu vrednost (OPT_DISP_ORIENTATION)
 * @arg         I_ORIENT_0              - Orijentacija 0
 * @arg         I_ORIENT_90             - Orijentacija 90
 * @arg         I_ORIENT_180            - Orijentacija 180
 * @arg         I_ORIENT_270            - Orijentacija 270
 */
/*-----------------------------------------------------------------------------------------------*/
void I_orientSet(
    lcdOrient_T orientation);

/*-----------------------------------------------------------------------------------------------*/
/**
 * @brief       Vraca trenutno postavljenu orijentaciju displeja
 *
 * @return
 * @retval      I_ORIENT_0              - Orijentacija 0
 * @retval      I_ORIENT_90             - Orijentacija 90
 * @retval      I_ORIENT_180            - Orijentacija 180
 * @retval      I_ORIENT_270            - Orijentacija 270
 */
/*-----------------------------------------------------------------------------------------------*/
lcdOrient_T I_orientGet(
    void);

/*-----------------------------------------------------------------------------------------------*/
/**
 * @brief       Zapocinje proces sukcesivnog upisa boja pixela.
 *
 * @note        Mora se pozvati pre I_pixelWR() funkcije.
 */
/*-----------------------------------------------------------------------------------------------*/
void I_pixelWRbegin(void);

/*-----------------------------------------------------------------------------------------------*/
/**
 * @brief       Upis boje piksela
 *
 * @param       color                   Boja pixela.
 */
/*-----------------------------------------------------------------------------------------------*/
void I_pixelWR(uint16_t color);

/*-----------------------------------------------------------------------------------------------*/
/**
 * @brief       Zaustavlja proces sukcesivnog upisa boja pixela.
 */
/*-----------------------------------------------------------------------------------------------*/
void I_pixelWRend(void);


/*-----------------------------------------------------------------------------------------------*
 * C++ zastita - kraj
 *-----------------------------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif


/*************************************************************************************************
 * CONFIGURATION ERRORS
 *************************************************************************************//** @cond */


/** @endcond *//** @} *//*************************************************************************
 * END of ILI9326_lld.h
 *************************************************************************************************/
#endif /* ILI9326_LLD_H_ */
