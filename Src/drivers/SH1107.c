/*
 * SH1107.c
 * @creator: 	Wagner John Reposilo
 * @date: 		30/04/2026
 * @brief: 		This module functions to do various updates for SH1107 over I2C.
 *
 */


#include "drivers/SH1107.h"

static I2C_HandleTypeDef *pi2c;					// Pointer to i2c handle

static uint8_t bufferDraw[BUFFER_SIZE]; 		// Buffer used for Drawing
static uint8_t bufferTx[BUFFER_SIZE];			// Buffer used for Display
static uint8_t *pbufferDraw = bufferDraw;		// Pointer to the buffer for drawing
static uint8_t *pbufferDisplay = bufferTx;		// Pointer to the buffer for display

static volatile bool isBusy = false; 			// For DMA use

static void SH1107_Init(void);
static void SH1107_SendCMD(uint8_t cmd);
static void SH1107_SetColumnAddress(uint8_t addr);
static void SH1107_SetPageAddress(uint8_t addr);
static void SH1107_StartDMATransfer(uint8_t *data);
static void SH1107_StartTransfer(uint8_t *data);

static void __reverse(char* begin,char* end);

/*
 * API Functions
 */
void SH1107_I2CInit(I2C_HandleTypeDef *hi2c){
	pi2c = hi2c;

	SH1107_Init();
}

/*
 * SH1107_BufferSwap
 * @brief:	 Displays Currently Drawn Frame by swapping Buffers and send them via I2C
 * @retval: 'true' if the frame is sent
 * 			'false' if DMA is busy
 */
bool SH1107_BufferSwap(void){
	if(isBusy) return false;

	uint8_t *tempBuffer = pbufferDisplay;
	pbufferDisplay = pbufferDraw;
	pbufferDraw = tempBuffer;

	SH1107_StartDMATransfer(pbufferDisplay);

	return true;
}

/*
 * SH1107_Display
 * @brief: Transfers frame buffer to display
 */
void SH1107_Display(void){
	(void) SH1107_BufferSwap();
}

/*
 * SH1107_NewFrame
 * @brief: Creates a new frame by clearing the back buffer
 */
void SH1107_NewFrame(void){
	memset(pbufferDraw,0x00,BUFFER_SIZE);
}

/*
 * SH1107_ClearDisplay
 * @brief: Clears current DisplayBuffer
 */
void SH1107_ClearDisplay(void){
	memset(pbufferDraw,0x00,BUFFER_SIZE);
	//(void) SH1107_BufferSwap();
	uint8_t *tempBuffer = pbufferDisplay;
	pbufferDisplay = pbufferDraw;
	pbufferDraw = tempBuffer;

	SH1107_StartTransfer(pbufferDisplay);

}

/*
 * SH1107_DrawPixel
 * @brief: Sets pixel at given coordinates.
 * @param: x - X position (0 - 127)
 * 		   y - Y position (0 - 127)
 * 		   pixel - pixel color
 */
void SH1107_DrawPixel(int16_t x, int16_t y, uint8_t pixel){
	if ((x < 0) || (x >= SH1107_WIDTH) || (y < 0) || (y >= SH1107_HEIGHT))
		return;

	uint8_t *cell = &pbufferDraw[x + ((y/8) * (SH1107_WIDTH))];

	if(pixel){
		*cell |= (1 << (y % 8));
	} else {
		*cell &= ~(1 << (y % 8));
	}
}

/*
 * SH1107 isBusy
 * @brief:	checks if the MCU is busy transferring data to the OLED Display
 * @retval:	'true' if busy, otherwise 'false'
 */
bool SH1107_isBusy(void){
	return isBusy;
}

char* convertIntToStr(int numval, char* buffer, int base){
	static const char digits[] = "0123456789abcdef";

	char* buffer_cpy = buffer;
	int32_t sign = 0;
	int32_t currentVal = numval ,rem;

	if((base >= 2)&&(base<=16)){
		if(base == 10 && (sign = numval) < 0)
			numval = -numval;

		do{
			rem  = currentVal%base;
			currentVal /= base;
			*buffer++ = digits[rem];
		}while(currentVal != 0);

		if (sign<0)
			*buffer++='-';

		__reverse(buffer_cpy,buffer-1);
	}

	*buffer='\0';
	return buffer_cpy;
}

/*
 * Static Functions
 */

/*
 * SH1107_CMD
 * @brief: send command instructions to OLED Display
 * @param: cmd - command
 *
 * NOTE: check @SH1107_COMMAND_SET in the header file for instruction set reference
 */
static void SH1107_SendCMD(uint8_t cmd){
	uint8_t buf[2] = {SINGLE_COMMAND,cmd}; //Co = 0, D/C = 0
	HAL_I2C_Master_Transmit(pi2c, SH1107_I2CADDR, buf, 2, 1000);
}

/*
 * SH1107_SetColumnAddress
 * @brief: Set Column Address by using the commands high column Address (10H - 17H) and low column Address (00H - 0FH) via I2C
 * @param: addr - address
 *
 * NOTE: cinput adresses 00H - 7FH only for the addr parameter
 */
static void SH1107_SetColumnAddress(uint8_t addr){
	uint8_t highaddr = (SH1107_HIGHCOLADDR) | (addr >> 4);
	uint8_t lowaddr = (SH1107_LOWCOLADDR) | (addr & (0x0F));

	SH1107_SendCMD(highaddr);
	SH1107_SendCMD(lowaddr);
}

/*
 * SH1107_SetPageAddress
 * @brief: Set Page Address by using the command for setting the Page Address (B0H-BFH)
 * @param: addr - address
 *
 * NOTE: input adresses 00H - 0FH only for the addr parameter
 */
static void SH1107_SetPageAddress(uint8_t addr){
	SH1107_SendCMD(SH1107_SETPAGEADDR | (addr & (0x0F)));
}

/*
 * SH1107_StartDMATransfer
 * @brief: Starts Sending Data to the OLED Display via I2C DMA.
 * @param: data - the data to be sent to the oled display
 */
static void SH1107_StartDMATransfer(uint8_t *data){
	if (isBusy) return;

	static uint8_t txBuffer[SH1107_WIDTH + 1] = {0x00};

	txBuffer[0] = SINGLE_DATA;

	for(uint8_t page = 0; page < (SH1107_HEIGHT/8); page++){
		SH1107_SetPageAddress(page);
		SH1107_SetColumnAddress(0x00);

		//memcpy(&(txBuffer[1]),data,len);
		memcpy(&txBuffer[1], &(data[page * SH1107_WIDTH]), SH1107_WIDTH);
		isBusy = true;

		HAL_I2C_Master_Transmit_DMA(pi2c, SH1107_I2CADDR, txBuffer, SH1107_WIDTH+1);
		while(isBusy);
	}


}

/*
 * SH1107_StartTransfer
 * @brief: Starts Sending Data to the OLED Display via I2C (blocking mode).
 * @param: data - the data to be sent to the oled display
 */
static void SH1107_StartTransfer(uint8_t *data){
	if (isBusy) return;

	static uint8_t txBuffer[SH1107_WIDTH + 1];
	txBuffer[0] = 0x40;

	for(uint8_t page = 0; page < (SH1107_HEIGHT/8); page++){
		SH1107_SetPageAddress(page);
		SH1107_SetColumnAddress(0x00);

		memcpy(&txBuffer[1], &(data[page * SH1107_WIDTH]), SH1107_WIDTH);
		isBusy = true;

		HAL_I2C_Master_Transmit(pi2c, SH1107_I2CADDR, txBuffer, SH1107_WIDTH+1, 1000);
		isBusy = false;
	}
}

/*
 * SH1107_Init
 * @brief: sends the initialization sequence to SH1107 via I2C
 */
static void SH1107_Init(void){
	HAL_Delay(100);
	SH1107_SendCMD(SH1107_DSPLOFF);

	SH1107_SendCMD(SH1107_OSCFREQ);
	SH1107_SendCMD(0x51);
	SH1107_SendCMD(SH1107_MULTPLX);
	SH1107_SendCMD(0x7F);
	SH1107_SendCMD(SH1107_DSPOFFSET);
	SH1107_SendCMD(0x00);
	SH1107_SendCMD(SH1107_DSPSTART);
	SH1107_SendCMD(0x00);
	SH1107_SendCMD(SH1107_DCTODC_CTRL);
	SH1107_SendCMD(0x8B);
	//HAL_Delay(100);
	SH1107_SendCMD(SH1107_SEGNORM);
	SH1107_SendCMD(SH1107_SCANDIRL);
	SH1107_SendCMD(SH1107_CONTRASTCTRL);
	SH1107_SendCMD(0x4F);
	SH1107_SendCMD(SH1107_CHARGEPERIOD);
	SH1107_SendCMD(0x22);
	SH1107_SendCMD(SH1107_VCOM);
	SH1107_SendCMD(0x35);
	SH1107_SendCMD(SH1107_DSPSTATUSL);
	SH1107_SendCMD(SH1107_DSPNORMAL);

	SH1107_ClearDisplay();
	HAL_Delay(10);

	SH1107_SendCMD(SH1107_DSPLON);
	//HAL_Delay(100);
}

static void __reverse(char* begin,char* end)
{
	char temp;

	while (end>begin)
	{
		temp=*end;
		*end--=*begin;
		*begin++=temp;
	}
}


void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c){
	if(hi2c == pi2c)
		isBusy = false;
}
