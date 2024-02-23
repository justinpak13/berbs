#include "raylib.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

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

	berb -> x_vel = rand() % 5 * ((rand() % 2) * -1) + 1;
	berb -> y_vel = rand() % 5 * ((rand() % 2) * -1) + 1;

	berb -> radius = 5;

	return berb;

}

void updatePos(Berb *berb, float screenWidth, float screenHeight){
	if (berb -> x_pos + berb -> x_vel >= screenWidth){
		berb -> x_vel *= -1;
	} 


	if (berb -> y_pos + berb -> y_vel >= screenHeight){
		berb -> y_vel *= -1;
	} 

	if (berb -> x_pos + berb -> x_vel <= berb -> radius / 2.0){
		berb -> x_vel *= -1;
	} 


	if (berb -> y_pos + berb -> y_vel < berb -> radius / 2.0){
		berb -> y_vel *= -1;
	} 


	berb -> x_pos += berb -> x_vel;
	berb -> y_pos += berb -> y_vel;
}

int main(void){
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(GetScreenWidth(), GetScreenHeight(), "berbs");

	SetTargetFPS(144);

	int number_of_berbs = 100;


	Berb *berb_list[number_of_berbs];
	
	srand(time(NULL));
	for (int i = 0; i < number_of_berbs; i++){
		berb_list[i] = makeBerb(GetScreenWidth(), GetScreenHeight());
	}


	while (!WindowShouldClose()){
		BeginDrawing();

		ClearBackground(RAYWHITE);
		DrawText("berbs", GetScreenWidth() / 2,  GetScreenHeight()/ 2, 20, BLACK);


		for (int i = 0; i < number_of_berbs; i++){
			DrawCircle(berb_list[i] -> x_pos, berb_list[i] -> y_pos, berb_list[i] ->radius, BLUE);
		}

		for (int i = 0; i < number_of_berbs; i++){
			updatePos(berb_list[i], GetScreenWidth(), GetScreenHeight());
		}
		


		EndDrawing();
	}

}
