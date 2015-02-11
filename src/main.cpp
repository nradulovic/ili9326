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
 * @brief       Testni program za ILI9326 drajver.
 *
 * ------------------------------------------------------------------------------------------------
 *
 * @addtogroup  test_prog
 *
 ****************************************************************************************//** @{ */


/*************************************************************************************************
 * INCLUDE FILES
 *************************************************************************************************/

#include "mbed.h"
#include "ILI9326.h"

/*************************************************************************************************
 * LOCAL DEFINES
 *************************************************************************************************/

/*-------------------------------------------------------------------------------------------*//**
 * @name        Pozicija i velicina grafika na displeju
 * @{ *//*---------------------------------------------------------------------------------------*/

#define GRID_X_POS                      20
#define GRID_Y_POS                      40
#define GRID_X_SIZE                     256
#define GRID_Y_SIZE                     128

/** @} *//*--------------------------------------------------------------------------------------*/

/**
 * @brief       Stanja FSM automata
 */
typedef enum signalFsm {
    STATE_RED_SIG, //!< STATE_RED_SIG
    STATE_GREEN_SIG//!< STATE_GREEN_SIG
} signalFsm_T;

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

/**
 * @brief       Struktura grafikona
 */
typedef struct gridObject {
    uint16_t x;
    uint16_t y;
    uint16_t xSize;
    uint16_t ySize;
    uint16_t gridColor;
    uint16_t backColor;
    uint8_t  numRow;
    uint8_t  numCol;
} gridObject_T;

/**
 * @brief       Struktura signala (objekat)
 */
typedef struct signalObject {
    struct gridObject * grid;
    uint16_t    x;
    uint16_t    y;
    uint16_t    color;
} signalObject_T;

/*************************************************************************************************
 * LOCAL TABLES
 *************************************************************************************************/


/*************************************************************************************************
 * SHARED GLOBAL VARIABLES
 *************************************************************************************************/


/*************************************************************************************************
 * LOCAL GLOBAL VARIABLES
 *************************************************************************************************/

/**
 * @brief       Konfiguracija testnog UART interfejsa
 */
Serial      uart(p28, p27);

/**
 * @brief       Statusna LED dioda
 */
DigitalOut  status(LED1);

/*************************************************************************************************
 * LOCAL FUNCTION PROTOTYPES
 *************************************************************************************************/

/**
 * @brief       Kreira nov grafikon
 *
 * @param       aGrid                   Pokazivac na struktura grafikona,
 * @param       xPos                    x pozicija,
 * @param       yPos                    y pozicija,
 * @param       xSize                   x velicina grafikona,
 * @param       ySize                   y velicina grafikona,
 * @param       gridColor               boja mrezice,
 * @param       backColor               boja pozadine,
 * @param       rows                    broj redova u mrezici,
 * @param       columns                 broj kolona u mrezici.
 */
void newGridObject (
    gridObject_T    * aGrid,
    uint16_t        xPos,
    uint16_t        yPos,
    uint16_t        xSize,
    uint16_t        ySize,
    uint16_t        gridColor,
    uint16_t        backColor,
    uint8_t         rows,
    uint8_t         columns);

/**
 * @brief       Crta grafikon koji je opisan u @c aGrid
 *
 * @param       aGrid                   Pokazivac na grafikon koji treba nacrtati.
 */
void drawGridObject (
    gridObject_T    * aGrid);

/**
 * @brief       Kreira nov signal objekat za prikaz na gradikonu.
 *
 * @param       aSignal                 Pokazivac na strukturu signal objekta,
 * @param       aGrid                   pokazivac na grafikon gde treba da se crta signal,
 * @param       color                   boja signala na grafikonu.
 */
void newSignal(
    signalObject_T  * aSignal,
    gridObject_T    * aGrid,
    uint16_t        color);

/**
 * @brief       Dodaje novu izmerenu vrednost @c aSignalSample u signal @c aSignal
 *
 * @param       aSignal                 Pokazivac na strukturu signala,
 * @param       aSignalSample           nova izmerena vrednost signala.
 */
void drawSignal(
    signalObject_T  * aSignal,
    uint16_t        aSignalSample);


/*************************************************************************************************
 ***                                I M P L E M E N T A T I O N                                ***
 *************************************************************************************************/

int main() {
    signalFsm_T     state;
    gridObject_T    grid;
    signalObject_T  redSig;
    signalObject_T  greenSig;

    I_resetDevice();
    I_orientSet(
        I_ORIENT_180);
    drawFilledRectangle(
        0,
        0,
        431,
        239,
        BLACK);
    drawText(
        20,
        20,
        "Acquired data:",
        &ARIAL[0],
        1,
        YELLOW);
    drawText(
        20,
        188,
        "Legend:",
        &ARIAL[0],
        1,
        YELLOW
        );
    drawText(
        20,
        198,
        "Stream 1",
        &ARIAL[0],
        1,
        RED
        );
    drawText(
        20,
        208,
        "Stream 2",
        &ARIAL[0],
        1,
        GREEN
        );
    drawText(
        300,
        20,
        "Hor: 64 samples",
        &ARIAL[0],
        1,
        YELLOW
        );
    drawText(
        300,
        30,
        "Ver: 16%",
        &ARIAL[0],
        1,
        YELLOW
        );
    newGridObject(
        &grid,
        GRID_X_POS,
        GRID_Y_POS,
        GRID_X_SIZE,
        GRID_Y_SIZE,
        DARKGRAY,
        BLACK,
        6,
        4);
    drawGridObject(
        &grid);
    newSignal(
        &redSig,
        &grid,
        RED);
    newSignal(
        &greenSig,
        &grid,
        GREEN);
    state = STATE_RED_SIG;

    while (true) {

        if (uart.readable()) {
            status = !status;

            switch (state) {
                case STATE_RED_SIG : {
                    drawSignal(
                        &redSig,
                        uart.getc() << 8);
                    state = STATE_GREEN_SIG;
                    break;
                }

                case STATE_GREEN_SIG : {
                    drawSignal(
                        &greenSig,
                        uart.getc() << 8);
                    state = STATE_RED_SIG;
                    break;
                }

                default : {
                    break;
                }
            }
        }
    }
    return (0);
}


/*************************************************************************************************
 * GLOBAL FUNCTION DEFINITIONS
 *************************************************************************************************/


/*************************************************************************************************
 * LOCAL FUNCTION DEFINITIONS
 *************************************************************************************************/

void newGridObject (
    gridObject_T    * aGrid,
    uint16_t        xPos,
    uint16_t        yPos,
    uint16_t        xSize,
    uint16_t        ySize,
    uint16_t        gridColor,
    uint16_t        backColor,
    uint8_t         rows,
    uint8_t         columns) {

    aGrid->x = xPos;
    aGrid->y = yPos;
    aGrid->xSize = xSize;
    aGrid->ySize = ySize;
    aGrid->gridColor = gridColor;
    aGrid->backColor = backColor;
    aGrid->numRow = rows;
    aGrid->numCol = columns;
}


void drawGridObject (
    gridObject_T    * aGrid) {

    uint16_t x1;
    uint16_t y1;
    uint8_t  i;
    float    dref;
    float    dtmp;

    x1 = aGrid->x + aGrid->xSize;
    y1 = aGrid->y + aGrid->ySize;
    drawRectangle(
        aGrid->x,
        aGrid->y,
        x1,
        y1,
        WHITE);
    drawFilledRectangle(
        aGrid->x + 1U,
        aGrid->y + 1U,
        x1 - 1U,
        y1 - 1U,
        aGrid->backColor);
    dtmp = 0.0f;
    dref = (float)(aGrid->ySize - 2U) / (float)(aGrid->numRow);

    for (i = 1U; i < (aGrid->numRow); i++) {
        dtmp += dref;
        drawCutLine(
            aGrid->x + 1U,
            aGrid->y + dtmp,
            x1 - 1U,
            aGrid->y + dtmp,
            aGrid->gridColor);
    }
    dtmp = 0.0f;
    dref = (float)(aGrid->xSize - 2U) / (float)(aGrid->numCol);

    for (i = 1U; i < (aGrid->numCol); i++) {
        dtmp += dref;
        drawCutLine(
            aGrid->x + dtmp,
            aGrid->y + 1U,
            aGrid->x + dtmp,
            y1 - 1U,
            aGrid->gridColor);
    }
}

void newSignal(
    signalObject_T  * aSignal,
    gridObject_T    * aGrid,
    uint16_t        color) {

    aSignal->grid = aGrid;
    aSignal->x = 0U;
    aSignal->y = aGrid->ySize - 1U;
    aSignal->color = color;

}

void drawSignal(
    signalObject_T  * aSignal,
    uint16_t        aSignalSample) {

    uint16_t xCord;
    uint16_t yCord;
    uint16_t xCordNext;
    uint16_t yCordNext;

    if ((aSignal->x + 1U) > (aSignal->grid->xSize - 1U)) {
        aSignal->x = 0U;
        drawGridObject(
            aSignal->grid);
    }
    xCord = aSignal->grid->x + 1U + aSignal->x;
    yCord = aSignal->grid->y + 1U + aSignal->y;
    aSignal->x++;
    aSignal->y = aSignalSample / (UINT16_MAX / aSignal->grid->ySize);

    if (aSignal->y > aSignal->grid->ySize - 1U) {
        aSignal->y = aSignal->grid->ySize - 1U;
    }
    xCordNext = aSignal->grid->x + 1U + aSignal->x;
    yCordNext = aSignal->grid->y + 1U + aSignal->y;
    drawLine(
        xCord,
        yCord,
        xCordNext,
        yCordNext,
        aSignal->color);
}


/*************************************************************************************************
 * CONFIGURATION ERRORS
 *************************************************************************************//** @cond */


/** @endcond *//** @} *//*************************************************************************
 * END of main.c
 *************************************************************************************************/
