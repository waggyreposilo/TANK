/*
 * input.c
 *
 *  Created on: Jun 28, 2026
 *      Author: Rogyr
 */

#include "game/input.h"

static ADC_HandleTypeDef *_hadc;
static bool _buttonPressed = false;
static ButtonHandle_TypeDef _btn;

static void onBtnPressed(void);
static void Button_RunIdleTask(ButtonHandle_TypeDef *btn);
static void Button_RunDebounceTask(ButtonHandle_TypeDef *btn);
static void Button_RunPressedTask(ButtonHandle_TypeDef *btn);
static void Button_RunReleaseTask(ButtonHandle_TypeDef *btn);

void InputInit(ADC_HandleTypeDef* hadc, GPIO_TypeDef *btnPort, uint16_t btnPin){
	_hadc = hadc;

	ButtonInit(&_btn, btnPort, btnPin, 16);
	ButtonPressedCallback(&_btn, onBtnPressed);
}

GPIO_PinState InputRead(void){
	Button_RunTask(&_btn);
	if(_buttonPressed){
		_buttonPressed = false;
		return GPIO_PIN_SET;
	}
	return GPIO_PIN_RESET;
}

void ButtonInit(ButtonHandle_TypeDef *btn, GPIO_TypeDef *GPIOPort, uint16_t GPIOPin, uint32_t DebounceValue){
	btn->ButtonState = BUTTON_IDLE;

	btn->GPIOPort = GPIOPort;
	btn->GPIOPin = GPIOPin;

	btn->Debounce = DebounceValue;
}

void Button_SetDebounce(ButtonHandle_TypeDef *btn, uint32_t DebounceMS){
	btn->Debounce = DebounceMS;
}

void ButtonPressedCallback(ButtonHandle_TypeDef *btn, void *Callback){
	btn->PressedTask = Callback;
}

void ButtonReleasedCallback(ButtonHandle_TypeDef *btn, void *Callback){
	btn->ReleasedTask = Callback;
}

void Button_RunTask(ButtonHandle_TypeDef *btn){
	switch(btn->ButtonState){
	case BUTTON_IDLE:		Button_RunIdleTask(btn); 	break;
	case BUTTON_DEBOUNCE:	Button_RunDebounceTask(btn);break;
	case BUTTON_PRESSED:	Button_RunPressedTask(btn); break;
	case BUTTON_RELEASED:	Button_RunReleaseTask(btn);	break;
	}
}

static void onBtnPressed(void){
	_buttonPressed = true;
}

static void Button_RunIdleTask(ButtonHandle_TypeDef *btn){
	if(HAL_GPIO_ReadPin(btn->GPIOPort, btn->GPIOPin)){
		btn->ButtonState = BUTTON_DEBOUNCE;
		btn->Tick = HAL_GetTick();
	}
}
static void Button_RunDebounceTask(ButtonHandle_TypeDef *btn){
	if(HAL_GetTick()-btn->Tick > btn->Debounce){
		if(HAL_GPIO_ReadPin(btn->GPIOPort, btn->GPIOPin)){
			btn->ButtonState = BUTTON_PRESSED;
			btn->Tick = HAL_GetTick();
			if(btn->PressedTask != NULL)
				btn->PressedTask();
		} else {
			btn->ButtonState = BUTTON_IDLE;
		}
	}
}
static void Button_RunPressedTask(ButtonHandle_TypeDef *btn){
	if(!HAL_GPIO_ReadPin(btn->GPIOPort, btn->GPIOPin)){
		btn->ButtonState = BUTTON_RELEASED;
	}
}
static void Button_RunReleaseTask(ButtonHandle_TypeDef *btn){
	if(btn->ReleasedTask != NULL)
		btn->ReleasedTask();
	btn->ButtonState = BUTTON_IDLE;
}

