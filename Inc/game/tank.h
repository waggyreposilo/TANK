/*
 * tank.h
 * @creator:	Wagner John Reposilo
 * @date:		6/6/2026
 * @brief:		This module contains main game functions
 */

#ifndef INC_GAME_TANK_H_
#define INC_GAME_TANK_H_

#include "drivers/SH1107.h"
#include "drivers/HW504.h"
#include "gfx/SH1107_GFX.h"
#include "gfx/graphics.h"
#include "game/input.h"
#include "game/BFS.h"
#include <math.h>

#define TANK_H					8
#define TANK_W					8
#define TANK_SPEED				2

#define MAX_SHOTS				20
#define BULLET_SPEED			3

#define MAX_ENEMIES				4
#define MAX_ENEMY_PATH_LENGTH	64
#define ENEMY_SHOT_COOLDOWN_MS  400

typedef enum{
	DIR_UP = 0,
	DIR_DOWN,
	DIR_LEFT,
	DIR_RIGHT,
	DIR_STOP
}TankDirection;

typedef enum{
	ENEMY_PATROL = 0,
	ENEMY_SHOOT
}EnemyState;

typedef enum{
	BULLET_OWNER_PLAYER = 0,
	BULLET_OWNER_ENEMY
}BulletOwner;

typedef enum{
	GAMESTATE_MENU=0,
	GAMESTATE_PLAY,
	GAMESTATE_DEAD
}GameState;

typedef struct{
	uint16_t 				x, y;
	TankDirection 			dir;
	uint8_t 				isAlive;
	uint8_t 				(*sprite)[8];
	uint8_t 				*currentsprite;
}TankHandle_TypeDef;

typedef struct{
	int16_t 				x,y;
	TankDirection 			dir;
	bool 					isActive;
	BulletOwner 			owner;
	uint8_t 				ownerIndex;
}BulletHandle_TypeDef;

typedef struct{
	uint8_t 				x,y;
	uint8_t					explosionFrame;
	uint32_t				explosionTick;
	bool 					isActive;
}ExplosionHandle_TypeDef;

typedef struct{
	TankHandle_TypeDef 		tankInfo;
	TilePathHandle_TypeDef 	path;
    uint8_t 				pathLength;
    uint8_t 				pathIndex;            // current step
    uint32_t 				lastPathCalcTick;
    uint8_t 				lastKnownPlayerTx;
    uint8_t 				lastKnownPlayerTy;
    uint8_t 				patrolTargetX;
    uint8_t 				patrolTargetY;
    uint32_t 				lastShotTick;
    bool 					hasPatrolTarget;
    TankDirection 			bulletDir;
    EnemyState				state;
}EnemyTankHandle_TypeDef;

typedef struct{
	TankHandle_TypeDef 		player;
	EnemyTankHandle_TypeDef enemies[MAX_ENEMIES];
	BulletHandle_TypeDef 	shots[MAX_SHOTS];
	ExplosionHandle_TypeDef explosion;
	TankDirection 		 	bulletDir;
	uint8_t				 	activeShots;
	uint8_t					activeEnemies;
	uint8_t 			 	map[2048];
	uint32_t				enemySpawnTick;
	bool					logicTimer;
	bool					renderTimer;
	uint16_t				gamescore;
    GameState				gamestate;
}GameHandle_TypeDef;

void tank_update(GameHandle_TypeDef *gameObject, JoystickData_TypeDef *joystickdata);
void tank_shot(GameHandle_TypeDef *gameObject);
void bullet_update(GameHandle_TypeDef *gameObject);
void enemy_update(GameHandle_TypeDef *gameObject);

void game_menu(GameHandle_TypeDef *gameObject);
void game_play(GameHandle_TypeDef *gameObject, JoystickData_TypeDef *JoystickSnapshot);
void game_dead(GameHandle_TypeDef *gameObject);

#endif /* INC_GAME_TANK_H_ */
