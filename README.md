# GradientFeatImage
This repository contains a program for creating sample images which can be useful to illustrate the process of Gradient Feature Extraction.

## Requirements
For compiling and running this program, Magick++ and GSL are required.

## Compilation
To compile the program, just type  
`make`.

## Running
Usage of the program is  
`./GradientImage [image file] [widthxheight]`

for example,  
`./GradientImage sample.png 600x680`

The program outputs 5 sample images

* 01org_[sample.png]: Original image
* 02norm_[sample.png]: resized image
* 03smth_[sample.png]: smoothed image
* 04cano_[sample.png]: image after canonicalzation
* 05grad_[sample.png]: gradient image
