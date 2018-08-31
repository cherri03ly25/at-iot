#ifndef BOARD_CUSTOM_H
#define BOARD_CUSTOM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nrf_gpio.h"

// LEDs definitions
#define LEDS_NUMBER    2

#define LED_1          NRF_GPIO_PIN_MAP(1,14)
#define LED_2          NRF_GPIO_PIN_MAP(1,12)
#define LED_START      LED_1
#define LED_STOP       LED_2

#define LEDS_ACTIVE_STATE 0

#define LEDS_LIST { LED_1, LED_2 }

#define LEDS_INV_MASK  LEDS_MASK


//#define BSP_LED_0      13
//#define BSP_LED_1      14
//#define BSP_LED_2      15
//#define BSP_LED_3      16

#define BUTTONS_NUMBER 1

#define BUTTON_1       10	//SWITCH
//#define BUTTON_2	18	//RESET

#define BUTTON_PULL    NRF_GPIO_PIN_PULLUP

#define BUTTONS_ACTIVE_STATE 0

#define BUTTONS_LIST { BUTTON_1 }
//#define BUTTONS_LIST { BUTTON_1, BUTTON_2}

#define BSP_BUTTON_0   BUTTON_1
//#define BSP_BUTTON_1   BUTTON_2

#define ARDUINO_SCL_PIN             26    // SCL signal pin
#define ARDUINO_SDA_PIN             25    // SDA signal pin



#ifdef __cplusplus
}
#endif

#endif // BOARD_CUSTOM_H
