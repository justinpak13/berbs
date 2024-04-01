#include "raylib.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "berbs.h"


const int NUMBER_OF_BERBS = 200;

typedef struct Neuron {
	int number_of_inputs;
	float bias;
	float weights[];
} Neuron;

typedef struct Layer{
	int number_of_neurons;
	Neuron *neurons[];
} Layer;

Neuron *createNeuron(int number_of_inputs){
	Neuron *neuron = malloc(sizeof(Neuron) + number_of_inputs * sizeof(float));
	neuron -> number_of_inputs = number_of_inputs;
	neuron -> bias = 0;
	for (int i = 0; i < number_of_inputs; i++){
		neuron -> weights[i] = ((double)rand() / (double)RAND_MAX) * 2.0 - 1;
	}
	return neuron;
}

Layer *createLayer(int number_of_inputs, int number_of_neurons){
	Layer *layer = malloc(sizeof(Layer) + number_of_neurons * (sizeof(Neuron) + sizeof(float) * number_of_inputs));

	layer -> number_of_neurons = number_of_neurons;

	for (int i = 0; i < number_of_neurons; i++){
		layer -> neurons[i] = createNeuron(number_of_inputs);
	}
	return layer;
}

float forwardPass(float inputs[], Neuron *neuron){
	float output = 0;
	for (int i = 0; i < neuron -> number_of_inputs; i++){
		output += inputs[i] * neuron -> weights[i] ;
	}
	
	output += neuron ->bias;

	return output;
}

float ReLU(float output){
	if (output < 0){
		return 0;
	}

	return output;
}

float *getOutputFromLayer(float inputs[], Layer *layer){
	float *result = malloc(sizeof(float) * layer -> number_of_neurons); 

	for (int i = 0; i < layer -> number_of_neurons; i++){
		result[i] = forwardPass(inputs,layer -> neurons[i]);
	}
	return result;
}

void freeLayer(Layer *layer){
	for (int i = 0; i < layer -> number_of_neurons; i++){
		free(layer -> neurons[i]);
	}

	free(layer);
}




void printNeuron(Neuron *neuron){
	printf("Number of inputs: %d\n", neuron -> number_of_inputs);
	printf("Bias Value: %f\n", neuron -> bias);
	printf("values of the weights: \n");
	for (int i = 0; i < neuron -> number_of_inputs; i++){
		printf("%f\n", neuron-> weights[i]);
	}
}


int getMax(float neuronUp, float neuronDown ,float neuronLeft ,float neuronRight){
	int max = 0;
	int max_result = 0;
	printf("Neuron Up: %f", neuronUp);
	printf("Neuron Down: %f", neuronDown);
	printf("Neuron Left: %f", neuronLeft);
	printf("Neuron Right: %f", neuronRight);
	if (neuronUp > max){
		max = neuronUp;
		max_result = 0;
	}
	if (neuronDown > max){
		max = neuronDown;
		max_result = 1;
	}
	if (neuronLeft > max){
		max = neuronLeft;
		max_result = 2;
	}
	if (neuronRight > max){
		max = neuronRight;
		max_result = 3;
	}

	return max_result;
}


int main(void){
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(GetScreenWidth(), GetScreenHeight(), "berbs");

	SetTargetFPS(60);
	HideCursor();

	int number_of_berbs = NUMBER_OF_BERBS;


	Berb *berb_list[number_of_berbs];

	
	srand(time(NULL));
	for (int i = 0; i < number_of_berbs; i++){
		berb_list[i] = makeBerb(GetScreenWidth(), GetScreenHeight());
	}




	float close_dx, close_dy;
	int player_x = GetScreenWidth() / 2, player_y = GetScreenHeight() / 2;
	int goal_x, goal_y;

	int count = 0;
	float total_distance = 0;

	Neuron *neuronUp = createNeuron(NUMBER_OF_BERBS * 2 + 2);
	Neuron *neuronDown = createNeuron(NUMBER_OF_BERBS * 2 + 2);
	Neuron *neuronLeft = createNeuron(NUMBER_OF_BERBS * 2 + 2);
	Neuron *neuronRight = createNeuron(NUMBER_OF_BERBS * 2 + 2);



	Layer *hiddenLayer = createLayer(NUMBER_OF_BERBS * 2 + 2, 5);
	Layer *outputLayer = createLayer(5, 4);




	while (!WindowShouldClose()){
		BeginDrawing();


		ClearBackground(RAYWHITE);
		DrawText("berbs", GetScreenWidth() / 2,  GetScreenHeight()/ 2, 20, BLACK);


		DrawCircle(player_x, player_y, 10, PURPLE);

		drawBerb(berb_list, number_of_berbs);


		updateBerbPosition(berb_list, number_of_berbs, player_x, player_y);

		// get input list
		float input_list[NUMBER_OF_BERBS * 2 + 2];
		int i = 0;
		for (int j = 0; j < NUMBER_OF_BERBS; j++){
			input_list[i] = berb_list[j] -> x_pos / (float)GetScreenWidth();
			input_list[i + 1] = berb_list[j] -> y_pos / (float)GetScreenHeight();
			i += 2;
		}
		input_list[i] = player_x / (float)GetScreenWidth();
		input_list[i + 1] = player_y / (float)GetScreenHeight();

		float *output_results = getOutputFromLayer(input_list, hiddenLayer);
		float *directions = getOutputFromLayer(output_results, outputLayer);

		int result = getMax(ReLU(directions[0]), ReLU(directions[1]), ReLU(directions[2]), ReLU(directions[3]));
		free(output_results);
		free(directions);

		printf("%d\n", result);

		if (result == 0){
			player_y -= 3;
		}
		if (result == 1){
			player_y += 3;
		}
		if (result == 2){
			player_x -= 3;
		}
		if (result == 3){
			player_x += 3;
		}




		//for (int i = 0; i < NUMBER_OF_BERBS; i++){
		//	printf("x: %f y: %f\n", berb_list[i] -> x_pos, berb_list[i] -> y_pos);
		//}

		total_distance +=  calculateTotalDistance(berb_list, NUMBER_OF_BERBS);

		EndDrawing();
		count++;
	}

	printf("Average distance: %f\n", total_distance / count);
	freeBerbs(berb_list, NUMBER_OF_BERBS);

}
