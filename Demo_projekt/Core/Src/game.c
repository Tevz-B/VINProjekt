/*
 * game.c
 *
 *  Created on: Aug 18, 2022
 *      Author: zvet2
 */
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "game.h"
#include "lcd.h"
#include "ugui.h"
#include "ugui_config.h"
#include "joystick.h"

#define PLATE_DIM_Y 20
#define PLATE_DIM_X 2
#define PLATE_DIM_FLAT 6
#define BALL_SPEED_BONUS 0.5

static GFrame* gFrame;
static coord_t cursor;
static GPlate plate;
static GPlate plate2;

static GBall ball;
static int lost;
static int lost2;

static int score;
static int lightTimer;
static int ballSpeedLevel;

static GDebuff debuff;
//static uint8_t debuffTimer;
//static uint8_t debuffDespawnTimer;
//static uint16_t debuffActiveTimer;


void G_Init(GFrame* frame)
{
	// frame init
	frame->w = 200; //ILI9341_GetParam(LCD_WIDTH);
	frame->h = 150; //ILI9341_GetParam(LCD_HEIGHT);
	frame->len = frame->w * frame->h;
	frame->matrix = (uint16_t*) malloc(frame->len);

	gFrame = frame;

	// cursor init
	cursor.x = 50;
	cursor.y = 50;

	plate.pos.x = 10;
	plate.pos.y = 50;
	plate.xDim = PLATE_DIM_X;
	plate.yDim = PLATE_DIM_Y;
	plate.flatHalfDim = PLATE_DIM_FLAT;

	plate2.pos.x = frame->w -10;
	plate2.pos.y = 50;
	plate2.xDim = PLATE_DIM_X;
	plate2.yDim = PLATE_DIM_Y;
	plate2.flatHalfDim = PLATE_DIM_FLAT;

	// ball init
	ball.pos.x = 20;
	ball.pos.y = 50;
	ball.speed.x = 3.f;
	ball.speed.y = 1.5f;
	ball.r = 5;
	ballSpeedLevel = 0;

	// score
	lost = 0;
	lost2 = 0;
	score = 0;

	lightTimer = 0;

	// Debuff
	srand(344);
	debuff.kind = NONE;
	debuff.pos.x = 70;
	debuff.pos.y = 70;
	debuff.r = 10;
	debuff.active = 0;
	debuff.debuffTimer = 0;
	debuff.debuffDespawnTimer = 0;
	debuff.debuffActiveTimer = 0;

}

void G_setFramePixel(UG_S16 x, UG_S16 y, UG_COLOR c)
{
	uint32_t index = (uint32_t)(uint16_t)gFrame->w * (uint32_t)(uint16_t)y + (uint32_t)(uint16_t)x;
	gFrame->matrix[ index ] = c;
}

void G_update()
{
	// background
	UG_FillScreen(C_MIDNIGHT_BLUE);
	// cursor
	//_updateCursor( joystick_get() );
	if (!lost && !lost2)
	{

		_updateDebuff();
		_updatePlateCursor(&plate, joystick_get() );
		_updatePlateCursor(&plate2, _getButtonInput() );
		_updateBall();

		// score
		_drawScore();
		ILI9341_SendData(gFrame->matrix, gFrame->len);


	}
	else
	{
		if (lost2)
			_loseScreen2();
		else
			_loseScreen();
		_Grestart();
		ILI9341_SendData(gFrame->matrix, gFrame->len);

		for (int i=0; i<3; i++)
		{
			LEDs_on(0xFF);
			HAL_Delay(200);
			LEDs_off(0xFF);
			HAL_Delay(200);
		}

		_Grestart();

	}
}

coord_t _getButtonInput()
{
	int speed;
	coord_t out;

	out.x = 0;
	out.y = 0;
	speed = 5;
	if (!KBD_get_button_state(BTN_UP)) {
		out.y = -speed;
	}
	if (!KBD_get_button_state(BTN_DOWN)) {
		out.y = speed;
	}
	if (!KBD_get_button_state(BTN_LEFT)) {
		out.x = -speed;
	}
	if (!KBD_get_button_state(BTN_RIGHT)) {
		out.x = speed;
	}

	return out;
}

void _updateBall()
{
	ball.pos.x += (int) ball.speed.x;
	ball.pos.y += (int) ball.speed.y;

	lightTimer++;

	// bounce walls

	int r = ball.r;
	int bounds[] = { 0+r, gFrame->w -r-1, 0+r, gFrame->h -r-1 };
	if (ball.pos.x > bounds[1]) {
		lost2++;
		ball.pos.x = bounds[1];
		ball.speed.x = - ball.speed.x;
		//score += abs((int)ball.speed.x) + abs((int)ball.speed.y);

	} else if (ball.pos.x < bounds[0]) {
		lost++;
		ball.pos.x = bounds[0];
		ball.speed.x = - ball.speed.x;
	}
	if (ball.pos.y > bounds[3]) {
		ball.pos.y = bounds[3];
		ball.speed.y = - ball.speed.y;
		score += abs((int)ball.speed.x) + abs((int)ball.speed.y);
	} else if (ball.pos.y < bounds[2]) {
		ball.pos.y = bounds[2];
		ball.speed.y = - ball.speed.y;
		score += abs((int)ball.speed.x) + abs((int)ball.speed.y);
	}

	// bounce plate

	// if ball inside plate1
	if ( abs(ball.pos.x - plate.pos.x) < ball.r-1 + plate.xDim && abs(ball.pos.y - plate.pos.y) < ball.r-1 + plate.yDim ) {

		// dont bounce if too low etc.
		if (ball.speed.x > 0 || abs(ball.pos.x - plate.pos.x) < plate.xDim) { // -1 at plate.xDim?
			lost++;
			return;
		}

		// modifiy direction - max ball speed y = 3.15
		if (ball.pos.y < plate.pos.y - plate.flatHalfDim) {
			float currSpd2 = ball.speed.y * ball.speed.y + ball.speed.x * ball.speed.x;
			ball.speed.y -= 0.7;
			if (ball.speed.y > 3.15) ball.speed.y = 3.15;
			else if (ball.speed.y < -3.15) ball.speed.y = -3.15;
			ball.speed.x =  (float)sqrt(currSpd2 - ball.speed.y * ball.speed.y);
		} else if (ball.pos.y > plate.pos.y + plate.flatHalfDim) {
			float currSpd2 = ball.speed.y * ball.speed.y + ball.speed.x * ball.speed.x;
			ball.speed.y += 0.7;
			if (ball.speed.y > 3.15) ball.speed.y = 3.15;
			else if (ball.speed.y < -3.15) ball.speed.y = -3.15;
			ball.speed.x =  (float)sqrt(currSpd2 - ball.speed.y * ball.speed.y);
		} else {
			ball.speed.x = -ball.speed.x;
		}
		// lights
		LEDs_on(0xFF);
		lightTimer = -10;
	}

	// if ball inside plate2
	if ( abs(ball.pos.x - plate2.pos.x) < ball.r-1 + plate2.xDim && abs(ball.pos.y - plate2.pos.y) < ball.r-1 + plate2.yDim ) {

		// dont bounce if too low etc.
		if (ball.speed.x < 0 || abs(ball.pos.x - plate2.pos.x) < plate2.xDim) {
			lost2++;
			return;
		}

		// modifiy direction
		if (ball.pos.y < plate2.pos.y - plate2.flatHalfDim) {
			float currSpd2 = ball.speed.y * ball.speed.y + ball.speed.x * ball.speed.x;
			ball.speed.y -= 0.7;
			if (ball.speed.y > 3.15) ball.speed.y = 3.15;
			else if (ball.speed.y < -3.15) ball.speed.y = -3.15;
			ball.speed.x = - (float)sqrt(currSpd2 - ball.speed.y * ball.speed.y);
		} else if (ball.pos.y > plate2.pos.y + plate2.flatHalfDim) {
			float currSpd2 = ball.speed.y * ball.speed.y + ball.speed.x * ball.speed.x;
			ball.speed.y += 0.7;
			if (ball.speed.y > 3.15) ball.speed.y = 3.15;
			else if (ball.speed.y < -3.15) ball.speed.y = -3.15;
			ball.speed.x = - (float)sqrt(currSpd2 - ball.speed.y * ball.speed.y);
		} else {
			ball.speed.x = -ball.speed.x;
		}
		// lights
		LEDs_on(0xFF);
		lightTimer = -10;
	}

	if (lightTimer > 0) {
		LEDs_off(0xFF);
	}

	// increase speed level
	for ( int i=0; i<10; i++) {
		if (ballSpeedLevel == i && score > (i+1) * 10) {
			ballSpeedLevel = i+1;
			if (ball.speed.x > 0) ball.speed.x += BALL_SPEED_BONUS;
			else ball.speed.x -= BALL_SPEED_BONUS;

			break;
		}
	}

	// draw
	UG_FillCircle(ball.pos.x, ball.pos.y, ball.r, C_WHITE);
}

void _updateCursor(coord_t js)
{
	int r = 5;
	int bounds[] = { 0+r, gFrame->w -r-1, 0+r, gFrame->h -r-1 };

	cursor.x += js.x;
	cursor.y += js.y;

	if (cursor.x > bounds[1]) {
		cursor.x = bounds[1];
	} else if (cursor.x < bounds[0]) {
		cursor.x = bounds[0];
	}
	if (cursor.y > bounds[3]) {
		cursor.y = bounds[3];
	} else if (cursor.y < bounds[2]) {
		cursor.y = bounds[2];
	}
	UG_FillCircle(cursor.x, cursor.y, r, C_WHITE);
}

void _updatePlateCursor(GPlate* p_plate, coord_t js)
{
	int x = p_plate->xDim;
	int y = p_plate->yDim;
	int bounds[] = { 0+x, gFrame->w -x-1, 0+y, gFrame->h -y-1 };

	//p_plate->x += js.x;
	p_plate->pos.y += js.y;

	if (p_plate->pos.x > bounds[1]) {
		p_plate->pos.x = bounds[1];
	} else if (p_plate->pos.x < bounds[0]) {
		p_plate->pos.x = bounds[0];
	}
	if (p_plate->pos.y > bounds[3]) {
		p_plate->pos.y = bounds[3];
	} else if (p_plate->pos.y < bounds[2]) {
		p_plate->pos.y = bounds[2];
	}
	UG_FillFrame(p_plate->pos.x + x, p_plate->pos.y + y, p_plate->pos.x - x, p_plate->pos.y - y, C_GOLD);
}

void _drawScore()
{
	char str[8];
	sprintf(str, "%d", score);
	UG_FontSelect(&FONT_8X12);
	UG_PutString(100, 20, str);
}

void _Grestart()
{
	// cursor re init
	cursor.x = 50;
	cursor.y = 50;

	plate.xDim = PLATE_DIM_X;
	plate.yDim = PLATE_DIM_Y;
	plate.flatHalfDim = PLATE_DIM_FLAT;

	plate2.xDim = PLATE_DIM_X;
	plate2.yDim = PLATE_DIM_Y;
	plate2.flatHalfDim = PLATE_DIM_FLAT;


	// ball re init
	ball.pos.x = 20;
	ball.pos.y = 50;
	ball.speed.x = 3.f;
	ball.speed.y = 1.5f;
	ballSpeedLevel = 0;

	// score re init
	lost = 0;
	lost2 = 0;
	score = 0;

	// Debuff
	debuff.kind = NONE;
	debuff.pos.x = 70;
	debuff.pos.y = 70;
	debuff.debuffTimer = 0;
	debuff.debuffDespawnTimer = 0;
}

void _updateDebuff() {
	if (debuff.active) {
		debuff.debuffActiveTimer++;
		//debuffDespawnTimer++;
		if (debuff.debuffActiveTimer >= 300) {
			debuff.active = 0;
			debuff.debuffActiveTimer = 0;

			if (debuff.kind == SMALL_PLATES) {
				plate.yDim = PLATE_DIM_Y;
				plate.flatHalfDim = PLATE_DIM_FLAT;

				plate2.yDim = PLATE_DIM_Y;
				plate2.flatHalfDim = PLATE_DIM_FLAT;
			}
			debuff.kind = NONE;
		}
		return;
	}

	debuff.debuffTimer++;

	if (debuff.debuffTimer == 255) {
		debuff.kind = SMALL_PLATES;
//		int r11 = (int) rand() / 33;
//		int r22 = (int) rand() / 33;
//		int r1 = r11 % 160 + 20;
//		int r2 = r22 % 100 + 25;
		debuff.pos.x = (score * 1347) % 120 + 40;
		debuff.pos.y = (score * 1233) % 60 + 45;


	}
	if (debuff.kind != NONE) {
		UG_FillCircle(debuff.pos.x, debuff.pos.y, debuff.r, C_RED);
		debuff.debuffDespawnTimer++;
		if (debuff.debuffDespawnTimer == 128) {
			debuff.debuffTimer = 0;
			debuff.debuffDespawnTimer = 0;
		}
		// check collision
		if ((ball.pos.x - debuff.pos.x) * (ball.pos.x - debuff.pos.x) + (ball.pos.y - debuff.pos.y) * (ball.pos.y - debuff.pos.y) < (ball.r + debuff.r)*(ball.r + debuff.r)) {
			debuff.active = 1;
			if (debuff.kind == SMALL_PLATES) {
				plate.yDim = PLATE_DIM_Y - 8;
				plate.flatHalfDim = PLATE_DIM_FLAT - 3;

				plate2.yDim =  PLATE_DIM_Y - 8;
				plate2.flatHalfDim = PLATE_DIM_FLAT - 3;
			}
		}
	}


}

void _loseScreen()
{
	UG_FontSelect(&FONT_12X16);
	char str[40];
	sprintf(str, "Joystick Lost!\n  Score:%d", score);
	UG_PutString(15,50,str);
}

void _loseScreen2()
{
	UG_FontSelect(&FONT_12X16);
	char str[40];
	sprintf(str, "Buttons Lost!\n  Score:%d", score);
	UG_PutString(15,50,str);
}
