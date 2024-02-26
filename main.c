#include "raylib.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>

const int NUMBER_OF_BERBS = 75;
const double TURN_FACTOR = 0.2;
const int PROTECTED_RANGE = 10;
const int VISIBLE_RANGE = 48;
const float AVOID_FACTOR = 0.09;
const float MAX_SPEED = 10.0;
const float MIN_SPEED = 4.0;
const float SCREEN_MARGIN = 100;
const float MATCHING_FACTOR = 0.05;
const float CENTERING_FACTOR = 0.0005;

typedef struct Berb {
	float x_pos;
	float y_pos;
	float x_vel;
	float y_vel;
	float accel;
	int radius;
} Berb;

Berb *makeBerb(float screenWidth, float screenHeight){

	Berb *berb = malloc(sizeof(Berb));

	berb -> x_pos =  rand() % (int) screenWidth;
	berb -> y_pos =  rand() % (int) screenHeight;

	berb -> x_vel = rand() % (int) MAX_SPEED * ((rand() % 2) * -1) + 1;
	berb -> y_vel = rand() % (int) MAX_SPEED * ((rand() % 2) * -1) + 1;

	berb -> radius = 8;

	return berb;

}

float getDistance(Berb *berb1, Berb *berb2){
	float x = pow(berb1 -> x_pos - berb2 -> x_pos, 2);
	float y = pow(berb1 -> y_pos - berb2 -> y_pos, 2);

	return sqrt(x + y);
}

void updatePos(Berb *berb, float screenWidth, float screenHeight){
	if (berb -> x_pos + berb -> x_vel > screenWidth - SCREEN_MARGIN){
		berb -> x_vel -= TURN_FACTOR;
	} 


	if (berb -> y_pos + berb -> y_vel > screenHeight - SCREEN_MARGIN){
		berb -> y_vel -= TURN_FACTOR;
	} 

	if (berb -> x_pos + berb -> x_vel < berb -> radius / 2.0 + SCREEN_MARGIN){
		berb -> x_vel += TURN_FACTOR;
	} 


	if (berb -> y_pos + berb -> y_vel < berb -> radius / 2.0 + SCREEN_MARGIN){
		berb -> y_vel += TURN_FACTOR;
	} 


	berb -> x_pos += berb -> x_vel;
	berb -> y_pos += berb -> y_vel;

	if (fabs(berb -> x_vel) > MAX_SPEED){
		if (berb -> x_vel > 0){
			berb -> x_vel = MAX_SPEED;
		} else {
			berb -> x_vel = -MAX_SPEED;
		}
	}

	if (fabs(berb -> y_vel) > MAX_SPEED){
		if (berb -> y_vel > 0){
			berb -> y_vel = MAX_SPEED;
		} else {
			berb -> y_vel = -MAX_SPEED;
		}
	}


}

int main(void){
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(GetScreenWidth(), GetScreenHeight(), "berbs");

	SetTargetFPS(60);

	int number_of_berbs = NUMBER_OF_BERBS;


	Berb *berb_list[number_of_berbs];

	
	srand(time(NULL));
	for (int i = 0; i < number_of_berbs; i++){
		berb_list[i] = makeBerb(GetScreenWidth(), GetScreenHeight());
	}


	float close_dx, close_dy;
	while (!WindowShouldClose()){
		BeginDrawing();

		ClearBackground(RAYWHITE);
		DrawText("berbs", GetScreenWidth() / 2,  GetScreenHeight()/ 2, 20, BLACK);

		for (int i = 0; i < number_of_berbs; i++){
			// just to have one specific one to look at
			if (i == 0){
			DrawCircle(berb_list[i] -> x_pos, berb_list[i] -> y_pos, berb_list[i] ->radius, RED);
			} else {
			DrawCircle(berb_list[i] -> x_pos, berb_list[i] -> y_pos, berb_list[i] ->radius, BLUE);
			}
		}



		// separation
		for (int i = 0; i < number_of_berbs; i++){
			updatePos(berb_list[i], GetScreenWidth(), GetScreenHeight());
			close_dx = 0;
			close_dy = 0;

			for (int j = 0; j < number_of_berbs; j++){
				if (j != i && 
				getDistance(berb_list[i], berb_list[j]) < PROTECTED_RANGE){
					close_dx += berb_list[i] -> x_pos - berb_list[j] -> x_pos;
					close_dy += berb_list[i] -> y_pos - berb_list[j] -> y_pos;
				}
			}
			berb_list[i] -> x_vel += close_dx * AVOID_FACTOR;
			berb_list[i] -> y_vel += close_dy * AVOID_FACTOR;
		}

		// alignment
		for (int i = 0; i < number_of_berbs; i++){
			updatePos(berb_list[i], GetScreenWidth(), GetScreenHeight());
			float xvel_avg = 0;
			float yvel_avg = 0;
			int neighboring_berbs = 0;

			for (int j = 0; j < number_of_berbs; j++){
				if (j != i && 
				getDistance(berb_list[i], berb_list[j]) < VISIBLE_RANGE){
					xvel_avg += berb_list[j] -> x_vel;
					yvel_avg += berb_list[j] -> y_vel;
					neighboring_berbs++;
				}
			}
			if (neighboring_berbs > 0){
				xvel_avg = xvel_avg / neighboring_berbs;
				yvel_avg = yvel_avg / neighboring_berbs;
				berb_list[i] -> x_vel += (xvel_avg - berb_list[i] -> x_vel)*MATCHING_FACTOR;
				berb_list[i] -> y_vel += (yvel_avg - berb_list[i] -> y_vel)*MATCHING_FACTOR;
			}
		}

		// cohesion
		for (int i = 0; i < number_of_berbs; i++){
			updatePos(berb_list[i], GetScreenWidth(), GetScreenHeight());
			float xpos_avg = 0;
			float ypos_avg = 0;
			int neighboring_berbs = 0;

			for (int j = 0; j < number_of_berbs; j++){
				if (j != i && 
				getDistance(berb_list[i], berb_list[j]) < VISIBLE_RANGE){
					xpos_avg += berb_list[j] -> x_pos;
					ypos_avg += berb_list[j] -> y_pos;
					neighboring_berbs++;
				}
			}
			if (neighboring_berbs > 0){
				xpos_avg = xpos_avg / neighboring_berbs;
				ypos_avg = ypos_avg / neighboring_berbs;
				berb_list[i] -> x_vel += (xpos_avg - berb_list[i] -> x_pos)*CENTERING_FACTOR;
				berb_list[i] -> y_vel += (ypos_avg - berb_list[i] -> y_pos)*CENTERING_FACTOR;
			}
		}

		


		EndDrawing();
	}

}
