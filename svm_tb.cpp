#include <stdio.h>
#include "svm.h"

#define IMAGE_COUNT 2601

template <typename T>
void readFile(const char file[], T target[]) {
	FILE *fp = fopen(file, "r");

	int count = 0;
	double temp;

	while (fscanf(fp, "%lf", &temp) == 1) {
		target[count] = T(temp);
		count++;
	}
	fclose(fp);
}

int main() {
	// Read data from files
	t_alphas d_alphas[NSV];
	readFile<t_alphas>("data_files/alphas.dat", d_alphas);

	t_bias d_bias;
	readFile<t_bias>("data_files/bias.dat", &d_bias);

	int ground_truth[IMAGE_COUNT];
	readFile<int>("data_files/ground_truth.dat", ground_truth);

	// Define svs as a pointer
	t_svs *p_svs = (t_svs*)malloc(NSV*IMAGE_SIZE*sizeof(t_svs));
	readFile<t_svs>("data_files/svs.dat", p_svs);

	// Define test data as a pointer
	t_data *p_test_data = (t_data*)malloc(IMAGE_SIZE*IMAGE_COUNT*sizeof(t_data));
	readFile<t_data>("data_files/test_data.dat", p_test_data);


	bool result[IMAGE_COUNT];
	for (int i= 0; i< IMAGE_COUNT; i++) {
		t_data* imagePointer = p_test_data + (IMAGE_SIZE * i);
		svm(
			(ap_int<64>*)imagePointer,
			(ap_int<64>*)(imagePointer + IMAGE_SIZE/2),
			result + i
		);
	}

	// Check results
	// summary statistics
	int correct = 0;
	for (int i=0; i<IMAGE_COUNT; i++){
		if (result[i] == ground_truth[i])
			correct++;
	}
	double accuracy = correct/double(IMAGE_COUNT);
	printf("Classification Accuracy: %f\n", accuracy);

	// summary statistics - confusion matrix
	double CM[2][2];
	for (int i=0; i<2; i++)
		for (int j=0; j<2; j++)
			CM[i][j]=0.0;

	for (int i=0; i<IMAGE_COUNT; i++){
			CM[ground_truth[i]][result[i]]++;
	}
	printf("Confusion Matrix (%d test points):\n", IMAGE_COUNT);
	printf("%f, %f\n", CM[0][0]/IMAGE_COUNT, CM[0][1]/IMAGE_COUNT);
	printf("%f, %f\n", CM[1][0]/IMAGE_COUNT, CM[1][1]/IMAGE_COUNT);
	return 0;
}
