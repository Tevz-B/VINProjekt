/*
 * game.h
 *
 *  Created on: Aug 18, 2022
 *      Author: zvet2
 */

#ifndef INC_GAME_H_
#define INC_GAME_H_

#include "joystick.h"
#include "lcd.h"
#include "ugui.h"
#include "ugui_config.h"
#include "lcd_ili9341.h"
#include "LED.h"
#include "kbd.h"

typedef struct
{
	float x;
	float y;

} spd_t;

typedef struct
{
	coord_t pos; //
	int w;
	int h;
	//char[32] label;

} game_button;

typedef struct
{
	uint16_t* matrix;
	uint32_t len;
	uint32_t w;
	uint32_t h;

} GFrame;

typedef struct
{
	coord_t pos;
	spd_t speed;
	int r;

} GBall;

typedef struct
{
	coord_t pos;
	int xDim;
	int yDim;
	int flatHalfDim;
} GPlate;

typedef struct
{
#define NONE 0
#define SWITCH_CONTROLS 1
#define SWITCH_DIRECTION 2
#define SMALL_PLATES 3
	coord_t pos;
	int r;
	int kind;
	int active;

	uint8_t debuffTimer;
	uint8_t debuffDespawnTimer;
	uint16_t debuffActiveTimer;
} GDebuff;

void G_Init(GFrame* f);
void G_setFramePixel(UG_S16 x, UG_S16 y, UG_COLOR c);
void G_update();
void _updateCursor(coord_t js);
void _updatePlateCursor(GPlate* p_plate, coord_t js);
void _updateBall();
void _drawScore();
void _updateDebuff();
void _Grestart();
void _loseScreen();
void _loseScreen2();
coord_t _getButtonInput();


#endif /* INC_GAME_H_ */
