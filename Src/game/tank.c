/*
 * tank.c
 * @creator:	Wagner John Reposilo
 * @date:		6/6/2026
 * @brief:		This module contains functions for tank movement
 */

#include "game/tank.h"

static void game_init(GameHandle_TypeDef *gameObject);
static void game_deinit(GameHandle_TypeDef *gameObject);
static void enemy_spawn(GameHandle_TypeDef *gameObject);
static void enemy_Routine(GameHandle_TypeDef *gameObject, EnemyTankHandle_TypeDef *enemy, uint8_t enemyIndex);
static void enemy_patrol(GameHandle_TypeDef *gameObject, EnemyTankHandle_TypeDef *enemy);
static void enemy_shootRoutine(GameHandle_TypeDef *gameObject, EnemyTankHandle_TypeDef *enemy, uint8_t enemyIndex);
static void enemy_shot(GameHandle_TypeDef *gameObject, uint8_t enemyIndex);
static void enemy_follow_path(EnemyTankHandle_TypeDef *enemy, GameHandle_TypeDef *gameObject);
static void explosion_update(ExplosionHandle_TypeDef *exp);
static void explosion_draw(GameHandle_TypeDef *gameObject);
static bool tank_WallCollision(uint8_t x, uint8_t y, uint8_t w, uint8_t h, TankDirection dir,const uint8_t *map);
static bool player_TankCollision(uint8_t nextX, uint8_t nextY, GameHandle_TypeDef *gameObject);
static bool enemy_TankCollision(uint8_t nextX, uint8_t nextY, EnemyTankHandle_TypeDef *currentEnemy, GameHandle_TypeDef *gameObject);
static bool isLineofSight(EnemyTankHandle_TypeDef *enemy,GameHandle_TypeDef *gameObject);
static bool checkWallsH(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t *map);
static bool checkWallsV(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t *map);
static void swapPoints(uint8_t* x, uint8_t *y);

static JoystickData_TypeDef spawnpoints[4]={{{4,4},0},{{4,118},0},{{118,118},0},{{118,4},0}};
static TankDirection spawnDirection[4] = {DIR_DOWN,DIR_DOWN,DIR_UP,DIR_UP};
static uint8_t spawnIndex = 0;
static uint32_t finalscore = 0;

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
				if(!tank_WallCollision(gameObject->player.x, gameObject->player.y+1, TANK_W, TANK_H, currentDir, gameObject->map) &&
					!player_TankCollision(gameObject->player.x, gameObject->player.y+1, gameObject))
					gameObject->player.y++;
				else break;
			} else if (currentDir == DIR_DOWN  && gameObject->player.y > 0){
				if(!tank_WallCollision(gameObject->player.x, gameObject->player.y-2, TANK_W, TANK_H, currentDir, gameObject->map) &&
					!player_TankCollision(gameObject->player.x, gameObject->player.y-2, gameObject))
					gameObject->player.y--;
				else break;
			} else if (currentDir == DIR_LEFT && gameObject->player.x < 120){
				if(!tank_WallCollision(gameObject->player.x+1, gameObject->player.y, TANK_W, TANK_H, currentDir, gameObject->map) &&
					!player_TankCollision(gameObject->player.x+1, gameObject->player.y, gameObject))
					gameObject->player.x++;
				else break;
			} else if (currentDir == DIR_RIGHT && gameObject->player.x > 0){
				if(!tank_WallCollision(gameObject->player.x-2, gameObject->player.y, TANK_W, TANK_H, currentDir, gameObject->map) &&
					!player_TankCollision(gameObject->player.x-2, gameObject->player.y, gameObject))
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
            gameObject->shots[i].owner = BULLET_OWNER_PLAYER;
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

				//player bullet hits enemy.
				if(gameObject->shots[i].owner == BULLET_OWNER_PLAYER){
					for(uint8_t index = 0; index < MAX_ENEMIES; index++){
						if(!gameObject->enemies[index].tankInfo.isAlive) continue;

						int16_t enemyLocX = gameObject->enemies[index].tankInfo.x;
						int16_t enemyLocY = gameObject->enemies[index].tankInfo.y;
						int16_t shotsLocX = gameObject->shots[i].x;
						int16_t shotsLocY = gameObject->shots[i].y;

						if(shotsLocX >= enemyLocX && shotsLocX <= enemyLocX + TANK_W &&
								shotsLocY >= enemyLocY && shotsLocY <= enemyLocY + TANK_H){
							gameObject->gamescore++;
							gameObject->enemies[index].tankInfo.isAlive = 0;
							gameObject->activeEnemies--;
							gameObject->shots[i].isActive = false;
							shotsCollided++;
							break;
						}
					}
				}

				//enemy bullet hits player
				if(gameObject->shots[i].owner == BULLET_OWNER_ENEMY && gameObject->shots[i].isActive && gameObject->player.isAlive){
					int16_t playerLocX = gameObject->player.x;
					int16_t playerLocY = gameObject->player.y;
					int16_t shotsLocX = gameObject->shots[i].x;
					int16_t shotsLocY = gameObject->shots[i].y;

					if(shotsLocX >= playerLocX && shotsLocX <= playerLocX + TANK_W &&
							shotsLocY >= playerLocY && shotsLocY <= playerLocY + TANK_H){
						gameObject->activeShots = 0;
						gameObject->player.isAlive = 0;
						gameObject->shots[i].isActive = false;
						gameObject->explosion.isActive = true;
						gameObject->explosion.explosionTick = HAL_GetTick();
						//explosion_start(&gameObject->explosion, gameObject->player.x, gameObject->player.y);
						shotsCollided++;
					}
				}

				//enemy bullets hit enemy tank
				if(gameObject->shots[i].owner == BULLET_OWNER_ENEMY && gameObject->shots[i].isActive
						&& gameObject->activeEnemies != 0){
					for(uint8_t enemyIndex = 0 ; enemyIndex < MAX_ENEMIES; enemyIndex++){
						if(!gameObject->enemies[enemyIndex].tankInfo.isAlive
								|| enemyIndex == gameObject->shots[i].ownerIndex)
							continue;

						int16_t enemyLocX = gameObject->enemies[enemyIndex].tankInfo.x;
						int16_t enemyLocY = gameObject->enemies[enemyIndex].tankInfo.y;
						int16_t shotsLocX = gameObject->shots[i].x;
						int16_t shotsLocY = gameObject->shots[i].y;

						if(shotsLocX >= enemyLocX && shotsLocX <= enemyLocX + TANK_W &&
								shotsLocY >= enemyLocY && shotsLocY <= enemyLocY + TANK_H){
							gameObject->shots[i].isActive = false;
							shotsCollided++;
							break;
						}
					}
				}

				//bullet hits another bullet
				if(gameObject->shots[i].isActive && (gameObject->activeShots - 1 != 0)){
					for(uint8_t bulletIndex = 0; bulletIndex < MAX_SHOTS; bulletIndex++){
						if(bulletIndex == i || !gameObject->shots[bulletIndex].isActive)
							continue;

						int16_t enemyLocX = gameObject->shots[bulletIndex].x;
						int16_t enemyLocY = gameObject->shots[bulletIndex].y;
						int16_t shotsLocX = gameObject->shots[i].x;
						int16_t shotsLocY = gameObject->shots[i].y;

						if(shotsLocX >= enemyLocX && shotsLocX <= enemyLocX + 2 &&
								shotsLocY >= enemyLocY && shotsLocY <= enemyLocY + 2){
							gameObject->shots[i].isActive = false;
							gameObject->shots[bulletIndex].isActive = false;
							shotsCollided+=2;
							break;
						}

					}
				}
			}

			shotsUpdated++;
		}
	}

	gameObject->activeShots-=shotsCollided;
}

void enemy_update(GameHandle_TypeDef *gameObject){
	if(!gameObject->activeEnemies) return;
	uint8_t enemiesUpdated = 0;
	for(uint8_t i = 0 ; i < MAX_ENEMIES && enemiesUpdated < gameObject->activeEnemies; i++){
		if(!gameObject->enemies[i].tankInfo.isAlive) continue;
		EnemyTankHandle_TypeDef *enemy = &gameObject->enemies[i];

		enemy_Routine(gameObject, enemy, i);
		enemiesUpdated++;
	}

}

void game_menu(GameHandle_TypeDef *gameObject){
	if(gameObject->renderTimer && !SH1107_isBusy()){
		uint8_t logoLocX = (SH1107_WIDTH/2)-(52/2);
		uint8_t logoLocY = (SH1107_HEIGHT/2)-(52/2);
		gameObject->renderTimer = false;
		SH1107_NewFrame();
		SH1107_DrawBitMap(logoLocX, logoLocY, menu_logo, 52, 16, WHITE);
		SH1107_DrawString(47,58,(uint8_t *)"STM32",WHITE,5);
		//SH1107_DrawString(1,1,(uint8_t *)"TANK STM32",WHITE,9);
		SH1107_DrawRoundedRect(26, 76, 77, 16, 3, WHITE);
		SH1107_DrawBitMap(14, 80, enemy_tank[DIR_LEFT], 8, 8, WHITE);
		SH1107_DrawString(30,80,(uint8_t *)"START GAME",WHITE,10);
		SH1107_Display();
	}
	if(InputRead()) {
		game_init(gameObject);
		gameObject->gamestate = GAMESTATE_PLAY;
	}

}

void game_play(GameHandle_TypeDef *gameObject, JoystickData_TypeDef *JoystickSnapshot){
	  if(gameObject->logicTimer){
		  gameObject->logicTimer = false;

		  if(!gameObject->player.isAlive){
			  explosion_update(&gameObject->explosion);
		  } else {
			  bullet_update(gameObject);
			  tank_update(gameObject, JoystickSnapshot);

			  if(InputRead() && gameObject->player.isAlive){
				  tank_shot(gameObject);
			  }

			  if(gameObject->activeEnemies)
				  enemy_update(gameObject);
			  enemy_spawn(gameObject);
		  }
	  }


	  if(gameObject->renderTimer && !SH1107_isBusy()){
		  gameObject->renderTimer = false;
		  SH1107_NewFrame();
		  SH1107_DrawBitMap(0, 0, gameObject->map, 128, 128, WHITE);
		  if((gameObject->activeShots != 0)){
			  for(uint8_t i = 0, drawnShots = 0;i < MAX_SHOTS && (drawnShots <= gameObject->activeShots); i++){
				  if(gameObject->shots[i].isActive){
					  SH1107_DrawBitMap(gameObject->shots[i].x, gameObject->shots[i].y, bullet, 8, 2, WHITE);
					  drawnShots++;
				  }
			  }
		  }

		  if((gameObject->activeEnemies != 0)){
			  for(uint8_t i = 0, drawnEnemies = 0;i < MAX_ENEMIES && (drawnEnemies <= gameObject->activeEnemies); i++){
				  if(gameObject->enemies[i].tankInfo.isAlive){
					  SH1107_DrawBitMap(gameObject->enemies[i].tankInfo.x,
							  gameObject->enemies[i].tankInfo.y,
							  gameObject->enemies[i].tankInfo.currentsprite,
							  8, 8, WHITE);
					  drawnEnemies++;
				  }
			  }
		  }

		  if(gameObject->player.isAlive)
			  SH1107_DrawBitMap(gameObject->player.x, gameObject->player.y, gameObject->player.currentsprite, 8, 8, WHITE);
		  else {
			  //gameObject->activeEnemies = 0;
			  explosion_draw(gameObject);

			  if(!gameObject->explosion.isActive){
				  game_deinit(gameObject);
				  gameObject->gamestate = GAMESTATE_DEAD;
			  }
		  }
		  SH1107_Display();
	  }
}

void game_dead(GameHandle_TypeDef *gameObject){

	if(gameObject->renderTimer && !SH1107_isBusy()){
		uint8_t CenterX = (SH1107_WIDTH/2);
		uint8_t CenterY = (SH1107_HEIGHT/2);
		gameObject->renderTimer = false;
		SH1107_NewFrame();
		SH1107_DrawBitMap(CenterX - (118/2),CenterY - (20), gameover_logo, 117, 16, WHITE);
		SH1107_DrawString(CenterX - 34, CenterY + 5,(uint8_t *)"Score:",WHITE,5);
		SH1107_DrawInt(CenterX+7, CenterY + 6, finalscore, WHITE, 8);
		//SH1107_DrawString(CenterX - (62/2),CenterY-(62/2),(uint8_t *)"GAME OVER",WHITE,8);
		//SH1107_DrawString(1,20,(uint8_t *)"FIRE TO PLAY AGAIN",WHITE,18);
		SH1107_Display();
	}

	if(InputRead()) {
		gameObject->gamestate = GAMESTATE_MENU;
	}

}

/*
 * Static Function Implementations
 */

static void game_init(GameHandle_TypeDef *gameObject){
	gameObject->gamescore = 0;
	gameObject->player.dir = DIR_UP;
	gameObject->player.isAlive = 1;
	gameObject->player.x = 60;
	gameObject->player.y = 60;
	gameObject->player.sprite = player_tank;
	gameObject->player.currentsprite = player_tank[0];
	gameObject->bulletDir = DIR_UP;
	gameObject->logicTimer = false;
	gameObject->renderTimer = false;
	gameObject->explosion.explosionFrame = 0;
	memcpy(gameObject->map,map_1,sizeof(gameObject->map));
	build_tilemap(gameObject->map);
}

static void game_deinit(GameHandle_TypeDef *gameObject){
	finalscore = gameObject->gamescore * 2;

	gameObject->gamescore = 0;
	gameObject->activeEnemies = 0;
	for(uint8_t i = 0; i < MAX_ENEMIES; i++){
		EnemyTankHandle_TypeDef *enemy = &gameObject->enemies[i];

		if(!enemy->tankInfo.isAlive) continue;
		enemy->tankInfo.isAlive = 0;
	}
	gameObject->activeShots = 0;
	for(uint8_t i = 0; i < MAX_SHOTS; i++){
		BulletHandle_TypeDef *bullet = &gameObject->shots[i];

		if(!bullet->isActive) continue;
		bullet->isActive = false;
	}

	gameObject->explosion.isActive = false;
	gameObject->explosion.explosionFrame = 0;
}

static void enemy_spawn(GameHandle_TypeDef *gameObject){
	if(gameObject->activeEnemies >= MAX_ENEMIES) return;
	if(HAL_GetTick() - gameObject->enemySpawnTick <= 2500) return;

	for(uint16_t i = 0; i < MAX_ENEMIES; i++){
		if(gameObject->enemies[i].tankInfo.isAlive) {
			bool isInboundsX = (gameObject->enemies[i].tankInfo.x <= spawnpoints[spawnIndex].JoystickPosition[0] + TANK_W)
					|| (gameObject->enemies[i].tankInfo.x+TANK_W >= spawnpoints[spawnIndex].JoystickPosition[0]);
			bool isInboundsY = (gameObject->enemies[i].tankInfo.y <= spawnpoints[spawnIndex].JoystickPosition[1] + TANK_H)
					|| (gameObject->enemies[i].tankInfo.y+TANK_H >= spawnpoints[spawnIndex].JoystickPosition[1]);
			if(isInboundsX && isInboundsY)
			{
				spawnIndex++;
				if(spawnIndex >= 4) spawnIndex = 0;
			}

			continue;
		}
		gameObject->activeEnemies++;
		gameObject->enemySpawnTick = HAL_GetTick();
		gameObject->enemies[i].tankInfo.x = spawnpoints[spawnIndex].JoystickPosition[0];
		gameObject->enemies[i].tankInfo.y = spawnpoints[spawnIndex].JoystickPosition[1];
		gameObject->enemies[i].tankInfo.dir = spawnDirection[spawnIndex];
		gameObject->enemies[i].tankInfo.isAlive = 1;
		gameObject->enemies[i].tankInfo.sprite = enemy_tank;
		gameObject->enemies[i].tankInfo.currentsprite = enemy_tank[gameObject->enemies[i].tankInfo.dir];
		gameObject->enemies[i].state = ENEMY_PATROL;
		spawnIndex++;
		if(spawnIndex >= 4) {
			spawnIndex = 0;
		}
		break;
	}
}

static void enemy_Routine(GameHandle_TypeDef *gameObject, EnemyTankHandle_TypeDef *enemy, uint8_t enemyIndex){
	switch(enemy->state){
	case ENEMY_PATROL:
		enemy_patrol(gameObject, enemy);
		break;
	case ENEMY_SHOOT:
		enemy_shootRoutine(gameObject, enemy, enemyIndex);
		break;
	}
}

static void enemy_patrol(GameHandle_TypeDef *gameObject, EnemyTankHandle_TypeDef *enemy){
    uint8_t enemy_Tx = enemy->tankInfo.x / TILE_SIZE;
    uint8_t enemy_Ty = enemy->tankInfo.y / TILE_SIZE;

    bool isPathExhausted = (enemy->pathIndex >= enemy->path.length);
    bool isTimedOut      = (HAL_GetTick() - enemy->lastPathCalcTick > 1000);

    if(isPathExhausted || isTimedOut || !enemy->hasPatrolTarget){
        // pick a new random patrol target
        uint8_t destTx, destTy;
        if(get_random_walkable_tile(&destTx, &destTy)){
            if(BFS(enemy_Tx, enemy_Ty, destTx, destTy, &enemy->path)){
                enemy->pathIndex          = 0;
                enemy->lastPathCalcTick   = HAL_GetTick();
                enemy->patrolTargetX      = destTx;
                enemy->patrolTargetY      = destTy;
                enemy->hasPatrolTarget    = true;
            }
        }
    }

    enemy_follow_path(enemy, gameObject);
    enemy->bulletDir = enemy->tankInfo.dir;
   if(isLineofSight(enemy, gameObject)) enemy->state = ENEMY_SHOOT;
}

static void enemy_shootRoutine(GameHandle_TypeDef *gameObject, EnemyTankHandle_TypeDef *enemy, uint8_t enemyIndex){
    if(!isLineofSight(enemy, gameObject) || !gameObject->player.isAlive){
        enemy->state = ENEMY_PATROL;  // lost sight — go back to patrolling
        return;
    }
	bool iscoolDownReady = (HAL_GetTick() - enemy->lastShotTick) > ENEMY_SHOT_COOLDOWN_MS;
	if(iscoolDownReady){
		enemy_shot(gameObject,enemyIndex);
		enemy->lastShotTick = HAL_GetTick();
	}
}

static void enemy_shot(GameHandle_TypeDef *gameObject, uint8_t enemyIndex){

    EnemyTankHandle_TypeDef *enemy = &gameObject->enemies[enemyIndex];
    if(enemy->bulletDir == DIR_STOP) return;

    for(uint8_t i = 0; i < MAX_SHOTS; i++){
        if(gameObject->shots[i].isActive) continue;

        gameObject->activeShots++;
        gameObject->shots[i].isActive   = true;
        gameObject->shots[i].owner      = BULLET_OWNER_ENEMY;
        gameObject->shots[i].ownerIndex = enemyIndex;
        gameObject->shots[i].dir        = enemy->bulletDir;

        switch(enemy->bulletDir){
        case DIR_UP:
            gameObject->shots[i].x = enemy->tankInfo.x + 3;
            gameObject->shots[i].y = enemy->tankInfo.y + 7;
            break;
        case DIR_DOWN:
            gameObject->shots[i].x = enemy->tankInfo.x + 3;
            gameObject->shots[i].y = enemy->tankInfo.y - 1;
            break;
        case DIR_LEFT:
            gameObject->shots[i].x = enemy->tankInfo.x + 7;
            gameObject->shots[i].y = enemy->tankInfo.y + 3;
            break;
        case DIR_RIGHT:
            gameObject->shots[i].x = enemy->tankInfo.x - 1;
            gameObject->shots[i].y = enemy->tankInfo.y + 3;
            break;
        default: break;
        }
        return;
    }
}

static void explosion_update(ExplosionHandle_TypeDef *exp){
	if(!exp->isActive) return;
	if(HAL_GetTick() - exp->explosionTick > 150){
		exp->explosionFrame++;
		exp->explosionTick = HAL_GetTick();
        if(exp->explosionFrame >= 4)
            exp->isActive = false;
	}
}

static void explosion_draw(GameHandle_TypeDef *gameObject){
	if(!gameObject->explosion.isActive) return;

	SH1107_DrawBitMap(gameObject->player.x, gameObject->player.y,
			explosion_frames[gameObject->explosion.explosionFrame], 8, 8, WHITE);
}

static bool tank_WallCollision(uint8_t x, uint8_t y, uint8_t w, uint8_t h, TankDirection dir,const uint8_t *map){

	uint16_t xEnd = x + w;
	uint16_t yEnd = y + h;
	uint16_t byte_index = 0, bit_index = 0, bitval = 0,
			loopX,loopY,loopEnd;

	switch(dir){
	case DIR_LEFT:
		loopX = xEnd;
		loopY = y;
		loopEnd = yEnd;
		break;
	case DIR_RIGHT:
		loopX = x;
		loopY = y;
		loopEnd = yEnd;
		break;
	case DIR_UP:
		loopX = x;
		loopY = yEnd;
		loopEnd = xEnd;
		break;
	case DIR_DOWN:
		loopX = x;
		loopY = y;
		loopEnd = xEnd;
		break;
	default:
		break;
	}
	if(dir == DIR_LEFT || dir == DIR_RIGHT){
		for(; loopY <= loopEnd; loopY++){
			byte_index = (loopY * 16) + (loopX / 8);
			bit_index  = 7 - (loopX % 8);
			bitval = ((map[byte_index] >> bit_index) & 0x01);
			if (bitval) return true;
		}
	}
	if(dir == DIR_UP || dir == DIR_DOWN){
		for(; loopX <= loopEnd; loopX++){
			byte_index = (loopY * 16) + (loopX / 8);
			bit_index  = 7 - (loopX % 8);
			bitval = ((map[byte_index] >> bit_index) & 0x01);
			if (bitval) return true;
		}
	}

	return false;
}

static bool player_TankCollision(uint8_t nextX, uint8_t nextY, GameHandle_TypeDef *gameObject){
    for(uint8_t i = 0; i < MAX_ENEMIES; i++){
        EnemyTankHandle_TypeDef *other = &gameObject->enemies[i];

        // skip dead enemies
        if(!other->tankInfo.isAlive) continue;

        uint8_t ox = other->tankInfo.x;
        uint8_t oy = other->tankInfo.y;

        // AABB overlap check
        bool overlapX = (nextX < ox + TANK_W) && (nextX + TANK_W > ox);
        bool overlapY = (nextY < oy + TANK_H) && (nextY + TANK_H > oy);

        if(overlapX && overlapY) return true;
    }
    return false;
}

static bool enemy_TankCollision(uint8_t nextX, uint8_t nextY, EnemyTankHandle_TypeDef *currentEnemy, GameHandle_TypeDef *gameObject){
    for(uint8_t i = 0; i < MAX_ENEMIES; i++){
        EnemyTankHandle_TypeDef *other = &gameObject->enemies[i];

        // skip self and dead enemies
        if(other == currentEnemy || !other->tankInfo.isAlive) continue;

        uint8_t ox = other->tankInfo.x;
        uint8_t oy = other->tankInfo.y;

        // AABB overlap check
        bool overlapX = (nextX < ox + TANK_W) && (nextX + TANK_W > ox);
        bool overlapY = (nextY < oy + TANK_H) && (nextY + TANK_H > oy);

        if(overlapX && overlapY) return true;
    }
    return false;
}

static void enemy_follow_path(EnemyTankHandle_TypeDef *enemy, GameHandle_TypeDef *gameObject){
	if(enemy->pathIndex == enemy->path.length)
		return;

	uint8_t targetPx, targetPy;
	int8_t dx, dy;
	convertTiletoPixel(enemy->path.tiles[enemy->pathIndex], &targetPx, &targetPy);

	dx = (int8_t)targetPx - (int8_t)enemy->tankInfo.x;
	dy = (int8_t)targetPy - (int8_t)enemy->tankInfo.y;

	int8_t tolerance = TANK_SPEED+1;
	if(dx >= 0-tolerance && dx <= tolerance && dy >= 0-tolerance && dy <= tolerance){
		enemy->pathIndex++;
		return;
	}

    TankDirection nextDir;
    uint8_t nextX = enemy->tankInfo.x;
    uint8_t nextY = enemy->tankInfo.y;

    if(dx > 0){
        nextDir = DIR_LEFT;
        nextX = (nextX < ENEMY_MAX_X) ? nextX + TANK_SPEED : ENEMY_MAX_X;
    } else if(dx < 0){
        nextDir = DIR_RIGHT;
        nextX = (nextX > 0) ? nextX - TANK_SPEED : 0;
    } else if(dy > 0){
        nextDir = DIR_UP;
        nextY = (nextY < ENEMY_MAX_Y) ? nextY + TANK_SPEED : ENEMY_MAX_Y;
    } else if(dy < 0){
        nextDir = DIR_DOWN;
        nextY = (nextY > 0) ? nextY - TANK_SPEED : 0;
    } else {
        return;
    }

    if(tank_WallCollision(nextX, nextY, TANK_W, TANK_H, nextDir, gameObject->map)){
        uint8_t enemy_Tx = enemy->tankInfo.x / TILE_SIZE;
        uint8_t enemy_Ty = enemy->tankInfo.y / TILE_SIZE;
        uint8_t dest_Tx, dest_Ty;

        // pick new patrol target instead of going to player
        if(get_random_walkable_tile(&dest_Tx, &dest_Ty)){
            if(BFS(enemy_Tx, enemy_Ty, dest_Tx, dest_Ty, &enemy->path)){
                enemy->pathIndex		 = 0;
                enemy->lastPathCalcTick	 = HAL_GetTick();
                enemy->patrolTargetX	 = dest_Tx;
                enemy->patrolTargetY	 = dest_Ty;
                enemy->hasPatrolTarget	 = true;
            }
        } else {
            enemy->pathIndex = enemy->path.length;
        }
        return;
    }

    if(enemy_TankCollision(nextX, nextY, enemy, gameObject)){
        // pick new patrol target instead of going to player
        if(HAL_GetTick() - enemy->lastPathCalcTick > 300){
            uint8_t enemy_Tx = enemy->tankInfo.x / TILE_SIZE;
            uint8_t enemy_Ty = enemy->tankInfo.y / TILE_SIZE;
            uint8_t dest_Tx, dest_Ty;

            if(get_random_walkable_tile(&dest_Tx, &dest_Ty)){
                if(BFS(enemy_Tx, enemy_Ty, dest_Tx, dest_Ty, &enemy->path)){
                    enemy->pathIndex       = 0;
                    enemy->lastPathCalcTick = HAL_GetTick();
                    enemy->patrolTargetX   = dest_Tx;
                    enemy->patrolTargetY   = dest_Ty;
                    enemy->hasPatrolTarget = true;
                }
            }
        }
        return;
    }

    enemy->tankInfo.x = nextX;
    enemy->tankInfo.y = nextY;
    enemy->tankInfo.dir = nextDir;
	enemy->tankInfo.currentsprite = enemy->tankInfo.sprite[enemy->tankInfo.dir];

	if(enemy->tankInfo.x > ENEMY_MAX_X) enemy->tankInfo.x = ENEMY_MAX_X;
	if(enemy->tankInfo.y > ENEMY_MAX_Y) enemy->tankInfo.y = ENEMY_MAX_Y;
	if(enemy->tankInfo.x > 0xF000) enemy->tankInfo.x = 0;
	if(enemy->tankInfo.y > 0xF000) enemy->tankInfo.y = 0;
}

static bool isLineofSight(EnemyTankHandle_TypeDef *enemy,GameHandle_TypeDef *gameObject){
	TankDirection enemyDir = enemy->tankInfo.dir;
	uint16_t enemyLocX = enemy->tankInfo.x;
	uint16_t enemyLocY = enemy->tankInfo.y;
	uint16_t playerLocX = gameObject->player.x;
	uint16_t playerLocY = gameObject->player.y;

	bool LocationInline = false;

	if((enemyDir == DIR_UP || enemyDir ==DIR_DOWN) &&
		((playerLocX + 7) >= enemyLocX && (playerLocX + 7) <= (enemyLocX+7)))
		LocationInline = true;
	if((enemyDir == DIR_LEFT || enemyDir == DIR_RIGHT) &&
		((playerLocY) >= enemyLocY && (playerLocY) <= (enemyLocY+7)))
		LocationInline = true;

	if(!LocationInline) return false;

	uint16_t shotX,shotY;
	bool isWall = false;

	switch(enemyDir){
		case DIR_UP:
	        shotX = enemyLocX + 3;
	        shotY = enemyLocY + 7;
			break;
		case DIR_DOWN:
	        shotX = enemyLocX + 3;
	        shotY = enemyLocY - 1;
			break;
		case DIR_LEFT:
	        shotX = enemyLocX + 7;
	        shotY = enemyLocY + 3;
			break;
		case DIR_RIGHT:
	        shotX = enemyLocX - 1;
	        shotY = enemyLocY + 3;
			break;
		case DIR_STOP:
			break;
	}

	//Use Bresenham's Line Algo to check if there are walls between the enemy and player tank.
	if(enemyDir == DIR_UP || enemyDir ==DIR_DOWN)
		isWall = checkWallsV(shotX,shotY,shotX,playerLocY,gameObject->map);
	if(enemyDir == DIR_LEFT || enemyDir == DIR_RIGHT)
        isWall = checkWallsH(shotX,shotY,playerLocX,shotY,gameObject->map);

	if(isWall) return false;

	return true;
}


static bool checkWallsH(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t *map){
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
			//SH1107_DrawPixel(x1 + i, currentY, WHITE);
			uint8_t cell = map[(x1 + i) + ((currentY/8) * (128))];

			cell &= (1 << (currentY % 8));

			if(cell) return true;
			if(p >= 0){
				currentY += dirY;
				p = p - (2*dx);
			}
			p = p + (2*dy);
		}
	}
	return false;
}

static bool checkWallsV(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t *map){
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
			//SH1107_DrawPixel(currentX, y1 + i, color);
			uint8_t cell = map[currentX + (((y1 + i)/8) * (128))];

			cell &= (1 << ((y1 + i) % 8));

			if(cell) return true;

			if(p >= 0){
				currentX += dirX;
				p = p - (2*dy);
			}
			p = p + (2*dx);
		}
	}
	return false;
}

static void swapPoints(uint8_t* x, uint8_t *y){
	uint8_t temp = 0;
	temp = *x;
	*x = *y;
	*y = temp;
}
