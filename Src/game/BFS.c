/*
 * BFS.c
 * @creator:	Wagner John Reposilo
 * @date:		7/1/2026
 * @brief:		This module contains functions to implement the Breath First Search Algorithm which is used
 * 				for the enemy AI pathfinding
 */

#include "game/BFS.h"

uint8_t tilemap[TILE_ROWS][TILE_COLS];

static uint16_t lfsr = 0xACE1;
//Used for checking the neighboring pixels around the current point
static const int8_t dirX[4] = {0,0,1,-1};		//DIR_UP,DIR_DOWN,DIR_LEFT,DIR_RIGHT
static const int8_t dirY[4] = {1,-1,0,0};

static uint16_t lfsr_rand(void);

/*
 * build_tilemap
 *
 * @brief - Downsamples the map into a tile map representation to lessen the BFS search time
 * 			thus, less memory usage.
 * @param - pixelMap: 128x128 map to be used for downsampling
 */
void build_tilemap(uint8_t *pixelMap){
	for(uint8_t t_rows = 0 ; t_rows < TILE_ROWS; t_rows++){
		for(uint8_t t_cols = 0; t_cols < TILE_COLS; t_cols++){
			uint8_t wallCount = 0;

			for (uint8_t p_rows = 0; p_rows < TILE_SIZE; p_rows++){
				for (uint8_t p_cols = 0; p_cols < TILE_SIZE; p_cols++){
					uint8_t pixX = t_cols * TILE_SIZE + p_cols;
					uint8_t pixY = t_rows * TILE_SIZE + p_rows;

					uint16_t byte_index = (pixY * 16) + (pixX / 8);
					uint16_t bit_index = 7 - (pixX % 8);

					if((pixelMap[byte_index] >> bit_index) & 0x01)
						wallCount++;
				}
			}
			tilemap[t_rows][t_cols] = (wallCount > (TILE_SIZE * TILE_SIZE) / 2) ? 1 : 0;;
		}
	}
}

/*
 * findpath_BFS
 *
 * @brief - Does path finding from source to destination by using Breadth First Search Algorithm on the tile map
 * @param - startX, startY: x and y coordinates of the start point
 * 		  - destX, destY: x and y coordinates of the destination point
 * 		  - result: pointer to the tile path where the tank should be followed
 * @note  - this function uses the tile map as the graph. Use build_tilemap first during initialization before
 * 			running this funtion
 */
bool BFS(uint8_t startX, uint8_t startY, uint8_t destX, uint8_t destY, TilePathHandle_TypeDef *result){
	static uint16_t queue[TILE_COUNT];
	static uint16_t parent[TILE_COUNT];
	static uint8_t visited[TILE_COUNT];

	memset(visited, 0, sizeof(visited));
	memset(parent, 0xFF, sizeof(parent));

	uint16_t front = 0, rear = 0; 		// the front and rear location of the queue
	uint16_t start_index = GET_TILEINDEX(startX,startY);
	uint16_t dest_index = GET_TILEINDEX(destX,destY);

	queue[rear++] = start_index;
	visited[start_index] = 1;

	while (front < rear){
		uint16_t current_index = queue[front++];
		uint8_t currentX = GET_TILETX(current_index);
		uint8_t currentY = GET_TILETY(current_index);

		if(current_index == dest_index)
			break;

		for(uint8_t index = 0; index < 4; index++){
			int8_t tileX = currentX + dirX[index];
			int8_t tileY = currentY + dirY[index];

			if(tileX < 0 || tileX >= TILE_COLS || tileY < 0 || tileY >= TILE_ROWS)
				continue;

			uint16_t tileIndex = GET_TILEINDEX(tileX,tileY);

			if(!visited[tileIndex] && !tilemap[tileY][tileX]){
				visited[tileIndex] = 1;
				parent[tileIndex] = current_index;
				queue[rear++] = tileIndex;
			}
		}
	}
	if(parent[dest_index] == 0xFFFF) return false; //return if no path is found

	uint16_t tempPath[MAXPATH_LEN];
	uint8_t len = 0;
	uint16_t step = dest_index;

	while(step != start_index && len < MAXPATH_LEN){
		tempPath[len++] = step;
		step = parent[step];
	}

	result->length = len;
	for(uint8_t i = 0 ; i < len; i++){
		result->tiles[i] = tempPath[len - 1 - i];
	}

	return true;
}

void convertTiletoPixel(uint8_t tileIndex, uint8_t *pixelX, uint8_t *pixelY){
	*pixelX = GET_TILETX(tileIndex) * TILE_SIZE +(TILE_SIZE/2);
	*pixelY = GET_TILETY(tileIndex) * TILE_SIZE +(TILE_SIZE/2);
}

// get a random walkable tile
bool get_random_walkable_tile(uint8_t *outTx, uint8_t *outTy){
    // try up to 32 times to find a walkable tile
    for(uint8_t attempt = 0; attempt < 32; attempt++){
        uint8_t tx = lfsr_rand() % TILE_COLS;
        uint8_t ty = lfsr_rand() % TILE_ROWS;
        if(!tilemap[ty][tx]){
            *outTx = tx;
            *outTy = ty;
            return true;
        }
    }
    return false;
}

static uint16_t lfsr_rand(void){
    // 16-bit Galois LFSR — period of 65535
    lfsr ^= lfsr >> 7;
    lfsr ^= lfsr << 9;
    lfsr ^= lfsr >> 13;
    return lfsr;
}
