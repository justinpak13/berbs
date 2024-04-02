#include "raylib.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "berbs.h"
#include <math.h>


const int NUMBER_OF_BERBS = 5;

typedef struct vector {
	size_t length;
	double array[];
} vector;

typedef	enum type {
	weights,
	biases,
	empty
} type;

double randn (double mu, double sigma){
  double U1, U2, W, mult;
  static double X1, X2;
  static int call = 0;
 
  if (call == 1)
    {
      call = !call;
      return (mu + sigma * (double) X2);
    }
 
  do
    {
      U1 = -1 + ((double) rand () / RAND_MAX) * 2;
      U2 = -1 + ((double) rand () / RAND_MAX) * 2;
      W = pow (U1, 2) + pow (U2, 2);
    }
  while (W >= 1 || W == 0);
 
  mult = sqrt ((-2 * log (W)) / W);
  X1 = U1 * mult;
  X2 = U2 * mult;
 
  call = !call;
 
  return (mu + sigma * (double) X1);
}

vector *make_vector(size_t n, type type_of_array){
	vector *v = malloc(sizeof(*v) + n * sizeof(v-> array[0]));
	v -> length = n;

	// if creating weights matrix 
	if (type_of_array == weights){
		for (size_t i = 0; i < n; i++){
			v -> array[i] = randn(0, 1);
		}
	
	// if creating biases array
	} else if (type_of_array == biases){
		for (size_t i = 0; i < n; i++){
			v -> array[i] = 0;
		}
	} 

	return v;
}

vector *copy_vector(vector *original_vector){
	vector *v = malloc(sizeof(*v) + original_vector -> length * sizeof(v -> array[0]));

	v -> length = original_vector -> length;

	for (int i = 0; i < original_vector->length; i++){
		v -> array[i] = original_vector -> array[i];
	}

	return v;
}

vector *forward(vector *input, vector *weights, vector *biases){
	vector *output = make_vector(biases -> length, empty);

	int matrix_width = input -> length;
	int matrix_height = weights -> length / input -> length;

	for (int i = 0; i < matrix_height; i++){
		double output_value = biases -> array[i];
		for (int j = 0; j < matrix_width; j++){
			output_value += input -> array[j] * (weights -> array[i * matrix_width + j]);  
		}
		output -> array[i] = output_value;
	}

	free(input);
	free(weights);
	free(biases);

	return output;

}

vector *activation_relu(vector *output){
	vector *result = make_vector(output -> length, empty);

	for (int i = 0; i < output -> length; i++){
		if (output -> array[i] < 0){
			result -> array[i] = 0;
		} else {
			result -> array[i] = output -> array[i];
		}
	}

	free(output);
	return result;
}

vector *softmax_activation_function(vector *output){
	vector *result = make_vector(output -> length, empty);
	double sum = 0;

	for (int i = 0; i < output -> length; i++){
		sum += exp(output -> array[i]);
	}


	for (int i = 0; i < output -> length; i++){
		result -> array[i] = exp(output -> array[i]) / sum;
	}

	free(output);
	return result;
}

int get_move(vector *output){
	double max_value = 0;
	int max_index = 0;;

	for (int i = 0; i < output -> length; i++){
		printf("%f ", output -> array[i]);
		if (output -> array[i] > max_value){
			max_value = output -> array[i];
			max_index = i;
		}
	}
	return max_index;
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

	vector *input_layer = make_vector((NUMBER_OF_BERBS * 2 + 2) * 10, weights);
	vector *input_layer_biases = make_vector(10, biases);

	vector *hidden_layer = make_vector(10 * 10, weights);
	vector *hidden_layer_biases = make_vector(10, biases);


	vector *output_layer = make_vector(10 * 4, weights);
	vector *output_layer_biases = make_vector(4, biases);

	while (!WindowShouldClose()){
		BeginDrawing();


		ClearBackground(RAYWHITE);
		DrawText("berbs", GetScreenWidth() / 2,  GetScreenHeight()/ 2, 20, BLACK);


		DrawCircle(player_x, player_y, 10, PURPLE);

		drawBerb(berb_list, number_of_berbs);


		updateBerbPosition(berb_list, number_of_berbs, player_x, player_y);

		vector *input_layer_copy = copy_vector(input_layer);
		vector *input_layer_biases_copy = copy_vector(input_layer_biases);

		vector *hidden_layer_copy = copy_vector(hidden_layer);
		vector *hidden_layer_biases_copy = copy_vector(hidden_layer_biases);

		vector *output_layer_copy = copy_vector(output_layer);
		vector *output_layer_biases_copy = copy_vector(output_layer_biases);


		// get input list
		vector *input = make_vector(NUMBER_OF_BERBS * 2 + 2, empty);
		int i = 0;
		for (int j = 0; j < NUMBER_OF_BERBS; j++){
			input -> array[i] = berb_list[j] -> x_pos / (float)GetScreenWidth();
			input -> array[i + 1] = berb_list[j] -> y_pos / (float)GetScreenHeight();
			i += 2;
		}
		input -> array[i] = player_x / (float)GetScreenWidth();
		input -> array[i + 1] = player_y / (float)GetScreenHeight();

		vector *input_layer_results = forward(input, input_layer_copy, input_layer_biases_copy);
		vector *input_layer_relu = activation_relu(input_layer_results); 

		vector *hidden_layer_results = forward(input_layer_relu, hidden_layer_copy, hidden_layer_biases_copy);

		vector *output_layer_results = forward(hidden_layer_results, output_layer_copy, output_layer_biases_copy);

		vector *output = softmax_activation_function(output_layer_results);

		int move = get_move(output);
		printf("%d\n",move);

		if (move == 0){
			player_y -= 3;
		}
		if (move == 1){
			player_y += 3;
		}
		if (move == 2){
			player_x -= 3;
		}
		if (move == 3){
			player_x += 3;
		}

		total_distance +=  calculateTotalDistance(berb_list, NUMBER_OF_BERBS);

		EndDrawing();
		count++;
	}

	printf("Average distance: %f\n", total_distance / count);
	freeBerbs(berb_list, NUMBER_OF_BERBS);
	free(input_layer);
	free(input_layer_biases);
	free(hidden_layer);
	free(hidden_layer_biases);
	free(output_layer);
	free(output_layer_biases);
}
