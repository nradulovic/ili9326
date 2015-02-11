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
 * @brief       Implementacija ILI9326 Low Level Driver-a (LLD).
 *
 * ------------------------------------------------------------------------------------------------
 *
 * @addtogroup  ILI9326_lld_impl
 *
 ****************************************************************************************//** @{ */


/*************************************************************************************************
 * INCLUDE FILES
 *************************************************************************************************/

#include "ILI9326_lld.h"


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

#if defined(__GNUC__)
# define C_INLINE                       static inline __attribute__((always_inline))
#else
# define C_INLINE                       static
#endif

#define VALID_BOUNDARIES(x,y)                                                   \
    ((window_.x1 =< x) && (window_.x2 => x) && (window_.y1 =< y) && (window_.y2 => y))

/*-----------------------------------------------------------------------------------------------*/
/**
 * @brief       CS = 1
 */
/*-----------------------------------------------------------------------------------------------*/
#define LCD_DISABLE()                                                           \
    GPIO_SetValue(OPT_LCD_CS_PORT, (1U << OPT_LCD_CS_PIN))

/*-----------------------------------------------------------------------------------------------*/
/**
 * @brief       CS = 0
 */
/*-----------------------------------------------------------------------------------------------*/
#define LCD_ENABLE()                                                            \
    GPIO_ClearValue(OPT_LCD_CS_PORT, (1U << OPT_LCD_CS_PIN))

/*-----------------------------------------------------------------------------------------------*/
/**
 * @brief       LED = 0
 */
/*-----------------------------------------------------------------------------------------------*/
#define LCD_LED_DISABLE()                                                       \
    GPIO_ClearValue(OPT_LCD_LED_PORT, (1U << OPT_LCD_LED_PIN))

/*-----------------------------------------------------------------------------------------------*/
/**
 * @brief       LED = 1
 */
/*-----------------------------------------------------------------------------------------------*/
#define LCD_LED_ENABLE()                                                        \
    GPIO_SetValue(OPT_LCD_LED_PORT, (1U << OPT_LCD_LED_PIN))

/*-----------------------------------------------------------------------------------------------*/
/**
 * @brief       RST = 0
 */
/*-----------------------------------------------------------------------------------------------*/
#define LCD_RESET_ACTIVE()                                                      \
    GPIO_ClearValue(OPT_LCD_RST_PORT, (1U << OPT_LCD_RST_PIN))

/*-----------------------------------------------------------------------------------------------*/
/**
 * @brief       RST = 1
 */
/*-----------------------------------------------------------------------------------------------*/
#define LCD_RESET_INACTIVE()                                                    \
    GPIO_SetValue(OPT_LCD_RST_PORT, (1U << OPT_LCD_RST_PIN))

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
        while(LPC_SSP0->SR & (1 << 4));                                         \
        SSP_Cmd(LPC_SSP0, DISABLE);                                             \
        SSP_cfgStruct.Databit = SSP_DATABIT_8;                                  \
        SSP_Init(LPC_SSP0, &SSP_cfgStruct);                                     \
        SSP_Cmd(LPC_SSP0, ENABLE);                                              \
    } while (0)

/*-----------------------------------------------------------------------------------------------*/
/**
 * @brief       Prebacivanje u 16-bitni rezim rada SPI modula
 */
/*-----------------------------------------------------------------------------------------------*/
#define SPI_MODE_16B()                                                          \
    do {                                                                        \
        while(LPC_SSP0->SR & (1 << 4));                                         \
        SSP_Cmd(LPC_SSP0, DISABLE);                                             \
        SSP_cfgStruct.Databit = SSP_DATABIT_16;                                 \
        SSP_Init(LPC_SSP0, &SSP_cfgStruct);                                     \
        SSP_Cmd(LPC_SSP0, ENABLE);                                              \
    } while (0)

/*-----------------------------------------------------------------------------------------------*/
/**
 * @brief       Inicijalizacija GPIO modula
 */
/*-----------------------------------------------------------------------------------------------*/
#define GPIO_INIT()                                                             \
    do {                                                                        \
        PINSEL_CFG_Type PinCfg;                                                 \
        PinCfg.Funcnum = PINSEL_FUNC_0;                                         \
        PinCfg.OpenDrain = PINSEL_PINMODE_OPENDRAIN;                            \
        PinCfg.Pinmode = PINSEL_PINMODE_TRISTATE;                               \
        PinCfg.Portnum = OPT_LCD_LED_PORT;                                      \
        PinCfg.Pinnum = OPT_LCD_LED_PIN;                                        \
        PINSEL_ConfigPin(&PinCfg);                                              \
        GPIO_SetDir(OPT_LCD_LED_PORT, (1U << OPT_LCD_LED_PIN), 1);              \
        GPIO_SetDir(OPT_LCD_CS_PORT, (1U << OPT_LCD_CS_PIN), 1);                \
        GPIO_SetDir(OPT_LCD_RST_PORT, (1U << OPT_LCD_RST_PIN), 1);              \
    } while (0)

/*-----------------------------------------------------------------------------------------------*/
/**
 * @brief       Inicijalizacija SPI modula - mala brzina
 */
/*-----------------------------------------------------------------------------------------------*/
#define SPI_INIT()                                                              \
    do {                                                                        \
        PINSEL_CFG_Type PinCfg;                                                 \
        CLKPWR_SetPCLKDiv(CLKPWR_PCLKSEL_SSP0, CLKPWR_PCLKSEL_CCLK_DIV_1);      \
        PinCfg.Funcnum = PINSEL_FUNC_2;                                         \
        PinCfg.OpenDrain = PINSEL_PINMODE_NORMAL;                               \
        PinCfg.Pinmode = PINSEL_PINMODE_TRISTATE;                               \
        PinCfg.Portnum = PINSEL_PORT_0;                                         \
        PinCfg.Pinnum = 15;                                                     \
        PINSEL_ConfigPin(&PinCfg);                                              \
        PinCfg.Pinnum = 17;                                                     \
        PINSEL_ConfigPin(&PinCfg);                                              \
        PinCfg.Pinnum = 18;                                                     \
        PINSEL_ConfigPin(&PinCfg);                                              \
        SSP_ConfigStructInit(&SSP_cfgStruct);                                   \
        SSP_cfgStruct.ClockRate = 10000;                                        \
        SSP_cfgStruct.CPHA = OPT_SSP_PHA;                                       \
        SSP_cfgStruct.CPOL = OPT_SSP_POL;                                       \
        SSP_cfgStruct.Databit = SSP_DATABIT_8;                                  \
        SSP_cfgStruct.FrameFormat = SSP_FRAME_SPI;                              \
        SSP_Init(LPC_SSP0, &SSP_cfgStruct);                                     \
        SSP_Cmd(LPC_SSP0, ENABLE);                                              \
    } while (0)

/*-----------------------------------------------------------------------------------------------*/
/**
 * @brief       Inicijalizacija SPI modula - velika brzina
 */
/*-----------------------------------------------------------------------------------------------*/
#define SPI_INIT_FASTEST()                                                      \
    do {                                                                        \
        SSP_Cmd(LPC_SSP0, DISABLE);                                             \
        SSP_ConfigStructInit(&SSP_cfgStruct);                                   \
        SSP_cfgStruct.ClockRate = OPT_SPI_SPEED;                                \
        SSP_cfgStruct.CPHA = OPT_SSP_PHA;                                       \
        SSP_cfgStruct.CPOL = OPT_SSP_POL;                                       \
        SSP_cfgStruct.Databit = SSP_DATABIT_8;                                  \
        SSP_cfgStruct.FrameFormat = SSP_FRAME_SPI;                              \
        SSP_Init(LPC_SSP0, &SSP_cfgStruct);                                     \
        SSP_Cmd(LPC_SSP0, ENABLE);                                              \
    } while (0)

/*-----------------------------------------------------------------------------------------------*/
/**
 * @brief       Slanje podataka sa SPI modula
 */
/*-----------------------------------------------------------------------------------------------*/
#define SPI_TX(data)                                                            \
    SSP_SendData(LPC_SSP0, data)

/*-----------------------------------------------------------------------------------------------*/
/**
 * @brief       Cekanje u milisekundama
 */
/*-----------------------------------------------------------------------------------------------*/
#define WAIT_MS(time)                                                           \
    wait_ms(time)

/*-----------------------------------------------------------------------------------------------*/
/**
 * @brief       Pocetak sukcesivnog upisa boja pixela
 */
/*-----------------------------------------------------------------------------------------------*/
#define WR_PIXEL_BEGIN()                                                        \
    do {                                                                        \
        LCD_ENABLE();                                                           \
        SPI_TX(LCD_START_BYTE(INDEX_WR));                                       \
        SPI_MODE_16B();                                                         \
        SPI_TX(I_GRAM_WR_DATA);                                                 \
        SPI_MODE_8B();                                                          \
        LCD_DISABLE();                                                          \
        LCD_ENABLE();                                                           \
        SPI_TX(LCD_START_BYTE(REG_WR));                                         \
        SPI_MODE_16B();                                                         \
    } while (0)

/*-----------------------------------------------------------------------------------------------*/
/**
 * @brief       Upis boje jednog piksela
 */
/*-----------------------------------------------------------------------------------------------*/
#define WR_PIXEL(color)                                                         \
    SPI_TX(color)

/*-----------------------------------------------------------------------------------------------*/
/**
 * @brief       Zavrsetak sukcesivnog upisa boja
 */
/*-----------------------------------------------------------------------------------------------*/
#define WR_PIXEL_END()                                                          \
    do {                                                                        \
        SPI_MODE_8B();                                                          \
        LCD_DISABLE();                                                          \
    } while (0)


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

static SSP_CFG_Type SSP_cfgStruct;
static lcdOrient_T orientation_;


/*************************************************************************************************
 * LOCAL FUNCTION PROTOTYPES
 *************************************************************************************************/

C_INLINE uint16_t getMaxX(
    void);

C_INLINE uint16_t getMaxY(
    void);

C_INLINE void wrIndex(
    uint16_t index);

C_INLINE void wrData(
    uint16_t data);

C_INLINE void setReg(
    uint16_t reg,
    uint16_t value);

void setAddres(
    uint16_t x,
    uint16_t y);


/*************************************************************************************************
 ***                                I M P L E M E N T A T I O N                                ***
 *************************************************************************************************/


/*************************************************************************************************
 * GLOBAL FUNCTION DEFINITIONS
 *************************************************************************************************/

void I_drawPoint(
    uint16_t x,
    uint16_t y,
    uint16_t color) {

    setAddres(
        x,
        y);
    setReg(
        I_GRAM_WR_DATA,
        color);
}

void I_drawHLine(
    uint16_t x,
    uint16_t y,
    uint16_t length,
    uint16_t color) {

    uint32_t i;

    for (i = 0; i < length; i++) {
        I_drawPoint(
            x + i,
            y,
            color);
    }
}

void I_drawVLine(
    uint16_t x,
    uint16_t y,
    uint16_t length,
    uint16_t color) {

    uint32_t i;

    setAddres(x, y);

    WR_PIXEL_BEGIN();
    for (i = 0; i < length; i++) {
        WR_PIXEL(color);
    }
    WR_PIXEL_END();
}

uint16_t I_getMaxX(
    void) {

    return (getMaxX());
}

uint16_t I_getMaxY(
    void) {

    return (getMaxY());
}

void I_orientSet(
    lcdOrient_T orientation) {

    switch (orientation) {

        case I_ORIENT_0 : {
            orientation_ = I_ORIENT_0;
            setReg(
                I_ENTRY_MODE,
                BIT_BGR | BITS_IDIR(0x03));
            break;
        }

        case I_ORIENT_90 : {
            orientation_ = I_ORIENT_90;
            setReg(
                I_ENTRY_MODE,
                BIT_BGR | BIT_AM | BITS_IDIR(0x02));
            break;
        }

        case I_ORIENT_180 : {
            orientation_ = I_ORIENT_180;
            setReg(
                I_ENTRY_MODE,
                BIT_BGR);
            break;
        }

        case I_ORIENT_270 : {
            orientation_ = I_ORIENT_270;
            setReg(
                I_ENTRY_MODE,
                BIT_BGR | BIT_AM | BITS_IDIR(0x01));
            break;
        }

        default : {
#if (OPT_DISP_ORIENTATION == 0)
            orientation_ = I_ORIENT_0;
            setReg(
                I_ENTRY_MODE,
                BIT_BGR | BITS_IDIR(0x03));
#elif (OPT_DISP_ORIENTATION == 90)
            orientation_ = I_ORIENT_90;
            setReg(
                I_ENTRY_MODE,
                BIT_BGR | BIT_AM | BITS_IDIR(0x02));
#elif (OPT_DISP_ORIENTATION == 180)
            orientation_ = I_ORIENT_180;
            setReg(
                I_ENTRY_MODE,
                BIT_BGR);
#elif (OPT_DISP_ORIENTATION == 270)
            orientation_ = I_ORIENT_270;
            setReg(
                I_ENTRY_MODE,
                BIT_BGR | BIT_AM | BITS_IDIR(0x01));
#endif
            break;
        }
    }
}

lcdOrient_T I_orientGet(
    void) {

    return orientation_;
}

void I_pixelWR(
    uint16_t color) {

    WR_PIXEL(color);
}

void I_pixelWRbegin(
    void) {

    WR_PIXEL_BEGIN();
}

void I_pixelWRend(
    void) {


    WR_PIXEL_END();
}

void I_resetDevice(
    void) {

    GPIO_INIT();
    SPI_INIT();
    LCD_LED_DISABLE();
    LCD_RESET_INACTIVE();
    LCD_DISABLE();
    WAIT_MS(100);
    LCD_RESET_ACTIVE();
    WAIT_MS(10);
    LCD_RESET_INACTIVE();
    WAIT_MS(10);

/*-----------------------------------------------------------------------------------------------*
 * Display OFF setting
 *-----------------------------------------------------------------------------------------------*/
    setReg(
        0x0702,
        0x3008);                                                                /* Set internal timing,don't change this value              */
    setReg(
        0x0705,
        0x0036);                                                                /* Set internal timing,don't change this value              */
    setReg(
        0x070B,
        0x1213);                                                                /* Set internal timing,don't change this value              */
    setReg(
        I_DISP_CTRL_1,
        0x0000);
    setReg(
        I_PWR_CTRL_1,
        0x0000);
    setReg(
        I_PWR_CTRL_3,
        0x0000);
    setReg(
        I_PWR_CTRL_4,
        0x0000);
    WAIT_MS(10);

/*-----------------------------------------------------------------------------------------------*
 * Power supply initial setting VC, VDV, VRH, VCM
 *-----------------------------------------------------------------------------------------------*/
    setReg(
        I_PWR_CTRL_2,
        BITS_VC(OPT_VC));                                                       /* Vc = 2.475V                                              */
    WAIT_MS(10);
    setReg(
        I_PWR_CTRL_3,
        BITS_VRH(OPT_VRH));                                                     /* Vrh = 4.33V                                              */
    WAIT_MS(10);
    setReg(
        I_VCOMH,
        OPT_VCM);                                                               /* Vcm = 4.33V                                              */
    WAIT_MS(10);
    setReg(
        I_PWR_CTRL_4,
        BITS_VDV(OPT_VDV));                                                     /* Vdv = 5.6V                                               */
    WAIT_MS(100);

/*-----------------------------------------------------------------------------------------------*
 * Power supply operation setting, part 1
 *-----------------------------------------------------------------------------------------------*/
    setReg(
        I_PWR_CTRL_1,
        BITS_BT(0x0) | BITS_AP(OPT_AP));                                        /* BT = 0, AP = gamma and source driver config              */
    WAIT_MS(10);
    setReg(
        I_PWR_CTRL_2,
        BITS_VC(OPT_VC) | BITS_DC0(OPT_DC0) | BITS_DC1(OPT_DC1));               /* DC0 i DC1 izvori konstantne struje                       */
    WAIT_MS(10);
    setReg(
        I_PWR_CTRL_3,
        BITS_VRH(OPT_VRH) | BIT_PON | BIT_PSON | BIT_VREG1R(OPT_VREG1R));       /* PON = 1, Vgl output is enabled                           */
    WAIT_MS(10);
    setReg(
        I_PWR_CTRL_1,
        BITS_BT(0x0) | BITS_AP(OPT_AP) | BIT_APE);                              /* APE = 1, start the generation of power supply            */
    WAIT_MS(200);

/*-----------------------------------------------------------------------------------------------*
 * Power supply operation setting, part 2
 *-----------------------------------------------------------------------------------------------*/
    setReg(
        I_PWR_CTRL_1,
        BITS_BT(OPT_BT) | BITS_AP(OPT_AP) | BIT_APE);                           /* set Vgh, Vgl, Vcl, Vlcd                                  */
    WAIT_MS(10);
    setReg(
        I_PWR_CTRL_4,
        BITS_VDV(OPT_VDV) | BIT_VCOMG);                                         /* VCOMG = 1, VCOM output low is fixed to VCOML             */
    WAIT_MS(200);

/*-----------------------------------------------------------------------------------------------*
 * Ostala podesavanja
 *-----------------------------------------------------------------------------------------------*/
    setReg(
        I_GRAM_ADDR_H_SET,
        0x0000);
    setReg(
        I_GRAM_ADDR_V_SET,
        0x0000);
    setReg(
        I_DISP_CTRL_2,
        0x0808);                                                                /* set the back porch and front porch                       */
    setReg(
        I_DISP_CTRL_3,
        0x0000);                                                                /* set non-display area refresh cycle ISC[3:0]              */
    setReg(
        I_RGB_DISP_INTF_CTRL_1 ,
        0x0000);                                                                /* RGB interface setting 16 bits color depth                */
    setReg(
        I_RGB_DISP_INTF_CTRL_2,
        0x0000);                                                                /* RGB interface polarity                                   */

    /*
     * Podesavanje gamma krive
     */
    setReg(
        I_GAMMA_CTRL_1,
        0x0000);
    setReg(
        I_GAMMA_CTRL_2,
        0x0707);
    setReg(
        I_GAMMA_CTRL_3,
        0x0606);
    setReg(
        I_GAMMA_CTRL_4,
        0x0006);
    setReg(
        I_GAMMA_CTRL_5,
        0x0A09);
    setReg(
        I_GAMMA_CTRL_6,
        0x0203);
    setReg(
        I_GAMMA_CTRL_7,
        0x0005);
    setReg(
        I_GAMMA_CTRL_8,
        0x0007);
    setReg(
        I_GAMMA_CTRL_9,
        0x0400);
    setReg(
        I_GAMMA_CTRL_10,
        0x000A);

    /*
     * Podesavanje GRAM oblasti
     */
    setReg(
        I_H_ADDR_START_POS,                                                     /* Horizontal GRAM Start Address                            */
        0x0000);
    setReg(
        I_H_ADDR_END_POS,                                                       /* Horizontal GRAM End Address                              */
        OPT_DISP_X);
    setReg(
        I_V_ADDR_START_POS,                                                     /* Vertical GRAM Start Address                              */
        0x0000);
    setReg(
        I_V_ADDR_END_POS,                                                       /* Vertical GRAM End Address                                */
        OPT_DISP_Y);
    setReg(
        I_BASE_IMG_DISP_CTRL_1,
        BITS_NL(0x35));
    setReg(
        I_BASE_IMG_DISP_CTRL_2,
        BIT_REV);
    setReg(
        I_BASE_IMG_DISP_CTRL_3,                                                 /* Sets the scrolling amount of base image                  */
        0x0000);

    /*
     * Kontrola prikaza parcijalne slike
     */
    setReg(
        I_PART_IMG_1_DISP_POS,                                                  /* Partial Image 1 Display Position                         */
        0x0000);
    setReg(
        I_PART_IMG_1_AREA_START,                                                /* Partial Image 1 RAM Start Address                        */
        0x0000);
    setReg(
        I_PART_IMG_1_AREA_END,                                                  /* Partial Image 1 RAM End Address                          */
        0x0000);
    setReg(
        I_PART_IMG_2_DISP_POS,                                                  /* Partial Image 2 Display Position                         */
        0x0000);
    setReg(
        I_PART_IMG_2_AREA_START,                                                /* Partial Image 2 RAM Start Address                        */
        0x0000);
    setReg(
        I_PART_IMG_2_AREA_END,                                                  /* Partial Image 2 RAM End Address                          */
        0x0000);

    /*
     * Kontrola panel-a
     */
    setReg(
        I_PANEL_INTF_CTRL_1,
        BITS_RTNI(0x10));
    setReg(
        I_PANEL_INTF_CTRL_2,
        0x0000);
    setReg(
        I_PANEL_INTF_CTRL_4,
        BITS_RTNE(0x2));
    setReg(
        I_FRAME_RATE_COLOR_CTRL,
        0x0000);

/*-----------------------------------------------------------------------------------------------*
 * Display ON sekvenca
 *-----------------------------------------------------------------------------------------------*/
    setReg(
        I_PWR_CTRL_1,
        BITS_BT(OPT_BT) | BITS_AP(OPT_AP) | BIT_APE | BIT_SAP);                 /* SAP = 1, Source driver is enabled                        */
    WAIT_MS(10);
    setReg(
        I_DISP_CTRL_1,
        BIT_BASEE | BIT_VON | BIT_GON | BITS_D(0x01));
    WAIT_MS(10);
    setReg(
        I_DISP_CTRL_1,
        BIT_BASEE | BIT_VON | BIT_GON | BITS_D(0x03));
    WAIT_MS(10);
    setReg(
        I_DISP_CTRL_1,
        BIT_BASEE | BIT_VON | BIT_GON | BIT_DTE | BITS_D(0x03));
    WAIT_MS(10);
    SPI_INIT_FASTEST();
    WAIT_MS(10);
    I_orientSet(
        I_ORIENT_NORMAL);
    LCD_LED_ENABLE();
}

void I_windowReset(
    void) {

    setReg(
        I_H_ADDR_START_POS,
        0);
    setReg(
        I_H_ADDR_END_POS,
        OPT_DISP_X);
    setReg(
        I_V_ADDR_START_POS,
        0);
    setReg(
        I_V_ADDR_END_POS,
        OPT_DISP_Y);
}

void I_windowSet(
    uint16_t x1,
    uint16_t y1,
    uint16_t x2,
    uint16_t y2) {

    uint16_t mx1;
    uint16_t mx2;
    uint16_t my1;
    uint16_t my2;
    uint16_t sizeY;
    uint16_t sizeX;

    if (x2 > getMaxY()) {                                                       /* Ovde su obrnute X i Y koordinate                         */
        x2 = getMaxY();
    }

    if (y2 > getMaxX()) {
        y2 = getMaxX();
    }

    sizeX = x2 - x1;
    sizeY = y2 - y1;

    switch (orientation_) {

        case I_ORIENT_90 : {
            mx1 = getMaxY() - y1 - sizeY;
            mx2 = getMaxY() - y1;
            my1 = x1;
            my2 = x1 + sizeX;
            break;
        }

        case I_ORIENT_180 : {                                                   /* Ovde su obrnute X i Y koordinate                         */
            mx1 = getMaxX() - y1 - sizeY;
            mx2 = getMaxX() - y1;
            my1 = getMaxY() - x1 - sizeX;
            my2 = getMaxY() - x1;
            break;
        }

        case I_ORIENT_270 : {
            mx1 = y1;
            mx2 = y1 + sizeY;
            my1 = getMaxX() - x1 -sizeX;
            my2 = getMaxX() - x1;
            break;
        }

        default : {
            mx1 = x1;
            mx2 = x2;
            my1 = y1;
            my2 = y2;
            break;
        }
    }
//    setReg(
//        I_H_ADDR_START_POS,
//        mx1);
//    setReg(
//        I_H_ADDR_END_POS,
//        mx2);
//    setReg(
//        I_V_ADDR_START_POS,
//        my1);
//    setReg(
//        I_V_ADDR_END_POS,
//        my2);

    setReg(
        I_H_ADDR_START_POS,
        mx1);
    setReg(
        I_H_ADDR_END_POS,
        mx2);
    setReg(
        I_V_ADDR_START_POS,
        my1);
    setReg(
        I_V_ADDR_END_POS,
        my2);
    setAddres(
        x1,
        y1);
}


/*************************************************************************************************
 * LOCAL FUNCTION DEFINITIONS
 *************************************************************************************************/

C_INLINE uint16_t getMaxX(
    void) {

    if ((I_ORIENT_90 == orientation_) || (I_ORIENT_270 == orientation_)) {

        return (OPT_DISP_Y);
    } else {

        return (OPT_DISP_X);
    }
}

C_INLINE uint16_t getMaxY(
    void) {

    if ((I_ORIENT_90 == orientation_) || (I_ORIENT_270 == orientation_)) {

        return (OPT_DISP_X);
    } else {

        return (OPT_DISP_Y);
    }
}

C_INLINE void wrIndex(
    uint16_t index) {

    LCD_ENABLE();
    SPI_TX(LCD_START_BYTE(INDEX_WR));
    SPI_MODE_16B();
    SPI_TX(index);
    SPI_MODE_8B();
    LCD_DISABLE();
}

C_INLINE void wrData(
    uint16_t data) {

    LCD_ENABLE();
    SPI_TX(LCD_START_BYTE(REG_WR));
    SPI_MODE_16B();
    SPI_TX(data);
    SPI_MODE_8B();
    LCD_DISABLE();
}

C_INLINE void setReg(
    uint16_t reg,
    uint16_t value) {

    wrIndex(
        reg);
    wrData(
        value);
}

void setAddres(
    uint16_t x,
    uint16_t y) {

    uint32_t addr;

    switch (orientation_) {

        case I_ORIENT_90 : {
            addr = (x * 0x100) + (getMaxY() - y);
            break;
        }

        case I_ORIENT_180 : {
            addr = ((getMaxY() - x) * 0x100) +(getMaxX() - y);                  /* Obrnute su X i Y koordinate                              */
            break;
        }

        case I_ORIENT_270 : {
            addr = ((getMaxX() - x) * 0x100) + y;
            break;
        }

        default : {
            addr = (y * 0x100) + x;
            break;
        }
    }

    setReg(
        I_GRAM_ADDR_H_SET,
        addr & 0x00FF);
    setReg(
        I_GRAM_ADDR_V_SET,
        addr >> 8);
}


/*************************************************************************************************
 * CONFIGURATION ERRORS
 *************************************************************************************//** @cond */


/** @endcond *//** @} *//*************************************************************************
 * END of ILI9326_lld.c
 *************************************************************************************************/
