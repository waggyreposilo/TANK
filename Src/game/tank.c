/*
 * tank.c
 * @creator:	Wagner John Reposilo
 * @date:		6/6/2026
 * @brief:		This module contains functions for tank movement
 */

#include "game/tank.h"

static bool tank_WallCollision(uint8_t x, uint8_t y, uint8_t w, uint8_t h, TankDirection dir,const uint8_t *map);

void tank_update(GameHandle_TypeDef *gameObject, JoystickData_TypeDef *joystickdata){

	uint16_t tempx = joystickdata->JoystickPosition[0];
	uint16_t tempy = joystickdata->JoystickPosition[1];
	TankDirection currentDir;

	if(tempx >= 800 && (tempx > tempy)){
		currentDir = DIR_LEFT;
	} else if (tempx <= 100 && (tempx < tempy)){
		currentDir = DIR_RIGHT;
	} else if (tempy >= 800 && (tempy > tempx)){
		currentDir = DIR_UP;
	} else if (tempy <= 100 && (tempy < tempx)){
		currentDir = DIR_DOWN;
	} else {
		currentDir = DIR_STOP;
	}

	if(currentDir != DIR_STOP){
		//update the current sprite to its new direction
		gameObject->player.currentsprite = gameObject->player.sprite[currentDir];
		for(uint8_t step = 0; step < TANK_SPEED; step++){
			if(currentDir == DIR_UP && gameObject->player.y < 120){
				if(!tank_WallCollision(gameObject->player.x, gameObject->player.y+1, TANK_W, TANK_H, currentDir, gameObject->map))
					gameObject->player.y++;
				else break;
			} else if (currentDir == DIR_DOWN  && gameObject->player.y > 0){
				if(!tank_WallCollision(gameObject->player.x, gameObject->player.y-2, TANK_W, TANK_H, currentDir, gameObject->map))
					gameObject->player.y--;
				else break;
			} else if (currentDir == DIR_LEFT && gameObject->player.x < 120){
				if(!tank_WallCollision(gameObject->player.x+1, gameObject->player.y, TANK_W, TANK_H, currentDir, gameObject->map))
					gameObject->player.x++;
				else break;
			} else if (currentDir == DIR_RIGHT && gameObject->player.x > 0){
				if(!tank_WallCollision(gameObject->player.x-2, gameObject->player.y, TANK_W, TANK_H, currentDir, gameObject->map))
					gameObject->player.x--;
				else break;
			}
		}
		gameObject->bulletDir = currentDir;
	}
	gameObject->player.dir = currentDir;

}

void tank_shot(GameHandle_TypeDef *gameObject){
	if(gameObject->bulletDir == DIR_STOP) return;
	for(uint8_t i = 0; i < MAX_SHOTS; i++){
		if(!gameObject->shots[i].isActive){
			gameObject->activeShots++;
			gameObject->shots[i].isActive = true;
			gameObject->shots[i].dir = gameObject->bulletDir;
			switch(gameObject->shots[i].dir){
			case DIR_UP:
				gameObject->shots[i].x = gameObject->player.x + 3;
				gameObject->shots[i].y = gameObject->player.y + 7;
				break;
			case DIR_DOWN:
				gameObject->shots[i].x = gameObject->player.x + 3;
				gameObject->shots[i].y = gameObject->player.y - 1;
				break;
			case DIR_LEFT:
				gameObject->shots[i].x = gameObject->player.x + 7;
				gameObject->shots[i].y = gameObject->player.y + 3;
				break;
			case DIR_RIGHT:
				gameObject->shots[i].x = gameObject->player.x - 1;
				gameObject->shots[i].y = gameObject->player.y + 3;
				break;
			case DIR_STOP:
				break;
			}
			return;
		}
	}
}

void bullet_update(GameHandle_TypeDef *gameObject){
	if(!gameObject->activeShots)
		return;
	uint8_t shotsUpdated = 0;
	uint8_t shotsCollided = 0;
	for(uint8_t i = 0; i < MAX_SHOTS && (shotsUpdated <= gameObject->activeShots); i++){
		if(gameObject->shots[i].isActive){
			for(uint8_t step = 0; step < BULLET_SPEED; step++){

				switch(gameObject->shots[i].dir){
					case DIR_UP:	gameObject->shots[i].y++;break;
					case DIR_DOWN:	gameObject->shots[i].y--;break;
					case DIR_LEFT:	gameObject->shots[i].x++;break;
					case DIR_RIGHT:	gameObject->shots[i].x--;break;
					case DIR_STOP:	break;
				}
				//Check if the bullet hits the wall or is out of bounds.
				if((gameObject->shots[i].y < 0 || gameObject->shots[i].y > 124) ||
					(gameObject->shots[i].x < 0 || gameObject->shots[i].x > 124) ||
					tank_WallCollision(gameObject->shots[i].x,
									   gameObject->shots[i].y,
									   2,
									   2,
									   gameObject->shots[i].dir,
									   gameObject->map)){
					gameObject->shots[i].isActive = false;
					shotsCollided++;
					break;
				}
			}

			shotsUpdated++;
		}
	}

	gameObject->activeShots-=shotsCollided;
}

static bool tank_WallCollision(uint8_t x, uint8_t y, uint8_t w, uint8_t h, TankDirection dir,const uint8_t *map){

	uint16_t xLimit = x + w;
	uint16_t yLimit = y + h;
	uint16_t byte_index = 0, bit_index = 0, bitval = 0,
			tempLoop1,tempLoop2,tempLimit2;

	switch(dir){
	case DIR_LEFT:
		tempLoop1 = xLimit;
		tempLoop2 = y;
		tempLimit2 = yLimit;
		break;
	case DIR_RIGHT:
		tempLoop1 = x;
		tempLoop2 = y;
		tempLimit2 = yLimit;
		break;
	case DIR_UP:
		tempLoop1 = x;
		tempLoop2 = yLimit;
		tempLimit2 = xLimit;
		break;
	case DIR_DOWN:
		tempLoop1 = x;
		tempLoop2 = y;
		tempLimit2 = xLimit;
		break;
	default:
		break;
	}
	if(dir == DIR_LEFT || dir == DIR_RIGHT){
		for(; tempLoop2 <= tempLimit2; tempLoop2++){
			byte_index = (tempLoop2 * 16) + (tempLoop1 / 8);
			bit_index  = 7 - (tempLoop1 % 8);
			bitval = ((map[byte_index] >> bit_index) & 0x01);
			if (bitval) return true;
		}
	}
	if(dir == DIR_UP || dir == DIR_DOWN){
		for(; tempLoop1 <= tempLimit2; tempLoop1++){
			byte_index = (tempLoop2 * 16) + (tempLoop1 / 8);
			bit_index  = 7 - (tempLoop1 % 8);
			bitval = ((map[byte_index] >> bit_index) & 0x01);
			if (bitval) return true;
		}
	}

	return false;
}
