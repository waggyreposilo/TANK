/*
 * SH1107.c
 * @creator: 	Wagner John Reposilo
 * @date: 		30/04/2026
 * @brief: 		This module functions to do various updates for SH1107 over I2C.
 *
 */

#ifndef INC_SH1107_H_
#define INC_SH1107_H_

#include <string.h>
#include <stdbool.h>

#include "main.h"

/*
 * Pin Configuration
 */
#define SDA_PORT			SH1107_SDA_GPIO_Port
#define SCL_PORT			SH1107_SCL_GPIO_Port
#define SDA_PIN				SH1107_SDA_Pin
#define SCL_PIN				SH1107_SCL_Pin

//I2C Address
#define SH1107_I2CADDR		0x78

//Control Bytes
#define DATA_STREAM			0xC0
#define COMMAND_STREAM		0x80
#define SINGLE_DATA			0x40
#define SINGLE_COMMAND		0x00

/*
 * Command Bytes (@CommandByte)
 * @brief: Command Byte Types
 */
#define SINGLEBYTE			1
#define DOUBLEBYTE			2

/*
 * Display Resolution
 */
#define SH1107_HEIGHT		128
#define SH1107_WIDTH		128

#define BUFFER_SIZE			((SH1107_HEIGHT * SH1107_WIDTH)/(8))

#define BLACK				0
#define WHITE				1

/*
 * OLED Commands (@SH1107_COMMAND_SET)
 * @brief:	SH1107 Instruction Set
 * {
 */

//Single Byte Commands
#define SH1107_LOWCOLADDR	0x00	//Set Lower Column Address: 0000[A3,A2,A1,A0]
#define SH1107_HIGHCOLADDR	0x10	//Set High Column Address: 10000[A6,A5,A4]
#define SH1107_PAGEADDRMODE	0x20
#define SH1107_COLADDRMODE	0x21
#define SH1107_SEGNORM		0xA0	//Set Segment Remap: Normal Direction
#define SH1107_SEGREV		0xA1	//Set Segment Remap: Reverse Direction
#define SH1107_DSPSTATUSL	0xA4	//Normal Display Status
#define SH1107_DSPSTATUSH	0xA5	//Display Status Always On
#define SH1107_DSPNORMAL	0XA6	//Normal Display
#define SH1107_DSPREVERSE	0XA7	//Reverse Display
#define SH1107_DSPLOFF		0xAE	//Display Off
#define SH1107_DSPLON		0xAF	//Display On
#define SH1107_SETPAGEADDR	0xB0	//Set Page Address: 1011[A3,A2,A1,A0]
#define SH1107_SCANDIRL		0xC0	//Scan Direction: Scan from COM0 to COM [N -1]
#define SH1107_SCANDIRH		0xC8	//Scan Direction: Scan from COM [N -1] to COM0
#define SH1107_RMW			0xE0	//Read-Modify-Write
#define SH1107_END			0xEE	//End Command

//Double Byte Commands (See datasheet for details)
#define SH1107_CONTRASTCTRL	0x81	//Contrast Control Mode (Send 8 bit data After sending the command(0x00-0xFF))
#define SH1107_MULTPLX		0xA8	//Multiplex Ration Set Mode (Send 8 bit data After sending the command(0x00-0x7F))
#define SH1107_DSPOFFSET	0xD3	//Display Offset (Send 8 bit data After sending the command(0x00-0x7F))
#define SH1107_DCTODC_CTRL	0xAD	//DC-DC Control Mode (Send 8 bit data After sending the command(0x8A-0x8B))
#define SH1107_OSCFREQ		0xD5	//Divide Ratio/Oscillator Frequency Mode Set (Send 8 bit data After sending the command(0x00-0xFF))
#define SH1107_CHARGEPERIOD	0xD9	//Pre-charge Period Mode Set (Send 8 bit data After sending the command(0x00-0xFF))
#define SH1107_VCOM			0xDB	//VCOM Deselect Level Mode Set (Send 8 bit data After sending the command(0x00-0xFF))
#define SH1107_DSPSTART		0xDC	//Display Start line Mode Set (Send 8 bit data After sending the command(0x00-0xFF))
/*
 * }
 */

/*
 * Functions
 * @brief:	API for SH1107 Control
 */

void SH1107_I2CInit(I2C_HandleTypeDef *i2c);
void SH1107_Display(void);
void SH1107_ClearDisplay(void);
void SH1107_NewFrame(void);
void SH1107_DrawPixel(int16_t x, int16_t y, uint8_t color);

bool SH1107_BufferSwap(void);
bool SH1107_isBusy(void);

char* convertIntToStr(int numval, char* buffer, int base);

#endif /* INC_SH1107_H_ */
