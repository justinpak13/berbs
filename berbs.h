#ifndef BERBS_H
#define BERBS_H

typedef struct Berb {
	float x_pos;
	float y_pos;
	float x_vel;
	float y_vel;
	float accel;
	int radius;
} Berb;

Berb *makeBerb(float screenWidth, float screenHeight);
float getDistance(Berb *berb1, Berb *berb2);
void avoidWalls(Berb *berb, float screenWidth, float screenHeight);
void avoidPlayer(Berb *berb, int player_x, int player_y);
void updateBerbPosition(Berb *berb_list[], int number_of_berbs, int player_x, int player_y);
void drawBerb(Berb *berb_list[], int number_of_berbs);
float calculateTotalDistance(Berb *berb_list[], int number_of_berbs);
void freeBerbs(Berb *berb_list[], int NUMBER_OF_BERBS);

#endif
