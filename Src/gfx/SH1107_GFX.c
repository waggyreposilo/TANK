/*
 * SH1107_GFX.c
 * @creator: 	Wagner John Reposilo
 * @date: 		30/04/2026
 * @brief: 		This module provides drawing functions for SH1107 OLED Display.
 *
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>

#include "gfx/SH1107_GFX.h"
#include "drivers/SH1107.h"

static void SH1107_DrawHLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color);
static void SH1107_DrawVLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color);
static void swapPoints(uint8_t* x, uint8_t *y);

const uint8_t font6x8[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // 0x20 'space'
	0x00, 0x00, 0xDF, 0xDF, 0x00, 0x00,  // 0x21 '!'
	0x00, 0x07, 0x00, 0x00, 0x07, 0x00,  // 0x22 '"'
	0x14, 0x7F, 0x14, 0x7F, 0x14, 0x00,  // 0x23 '#'
	0x24, 0x2E, 0x6B, 0x6B, 0x3A, 0x12,  // 0x24 '$'
	0xC3, 0x23, 0x30, 0x0C, 0xC6, 0xC1,  // 0x25 '%'
	0x36, 0x7F, 0x49, 0x57, 0x22, 0x70,  // 0x26 '&'
	0x00, 0x00, 0x07, 0x03, 0x00, 0x00,  // 0x27 '''
	0x00, 0x3C, 0x7E, 0xC3, 0x81, 0x00,  // 0x28 '('
	0x00, 0x81, 0xC3, 0x7E, 0x3C, 0x00,  // 0x29 ')'
	0x08, 0x2A, 0x1C, 0x1C, 0x2A, 0x08,  // 0x2A '*'
	0x08, 0x08, 0x3E, 0x3E, 0x08, 0x08,  // 0x2B '+'
	0x00, 0x00, 0xE0, 0x60, 0x00, 0x00,  // 0x2C ','
	0x08, 0x08, 0x08, 0x08, 0x08, 0x08,  // 0x2D '-'
	0x00, 0xC0, 0xC0, 0x00, 0x00, 0x00,  // 0x2E '.'
	0x80, 0xE0, 0x78, 0x1E, 0x07, 0x01,  // 0x2F '/'
	0x7E, 0xFF, 0x89, 0x85, 0xFF, 0x7E,  // 0x30 '0'
	0x80, 0x82, 0xFF, 0xFF, 0x80, 0x80,  // 0x31 '1'
	0xC2, 0xE3, 0xB1, 0x99, 0xCF, 0xC6,  // 0x32 '2'
	0x42, 0xC3, 0x89, 0x99, 0xFF, 0x76,  // 0x33 '3'
	0x18, 0x1C, 0x16, 0xFF, 0xFF, 0x10,  // 0x34 '4'
	0x47, 0xC7, 0x85, 0x85, 0xFD, 0x79,  // 0x35 '5'
	0x7C, 0xFE, 0x8B, 0x89, 0xF9, 0x70,  // 0x36 '6'
	0x01, 0x01, 0xF1, 0xFD, 0x0F, 0x03,  // 0x37 '7'
	0x76, 0xFF, 0x89, 0x89, 0xFF, 0x76,  // 0x38 '8'
	0x0E, 0x9F, 0x91, 0xD1, 0x7F, 0x3E,  // 0x39 '9'
	0x00, 0x00, 0x66, 0x66, 0x00, 0x00,  // 0x3A ':'
	0x00, 0x00, 0xE6, 0x66, 0x00, 0x00,  // 0x3B ';'
	0x08, 0x1C, 0x36, 0x63, 0x41, 0x00,  // 0x3C '<'
	0x14, 0x14, 0x14, 0x14, 0x14, 0x14,  // 0x3D '='
	0x00, 0x41, 0x63, 0x36, 0x1C, 0x08,  // 0x3E '>'
	0x02, 0x03, 0xD1, 0xD9, 0x0F, 0x06,  // 0x3F '?'
	0x7E, 0xFF, 0x81, 0x9D, 0xD7, 0x5E,  // 0x40 '@'
	0xFE, 0xFF, 0x09, 0x09, 0xFF, 0xFE,  // 0x41 'A'
	0xFF, 0xFF, 0x89, 0x89, 0xFF, 0x76,  // 0x42 'B'
	0x7E, 0xFF, 0x81, 0x81, 0xC3, 0x42,  // 0x43 'C'
	0xFF, 0xFF, 0x81, 0x81, 0xFF, 0x7E,  // 0x44 'D'
	0xFF, 0xFF, 0x89, 0x89, 0x89, 0x81,  // 0x45 'E'
	0xFF, 0xFF, 0x09, 0x09, 0x09, 0x01,  // 0x46 'F'
	0x7E, 0xFF, 0x81, 0x89, 0xFB, 0xFA,  // 0x47 'G'
	0xFF, 0xFF, 0x08, 0x08, 0xFF, 0xFF,  // 0x48 'H'
	0x81, 0x81, 0xFF, 0xFF, 0x81, 0x81,  // 0x49 'I'
	0x60, 0xE0, 0x80, 0x81, 0xFF, 0x7F,  // 0x4A 'J'
	0xFF, 0xFF, 0x1C, 0x36, 0xE3, 0xC1,  // 0x4B 'K'
	0xFF, 0xFF, 0x80, 0x80, 0x80, 0x80,  // 0x4C 'L'
	0xFF, 0xFF, 0x06, 0x06, 0xFF, 0xFF,  // 0x4D 'M'
	0xFF, 0xFF, 0x06, 0x0C, 0xFF, 0xFF,  // 0x4E 'N'
	0x7E, 0xFF, 0x81, 0x81, 0xFF, 0x7E,  // 0x4F 'O'
	0xFF, 0xFF, 0x09, 0x09, 0x0F, 0x06,  // 0x50 'P'
	0x7E, 0xFF, 0x81, 0xD1, 0x7F, 0xFE,  // 0x51 'Q'
	0xFF, 0xFF, 0x19, 0x29, 0xEF, 0xC6,  // 0x52 'R'
	0x46, 0xCF, 0x89, 0x89, 0xFB, 0x72,  // 0x53 'S'
	0x01, 0x01, 0xFF, 0xFF, 0x01, 0x01,  // 0x54 'T'
	0x7F, 0xFF, 0x80, 0x80, 0xFF, 0x7F,  // 0x55 'U'
	0x0F, 0x3F, 0xF0, 0xF0, 0x3F, 0x0F,  // 0x56 'V'
	0x3F, 0xFF, 0x70, 0x70, 0xFF, 0x3F,  // 0x57 'W'
	0xE3, 0xF7, 0x1C, 0x1C, 0xF7, 0xE3,  // 0x58 'X'
	0x03, 0x07, 0xFC, 0xFC, 0x07, 0x03,  // 0x59 'Y'
	0xE1, 0xF1, 0x99, 0x8D, 0xC7, 0xC3,  // 0x5A 'Z'
	0x00, 0xFF, 0xFF, 0x81, 0x81, 0x00,  // 0x5B '['
	0x01, 0x07, 0x1E, 0x78, 0xE0, 0x80,  // 0x5C '\\'
	0x00, 0x81, 0x81, 0xFF, 0xFF, 0x00,  // 0x5D ']'
	0x04, 0x06, 0x03, 0x03, 0x06, 0x04,  // 0x5E '^'
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80,  // 0x5F '_'
	0x00, 0x01, 0x03, 0x02, 0x00, 0x00,  // 0x60 '`'
	0x70, 0xFA, 0x8A, 0x4A, 0xFE, 0xFC,  // 0x61 'a'
	0xFF, 0xFF, 0x84, 0x84, 0xFC, 0x78,  // 0x62 'b'
	0x7C, 0xFE, 0x82, 0x82, 0xC6, 0x44,  // 0x63 'c'
	0x78, 0xFC, 0x84, 0x84, 0xFF, 0xFF,  // 0x64 'd'
	0x7C, 0xFE, 0x8A, 0x8A, 0xCE, 0x4C,  // 0x65 'e'
	0x04, 0xFE, 0xFF, 0x05, 0x05, 0x00,  // 0x66 'f'
	0x4C, 0xDE, 0x92, 0x92, 0xFE, 0x7E,  // 0x67 'g'
	0xFF, 0xFF, 0x04, 0x04, 0xFC, 0xF8,  // 0x68 'h'
	0x00, 0x84, 0xFD, 0xFD, 0x80, 0x00,  // 0x69 'i'
	0x40, 0xC0, 0x80, 0xFD, 0x7D, 0x00,  // 0x6A 'j'
	0xFF, 0xFF, 0x18, 0x3C, 0xE6, 0xC2,  // 0x6B 'k'
	0x00, 0x81, 0xFF, 0xFF, 0x80, 0x00,  // 0x6C 'l'
	0xFE, 0xFE, 0x04, 0xFE, 0xFE, 0x00,  // 0x6D 'm'
	0xFE, 0xFE, 0x02, 0x02, 0xFE, 0xFC,  // 0x6E 'n'
	0x7C, 0xFE, 0x82, 0x82, 0xFE, 0x7C,  // 0x6F 'o'
	0xFE, 0xFE, 0x12, 0x12, 0x1E, 0x0C,  // 0x70 'p'
	0x0C, 0x1E, 0x12, 0x12, 0xFE, 0xFE,  // 0x71 'q'
	0xFE, 0xFE, 0x04, 0x02, 0x06, 0x04,  // 0x72 'r'
	0x4C, 0xDE, 0x92, 0x92, 0xF6, 0x64,  // 0x73 's'
	0x04, 0x7F, 0xFF, 0x84, 0xC4, 0x40,  // 0x74 't'
	0x7E, 0xFE, 0x80, 0x40, 0xFE, 0xFE,  // 0x75 'u'
	0x0E, 0x3E, 0xF0, 0xF0, 0x3E, 0x0E,  // 0x76 'v'
	0x3E, 0xFE, 0x70, 0x78, 0xFE, 0x06,  // 0x77 'w'
	0xC2, 0xE6, 0x3C, 0x3C, 0xE6, 0xC2,  // 0x78 'x'
	0x26, 0x6E, 0x48, 0x48, 0x7E, 0x3E,  // 0x79 'y'
	0xC2, 0xE2, 0xB2, 0x9A, 0xCE, 0xC6,  // 0x7A 'z'
	0x00, 0x18, 0x7E, 0xE7, 0x81, 0x00,  // 0x7B '{'
	0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00,  // 0x7C '|'
	0x00, 0x81, 0xE7, 0x7E, 0x18, 0x00,  // 0x7D '}'
	0x04, 0x06, 0x02, 0x04, 0x0C, 0x08,  // 0x7E '~'
};
// 95 chars × 6 bytes = 570 bytes total

/*
 * API Functions
 */

/*
 * SH1107_DrawLine
 * @brief: Draws line to the drawing buffer using 2 points as parameters
 * @param: x1, y1 - first x and y coordinate
 * 		   x2, y2 - second x and y coordinate
 * 		   color - set the pixel on or off.
 */
void SH1107_DrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color){
	if (abs(x2 - x1) > abs(y2 - y1))
		SH1107_DrawHLine(x1, y1, x2, y2, color);
	else
		SH1107_DrawVLine(x1, y1, x2, y2, color);
}

/*
 * SH1107_DrawFastVLine
 * @brief: Draws Vertical Line
 * @param: x1, y1 - starting x and y coordinate
 * 		   h - height of the line in pixels.
 * 		   color - set the pixel on or off.
 */
void SH1107_DrawFastVLine(uint8_t x, uint8_t y, uint8_t h, uint8_t color){
	SH1107_DrawLine(x, y, x, y+h-1, color);
}

/*
 * SH1107_DrawFastHLine
 * @brief: Draws Horizontal Line
 * @param: x1, y1 - starting x and y coordinate
 * 		   w - width of the line in pixels.
 * 		   color - set the pixel on or off.
 */
void SH1107_DrawFastHLine(uint8_t x, uint8_t y, uint8_t w, uint8_t color){
	SH1107_DrawLine(x, y, x+w-1, y, color);
}

/*
 * SH1107_DrawCircle
 * @brief: Draws Circle
 * @param: xc, yc - Center x and y coordinate of the circle
 * 		   rad - radius of the circle
 * 		   color - set the pixel on or off.
 */
void SH1107_DrawCircle(uint8_t xc, uint8_t yc, uint8_t rad, uint8_t color){
	int currentX, currentY, radError;

	currentX = 0;
	currentY = rad;
	radError = 3 - (2 * rad);

	SH1107_DrawPixel(xc+currentX, yc+currentY, color);
	SH1107_DrawPixel(xc-currentX, yc+currentY, color);
	SH1107_DrawPixel(xc+currentX, yc-currentY, color);
	SH1107_DrawPixel(xc-currentX, yc-currentY, color);
	SH1107_DrawPixel(xc+currentY, yc+currentX, color);
	SH1107_DrawPixel(xc-currentY, yc+currentX, color);
	SH1107_DrawPixel(xc+currentY, yc-currentX, color);
	SH1107_DrawPixel(xc-currentY, yc-currentX, color);

	while(currentY >= currentX){
		if(radError > 0){
			currentY--;
			radError = radError + (4 * (currentX - currentY)) + 10;
		} else {
			radError = radError + (4 * currentX) + 6;
		}

		currentX++;

		SH1107_DrawPixel(xc+currentX, yc+currentY, color);
		SH1107_DrawPixel(xc-currentX, yc+currentY, color);
		SH1107_DrawPixel(xc+currentX, yc-currentY, color);
		SH1107_DrawPixel(xc-currentX, yc-currentY, color);
		SH1107_DrawPixel(xc+currentY, yc+currentX, color);
		SH1107_DrawPixel(xc-currentY, yc+currentX, color);
		SH1107_DrawPixel(xc+currentY, yc-currentX, color);
		SH1107_DrawPixel(xc-currentY, yc-currentX, color);
		HAL_Delay(50);
	}
}

void SH1107_FillCircle(uint8_t x, uint8_t y, uint8_t rad, uint8_t color){
	int8_t currentX, currentY, radError;

	currentX = 0;
	currentY = rad;
	radError = 3 - (2 * rad);

	SH1107_DrawLine(x, y - rad, x, y + rad, color);

	while(currentY >= currentX){
		if(radError > 0){
			currentY--;
			radError = radError + (4 * (currentX - currentY)) + 10;
		} else {
			radError = radError + (4 * currentX) + 6;
		}

		currentX++;

		SH1107_DrawLine(x+currentX, y+currentY, x+currentX, y-currentY, color);
		SH1107_DrawLine(x-currentX, y+currentY, x-currentX, y-currentY, color);
		SH1107_DrawLine(x+currentY, y+currentX, x+currentY, y-currentX, color);
		SH1107_DrawLine(x-currentY, y+currentX, x-currentY, y-currentX, color);
		HAL_Delay(50);
	}

}

void SH1107_FillCornerCircle(uint8_t xc, uint8_t yc, uint8_t rad, uint8_t corner, uint8_t delta, uint8_t color){
	int currentX, currentY, radError;

		currentX = 0;
		currentY = rad;
		radError = 3 - (2 * rad);

		while(currentY >= currentX){
			if(radError > 0){
				currentY--;
				radError = radError + (4 * (currentX - currentY)) + 10;
			} else {
				radError = radError + (4 * currentX) + 6;
			}

			currentX++;

			switch(corner){
			case CORNER_TOPLEFT:
				SH1107_DrawFastVLine(xc+currentX, yc-currentY, 2*currentY+1+delta, color);
				SH1107_DrawFastVLine(xc+currentY, yc-currentX, 2*currentX+1+delta, color);
				break;
			case CORNER_TOPRIGHT:
				SH1107_DrawFastVLine(xc-currentX, yc-currentY, 2*currentY+1+delta, color);
				SH1107_DrawFastVLine(xc-currentY, yc-currentX, 2*currentX+1+delta, color);
				break;
			default:
				break;
			}
			HAL_Delay(50);
		}
}

void SH1107_DrawCornerCircle(uint8_t xc, uint8_t yc, uint8_t rad, uint8_t corner, uint8_t color){
	int currentX, currentY, radError;

	currentX = 0;
	currentY = rad;
	radError = 3 - (2 * rad);

	while(currentY >= currentX){
		if(radError > 0){
			currentY--;
			radError = radError + (4 * (currentX - currentY)) + 10;
		} else {
			radError = radError + (4 * currentX) + 6;
		}

		currentX++;

		switch(corner){
		case CORNER_TOPLEFT:
			SH1107_DrawPixel(xc-currentX, yc-currentY, color);
			SH1107_DrawPixel(xc-currentY, yc-currentX, color);
			break;
		case CORNER_TOPRIGHT:
			SH1107_DrawPixel(xc+currentX, yc-currentY, color);
			SH1107_DrawPixel(xc+currentY, yc-currentX, color);
			break;
		case CORNER_BOTTOMLEFT:
			SH1107_DrawPixel(xc+currentX, yc+currentY, color);
			SH1107_DrawPixel(xc+currentY, yc+currentX, color);
			break;
		case CORNER_BOTTOMRIGHT:
			SH1107_DrawPixel(xc-currentX, yc+currentY, color);
			SH1107_DrawPixel(xc-currentY, yc+currentX, color);
			break;
		default:
			break;
		}
		HAL_Delay(50);
	}
}

void SH1107_DrawFilledRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color){
	for(uint8_t i=x; i <x+w; i++)
	{
		SH1107_DrawFastVLine(i, y, h, color);
	}
}

void SH1107_DrawRoundedRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t rad, uint8_t color){

	SH1107_DrawFastHLine(x+rad, y	 , w-2*rad, color);
	SH1107_DrawFastHLine(x+rad, y+h-1, w-2*rad, color);
	SH1107_DrawFastVLine(x	  ,	y+rad, h-2*rad, color);
	SH1107_DrawFastVLine(x+w-1, y+rad, h-2*rad, color);

	SH1107_DrawCornerCircle(x+rad	  , y+rad	 , rad, CORNER_TOPLEFT	  , color);
	SH1107_DrawCornerCircle(x+w-rad-1 , y+rad	 , rad, CORNER_TOPRIGHT	  , color);
	SH1107_DrawCornerCircle(x+w-rad-1 , y+h-rad-1, rad, CORNER_BOTTOMLEFT , color);
	SH1107_DrawCornerCircle(x+rad	  , y+h-rad-1, rad, CORNER_BOTTOMRIGHT, color);

}

void SH1107_FillRoundedRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t rad, uint8_t color){
	SH1107_DrawFilledRect(x+rad, y, w-2*rad, h, color);

	SH1107_FillCornerCircle(x+w-rad-1, y+rad, rad, 1, h-2*rad-1, color);
	SH1107_FillCornerCircle(x+rad, y+rad, rad, 2, h-2*rad-1, color);
}

void SH1107_DrawChar(uint8_t x, uint8_t y, uint8_t ch, uint8_t color, uint8_t size){

	if((x >= SH1107_WIDTH) || (y >= SH1107_HEIGHT) ||
	   ((x + 7 * size - 1) < 0) || ((y + 8 * size - 1) < 0))
		    return;

	uint8_t charbmp;

	for(uint8_t col_index = 0 ; col_index < 6; col_index++){
		charbmp = font6x8[(ch - 32) * 6 + col_index];

		for(uint8_t row_index = 0; row_index < 8; row_index++){
			SH1107_DrawPixel(x+col_index, y+row_index, (charbmp & 0x01));

			charbmp >>= 1;
		}
	}
}


void SH1107_DrawString(uint8_t x, uint8_t y, uint8_t *str, uint8_t color, uint8_t size){
	//uint8_t *pString = &str;
	uint8_t tempx = x;
	for(uint8_t char_idx = 0; char_idx < strlen((char*)str); char_idx++){
		tempx = x+(char_idx * (FONT_WIDTH + FONT_SPACING));
		if(tempx >= SH1107_WIDTH) tempx-=SH1107_WIDTH;
		SH1107_DrawChar(tempx, y, str[char_idx], WHITE, 1);
	}
}

void SH1107_DrawInt(uint8_t x, uint8_t y, int num, uint8_t color, uint8_t size){
	char buffer[16];
	SH1107_DrawString(x, y, (uint8_t*)(convertIntToStr(num, buffer, 10)), color, size);
}

void SH1107_DrawBitMap(int16_t x, int16_t y, const uint8_t *bitmap,uint16_t w, uint16_t h, uint16_t color) {

    uint16_t byteWidth = (w + 7) / 8;
    uint8_t byte = 0;

    for (uint16_t j = 0; j < h; j++) {
        for (uint16_t i = 0; i < w; i++) {
            if (i & 7)
                byte <<= 1;
            else
                byte = bitmap[(uint16_t)j * byteWidth + i / 8];

            if (byte & 0x80)
                SH1107_DrawPixel(x + i, y + j, color);
        }
    }
}


/*
 * Static functions
 */

/*
 * swapPoints
 * @brief: swaps 2 numbers
 * @param: x - first number
 * 		   y - second number
 */
static void swapPoints(uint8_t* x, uint8_t *y){
	uint8_t temp = 0;
	temp = *x;
	*x = *y;
	*y = temp;
}

/*
 * SH1107_DrawHLine
 * @brief: Draws Horizontal Line in the OLED Display based on Bresenham's line Algorithm (See Wikipedia)
 * @param:	x1, y1 - first x and y point
 * 			x2, y2 - second x and y point
 * 			color - if color is set to 1 or 0
 */
static void SH1107_DrawHLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color){
	int8_t  dx,dy,		//dx - change in x axis, dy - change in y axis
			dirY,		//dirY - direction of line in y direction
			p,			//p - Accumulated Precision Error. Decides if the point gets drawn to +y or -y
			currentY;	//y - Current y being drawn

	if(x1 > x2){
		swapPoints(&x1,&x2);
		swapPoints(&y1,&y2);
	}

	dx = x2 - x1;
	dy = y2 - y1;

	if(dy < 0){
		dirY = -1;
	} else {
		dirY = 1;
	}

	dy *= dirY;

	if(dx != 0){
		currentY = y1;
		p = (2*dy) - dx;
		for(uint8_t i = 0; i < dx+1; i++){
			SH1107_DrawPixel(x1 + i, currentY, color);
			if(p >= 0){
				currentY += dirY;
				p = p - (2*dx);
			}
			p = p + (2*dy);
		}
	}
}

/*
 * SH1107_DrawVLine
 * @brief: Draws Vertical Line in the OLED Display based on Bresenham's line Algorithm (See Wikipedia)
 * @param:	x1, y1 - first x and y point
 * 			x2, y2 - second x and y point
 * 			color - if color is set to 1 or 0
 */
static void SH1107_DrawVLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color){
	int8_t  dx,dy,		//dx - change in x axis, dy - change in y axis
			dirX,		//dir - direction of increment in x direction
			p,			//p - Accumulated Precision Error. Decides if the point gets drawn to +x or -x
			currentX;			//x - Current x being drawn

	if(y1 > y2){
		swapPoints(&x1,&x2);
		swapPoints(&y1,&y2);
	}

	dx = x2 - x1;
	dy = y2 - y1;

	if(dx < 0){
		dirX = -1;
	} else {
		dirX = 1;
	}

	dx *= dirX;

	if(dy != 0){
		currentX = x1;
		p = (2*dx) - dy;
		for(uint8_t i = 0; i < dy+1; i++){
			SH1107_DrawPixel(currentX, y1 + i, color);
			if(p >= 0){
				currentX += dirX;
				p = p - (2*dy);
			}
			p = p + (2*dx);
		}
	}
}
