#pragma once
#include <iostream>
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include<opencv2/opencv.hpp>
#include <highgui.h>
#include <cv.h>
using namespace std;
using namespace cv;
#ifndef his_H
#define his_H

void his_gray(Mat img, int Histogram[256]);
double which_is_max(int *tmp);
Mat on_MeanBlur(Mat g_srcImage, int g_nMeanBlurValue);
void Histogram_Output(int Histogram[256]);
void Ima_Element_Output(Mat g_srcImage);
void Ima_Element_Double_Output(Mat g_srcImage);
void sum_of_Histogram(int Histogram[256],int size);
void Photograph_Histogram(int Histogram[256]);
#endif