#include "Dark_Channel_Prior.h"
#include"his.h"
double A_Value[3] = { 0 };
int amount_of_one_in_a_thousand = 0;
Mat Dark_Channel_Img(Mat srcImage, int ksize, int Filter_Flag)
{
	//600*525 长600代表了有600列(矩阵概念)，宽525代表了有525行(矩阵概念)
	int height = srcImage.rows;	
	int width = srcImage.cols;
	cout << "原始图像尺寸(高|宽)：  "<<height<< "    " << width << endl;
	cout << height*width << endl;
	Mat g_dstImage(height, width, CV_8UC1);  //Mat(int rows, int cols, int type);
	Mat darkchannel_img(height, width, CV_8UC1);  
	uchar min = 0, img_r = 0, img_g = 0, img_b = 0;

	for (int i = 0;i < height;i++)
	{
		for (int j = 0;j < width;j++)
		{
			//事实证明,OpenCV中对RGB图像数据的存储顺序是BGR,
			img_b = srcImage.at<Vec3b>(i, j)[0]; //这里真的是RGB的排序吗
			img_g = srcImage.at<Vec3b>(i, j)[1];
			img_r = srcImage.at<Vec3b>(i, j)[2];
			min = uchar(((img_r < img_g) ? (img_r < img_b ? img_r : img_b) : (img_g < img_b ? img_g : img_b)));
			g_dstImage.at<uchar>(i, j) = min;
		}
	}
			
	darkchannel_img= Extreme_value_Filter(g_dstImage, ksize, Filter_Flag);
	imshow("暗通道图像", darkchannel_img);
	imwrite("darkchannel_img.jpg", darkchannel_img);
	return  darkchannel_img;
}

Mat Extreme_value_Filter(Mat srcImage, int ksize,int Filter_Flag)
{
	int r = (ksize - 1) / 2;	
    double minVal = 0, maxVal = 0;
	Point minPt, maxPt;
	Mat roi_of_image(ksize, ksize, CV_8UC1);  //滑动窗口
	Mat dst_ex(srcImage.rows + ksize - 1, srcImage.cols + ksize - 1, CV_8UC1); //扩边图像	
	Mat g_dstImage(srcImage.rows, srcImage.cols, CV_8UC1);
	copyMakeBorder(srcImage, dst_ex, r, r, r, r, BORDER_DEFAULT);
	cout << "扩边图像尺寸(高|宽)：  " << dst_ex.rows << "     " << dst_ex.cols << endl;
	//扩充src的边缘，将图像变大  
	//src,dst:原图与目标图像
	//top, bottom, left, right分别表示在原图四周扩充边缘的大小
    //borderType：扩充边缘的类型，就是外插的类型

	//使用扩张边界来处理
	if (Filter_Flag == Min_Value_Filter)
	{
		for (int i = 0; i < srcImage.rows; i++)
		{
			for (int j = 0; j < srcImage.cols; j++)
			{
				//构造函数 Rect(x,y,width,height)，x, y 为左上角坐标, width, height 则为长和宽
				Rect roi(j, i, ksize, ksize);  roi_of_image = dst_ex(roi);
				//minMaxLoc 函数：在给定的矩阵中寻找最大和最小值，并给出它们的位置
				minMaxLoc(roi_of_image, &minVal, &maxVal, &minPt, &maxPt);
				g_dstImage.at<uchar>(i, j) = uchar(minVal);
			}
		}
	}
	else if (Filter_Flag == Max_Value_Filter)
	{
		for (int i = 0; i < srcImage.rows; i++)
		{
			for (int j = 0; j < srcImage.cols; j++)
			{
				//构造函数 Rect(x,y,width,height)，x, y 为左上角坐标, width, height 则为长和宽
				Rect roi(j, i, ksize, ksize);  roi_of_image = dst_ex(roi);
				//minMaxLoc 函数：在给定的矩阵中寻找最大和最小值，并给出它们的位置
				minMaxLoc(roi_of_image, &minVal, &maxVal, &minPt, &maxPt);
				g_dstImage.at<uchar>(i, j) = uchar(maxVal);
			}
		}
	}
	else		printf("请输入正确的滤波器类型(Min_Value_Filter,Max_Value_Filter)\r\n");
   //roi_of_image = dst_ex( Rect(400, 300, ksize, ksize) ); //j>300就会出错 ，i>300不会有错，难道ij是反的？  j是列数，却代表了横坐标x，也意味着长度
	//cout << "暗通道图像尺寸(高|宽)：  " << g_dstImage.rows << "     " << g_dstImage.cols << endl;

	return g_dstImage;
}


coordinate_info serach_one_in_a_thousand(Mat srcImage_dark_channel,int Histogram[256])
{
	////求解全球大气光照A的一个步骤，从暗通道图像中，按照亮度去取0.1%的像素，并记录坐标(应该大量重叠的元素)
	amount_of_one_in_a_thousand =int( (srcImage_dark_channel.cols * srcImage_dark_channel.rows) / 1000 );
	cout << endl<<"前0.1%，一共包含了" << amount_of_one_in_a_thousand << " 元素" << endl;
	coordinate_info location_one_in_a_thousand_dark_channel_img;//相当于二维动态数组，用来存放千分之一像素的坐标
	vector<int> data_one_in_a_thousand_dark_channel_img;//相当于一维动态数组，用来存放千分之一的像素值
	double remaining_number = amount_of_one_in_a_thousand;
	//使用直方图确实好找到前0.1%的点。可是该如何确定坐标，难道遍历所有图像像素点
	re_serach:for (int i = 255;i >-1;i--)
			 {
				if (Histogram[i] == 0) continue;

				else if (Histogram[i] >= amount_of_one_in_a_thousand)
				{
					for(int j=0;j<amount_of_one_in_a_thousand;j++)
					{
						data_one_in_a_thousand_dark_channel_img.push_back(i);
					}
					remaining_number = 0;
					break;
				}

				else if (Histogram[i] < amount_of_one_in_a_thousand) //一旦找到不为0的点，就不在进行for循环，而是在内部用while
				{
					for (int j = 0;j<Histogram[i];j++)
					{
						data_one_in_a_thousand_dark_channel_img.push_back(i);
					}
					remaining_number = amount_of_one_in_a_thousand - Histogram[i];
					while(remaining_number >0)
					{ 
						if (Histogram[i - 1] >= remaining_number)
						{
							for (int j = 0;j < remaining_number;j++)
							{
								data_one_in_a_thousand_dark_channel_img.push_back(i - 1);
							}
							remaining_number = 0;
						}
						else
						{
							for (int j = 0;j<Histogram[i-1];j++)
							{
								data_one_in_a_thousand_dark_channel_img.push_back(i-1);
							}
							remaining_number = remaining_number - Histogram[i - 1];
						}
						i--;
					}
					break;
				}
			}
	//输出前0.1%的像素点的值
	if (data_one_in_a_thousand_dark_channel_img.size() == amount_of_one_in_a_thousand)
	{
		for (int i = 0;i < amount_of_one_in_a_thousand;i++)
		{
			cout << data_one_in_a_thousand_dark_channel_img[i] << "  ";
		}
		cout << endl;
	}
	else
	{
		cout << "还没有找全前%0.1的数据点" << endl;
		//if( ! data_one_in_a_thousand_dark_channel_img.empty() ) 
		data_one_in_a_thousand_dark_channel_img.clear();
		remaining_number = amount_of_one_in_a_thousand;
		goto re_serach;
	}
	//.clear          清空当前的vector
	//.empty        判断vector是否为空 ,空返回true

	//记录坐标
	Mat srcImage_dark_channnel_copy=srcImage_dark_channel.clone();  // imshow("srcImage_dark_channnel_copy", srcImage_dark_channnel_copy);
	int k =1;
    ret:for (int i = 0; i < srcImage_dark_channel.rows; i++)
	{
		if (k > amount_of_one_in_a_thousand) break;  //一共要找(amount_of_one_in_a_thousand)次
		for (int j = 0; j < srcImage_dark_channel.cols; j++)
		{
			if (data_one_in_a_thousand_dark_channel_img[k-1] != int(srcImage_dark_channnel_copy.at<uchar>(i, j)))   continue;
			else 
			{
				location_one_in_a_thousand_dark_channel_img.x.push_back(i);
				location_one_in_a_thousand_dark_channel_img.y.push_back(j);
				srcImage_dark_channnel_copy.at<uchar>(i, j) = 0;
				//cout << "找到匹配点时K的值是：" << k <<"   "<<"此时需要匹配的像素值是："<< data_one_in_a_thousand_dark_channel_img[k - 1] <<"   "<< "找到匹配点时的坐标(行/列)：" <<i <<"   "<<j<<endl;
				k++;
				goto ret;
			}
		}
	}
		//imwrite("srcImage_dark_channnel_copy.jpg", srcImage_dark_channnel_copy);
		//在这里srcImage_dark_channel和srcImage_dark_channnel_copy一样的数据//不能 Mat srcImage_dark_channnel_copy(srcImage_dark_channel); 
		return location_one_in_a_thousand_dark_channel_img;
}
//计算三通道下A的数值
void cal_A(Mat srcImage, coordinate_info location_info)
{		
	//第一种算法 取最大值
	value_info data_one_in_a_thousand_rgb_img; 
	int r_value = 0, g_value = 0, b_value = 0;	
	double A_Value_Thread = 220;
	Mat srcImage_rgb_copy = srcImage.clone();

	//imshow("B", channel[0]);
	//imshow("G", channel[1]);
	//imshow("R", channel[2]);
	
	for (int i = 0;i < amount_of_one_in_a_thousand;i++) //现在已知坐标，从原图中取出RGB三通道的数据
	{
		b_value = int(srcImage_rgb_copy.at<Vec3b>(location_info.x[i], location_info.y[i])[0]);		data_one_in_a_thousand_rgb_img.b_channel.push_back(b_value);
		g_value = int(srcImage_rgb_copy.at<Vec3b>(location_info.x[i], location_info.y[i])[1]);		data_one_in_a_thousand_rgb_img.g_channel.push_back(g_value);
		r_value = int(srcImage_rgb_copy.at<Vec3b>(location_info.x[i], location_info.y[i])[2]);		data_one_in_a_thousand_rgb_img.r_channel.push_back(r_value);
	}
	//从取出来的RGB数据，依次找最大值
	A_Value[0] = double(*max_element(data_one_in_a_thousand_rgb_img.r_channel.begin(), data_one_in_a_thousand_rgb_img.r_channel.end()));
	A_Value[1] = double(*max_element(data_one_in_a_thousand_rgb_img.g_channel.begin(), data_one_in_a_thousand_rgb_img.g_channel.end()));
	A_Value[2] = double(*max_element(data_one_in_a_thousand_rgb_img.b_channel.begin(), data_one_in_a_thousand_rgb_img.b_channel.end()));
	cout << "由千分之一数据计算得到的A:   " << endl << "A_R的值：" << A_Value[0] << endl << "A_G的值：" << A_Value[1] << endl << "A_B的值：" << A_Value[2] << endl;

	//A_Value[0] = A_Value[0] < A_Value_Thread ? A_Value[0] : A_Value_Thread;
	//A_Value[1] = A_Value[1] < A_Value_Thread ? A_Value[1] : A_Value_Thread;
	//A_Value[2] = A_Value[2] < A_Value_Thread ? A_Value[2] : A_Value_Thread;
	//cout << "经过限制后的A的值：" << endl << "A_R的值：" << A_Value[0] << endl << "A_G的值：" << A_Value[1] << endl << "A_B的值：" << A_Value[2] << endl;

	//第二种算法  取的符合条件的所有点的平均值作为A的值，我这样做是因为，如果是取一个点，则各通道的A值很有可能全部很接近255，这样的话会造成处理后的图像偏色和出现大量色斑

}

Mat cal_T(Mat srcImage)
{
	//这个程序和找暗通道图像类似，一个是uchar型数据，一个是double型数据，有必要写一个模板函数吗
	//算透射率t
	Mat channel[3];
	split(srcImage, channel); //分离//matlab 的排列顺序是R,G,B； 而在opencv中，排列顺序是B,G,R。
	Mat srcImage_norm(srcImage.rows, srcImage.cols, CV_64FC3);
	Mat srcImage_norm_r(srcImage.rows, srcImage.cols, CV_64FC1);
	Mat srcImage_norm_g(srcImage.rows, srcImage.cols, CV_64FC1);
	Mat srcImage_norm_b(srcImage.rows, srcImage.cols, CV_64FC1);

	//第一层，算归一化的三通道值
	//Ima_Element_Output(channel[0]);cout << endl;
	for (int i = 0;i < srcImage_norm.rows;i++)
	{
		for (int j = 0;j < srcImage_norm.cols;j++)
		{
			//srcImage_norm_r.at<double>(i, j) = (double(channel[2].at<uchar>(i, j)) / (A_Value[0]));
			//srcImage_norm_g.at<double>(i, j) = (double(channel[1].at<uchar>(i, j)) / (A_Value[1]));
			//srcImage_norm_b.at<double>(i, j) = (double(channel[0].at<uchar>(i, j)) / (A_Value[2]));

			srcImage_norm.at<Vec3d>(i, j)[2] = (double(channel[2].at<uchar>(i, j)) / (A_Value[0]));
			srcImage_norm.at<Vec3d>(i, j)[1] = (double(channel[1].at<uchar>(i, j)) / (A_Value[1]));
			srcImage_norm.at<Vec3d>(i, j)[0] = (double(channel[0].at<uchar>(i, j)) / (A_Value[2]));
			//cout << srcImage_norm.at<Vec3d>(i, j)[2] << endl;
		}
	}

	//第二层，找三通道中的最小值，三通道的数值在经过不同比例的归一化后，大小关系和原始的可能会发生改变
	double min = 0, img_r = 0, img_g = 0, img_b = 0;
	Mat g_dstImage(srcImage_norm.rows, srcImage_norm.cols, CV_64FC1);	
	for (int i = 0;i < g_dstImage.rows;i++)
	{
		for (int j = 0;j < g_dstImage.cols;j++)
		{
			//事实证明,OpenCV中对RGB图像数据的存储顺序是BGR,
			img_b = srcImage_norm.at<Vec3d>(i, j)[0];
			img_g = srcImage_norm.at<Vec3d>(i, j)[1];
			img_r = srcImage_norm.at<Vec3d>(i, j)[2];
			min = (((img_r < img_g) ? (img_r < img_b ? img_r : img_b) : (img_g < img_b ? img_g : img_b)));
			g_dstImage.at<double>(i, j) = min;
		}
	}
	//imshow("g_dstImage(t)", g_dstImage);


	//第三层，找到三通道的各自最小后，对整个结果在进行最小值滤波
	int ksize = 3; //这里滤波器的kernel不能取大了
	int r = (ksize - 1) / 2;
	double minVal = 0, maxVal = 0;
	double w_omega = 0.95;//ω具有着明显的意义，其值越小，去雾效果越不明显
	Point minPt, maxPt;
	Mat g_dstImage_copy = g_dstImage.clone();
	Mat t_matix(g_dstImage_copy.rows, g_dstImage_copy.cols, CV_64FC1);
	Mat roi_of_image(ksize, ksize, CV_64FC1);  //滑动窗口
	Mat dst_ex(g_dstImage.rows + ksize - 1, g_dstImage.cols + ksize - 1, CV_64FC1); //扩边图像	
	copyMakeBorder(g_dstImage_copy, dst_ex, r, r, r, r, BORDER_DEFAULT);
	for (int i = 0; i < g_dstImage_copy.rows; i++)
	{
		for (int j = 0; j < g_dstImage_copy.cols; j++)
		{
			Rect roi(j, i, ksize, ksize);  roi_of_image = dst_ex(roi);
			minMaxLoc(roi_of_image, &minVal, &maxVal, &minPt, &maxPt);
			g_dstImage_copy.at<double>(i, j) = double(minVal);
			t_matix.at<double>(i, j) = 1 - w_omega*g_dstImage_copy.at<double>(i, j);
		}
	}
	//imshow("透射率t的预估值", t_matix);	
	//imwrite("t_matix.jpg", t_matix);
	return t_matix;
}

Mat img_restore(Mat srcImage ,Mat t_matix)
{
	Mat restored_img(srcImage.rows, srcImage.cols, CV_64FC3);
	double r_value = 0, g_value = 0, b_value = 0, t = 0, t_thread = 0.2;
	for (int i = 0;i < srcImage.rows;i++)
	{
		for (int j = 0;j < srcImage.cols;j++)
		{
			r_value = double(srcImage.at<Vec3b>(i, j)[2]) - A_Value[0];
			g_value = double(srcImage.at<Vec3b>(i, j)[1]) - A_Value[1];
			b_value = double(srcImage.at<Vec3b>(i, j)[0]) - A_Value[2];

			// 当投射图t 的值很小时，会导致restored_img的值偏大，从而使淂图像整体向白场过度
			t = t_matix.at<double>(i, j) > t_thread ? t_matix.at<double>(i, j) : t_thread;

			restored_img.at<Vec3d>(i, j)[2] = r_value / t + A_Value[0];
			restored_img.at<Vec3d>(i, j)[1] = g_value / t + A_Value[1];
			restored_img.at<Vec3d>(i, j)[0] = b_value / t + A_Value[2];
		}
	}
	imwrite("restored_img.jpg", restored_img);
	return restored_img;

}