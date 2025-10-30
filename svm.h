#ifndef SVM_H
#define SVM_H

#include <math.h>
#include <ap_int.h>

#define NSV 165
#define IMAGE_SIZE 784

#include "ap_fixed.h"
typedef ap_fixed<8,5> t_alphas;
typedef ap_fixed<8,7> t_svs;
typedef ap_fixed<8,1> t_bias;
typedef ap_fixed<8,7> t_data;
typedef ap_fixed<15,5> t_kernal;

void svm(ap_int<64> image1[IMAGE_SIZE/16], ap_int<64> image2[IMAGE_SIZE/16], bool *result);

#define ITERATIONS 8
#define INIT_X 1.2051240991530001
static t_kernal atanh_arr[8] = {
	atanh(exp2(-1)),
	atanh(exp2(-2)),
	atanh(exp2(-3)),
	atanh(exp2(-4)),
	atanh(exp2(-5)),
	atanh(exp2(-6)),
	atanh(exp2(-7)),
	atanh(exp2(-8))
};
static t_kernal ln2 = log(2);
#define MAX_POW_LN2 3

#endif
