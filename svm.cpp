#include "svm.h"
#include <math.h>
#include <stdint.h>

#include "alphas.h"
#include "svs.h"


static t_bias bias = -0.1796875;
static t_kernal gamma_mulitplier = 0.001;

t_kernal cordic_exp_minus(t_kernal z);

void svm(ap_int<64> image1[IMAGE_SIZE/16], ap_int<64> image2[IMAGE_SIZE/16], bool *result) {
#pragma HLS INTERFACE s_axilite port=return
#pragma HLS RESOURCE variable=svs core=ROM_1P_LUTRAM
#pragma HLS INTERFACE s_axilite register port=result
#pragma HLS INTERFACE m_axi depth=49 port=image2 offset=slave bundle=svm_in2
#pragma HLS INTERFACE m_axi depth=49 port=image1 offset=slave bundle=svm_in1
#pragma HLS ARRAY_PARTITION variable=atanh_arr complete dim=1

#pragma HLS ARRAY_PARTITION variable=svs cyclic factor=28 dim=2
#pragma HLS ARRAY_PARTITION variable=svs cyclic factor=15 dim=1
#pragma HLS ARRAY_PARTITION variable=alphas cyclic factor=15
#define OLF 15 // OLF Outer loop factor
	t_data image_copy[IMAGE_SIZE];
#pragma HLS ARRAY_PARTITION variable=image_copy cyclic factor=28
	buffer:for (int i= 0; i< IMAGE_SIZE/16; i++) {
#pragma HLS PIPELINE
		image_copy[8*i].range(7,0) = image1[i](7,0);
		image_copy[8*i+1].range(7,0) = image1[i](15,8);
		image_copy[8*i+2].range(7,0) = image1[i](23,16);
		image_copy[8*i+3].range(7,0) = image1[i](31,24);
		image_copy[8*i+4].range(7,0) = image1[i](39,32);
		image_copy[8*i+5].range(7,0) = image1[i](47,40);
		image_copy[8*i+6].range(7,0) = image1[i](55,48);
		image_copy[8*i+7].range(7,0) = image1[i](63,56);

		image_copy[8*i + IMAGE_SIZE/2].range(7,0) = image2[i](7,0);
		image_copy[8*i+1 + IMAGE_SIZE/2].range(7,0) = image2[i](15,8);
		image_copy[8*i+2 + IMAGE_SIZE/2].range(7,0) = image2[i](23,16);
		image_copy[8*i+3 + IMAGE_SIZE/2].range(7,0) = image2[i](31,24);
		image_copy[8*i+4 + IMAGE_SIZE/2].range(7,0) = image2[i](39,32);
		image_copy[8*i+5 + IMAGE_SIZE/2].range(7,0) = image2[i](47,40);
		image_copy[8*i+6 + IMAGE_SIZE/2].range(7,0) = image2[i](55,48);
		image_copy[8*i+7 + IMAGE_SIZE/2].range(7,0) = image2[i](63,56);
	}

	// Pragmas for arrays
	ap_fixed<17,10, AP_RND, AP_SAT> sum = 0.0;

	// For each support vector
	Outerloop:for (int i=0; i< NSV; i+=OLF) {
		ap_ufixed<17,15> kernal_val[4][OLF];
#pragma HLS ARRAY_PARTITION variable=kernal_val complete dim=0
		SetKernalLoop:for (int j=0; j<OLF; j++) {
#pragma HLS UNROLL
			kernal_val[0][j] = 0.0;
			kernal_val[1][j] = 0.0;
			kernal_val[2][j] = 0.0;
			kernal_val[3][j] = 0.0;
		}
		// For each image pixel
		Innerloop1:for (int j=0; j< IMAGE_SIZE; j++) {
#pragma HLS PIPELINE II=2
#pragma HLS UNROLL factor=28
			t_data pixel = image_copy[j];
			Innerloop2:for (int k=0; k<OLF; k++) {
#pragma HLS UNROLL
				ap_fixed<8,7> val_root = (t_svs)(svs[i+k][j]) - pixel;
				ap_ufixed<17,15> val = val_root * val_root;
				kernal_val[j%4][k] += val;
			}
		}
		ap_fixed<17,10, AP_RND, AP_SAT> sum_temp[5] = {0.0, 0.0, 0.0, 0.0, 0.0};
		GetSumLoop:for (int j=0; j<OLF; j++) {
#pragma HLS UNROLL factor=5
#pragma HLS PIPELINE II=3
			kernal_val[0][j] += kernal_val [1][j] + kernal_val [2][j] + kernal_val [3][j];
			t_kernal kernal = cordic_exp_minus(gamma_mulitplier * kernal_val[0][j]);
			ap_fixed<17,10, AP_RND, AP_SAT> temp =  kernal * alphas[i+j];
			sum_temp[j%5] += temp;
		}
		sum += sum_temp[0] + sum_temp[1] + sum_temp[2] + sum_temp[3] + sum_temp[4];
	}
	sum += bias;

	*result = (sum[sum.wl()-1] == 0);
}

t_kernal cordic_exp_minus(t_kernal z) {
	// Normalise
	if (z > (ln2 << MAX_POW_LN2)) {
		return 0.0;
	} else {
		int shifts = 0;
		cordic_exp_minus_loop1:for (int i= MAX_POW_LN2-1; i >= 0; i--) {
#pragma HLS UNROLL
			if (z > (ln2 << i)) {
				z -= (ln2 << i);
				shifts += (1 << i);
			}
		}
		t_kernal x = INIT_X;
		t_kernal y = 0.0;
		cordic_exp_minus_loop2:for (int i= 1; i<= ITERATIONS; i++) {
#pragma HLS UNROLL
			t_kernal new_x,new_y,new_z;
			if (z[z.wl()-1] == 0) {
				new_x = x + (y >> i);
				new_y = y + (x >> i);
				new_z = z - atanh_arr[i-1];
			} else {

				new_x = x - (y >> i);
				new_y = y - (x >> i);
				new_z = z + atanh_arr[i-1];
			}
			x = new_x;
			y = new_y;
			z = new_z;
		}
		z = (x-y) >> shifts;
		return z;
	}
}
