#include <iostream>
#include "stdio.h" 
#include <stdlib.h> 
#include <string> 
#include<opencv2/opencv.hpp>
#include <highgui.h>
#include <cv.h>
#include "Dark_Channel_Prior.h"
#include "his.h"
#include <chrono>

namespace cho = std::chrono;
using namespace std;
using namespace cv;
Mat channel[3];
void report_time_information(
								const std::chrono::steady_clock::time_point& timer_start,
								const std::chrono::steady_clock::time_point& timer_one_in_a_thousand_finish,
								const std::chrono::steady_clock::time_point& timer_darkchannel_img_finish,
								const std::chrono::steady_clock::time_point& timer_cal_a_finish,
								const std::chrono::steady_clock::time_point& timer_cal_t_finish,
								const std::chrono::steady_clock::time_point& timer_restore_img_finish
							)
{
	std::cout << "time information:" << std::endl;
	std::cout << "\t得到暗通道图像所花的时间:" << cho::duration_cast<cho::milliseconds>(timer_darkchannel_img_finish - timer_start).count() << "ms\n";
	std::cout << "\t得到前0.1%像素点的值及其坐标所花的时间:" << cho::duration_cast<cho::milliseconds>(timer_one_in_a_thousand_finish - timer_darkchannel_img_finish).count() << "ms\n";
	std::cout << "\t计算A所花的时间:" << cho::duration_cast<cho::milliseconds>(timer_cal_a_finish - timer_one_in_a_thousand_finish).count() << "ms\n";
	std::cout << "\t计算t所花的时间:" << cho::duration_cast<cho::milliseconds>(timer_cal_t_finish - timer_cal_a_finish).count() << "ms\n";
	std::cout << "\t得到所有参数后重构所花的时间:" << cho::duration_cast<cho::milliseconds>(timer_restore_img_finish - timer_cal_t_finish).count() << "ms\n";
	std::cout << "\t整个程序所花的时间:" << cho::duration_cast<cho::milliseconds>(timer_restore_img_finish - timer_start).count() << "ms\n";

}
int main()
{
	cho::steady_clock::time_point timer_start = cho::steady_clock::now(); //计时开始

	Mat haze_img = imread("haze4.png");
	imshow("原始的待去雾的图像", haze_img);
	int Gray_Histogram[256] = { 0 };
	Mat darkchannel_img =Dark_Channel_Img(haze_img,25, Min_Value_Filter);

	cho::steady_clock::time_point timer_darkchannel_img_finish = cho::steady_clock::now(); 

	his_gray(darkchannel_img, Gray_Histogram);
	Photograph_Histogram(Gray_Histogram);
	Histogram_Output(Gray_Histogram);
	coordinate_info location=serach_one_in_a_thousand(darkchannel_img, Gray_Histogram);
	cho::steady_clock::time_point timer_one_in_a_thousand_finish = cho::steady_clock::now();

	cal_A(haze_img, location);	cho::steady_clock::time_point timer_cal_a_finish = cho::steady_clock::now();

	Mat t_value(haze_img.rows, haze_img.cols, CV_64FC1);
	t_value =cal_T(haze_img);  cho::steady_clock::time_point timer_cal_t_finish = cho::steady_clock::now();

	img_restore(haze_img, t_value);  cho::steady_clock::time_point timer_restore_finish = cho::steady_clock::now();

	Mat restored_img= imread("restored_img.jpg");	imshow("restored_img", restored_img);
	
	report_time_information(timer_start, timer_one_in_a_thousand_finish, timer_darkchannel_img_finish, timer_cal_a_finish, timer_cal_t_finish, timer_restore_finish);

	waitKey(0);
	return 1;
}
