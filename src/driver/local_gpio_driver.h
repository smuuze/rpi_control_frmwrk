#ifndef _LOCAL_GPIO_DRIVER_H_
#define _LOCAL_GPIO_DRIVER_H_



void gpio_driver_set_direction(u8 pin_num, SYSTEM_INTERFACE_GPIO_DIRECTION direction);

void gpio_driver_set_level(u8 pin_num, SYSTEM_INTERFACE_IO_PIN_LEVEL level);

SYSTEM_INTERFACE_IO_PIN_LEVEL gpio_driver_get_level(u8 pin_num);

#endif