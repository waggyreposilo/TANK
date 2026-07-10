/*
 * HW504.c
 * @creator: 	Wagner John Reposilo
 * @date: 		25/05/2026
 * @brief: 		This module functions to do various updates for HW504 via ADC.
 */

#ifndef INC_DRIVERS_HW504_H_
#define INC_DRIVERS_HW504_H_

#include <string.h>
#include <stdbool.h>

#include "main.h"

#define DMA_MODE	1

typedef struct{
	ADC_HandleTypeDef *hadc;				//ADC Handle
	GPIO_TypeDef* KeyPort;					//Joystick pushbutton GPIO port
	uint16_t KeyPin;						//Joystick pushbutton GPIO pin
}JoystickHandle_TypeDef;

typedef struct{
	uint16_t JoystickPosition[2];			//ADC Data
	uint8_t KeyState;						//State of the Joystick push button
}JoystickData_TypeDef;

void HW504_Init(JoystickHandle_TypeDef *JoystickHandle);
void HW504_AnalogDataRead(JoystickData_TypeDef *data);
void HW504_ReadKeyState(JoystickData_TypeDef *data);

#endif /* INC_DRIVERS_HW504_H_ */
