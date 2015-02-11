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
 * @author      nenad
 *
 * @brief       Implementation of ILI9326.
 *
 * ------------------------------------------------------------------------------------------------
 *
 * @addtogroup  ILI9326_impl
 *
 ****************************************************************************************//** @{ */


/*************************************************************************************************
 * INCLUDE FILES
 *************************************************************************************************/

#include "ILI9326.h"


/*************************************************************************************************
 * LOCAL DEFINES
 *************************************************************************************************/

/*-----------------------------------------------------------------------------------------------*
 * Local debug defines
 *-----------------------------------------------------------------------------------*//** @cond */


/** @endcond *//*--------------------------------------------------------------------------------*
 * Local user defines
 *-----------------------------------------------------------------------------------------------*/


/*************************************************************************************************
 * LOCAL MACRO's
 *************************************************************************************************/


/*************************************************************************************************
 * LOCAL CONSTANTS
 *************************************************************************************************/


/*************************************************************************************************
 * LOCAL DATA TYPES
 *************************************************************************************************/


/*************************************************************************************************
 * LOCAL TABLES
 *************************************************************************************************/


/*************************************************************************************************
 * SHARED GLOBAL VARIABLES
 *************************************************************************************************/


/*************************************************************************************************
 * LOCAL GLOBAL VARIABLES
 *************************************************************************************************/


/*************************************************************************************************
 * LOCAL FUNCTION PROTOTYPES
 *************************************************************************************************/


/*************************************************************************************************
 ***                                I M P L E M E N T A T I O N                                ***
 *************************************************************************************************/


/*************************************************************************************************
 * GLOBAL FUNCTION DEFINITIONS
 *************************************************************************************************/

void drawFlashImage(
    const uint8_t * image,
    uint16_t x,
    uint16_t y) {

    const uint16_t * tmpPtr;
    uint16_t xSize;
    uint16_t ySize;
    uint32_t m;
    uint32_t i;

    tmpPtr = (uint16_t *)image;
    ++tmpPtr;
    ySize = *tmpPtr;
    ++tmpPtr;
    xSize = *tmpPtr;
    ++tmpPtr;
    I_windowSet(x, y, x + xSize - 1, y + ySize - 1);
    m = xSize * ySize;

    I_pixelWRbegin();
    for (i = 0; i < m; i++) {
        I_pixelWR(*tmpPtr);
        tmpPtr++;
    }
    I_pixelWRend();
    I_windowReset();
}

void drawLine(
    uint16_t x1,
    uint16_t y1,
    uint16_t x2,
    uint16_t y2,
    uint16_t color) {

    uint16_t dy;
    uint16_t dx;
    uint16_t i = 0;
    int16_t  addx = 1;
    int16_t  addy = 1;
    int16_t  p;
    int16_t  diff;

    diff = ((int16_t)x2 - x1);

    if (diff < 0) {
        diff *= -1;
    }
    dx = diff;
    diff = ((int16_t)y2 - y1);

    if (diff < 0) {
        diff *= -1;
    }
    dy = diff;

    if (x1 > x2) {
        addx = -1;
    }

    if (y1 > y2) {
        addy = -1;
    }

    if (dx >= dy) {
        dy *= 2;
        p = dy - dx;
        diff = p - dx;

        for (; i<=dx ; ++i) {
            I_drawPoint(x1, y1, color);

            if (p < 0) {
                p  += dy;
                x1 += addx;
            } else {
                p  += diff;
                x1 += addx;
                y1 += addy;
            }
        }
    } else {
        dx *= 2;
        p = dx - dy;
        diff = p - dy;

        for (; i<=dy; ++i) {
            I_drawPoint(x1, y1, color);

            if (p < 0) {
                p  += dx;
                y1 += addy;
            } else {
                p  += diff;
                x1 += addx;
                y1 += addy;
            }
        }
    }
}

void drawCutLine(
    uint16_t x1,
    uint16_t y1,
    uint16_t x2,
    uint16_t y2,
    uint16_t color) {

    uint16_t dy;
    uint16_t dx;
    uint16_t i = 0;
    int16_t  addx = 1;
    int16_t  addy = 1;
    int16_t  p;
    int16_t  diff;

    diff = ((int16_t)x2 - x1);

    if (diff < 0) {
        diff *= -1;
    }
    dx = diff;
    diff = ((int16_t)y2 - y1);

    if (diff < 0) {
        diff *= -1;
    }
    dy = diff;

    if (x1 > x2) {
        addx = -1;
    }

    if (y1 > y2) {
        addy = -1;
    }

    if (dx >= dy) {
        dy *= 2;
        p = dy - dx;
        diff = p - dx;

        for (; i <= dx ; ++i) {

            if ((i & 0x01) == 0x0) {
                I_drawPoint(x1, y1, color);
            }

            if (p < 0) {
                p  += dy;
                x1 += addx;
            } else {
                p  += diff;
                x1 += addx;
                y1 += addy;
            }
        }
    } else {
        dx *= 2;
        p = dx - dy;
        diff = p - dy;

        for (; i <= dy; ++i) {

            if ((i & 0x01) == 0x0) {
                I_drawPoint(x1, y1, color);
            }

            if (p < 0) {
                p  += dx;
                y1 += addy;
            } else {
                p  += diff;
                x1 += addx;
                y1 += addy;
            }
        }
    }
}

void drawText(
    uint16_t x,
    uint16_t y,
    const char *pText,
    const int8_t (*font)[5],
    uint8_t height,
    uint16_t color) {

    uint16_t i;
    uint16_t j;
    uint16_t k;
    uint16_t l;
    uint16_t m;
    uint16_t n;
    uint16_t tmp;
    int8_t pointData[5];
    const int8_t *tmpPtr;

    n = x;

    while(*pText != '\0'){
        tmpPtr = (font + *pText - ' ')[0];

        for (i=0;i<5;i++) {
            pointData[i] = *tmpPtr++;
        }

        switch (*pText) {

            case '\n': {
                y += 8 * height;
                break;
            }

            case '\r': {
                x = n;
                break;
            }

            default: {

                if ((x + 5 * height) >= I_getMaxY()){
                    x = n;
                    y += 8 * height;
                }

                for (j = 0; j < 5; ++j, x += height) {

                    for (k = 0; k < 7; k++) {
                        tmp = (0x01 << k);

                        if ((pointData[j] & tmp) == tmp) {

                            for (l = 0; l < height; ++l) {

                                for (m=0; m < height; ++m) {
                                    I_drawPoint(x + m, y + k * height + l, color);
                                }
                            }
                        }
                    }
                }
                x++;
                break;
            }
        }
        pText++;
    }
}

void drawFilledRectangle(
    uint16_t x1,
    uint16_t y1,
    uint16_t x2,
    uint16_t y2,
    uint16_t color) {

    uint32_t i;
    uint32_t m;

    I_windowSet(x1, y1, x2, y2);
    m = ((uint32_t)(x2 - x1 + 1)) * ((uint32_t)(y2 - y1 + 1));

    I_pixelWRbegin();
    for (i = 0; i < m; i++) {
        I_pixelWR(color);
    }
    I_pixelWRend();
    I_windowReset();
}

void drawRectangle(
    uint16_t x1,
    uint16_t y1,
    uint16_t x2,
    uint16_t y2,
    uint16_t color) {


    I_drawHLine(
        x1,
        y1,
        x2 - x1,
        color);
    I_drawHLine(
        x1,
        y2,
        x2 - x1,
        color);
    I_drawVLine(
        x1,
        y1,
        y2 - y1,
        color);
    I_drawVLine(
        x2,
        y1,
        y2 - y1,
        color);
}


/*************************************************************************************************
 * LOCAL FUNCTION DEFINITIONS
 *************************************************************************************************/


/*************************************************************************************************
 * CONFIGURATION ERRORS
 *************************************************************************************//** @cond */


/** @endcond *//** @} *//*************************************************************************
 * END of ILI9326.c
 *************************************************************************************************/
