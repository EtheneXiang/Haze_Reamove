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

#ifndef Dark_Channel_Prior_H
#define Dark_Channel_Prior_H
struct coordinate_info
{
	vector<int> x;         
	vector<int> y;
};
struct value_info
{
	vector<int> r_channel;
	vector<int> g_channel;
	vector<int> b_channel;
};
enum Filter_type { Min_Value_Filter,Max_Value_Filter};

Mat Dark_Channel_Img(Mat srcImage , int ksize, int Filter_Flag);
Mat Extreme_value_Filter(Mat srcImage, int ksize, int Filter_Flag);
coordinate_info serach_one_in_a_thousand(Mat srcImage_dark_channel, int Histogram[256]);
void cal_A(Mat srcImage, coordinate_info location_info);
Mat cal_T(Mat srcImage);
Mat img_restore(Mat srcImage, Mat t_matix);
#endif

