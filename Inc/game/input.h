/*
 * input.h
 *
 *  Created on: Jun 28, 2026
 *      Author: Rogyr
 */

#ifndef INC_GAME_INPUT_H_
#define INC_GAME_INPUT_H_

#include "drivers/HW504.h"

typedef enum{
	BUTTON_IDLE = 0,
	BUTTON_DEBOUNCE,
	BUTTON_PRESSED,
	BUTTON_RELEASED
}ButtonState;

typedef struct{
	ButtonState		ButtonState;
	GPIO_TypeDef*	GPIOPort;
	uint16_t 		GPIOPin;
	uint32_t		Tick;
	uint32_t		Debounce;
	void(*PressedTask)(void);
	void(*ReleasedTask)(void);
}ButtonHandle_TypeDef;

void InputInit(ADC_HandleTypeDef* hadc, GPIO_TypeDef *btnPort, uint16_t btnPin);
GPIO_PinState InputRead(void);

void ButtonInit(ButtonHandle_TypeDef *btn, GPIO_TypeDef *GPIOPort, uint16_t GPIOPin, uint32_t DebounceValue);
void Button_SetDebounce(ButtonHandle_TypeDef *btn, uint32_t DebounceMS);
void Button_RunTask(ButtonHandle_TypeDef *btn);
void ButtonPressedCallback(ButtonHandle_TypeDef *btn, void *Callback);
void ButtonReleasedCallback(ButtonHandle_TypeDef *btn, void *Callback);
#endif /* INC_GAME_INPUT_H_ */
