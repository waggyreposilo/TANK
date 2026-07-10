/*
 * BFS.h
 * @creator:	Wagner John Reposilo
 * @date:		7/1/2026
 * @brief:		This module contains functions to implement the Breath First Search Algorithm which is used
 * 				for the enemy AI pathfinding
 */

#ifndef INC_GAME_BFS_H_
#define INC_GAME_BFS_H_

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#define OLED_WIDTH				128
#define OLED_HEIGHT				128
#define TILE_SIZE				8
#define TILE_ROWS				(OLED_HEIGHT / TILE_SIZE)
#define TILE_COLS				(OLED_WIDTH / TILE_SIZE)

#define MAXPATH_LEN				64
#define TILE_COUNT				(TILE_ROWS * TILE_COLS)

#define GET_TILEINDEX(tx,ty)	((ty) * TILE_COLS + (tx))
#define GET_TILETX(index)		((index) % TILE_COLS)
#define GET_TILETY(index)		((index) / TILE_COLS)

#define ENEMY_MAX_X   120
#define ENEMY_MAX_Y   120
#define ENEMY_MIN_X   0
#define ENEMY_MIN_Y   0

typedef struct{
	uint8_t tiles[MAXPATH_LEN];
	uint8_t length;
}TilePathHandle_TypeDef;

void build_tilemap(uint8_t *pixelMap);
bool BFS(uint8_t startX, uint8_t startY, uint8_t destX, uint8_t destY, TilePathHandle_TypeDef *result);
bool get_random_walkable_tile(uint8_t *outTx, uint8_t *outTy);
void convertTiletoPixel(uint8_t tileIndex, uint8_t *pixelX, uint8_t *pixelY);
#endif /* INC_GAME_BFS_H_ */
