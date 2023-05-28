//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//       Pin-Map   :  Raspberry Pi 40 Pin Header (since RPI B1 Plus)
//-------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "driver/gpio/gpio_interface.h"

// +----------+-----+-------+---------+------+---+----------+---+------+---------+-------+-----+----------+
// | Function | BCM | gioPi | Name    | Mode | V | Physical | V | Mode | Name    | gioPi | BCM | Function |
// +----------+-----+-------+---------+------+---+----++----+---+------+---------+-------+-----+----------+
//            |     |       | 3V3     |      |   |  1 || 2  |   |      | 5V      |       |     |
//            |  2  |   0   | SDA.1   |   IN | 1 |  3 || 4  |   |      | 5V      |       |     |
//            |  3  |   1   | SCL.1   |   IN | 1 |  5 || 6  |   |      | GND     |       |     |
//            |  4  |   2   | GPIO  7 |   IN | 1 |  7 || 8  | 0 | IN   | TxD     |  12   | 14  |
//            |     |       | GND     |      |   |  9 || 10 | 0 | IN   | RxD     |  13   | 15  |
// |  REQUEST | 17  |  15   | GPIO  0 |   IN | 1 | 11 || 12 | 0 | IN   | GPIO  1 |  16   | 18  |
// | KP_COL_3 | 27  |  25   | GPIO  2 |   IN | 0 | 13 || 14 |   |      | GND     |       |     |
// | KP_COL_2 | 22  |  20   | GPIO  3 |   IN | 0 | 15 || 16 | 1 | IN   | GPIO  4 |  21   | 23  | KP_COL_1 |
//            |     |       | 3V3     |      |   | 17 || 18 | 0 | IN   | GPIO  5 |  22   | 24  | KP_ROW_1 |
//            | 10  |   8   | MOSI    |   IN | 0 | 19 || 20 |   |      | GND     |       |     |
//            |  9  |   7   | MISO    |   IN | 0 | 21 || 22 | 0 | IN   | GPIO  6 |  23   | 25  | KP_ROW_2 |
//            | 11  |   9   | SCLK    |   IN | 0 | 23 || 25 | 1 | IN   | CE0     |   6   | 8   |
//            |     |       | GND     |      |   | 25 || 26 | 1 | IN   | RxD     |   5   | 7   |
//            |     |       | RESERVE |      |   | 27 || 28 |   |      | RESERVE |       |     |
// | KP_ROW_3 |  5  |   3   | GPIO 21 |   IN | 1 | 29 || 30 |   |      | GND     |       |     |
// | KP_ROW_4 |  6  |   4   | GPIO 22 |   IN | 1 | 31 || 32 | 0 | IN   | GPIO 26 |  10   | 12  | LCD_D4   |
// |          | 13  |  11   | GPIO 23 |   IN | 0 | 33 || 34 |   |      | GND     |       |     |
// |   LCD_RS | 19  |  17   | GPIO 24 |   IN | 0 | 35 || 36 | 1 | IN   | GPIO 27 |  14   | 16  | LCD_D5   |
// |   LCD_EN | 26  |  24   | GPIO 25 |   IN | 0 | 37 || 38 | 0 | IN   | GPIO 28 |  18   | 20  | LCD_D6   |
//            |     |       | GND     |      |   | 39 || 40 | 0 | IN   | GPIO 29 |  19   | 21  | LCD_D7   |
// +----------+-----+-------+---------+------+---+----------+---+------+---------+-------+-----+----------+
// | Function | BCM | gioPi | Name    | Mode | V | Physical | V | Mode | Name    | gioPi | BCM | Function |
// +----------+-----+-------+---------+------+---+----++----+---+------+---------+-------+-----+----------+

//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// PORT A
//---------     Name,           Port,           Bit,        Pin-Cfg                             SHC             wiring-Pi   HiFi-Berry DAC+

//  BUILD_GPIO ( 3V3,           GPIO_PORT_A,    GPIO_PIN_0,    UNUSEABLE                   )
//  BUILD_GPIO ( 5V,            GPIO_PORT_A,    GPIO_PIN_1,    UNUSEABLE                   )
    BUILD_GPIO ( I2C_SDA,       GPIO_PORT_A,    GPIO_PIN_2,    GPIO_DEACTIVATE             ) //    I2C_SDA         GPIO_08     ALT0
//     BUILD_GPIO ( 5V,            GPIO_PORT_A,    GPIO_PIN_3,    UNUSEABLE                   ) //
    BUILD_GPIO ( I2C_SCL,       GPIO_PORT_A,    GPIO_PIN_4,    GPIO_DEACTIVATE             ) //    I2C_SCL         GPIO_09     ALT0
//  BUILD_GPIO ( GND,           GPIO_PORT_A,    GPIO_PIN_5,    UNUSEABLE                   ) //
    BUILD_GPIO ( GPIO04,        GPIO_PORT_A,    GPIO_PIN_6,    GPIO_INPUT | GPIO_IDLE_HIGH ) //    GPIO04          GPIO_07
    BUILD_GPIO ( UART_TXD,      GPIO_PORT_A,    GPIO_PIN_7,    GPIO_DEACTIVATE             ) //    GPIO_15

//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// PORT B
//---------     Name,           Port,           Bit,        Pin-Cfg                             SHC             wiring-Pi   HiFi-Berry DAC+

//  BUILD_GPIO ( GND,           GPIO_PORT_B,    GPIO_PIN_0,    UNUSEABLE                   ) //
    BUILD_GPIO ( UART_RXD,      GPIO_PORT_B,    GPIO_PIN_1,    GPIO_DEACTIVATE             ) //    DEBUG_RX        GPIO_16
    BUILD_GPIO ( GPIO17,        GPIO_PORT_B,    GPIO_PIN_2,    GPIO_INPUT                  ) //    REQUEST_CLIENT  GPIO_00
    BUILD_GPIO ( GPIO18,        GPIO_PORT_B,    GPIO_PIN_3,    GPIO_DEACTIVATE             ) //    DEACTIVATED     GPIO_01     ALT0
    BUILD_GPIO ( GPIO27,        GPIO_PORT_B,    GPIO_PIN_4,    GPIO_INPUT | GPIO_IDLE_HIGH    ) //    KEYPAD_COL_3    GPIO_02
//  BUILD_GPIO ( GND,           GPIO_PORT_B,    GPIO_PIN_5,    UNUSEABLE                   ) //
    BUILD_GPIO ( GPIO22,        GPIO_PORT_B,    GPIO_PIN_6,    GPIO_INPUT | GPIO_IDLE_HIGH    ) //    KEYPAD_COL_2    GPIO_03
    BUILD_GPIO ( GPIO23,        GPIO_PORT_B,    GPIO_PIN_7,    GPIO_INPUT | GPIO_IDLE_HIGH    ) //    KEYPAD_COL_1    GPIO_04

//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// PORT C
//---------     Name,           Port,           Bit,        Pin-Cfg                             SHC             wiring-Pi   HiFi-Berry DAC+

//  BUILD_GPIO ( 3V3,           GPIO_PORT_C,    GPIO_PIN_0, UNUSEABLE                   ) //
    BUILD_GPIO ( GPIO24,        GPIO_PORT_C,    GPIO_PIN_1, GPIO_INPUT | GPIO_IDLE_HIGH ) //    KEYPAD_ROW_1    GPIO_05
    BUILD_GPIO ( SPI_MOSI,      GPIO_PORT_C,    GPIO_PIN_2, GPIO_DEACTIVATE             ) //    SPI_MOSI        GPIO_12
//  BUILD_GPIO ( GND,           GPIO_PORT_C,    GPIO_PIN_3, UNUSEABLE                   ) //
    BUILD_GPIO ( SPI_MISO,      GPIO_PORT_C,    GPIO_PIN_4, GPIO_DEACTIVATE             ) //    SPI_MISO        GPIO_13
    BUILD_GPIO ( GPIO25,        GPIO_PORT_C,    GPIO_PIN_5, GPIO_INPUT | GPIO_IDLE_HIGH    ) //    KEYPAD_ROW_2    GPIO_06
    BUILD_GPIO ( SPI_SCLK,      GPIO_PORT_C,    GPIO_PIN_6, GPIO_DEACTIVATE             ) //    SPI_SCLK        GPIO_14
    BUILD_GPIO ( SPI_CE0,       GPIO_PORT_C,    GPIO_PIN_7, GPIO_DEACTIVATE             ) //    SPI0_CS         GPIO_10

//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// PORT D
//---------     Name,           Port,           Bit,        Pin-Cfg                             SHC             wiring-Pi   HiFi-Berry DAC+

//  BUILD_GPIO ( GND,           GPIO_PORT_D,    GPIO_PIN_0,    UNUSEABLE                   ) //
    BUILD_GPIO ( SPI_CE1,       GPIO_PORT_D,    GPIO_PIN_1,    GPIO_DEACTIVATE             ) //    SPI_CE1         GPIO_11
//  BUILD_GPIO ( ID_SD,         GPIO_PORT_D,    GPIO_PIN_2,    UNUSEABLE                   ) //
//  BUILD_GPIO ( ID_SC,         GPIO_PORT_D,    GPIO_PIN_3,    UNUSEABLE                   ) //
    BUILD_GPIO ( GPIO05,        GPIO_PORT_D,    GPIO_PIN_4,    GPIO_INPUT | GPIO_IDLE_HIGH    ) //    KEYPAD_ROW_3    GPIO_21
//  BUILD_GPIO ( GND,           GPIO_PORT_D,    GPIO_PIN_5,    UNUSEABLE                   ) /
    BUILD_GPIO ( GPIO06,        GPIO_PORT_D,    GPIO_PIN_6,    GPIO_INPUT | GPIO_IDLE_HIGH    ) //    KEYPAD_ROW_4    GPIO_22
    BUILD_GPIO ( GPIO12,        GPIO_PORT_D,    GPIO_PIN_7,    GPIO_INPUT | GPIO_IDLE_HIGH    ) //    LCD_D4          GPIO_26

//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// PORT E
//---------     Name,           Port,           Bit,        Pin-Cfg                                                 SHC     wiring-Pi   HiFi-Berry DAC+

    BUILD_GPIO ( GPIO13,        GPIO_PORT_E,    GPIO_PIN_0,    GPIO_INPUT | GPIO_IDLE_HIGH ) //                                GPIO_23
//  BUILD_GPIO ( GND,           GPIO_PORT_E,    GPIO_PIN_1,    UNUSEABLE                   ) //     
    BUILD_GPIO ( GPIO19,        GPIO_PORT_E,    GPIO_PIN_2,    GPIO_DEACTIVATE | GPIO_INPUT | GPIO_IDLE_HIGH    ) //    LCD_RS  GPIO_24     ALT0
    BUILD_GPIO ( GPIO16,        GPIO_PORT_E,    GPIO_PIN_3,    GPIO_DEACTIVATE | GPIO_INPUT | GPIO_IDLE_HIGH    ) //    LCD_D5  GPIO_27
    BUILD_GPIO ( GPIO26,        GPIO_PORT_E,    GPIO_PIN_4,    GPIO_DEACTIVATE | GPIO_INPUT | GPIO_IDLE_HIGH    ) //    LCD_EN  GPIO_25
    BUILD_GPIO ( GPIO20,        GPIO_PORT_E,    GPIO_PIN_5,    GPIO_DEACTIVATE | GPIO_INPUT | GPIO_IDLE_HIGH    ) //    LCD_D6  GPIO_28     ALT0
//  BUILD_GPIO ( GND,           GPIO_PORT_E,    GPIO_PIN_6,    UNUSEABLE                   ) //     
    BUILD_GPIO ( GPIO21,        GPIO_PORT_E,    GPIO_PIN_7,    GPIO_DEACTIVATE | GPIO_INPUT | GPIO_IDLE_HIGH    ) //    LCD_D7  GPIO_29     ALT0

//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// ALIAS - WiringPi
//---------         Alias-Name  Existing-Name

    GPIO_ALIAS (    GPIO_07,    GPIO04)

    GPIO_ALIAS (    GPIO_00,    GPIO17 )
    GPIO_ALIAS (    GPIO_01,    GPIO18 )
    GPIO_ALIAS (    GPIO_02,    GPIO27 )
    GPIO_ALIAS (    GPIO_03,    GPIO22 )
    GPIO_ALIAS (    GPIO_04,    GPIO23 )

    GPIO_ALIAS (    GPIO_05,    GPIO24 )
    GPIO_ALIAS (    GPIO_06,    GPIO25 )

    GPIO_ALIAS (    GPIO_21,    GPIO05 )
    GPIO_ALIAS (    GPIO_22,    GPIO06 )
    GPIO_ALIAS (    GPIO_26,    GPIO12 )

    GPIO_ALIAS (    GPIO_23,    GPIO13 )
    GPIO_ALIAS (    GPIO_24,    GPIO19 )
    GPIO_ALIAS (    GPIO_27,    GPIO16 )
    GPIO_ALIAS (    GPIO_25,    GPIO26 )
    GPIO_ALIAS (    GPIO_28,    GPIO20 )
    GPIO_ALIAS (    GPIO_29,    GPIO21 )

//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// ALIAS - ControlBoaard v2
//---------         Alias-Name ,    Existing-Name

    GPIO_ALIAS (    REQUEST_CLIENT,     GPIO17 )
    GPIO_ALIAS (    SPI0_CS,            SPI_CE0 )

//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// ALIAS - LCD 16x2
//---------         Alias-Name      Existing-Name

    GPIO_ALIAS (    LCD_RS,         GPIO19 )
    GPIO_ALIAS (    LCD_EN,         GPIO26 )
    GPIO_ALIAS (    LCD_D4,         GPIO12 )
    GPIO_ALIAS (    LCD_D5,         GPIO16 )
    GPIO_ALIAS (    LCD_D6,         GPIO20 )
    GPIO_ALIAS (    LCD_D7,         GPIO21 )

//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// ALIAS - KEYPAD 3X4
//---------         Alias-Name      Existing-Name

    GPIO_ALIAS (    KEY_COL_1,      GPIO23 )
    GPIO_ALIAS (    KEY_COL_2,      GPIO22 )
    GPIO_ALIAS (    KEY_COL_3,      GPIO27 )
    GPIO_ALIAS (    KEY_ROW_1,      GPIO24 )
    GPIO_ALIAS (    KEY_ROW_2,      GPIO25 )
    GPIO_ALIAS (    KEY_ROW_3,      GPIO05 )
    GPIO_ALIAS (    KEY_ROW_4,      GPIO06 )

//-------------------------------------------------------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------------------------------------------------------------
