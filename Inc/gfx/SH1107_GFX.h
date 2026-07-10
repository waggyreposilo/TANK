/*
 * SH1107_GFX.h
 * @creator: 	Wagner John Reposilo
 * @date: 		30/04/2026
 * @brief: 		This module provides drawing functions for SH1107 OLED Display.
 *
 */

#ifndef INC_SH1107_GFX_H_
#define INC_SH1107_GFX_H_

#define CORNER_TOPLEFT		0x01
#define CORNER_TOPRIGHT		0x02
#define CORNER_BOTTOMLEFT	0x03
#define CORNER_BOTTOMRIGHT	0x04

#define SIZE_DEFAULT		1
#define FONT_WIDTH  		6
#define FONT_SPACING 		1
/*
 * Drawing Functions
 * @brief Graphics functions for shapes, bitmaps, and text
 */

void SH1107_DrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color);
void SH1107_DrawFastHLine(uint8_t x, uint8_t y, uint8_t h, uint8_t color);
void SH1107_DrawFastVLine(uint8_t x, uint8_t y, uint8_t w, uint8_t color);

void SH1107_DrawCircle(uint8_t x, uint8_t y, uint8_t rad, uint8_t color);
void SH1107_FillCircle(uint8_t x, uint8_t y, uint8_t rad, uint8_t color);
void SH1107_DrawCornerCircle(uint8_t xc, uint8_t yc, uint8_t rad, uint8_t corner, uint8_t color);
void SH1107_FillCornerCircle(uint8_t xc, uint8_t yc, uint8_t rad, uint8_t corner, uint8_t delta, uint8_t color);

void SH1107_DrawFilledRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color);
void SH1107_DrawRoundedRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t rad, uint8_t color);
void SH1107_FillRoundedRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t rad, uint8_t color);

void SH1107_DrawChar(uint8_t x, uint8_t y, uint8_t ch, uint8_t color, uint8_t size);
void SH1107_DrawString(uint8_t x, uint8_t y, uint8_t *str, uint8_t color, uint8_t size);
void SH1107_DrawInt(uint8_t x, uint8_t y, int num, uint8_t color, uint8_t size);

void SH1107_DrawBitMap(int16_t x, int16_t y, const uint8_t *bitmap,uint16_t w, uint16_t h, uint16_t color);

#endif /* INC_GFX_SH1107_GFX_H_ */
