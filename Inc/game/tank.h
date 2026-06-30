/*
 * tank.h
 * @creator:	Wagner John Reposilo
 * @date:		6/6/2026
 * @brief:		This module contains main game functions
 */

#ifndef INC_GAME_TANK_H_
#define INC_GAME_TANK_H_


#include "drivers/HW504.h"
#include "gfx/graphics.h"
#include <math.h>

#define TANK_H					8
#define TANK_W					8
#define TANK_SPEED				1

#define MAX_SHOTS				25
#define BULLET_SPEED			2

#define MAX_ENEMIES				10
#define MAX_ENEMY_PATH_LENGTH	4

typedef enum{
	DIR_UP = 0,
	DIR_DOWN,
	DIR_LEFT,
	DIR_RIGHT,
	DIR_STOP
}TankDirection;

typedef enum{
	ENEMY_PATROL = 0,
	ENEMY_CHASE,
	ENEMY_SHOOT,
	ENEMY_HIT
}EnemyState;

typedef struct{
	uint16_t x, y;
	TankDirection dir;
	uint8_t isAlive;
	uint8_t (*sprite)[8];
	uint8_t *currentsprite;
}TankHandle_TypeDef;

typedef struct{
	int16_t x,y;
	TankDirection dir;
	bool isActive;
}BulletHandle_TypeDef;

typedef struct{
	TankHandle_TypeDef 	 	player;
	BulletHandle_TypeDef 	shots[MAX_SHOTS];
	TankDirection 		 	bulletDir;
	uint8_t				 	activeShots;
	uint8_t 			 	map[2048];
	bool					logicTimer;
	bool					renderTimer;
}GameHandle_TypeDef;

void tank_update(GameHandle_TypeDef *gameObject, JoystickData_TypeDef *joystickdata);
void tank_shot(GameHandle_TypeDef *gameObject);

void bullet_update(GameHandle_TypeDef *gameObject);
#endif /* INC_GAME_TANK_H_ */
