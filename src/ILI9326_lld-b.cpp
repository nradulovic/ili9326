/*************************************************************************************************
 * This file is part of ILI9326
 *
 * Template version: 1.1.8 (26.01.2012)
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
 *************************************************************************************************/


/*********************************************************************************************//**
 * @file
 *
 * @author      nesa
 *
 * @brief       Implementation of ILI9326_lld.
 *
 *              long description
 *
 * @note        notes
 *
 * ------------------------------------------------------------------------------------------------
 *
 * @addtogroup  module_impl
 *
 ****************************************************************************************//** @{ */


/*************************************************************************************************
 * INCLUDE FILES
 *************************************************************************************************/

#include "ILI9326_lld.h"
#include "mbed.h"


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

#if defined(OPT_USE_CPP)
/*-----------------------------------------------------------------------------------------------*/
/**
 * @brief       CS = 1
 */
/*-----------------------------------------------------------------------------------------------*/
#define LCD_DISABLE()                                                           \
    lcd_cs = 1

/*-----------------------------------------------------------------------------------------------*/
/**
 * @brief       CS = 0
 */
/*-----------------------------------------------------------------------------------------------*/
#define LCD_ENABLE()                                                            \
    lcd_cs = 0

/*-----------------------------------------------------------------------------------------------*/
/**
 * @brief       RST = 0
 */
/*-----------------------------------------------------------------------------------------------*/
#define LCD_RESET_ACTIVE()                                                      \
    lcd_rst = 0

/*-----------------------------------------------------------------------------------------------*/
/**
 * @brief       RST = 1
 */
/*-----------------------------------------------------------------------------------------------*/
#define LCD_RESET_INACTIVE()                                                    \
    lcd_rst = 1

/*-----------------------------------------------------------------------------------------------*/
/**
 * @brief       Makro koji formira odgovarajuci start bajt
 */
/*-----------------------------------------------------------------------------------------------*/
#define LCD_START_BYTE(func)                                                    \
    (0x70 | (OPT_DEVICE_ID << 2) | (func))

/*-----------------------------------------------------------------------------------------------*/
/**
 * @brief       Prebacivanje u 8-bitni rezim rada SPI modula
 */
/*-----------------------------------------------------------------------------------------------*/
#define SPI_MODE_8B()                                                           \
    do {                                                                        \
        lcd_spi.format(8, OPT_SPI_POLARITY);                                    \
    } while (0)

/*-----------------------------------------------------------------------------------------------*/
/**
 * @brief       Prebacivanje u 16-bitni rezim rada SPI modula
 */
/*-----------------------------------------------------------------------------------------------*/
#define SPI_MODE_16B()                                                          \
    do {                                                                        \
        lcd_spi.format(16, OPT_SPI_POLARITY);                                   \
    } while (0)

/*-----------------------------------------------------------------------------------------------*/
/**
 * @brief       Inicijalizacija GPIO modula
 */
/*-----------------------------------------------------------------------------------------------*/
#define GPIO_INIT()                                                             \
    (void)0

/*-----------------------------------------------------------------------------------------------*/
/**
 * @brief       Inicijalizacija SPI modula
 *
 *              - 8-bitni rezim
 *              - CLK je HIGH.
 *              - SSEL je HIGH.
 *              - MOSI/MISO visoka impedansa
 */
/*-----------------------------------------------------------------------------------------------*/
#define SPI_INIT()                                                              \
    do {                                                                        \
        lcd_spi.format(8, OPT_SPI_POLARITY);                                    \
        lcd_spi.frequency(1000000);                                             \
    } while (0)

/*-----------------------------------------------------------------------------------------------*/
/**
 * @brief       Slanje podataka sa SPI modula
 */
/*-----------------------------------------------------------------------------------------------*/
#define SPI_TX(data)                                                            \
    lcd_spi.write(data)

/*-----------------------------------------------------------------------------------------------*/
/**
 * @brief       Cekanje u milisekundama
 */
/*-----------------------------------------------------------------------------------------------*/
#define WAIT_MS(time)                                                         \
    wait_ms(time)

/*-----------------------------------------------------------------------------------------------*/
/**
 * @brief       Upis boje jednog piksela
 */
/*-----------------------------------------------------------------------------------------------*/
#define WR_PIXEL(color)                                                         \
    SPI_TX(color)

#else
#endif

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
 * GLOBAL FUNCTION PROTOTYPES
 *************************************************************************************************/

void I_resetDevice(
    void);

void I_wrPixel(
    uint16_t xCord,
    uint16_t yCord,
    uint16_t color);

/*************************************************************************************************
 * LOCAL FUNCTION PROTOTYPES
 *************************************************************************************************/

static void wrIndex(
    uint16_t index);
static void wrData(
    uint16_t data);
static void setReg(
    uint16_t reg,
    uint16_t value);
static void setAddres(
    uint32_t addr);


/*************************************************************************************************
 ***                                I M P L E M E N T A T I O N                                ***
 *************************************************************************************************/

#if defined(OPT_USE_CPP)

SPI lcd_spi(p11, p12, p13);                                                        /* mosi, miso, sclk */
DigitalOut lcd_cs(p16);
DigitalOut lcd_rst(p17);
DigitalOut lcd_en(p25);

#else
#endif

void I_resetDevice(
    void) {

    GPIO_INIT();
    lcd_en = 1;
    SPI_INIT();
    LCD_RESET_INACTIVE();
    LCD_DISABLE();
    WAIT_MS(1);
    LCD_RESET_ACTIVE();
    WAIT_MS(10);
    LCD_RESET_INACTIVE();
    WAIT_MS(50);
    /* Nedokumentovani registri: 0x0702, 0x0705, 0x070B*/
    setReg(0x0702, 0x3008);                                                     /* Set internal timing,don't change this value!             */
    setReg(0x0705, 0x0036);                                                     /* Set internal timing,don't change this value!             */
    setReg(0x070B, 0x1213);                                                     /* Set internal timing,don't change this value!             */
    setReg(
        I_LCD_DRV_OUTPUT_CTRL,
        BIT_SS
        );                                                                      /* The shift direction of outputs is from S720 to S1        */
    setReg(
        I_LCD_DRIVING_WAVE_CTRL,
        BIT_LINE_INVERSION
        );                                                                      /* Set line inversion                                       */

#if (OPT_DISP_ORIENTATION == 0)
    setReg(
        I_ENTRY_MODE,
        BIT_SWAP_RGB |
        BITS_ORIENT_0
        );
#elif (OPT_DISP_ORIENTATION == 90)
    setReg(
        I_ENTRY_MODE,
        BIT_SWAP_RGB |
        BITS_ORIENT_90
        );
#elif (OPT_DISP_ORIENTATION == 180)
    setReg(
        I_ENTRY_MODE,
        BIT_SWAP_RGB |
        BITS_ORIENT_180
        );
#elif (OPT_DISP_ORIENTATION == 270)
    setReg(
        I_ENTRY_MODE,
        BIT_SWAP_RGB |
        BITS_ORIENT_270
        );
#endif
    setReg(
        I_DISP_CTRL_2,
        0x0202                                                                  /* Front Porch, back porch = 2                              */
        );
    setReg(
        I_DISP_CTRL_3,
        0x0000                                                                  /* Non display area: 0 frame, normal scan, source output    */
        );                                                                      /* level, VCOM normal                                       */

    setReg(
        I_RGB_DISP_INTF_CTRL_1,                                                 /* RGB interfejs                                            */
        0x0000
        );
    setReg(
        I_RGB_DISP_INTF_CTRL_2,                                                 /* RGB interfejs                                            */
        0x0000
        );
    setReg(
        I_PWR_CTRL_1,
        0x0000                                                                  /* Inicijalizacija na nulu                                  */
        );
    setReg(
        I_PWR_CTRL_3,
        0x0000                                                                  /* Inicijalizacija na nulu                                  */
        );
    setReg(
        I_PWR_CTRL_4,
        0x0000                                                                  /* Inicijalizacija na nulu                                  */
        );
    WAIT_MS(200);
    setReg(
        I_PWR_CTRL_1,
        BIT_SAP |                                                               /* Source driver is enabled                                 */
        BITS_AP(0x1) |                                                          /* Gamma i source pojacavaci na maksimalno pojacanje        */
        BIT_APE |                                                               /* Power supply enable bit: FIXME: mozda treba kasnije da se*/
                                                                                /* ukljuci                                                  */
        BITS_BT(0x1)                                                            /* Step-up faktor za napon napajanja                        */
        );
    setReg(
        I_PWR_CTRL_2,
        BITS_DC0(0x0) |                                                         /* Step-up 1 Fosc                                           */
        BITS_DC1(0x1) |                                                         /* Step-up 2 Fosc/8                                         */
        BITS_VC(0x7)                                                            /* Faktor VciLVL za VciOUT i Vci1                           */
        );
    WAIT_MS(50);
    setReg(
        I_PWR_CTRL_3,
        BIT_VREG1R |                                                            /* Internal reference voltage                               */
        BIT_PSON |                                                              /* Bit nije dokumentovan, ali sigurno treba da se ukljuci   */
        BIT_PON |                                                               /* VGL output is enabled                                    */
        BIT_UNDOC102_1 |                                                        /* Bit ne postoji u r102                                    */
        BITS_VRH(0xD)                                                           /* 2.5V x 1.85 = 4.625V                                     */
        );
    WAIT_MS(50);
    setReg(
        I_PWR_CTRL_4,
        BIT_VCOMG |                                                             /* VCOM output low is fixed to VCOML                        */
        BITS_VDV(0x15)                                                          /* VREG1OUT x 1.12 (mozda 0x0D)                             */
        );
    setReg(
        I_VCOMH,
        0x0017                                                                  /* VREG1OUT x 0.800                                         */
        );
    setReg(
        I_GRAM_ADDR_H_SET,
        0x0000                                                                  /* GRAM horizontal Address                                  */
        );
    setReg(
        I_GRAM_ADDR_V_SET,
        0x0000                                                                  /* GRAM vertical Address                                    */
        );
    setReg(
        I_GAMMA_CTRL_1,
        0x0000
        );
    setReg(
        I_GAMMA_CTRL_2,
        0x0707
        );
    setReg(
        I_GAMMA_CTRL_3,
        0x0606
        );
    setReg(
        I_GAMMA_CTRL_4,
        0x0006
        );
    setReg(
        I_GAMMA_CTRL_5,
        0x0A09
        );
    setReg(
        I_GAMMA_CTRL_6,
        0x0203
        );
    setReg(
        I_GAMMA_CTRL_7,
        0x0005
        );
    setReg(
        I_GAMMA_CTRL_8,
        0x0007
        );
    setReg(
        I_GAMMA_CTRL_9,
        0x0400
        );
    setReg(
        I_GAMMA_CTRL_10,
        0x000A
        );
    setReg(
        I_H_ADDR_START_POS,                                                     /* Horizontal GRAM Start Address                            */
        0U
        );
    setReg(
        I_H_ADDR_END_POS,                                                       /* Horizontal GRAM End Address                              */
        OPT_MAX_X
        );
    setReg(
        I_V_ADDR_START_POS,                                                     /* Vertical GRAM Start Address                              */
        0U
        );
    setReg(
        I_V_ADDR_END_POS,                                                       /* Vertical GRAM End Address                                */
        OPT_MAX_Y
        );
    setReg(
        I_BASE_IMG_DISP_CTRL_1,
        BITS_NL(0x35)                                                           /* LCD drive line = 432                                     */
        );
    setReg(
        I_BASE_IMG_DISP_CTRL_2,
        BIT_REV                                                                 /* Reverse grayscale                                        */
        );
    setReg(
        I_BASE_IMG_DISP_CTRL_3,                                                 /* Sets the scrolling amount of base image                  */
        0x0000
        );
    setReg(
        I_PART_IMG_1_DISP_POS,                                                  /* Partial Image 1 Display Position                         */
        0x0000
        );
    setReg(
        I_PART_IMG_1_AREA_START,                                                /* Partial Image 1 RAM Start Address                        */
        0x0000
        );
    setReg(
        I_PART_IMG_1_AREA_END,                                                  /* Partial Image 1 RAM End Address                          */
        0x0000
        );
    setReg(
        I_PART_IMG_2_DISP_POS,                                                  /* Partial Image 2 Display Position                         */
        0x0000
        );
    setReg(
        I_PART_IMG_2_AREA_START,                                                /* Partial Image 2 RAM Start Address                        */
        0x0000
        );
    setReg(
        I_PART_IMG_2_AREA_END,                                                  /* Partial Image 2 RAM End Address                          */
        0x0000
        );
    setReg(
        I_PANEL_INTF_CTRL_1,
        BITS_RTNI(0x10)                                                         /* Clock number of internal clock operating mode            */
        );
    setReg(
        I_PANEL_INTF_CTRL_2,
        0x0000                                                                  /* FIXME: mozda treba BITS_NOWI = 0x6                       */
        );
    setReg(
        I_PANEL_INTF_CTRL_4,
        BITS_RTNE(0x2)
        );
    setReg(
        I_FRAME_RATE_COLOR_CTRL,
        0x0                                                                     /* Frame rate = 95                                          */
        );
    setReg(
        I_DISP_CTRL_1,
        BIT_BASEE |                                                             /* Base image display enable bit                            */
        BIT_GON |                                                               /* GON = DTE = 1 VHG/CGL Normal Display                     */
        BIT_DTE |
        BITS_D(0x7)
        );
}

void I_wrPixel(
    uint16_t xCord,
    uint16_t yCord,
    uint16_t color) {

    uint32_t address;

#if (OPT_DISP_ORIENTATION == 0)
    address = (yCord * 0x100) + xCord;
#elif (OPT_DISP_ORIENTATION == 90)
    address = (xCord * 0x100) + (OPT_MAX_Y - yCord);
#elif (OPT_DISP_ORIENTATION == 180)
    address = ((OPT_MAX_Y - yCord) * 0x100) + (OPT_MAX_X - xCord);
#elif (OPT_DISP_ORIENTATION == 270)
    address = ((OPT_MAX_X - xCord) * 0x100) + yCord;
#endif

    setAddres(address);
    setReg(I_GRAM_WR_DATA, color);
}

static void wrIndex(
    uint16_t index) {

    LCD_ENABLE();
    SPI_MODE_8B();
    SPI_TX(LCD_START_BYTE(INDEX_WR));
    SPI_MODE_16B();
    SPI_TX(index);
    LCD_DISABLE();
}

static void wrData(
    uint16_t data) {

    LCD_ENABLE();
    SPI_MODE_8B();
    SPI_TX(LCD_START_BYTE(REG_WR));
    SPI_MODE_16B();
    SPI_TX(data);
    LCD_DISABLE();
}

static void setReg(
    uint16_t reg,
    uint16_t value) {

    wrIndex(reg);
    wrData(value);
}

static void setAddres(
    uint32_t addr) {

    setReg(
        I_GRAM_ADDR_H_SET,
        addr & 0x00FF
        );
    setReg(
        I_GRAM_ADDR_V_SET,
        addr >> 8
        );
}


/*************************************************************************************************
 * GLOBAL FUNCTION DEFINITIONS
 *************************************************************************************************/


/*************************************************************************************************
 * LOCAL FUNCTION DEFINITIONS
 *************************************************************************************************/


/*************************************************************************************************
 * CONFIGURATION ERRORS
 *************************************************************************************//** @cond */


/** @endcond *//** @} *//*************************************************************************
 * END of ILI9326_lld.c
 *************************************************************************************************/
