#include "his.h"
double max_Histogram = 0, num_max_Histogram_location = 0; //num_max_Histogram_location His中，哪个包含的数量最多 ，max_Histogram用来记录这个数量
double maxvalue_Histogram = 0, value_max_Histogram_location = 0;//用来确定整张图像中，像素最大值是哪个，maxvalue_Histogram用来记录这个最大像素有几个
void his_gray(Mat img, int Histogram[256])
{
	int tmp = 0;
	int height = img.rows;	int width = img.cols;
	int size = height * width;

	//计算灰度直方图分布，Histogram数组下标是灰度值，保存内容是灰度值对应像素点数 (分别记录每个灰度级有多少个像素点) 
	for (int i = 0;i < height;i++)
	{ 
		for (int j = 0;j < width;j++)
		{
			tmp =int( img.at<uchar>(i, j) );
			Histogram[tmp]++;
		}
	}

	sum_of_Histogram(Histogram, size);
	//Photograph_Histogram(Histogram);

}
void Photograph_Histogram(int Histogram[256])
{
	Mat image1(255, 255 * 5, CV_8UC3, Scalar(255, 255, 255));
	double  Histogram_scale_factor = 0;
	int Histogram_scale[256] = { 0 };
	max_Histogram = which_is_max(Histogram);
	Histogram_scale_factor = max_Histogram / 256;
	for (int i = 0; i<256; i++)
	{
		Histogram_scale[i] = (int)(Histogram[i] / Histogram_scale_factor);
		//	sprintf(ch, "%d", i);//字串格式化命令，主要功能是把格式化的数据写入某个字符串中。//	//将i写入到ch中
		if (i == num_max_Histogram_location)
		{
			for (int j = 0; j<5; j++)
			{
				line(image1, Point(i * 5 + j, 255), Point(i * 5 + j, 255 - Histogram_scale[i]), Scalar(0, 0, 255), 1, 8, 0);
			}
		}
		else
		{
			for (int j = 0; j<5; j++)
			{
				line(image1, Point(i * 5 + j, 255), Point(i * 5 + j, 255 - Histogram_scale[i]), Scalar(255, 0, 0), 1, 8, 0);
			}
		}
	}

	char ch[256] = { 0 };
	putText(image1, "Gary Histogram", Point(60, 20), 1, 1, Scalar(0, 0, 255));
	putText(image1, ch, Point(95, 30), 1, 1, Scalar(0, 0, 255));

	sprintf(ch, "%.3lf", Histogram_scale_factor);
	putText(image1, ch, Point(40, 40), 1, 1, Scalar(0, 0, 255));

	sprintf(ch, "%d", (int)max_Histogram);
	putText(image1, ch, Point(140, 40), 1, 1, Scalar(0, 0, 255));

	sprintf(ch, "%d", (int)num_max_Histogram_location);
	putText(image1, ch, Point(95, 50), 1, 1, Scalar(0, 0, 255));

	imshow("Gary Histogram", image1);
	imwrite("Gary_Histogram.jpg", image1);
}


double which_is_max(int *tmp)
{
	double max_h = tmp[0];
	for (int i = 0;i < 255;i++)
	{
		if (max_h < tmp[i]) { max_h = tmp[i]; num_max_Histogram_location = i; }
		//max_h = tmp[i] > max_h ? tmp[i] : max_h;
	}

	return max_h;
}
Mat on_MeanBlur(Mat g_srcImage, int g_nMeanBlurValue)
{
	Mat g_dstImage;
	medianBlur(g_srcImage, g_dstImage, g_nMeanBlurValue);//@brief Blurs an image using a Gaussian filter.
	imshow("中值滤波(3X3)", g_dstImage);
	return g_dstImage;
}

void Histogram_Output(int Histogram[256])
{
	cout << endl << "灰度直方图的详细数据如下：" << endl;
	for (int i = 0;i < 256;i++)
	{
		if ((i % 16 == 0) && (i != 0))
			cout << endl;
		cout << Histogram[i] << "  ";		
	}
	cout << endl;
}

void Ima_Element_Output(Mat g_srcImage)
{
	cout << endl << "图片的详细像素值如下：" << endl;
	for (int i = 0;i < g_srcImage.rows;i++)
	{
		for (int j = 0;j < g_srcImage.cols;j++)
		{
			cout << int( g_srcImage.at<uchar>(i, j) )<< " ";
		}
		cout << endl;		
		break;
	}	
}

void Ima_Element_Double_Output(Mat g_srcImage)
{
	cout << endl << "图片的详细像素值如下：" << endl;
	for (int i = 0;i < g_srcImage.rows;i++)
	{
		for (int j = 0;j < g_srcImage.cols;j++)
		{
			cout << g_srcImage.at<double>(i, j) << "   ";
		}
		cout << endl;
		break;
	}
}

void sum_of_Histogram(int Histogram[256], int size)
{
	int sum_h = 0;
	for (int i = 0;i < 256;i++)
		sum_h = sum_h + Histogram[i];
	if (sum_h == size)
	{
		printf("\r\n灰度直方图统计无误\r\n");
		printf("一共统计了%d个样本像素\r\n", sum_h);
	}
	else
	{
		printf("\r\n灰度直方图统计有误，参与统计的像素数量和图片的像素数量不一样\r\n");
		printf("一共统计了%d个样本像素,但图片的总像素为%d\r\n", sum_h,size);
	}
}