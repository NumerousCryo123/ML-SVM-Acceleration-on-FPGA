# ADSD_CW2
Repo for Advanced Digital Systems Design coursework part 2

## Vivado project management
To rebuild the vivado project; in a tcl console, run 'source vivado_project.tcl'
To export vivado project; in a tcl console, run 'source export_tcl.tcl'

# To do
## Initial Research
- Data sheets - Complete
- Python scripts / Algorithm understanding - Complete
- FPGA software implementation - Complete
- Performance of software only - Complete
## Hardware Implementation
- Initial design / will work straight away and no need for iteration
- Performance testing / will be better than anyone elses
## Report
- Write-up
- Proof read
- Submit
# Description of...
## ...Data
- All data is stored in .h files
- Image data is stored in test_data.h
	- All data for all images is stored as one dimension array
	- There are 2,039,184 elements in total
	- Each image is 784 pixels, 28 x 28
	- There are 2601 images in total
	- 2,039,184 = 784 x 2601
	- Values have granularty of 0.5, 1 fractional bit
- SV alphas for SVM are stored in alphas.h
	- There are 165 values in total
	- Values have granularty of 0.125, 3 fractional bits
- Support vectors matrices are stored in svs.h
	- There are 129,360 values in total
	- 129,360 = 784 x 165
	- Values have granularty of 0.5, 1 fractional bit
- Bias stored in bias.h
	- Array of single value
	- Values have granularty of 0.0078125, 7 fractional bits
- Ground truth stored in groung_turth.h
	- Array of 2601 values for each image
	- Boolean values 0,1
## .. Algorithm
For each image:
- For each support vector:
	- Calculate kernal:
		- Sum the square of the differences between the SV element and the image pixels
		- Multiply be gamma (-0.001)
		- Exp()
		- Multiply by SV alpha
	- Add kernal to total sum
- Add bias
If sum and bias are negative, then output is 0 else output is 1
