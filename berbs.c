#include <stdlib.h>
#include <math.h>
#include "raylib.h"
#include "berbs.h"
#include "settings.h"


Berb *makeBerb(float screenWidth, float screenHeight){

	Berb *berb = malloc(sizeof(Berb));

	berb -> x_pos =  rand() % (int) screenWidth;
	berb -> y_pos =  rand() % (int) screenHeight;

	//berb -> x_vel = rand() % (int) MAX_SPEED * ((rand() % 2) * -1) + 1;
	//berb -> y_vel = rand() % (int) MAX_SPEED * ((rand() % 2) * -1) + 1;

	berb -> x_vel = 0;
	berb -> y_vel = 0;

	berb -> radius = 8;

	return berb;

}

float getDistance(Berb *berb1, Berb *berb2){
	float x = pow(berb1 -> x_pos - berb2 -> x_pos, 2);
	float y = pow(berb1 -> y_pos - berb2 -> y_pos, 2);

	return sqrt(x + y);
}

void avoidWalls(Berb *berb, float screenWidth, float screenHeight){
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

void avoidPlayer(Berb *berb, int player_x, int player_y){
	float x = pow(berb -> x_pos -  player_x, 2);
	float y = pow(berb -> y_pos - player_y, 2);

	float distance = sqrt(x + y);


	if (distance < AVOID_PLAYER_RANGE){
		if (player_y - berb -> y_pos < 0) { //predator above boid
			berb->y_vel = (berb -> y_vel + PLAYER_AVOID_FACTOR) ;
		}
		if (player_y - berb -> y_pos > 0) { //predator below boid
			berb->y_vel = (berb -> y_vel - PLAYER_AVOID_FACTOR) ;
		}
		if (player_x - berb -> x_pos < 0) { //predator left of boid
			berb->x_vel = (berb -> x_vel + PLAYER_AVOID_FACTOR) ;
		}
		if (player_x - berb -> x_pos > 0) { //predator right of boid
			berb->x_vel = (berb -> y_vel - PLAYER_AVOID_FACTOR) ;
		}

	}
}



void updateBerbPosition(Berb *berb_list[], int number_of_berbs, int player_x, int player_y){
	int close_dx;
	int close_dy;
	float xvel_avg;
	float yvel_avg;
	float xpos_avg;
	float ypos_avg;
	int neighboring_berbs;

	for (int i = 0; i < number_of_berbs; i++){
		// avoidance of player berb
		
		avoidPlayer(berb_list[i], player_x, player_y);


		// separation from other berbs
		close_dx = 0;
		close_dy = 0;


		//allignment
		xvel_avg = 0;
		yvel_avg = 0;
		neighboring_berbs = 0;

		//cohesion
		xpos_avg = 0;
		ypos_avg = 0;
		neighboring_berbs = 0;

		for (int j = 0; j < number_of_berbs; j++){
			if (j != i && getDistance(berb_list[i], berb_list[j]) < PROTECTED_RANGE){
				close_dx += berb_list[i] -> x_pos - berb_list[j] -> x_pos;
				close_dy += berb_list[i] -> y_pos - berb_list[j] -> y_pos;
			}
			if (j != i && getDistance(berb_list[i], berb_list[j]) < VISIBLE_RANGE){
				xvel_avg += berb_list[j] -> x_vel;
				yvel_avg += berb_list[j] -> y_vel;
				xpos_avg += berb_list[j] -> x_pos;
				ypos_avg += berb_list[j] -> y_pos;
				neighboring_berbs++;
			}
		}
		berb_list[i] -> x_vel += close_dx * AVOID_FACTOR;
		berb_list[i] -> y_vel += close_dy * AVOID_FACTOR;


		if (neighboring_berbs > 0){
			xvel_avg = xvel_avg / neighboring_berbs;
			yvel_avg = yvel_avg / neighboring_berbs;
			berb_list[i] -> x_vel += (xvel_avg - berb_list[i] -> x_vel)*MATCHING_FACTOR;
			berb_list[i] -> y_vel += (yvel_avg - berb_list[i] -> y_vel)*MATCHING_FACTOR;

			xpos_avg = xpos_avg / neighboring_berbs;
			ypos_avg = ypos_avg / neighboring_berbs;
			berb_list[i] -> x_vel += (xpos_avg - berb_list[i] -> x_pos)*CENTERING_FACTOR;
			berb_list[i] -> y_vel += (ypos_avg - berb_list[i] -> y_pos)*CENTERING_FACTOR;
		}
		avoidWalls(berb_list[i], GetScreenWidth(), GetScreenHeight());
	}
}

void drawBerb(Berb *berb_list[], int number_of_berbs){
	for (int i = 0; i < number_of_berbs; i++){
		// just to have one specific one to look at
		if (i == 0){
		DrawCircle(berb_list[i] -> x_pos, berb_list[i] -> y_pos, berb_list[i] ->radius, RED);
		} else {
		DrawCircle(berb_list[i] -> x_pos, berb_list[i] -> y_pos, berb_list[i] ->radius, BLUE);
		}
	}

}

float calculateTotalDistance(Berb *berb_list[], int number_of_berbs){
	float result_value;

	result_value = 0;
	for (int i = 0; i < number_of_berbs; i++){
		for (int j = i + 1; j < number_of_berbs; j++){
			result_value += getDistance(berb_list[i], berb_list[j]);
		}
	}

	return result_value;
}

void freeBerbs(Berb *berb_list[], int NUMBER_OF_BERBS){
	for (int i = 0; i < NUMBER_OF_BERBS; i++){
		free(berb_list[i]);
	}
}
