/*
 * HW504.c
 * @creator: 	Wagner John Reposilo
 * @date: 		25/05/2026
 * @brief: 		This module functions to do various updates for HW504 via ADC.
 */

#include "drivers/HW504.h"

static ADC_HandleTypeDef *ADCHandle;
static GPIO_TypeDef* KeyPort;
static uint16_t KeyPin;
volatile bool isADCBusy = false;

/*
 * HW504_Init
 * @brief: initializes the joystick peripherals
 * @param: JoystickHandle - Pointer to the JoystickHandle
 * @retval: none
 */
void HW504_Init(JoystickHandle_TypeDef *JoystickHandle){
	ADCHandle = JoystickHandle->hadc;
	KeyPort = JoystickHandle->KeyPort;
	KeyPin = JoystickHandle->KeyPin;
}

/*
 * HW504_AnalogDataRead
 * @brief: Starts ADC Conversion for the Joystick
 * @param: data - Joystick Data
 */
void HW504_AnalogDataRead(JoystickData_TypeDef *data){

#ifdef DMA_MODE
	HAL_ADC_Start_DMA(ADCHandle, (uint32_t*)data->JoystickPosition, 2);
#endif

}

/*
 * HW504_ReadKeyState
 * @brief: Reads the push button on the Joystick
 * @param: data - Joystick Data
 */
void HW504_ReadKeyState(JoystickData_TypeDef *data){
	data->KeyState = (uint8_t)HAL_GPIO_ReadPin(KeyPort, KeyPin);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc){
	if(hadc->Instance == ADC1)
		isADCBusy = false;
}
