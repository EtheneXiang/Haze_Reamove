#include "Dark_Channel_Prior.h"
#include"his.h"
double A_Value[3] = { 0 };
int amount_of_one_in_a_thousand = 0;
Mat Dark_Channel_Img(Mat srcImage, int ksize, int Filter_Flag)
{
	//600*525 ��600��������600��(�������)����525��������525��(�������)
	int height = srcImage.rows;	
	int width = srcImage.cols;
	cout << "ԭʼͼ��ߴ�(��|��)��  "<<height<< "    " << width << endl;
	cout << height*width << endl;
	Mat g_dstImage(height, width, CV_8UC1);  //Mat(int rows, int cols, int type);
	Mat darkchannel_img(height, width, CV_8UC1);  
	uchar min = 0, img_r = 0, img_g = 0, img_b = 0;

	for (int i = 0;i < height;i++)
	{
		for (int j = 0;j < width;j++)
		{
			//��ʵ֤��,OpenCV�ж�RGBͼ�����ݵĴ洢˳����BGR,
			img_b = srcImage.at<Vec3b>(i, j)[0]; //���������RGB��������
			img_g = srcImage.at<Vec3b>(i, j)[1];
			img_r = srcImage.at<Vec3b>(i, j)[2];
			min = uchar(((img_r < img_g) ? (img_r < img_b ? img_r : img_b) : (img_g < img_b ? img_g : img_b)));
			g_dstImage.at<uchar>(i, j) = min;
		}
	}
			
	darkchannel_img= Extreme_value_Filter(g_dstImage, ksize, Filter_Flag);
	imshow("��ͨ��ͼ��", darkchannel_img);
	imwrite("darkchannel_img.jpg", darkchannel_img);
	return  darkchannel_img;
}

Mat Extreme_value_Filter(Mat srcImage, int ksize,int Filter_Flag)
{
	int r = (ksize - 1) / 2;	
    double minVal = 0, maxVal = 0;
	Point minPt, maxPt;
	Mat roi_of_image(ksize, ksize, CV_8UC1);  //��������
	Mat dst_ex(srcImage.rows + ksize - 1, srcImage.cols + ksize - 1, CV_8UC1); //����ͼ��	
	Mat g_dstImage(srcImage.rows, srcImage.cols, CV_8UC1);
	copyMakeBorder(srcImage, dst_ex, r, r, r, r, BORDER_DEFAULT);
	cout << "����ͼ��ߴ�(��|��)��  " << dst_ex.rows << "     " << dst_ex.cols << endl;
	//����src�ı�Ե����ͼ����  
	//src,dst:ԭͼ��Ŀ��ͼ��
	//top, bottom, left, right�ֱ��ʾ��ԭͼ���������Ե�Ĵ�С
    //borderType�������Ե�����ͣ�������������

	//ʹ�����ű߽�������
	if (Filter_Flag == Min_Value_Filter)
	{
		for (int i = 0; i < srcImage.rows; i++)
		{
			for (int j = 0; j < srcImage.cols; j++)
			{
				//���캯�� Rect(x,y,width,height)��x, y Ϊ���Ͻ�����, width, height ��Ϊ���Ϳ�
				Rect roi(j, i, ksize, ksize);  roi_of_image = dst_ex(roi);
				//minMaxLoc �������ڸ����ľ�����Ѱ��������Сֵ�����������ǵ�λ��
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
				//���캯�� Rect(x,y,width,height)��x, y Ϊ���Ͻ�����, width, height ��Ϊ���Ϳ�
				Rect roi(j, i, ksize, ksize);  roi_of_image = dst_ex(roi);
				//minMaxLoc �������ڸ����ľ�����Ѱ��������Сֵ�����������ǵ�λ��
				minMaxLoc(roi_of_image, &minVal, &maxVal, &minPt, &maxPt);
				g_dstImage.at<uchar>(i, j) = uchar(maxVal);
			}
		}
	}
	else		printf("��������ȷ���˲�������(Min_Value_Filter,Max_Value_Filter)\r\n");
   //roi_of_image = dst_ex( Rect(400, 300, ksize, ksize) ); //j>300�ͻ���� ��i>300�����д��ѵ�ij�Ƿ��ģ�  j��������ȴ�����˺�����x��Ҳ��ζ�ų���
	//cout << "��ͨ��ͼ��ߴ�(��|��)��  " << g_dstImage.rows << "     " << g_dstImage.cols << endl;

	return g_dstImage;
}


coordinate_info serach_one_in_a_thousand(Mat srcImage_dark_channel,int Histogram[256])
{
	////���ȫ���������A��һ�����裬�Ӱ�ͨ��ͼ���У���������ȥȡ0.1%�����أ�����¼����(Ӧ�ô����ص���Ԫ��)
	amount_of_one_in_a_thousand =int( (srcImage_dark_channel.cols * srcImage_dark_channel.rows) / 1000 );
	cout << endl<<"ǰ0.1%��һ��������" << amount_of_one_in_a_thousand << " Ԫ��" << endl;
	coordinate_info location_one_in_a_thousand_dark_channel_img;//�൱�ڶ�ά��̬���飬�������ǧ��֮һ���ص�����
	vector<int> data_one_in_a_thousand_dark_channel_img;//�൱��һά��̬���飬�������ǧ��֮һ������ֵ
	double remaining_number = amount_of_one_in_a_thousand;
	//ʹ��ֱ��ͼȷʵ���ҵ�ǰ0.1%�ĵ㡣���Ǹ����ȷ�����꣬�ѵ���������ͼ�����ص�
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

				else if (Histogram[i] < amount_of_one_in_a_thousand) //һ���ҵ���Ϊ0�ĵ㣬�Ͳ��ڽ���forѭ�����������ڲ���while
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
	//���ǰ0.1%�����ص��ֵ
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
		cout << "��û����ȫǰ%0.1�����ݵ�" << endl;
		//if( ! data_one_in_a_thousand_dark_channel_img.empty() ) 
		data_one_in_a_thousand_dark_channel_img.clear();
		remaining_number = amount_of_one_in_a_thousand;
		goto re_serach;
	}
	//.clear          ��յ�ǰ��vector
	//.empty        �ж�vector�Ƿ�Ϊ�� ,�շ���true

	//��¼����
	Mat srcImage_dark_channnel_copy=srcImage_dark_channel.clone();  // imshow("srcImage_dark_channnel_copy", srcImage_dark_channnel_copy);
	int k =1;
    ret:for (int i = 0; i < srcImage_dark_channel.rows; i++)
	{
		if (k > amount_of_one_in_a_thousand) break;  //һ��Ҫ��(amount_of_one_in_a_thousand)��
		for (int j = 0; j < srcImage_dark_channel.cols; j++)
		{
			if (data_one_in_a_thousand_dark_channel_img[k-1] != int(srcImage_dark_channnel_copy.at<uchar>(i, j)))   continue;
			else 
			{
				location_one_in_a_thousand_dark_channel_img.x.push_back(i);
				location_one_in_a_thousand_dark_channel_img.y.push_back(j);
				srcImage_dark_channnel_copy.at<uchar>(i, j) = 0;
				//cout << "�ҵ�ƥ���ʱK��ֵ�ǣ�" << k <<"   "<<"��ʱ��Ҫƥ�������ֵ�ǣ�"<< data_one_in_a_thousand_dark_channel_img[k - 1] <<"   "<< "�ҵ�ƥ���ʱ������(��/��)��" <<i <<"   "<<j<<endl;
				k++;
				goto ret;
			}
		}
	}
		//imwrite("srcImage_dark_channnel_copy.jpg", srcImage_dark_channnel_copy);
		//������srcImage_dark_channel��srcImage_dark_channnel_copyһ��������//���� Mat srcImage_dark_channnel_copy(srcImage_dark_channel); 
		return location_one_in_a_thousand_dark_channel_img;
}
//������ͨ����A����ֵ
void cal_A(Mat srcImage, coordinate_info location_info)
{		
	//��һ���㷨 ȡ���ֵ
	value_info data_one_in_a_thousand_rgb_img; 
	int r_value = 0, g_value = 0, b_value = 0;	
	double A_Value_Thread = 220;
	Mat srcImage_rgb_copy = srcImage.clone();

	//imshow("B", channel[0]);
	//imshow("G", channel[1]);
	//imshow("R", channel[2]);
	
	for (int i = 0;i < amount_of_one_in_a_thousand;i++) //������֪���꣬��ԭͼ��ȡ��RGB��ͨ��������
	{
		b_value = int(srcImage_rgb_copy.at<Vec3b>(location_info.x[i], location_info.y[i])[0]);		data_one_in_a_thousand_rgb_img.b_channel.push_back(b_value);
		g_value = int(srcImage_rgb_copy.at<Vec3b>(location_info.x[i], location_info.y[i])[1]);		data_one_in_a_thousand_rgb_img.g_channel.push_back(g_value);
		r_value = int(srcImage_rgb_copy.at<Vec3b>(location_info.x[i], location_info.y[i])[2]);		data_one_in_a_thousand_rgb_img.r_channel.push_back(r_value);
	}
	//��ȡ������RGB���ݣ����������ֵ
	A_Value[0] = double(*max_element(data_one_in_a_thousand_rgb_img.r_channel.begin(), data_one_in_a_thousand_rgb_img.r_channel.end()));
	A_Value[1] = double(*max_element(data_one_in_a_thousand_rgb_img.g_channel.begin(), data_one_in_a_thousand_rgb_img.g_channel.end()));
	A_Value[2] = double(*max_element(data_one_in_a_thousand_rgb_img.b_channel.begin(), data_one_in_a_thousand_rgb_img.b_channel.end()));
	cout << "��ǧ��֮һ���ݼ���õ���A:   " << endl << "A_R��ֵ��" << A_Value[0] << endl << "A_G��ֵ��" << A_Value[1] << endl << "A_B��ֵ��" << A_Value[2] << endl;

	//A_Value[0] = A_Value[0] < A_Value_Thread ? A_Value[0] : A_Value_Thread;
	//A_Value[1] = A_Value[1] < A_Value_Thread ? A_Value[1] : A_Value_Thread;
	//A_Value[2] = A_Value[2] < A_Value_Thread ? A_Value[2] : A_Value_Thread;
	//cout << "�������ƺ��A��ֵ��" << endl << "A_R��ֵ��" << A_Value[0] << endl << "A_G��ֵ��" << A_Value[1] << endl << "A_B��ֵ��" << A_Value[2] << endl;

	//�ڶ����㷨  ȡ�ķ������������е��ƽ��ֵ��ΪA��ֵ��������������Ϊ�������ȡһ���㣬���ͨ����Aֵ���п���ȫ���ܽӽ�255�������Ļ�����ɴ�����ͼ��ƫɫ�ͳ��ִ���ɫ��

}

Mat cal_T(Mat srcImage)
{
	//���������Ұ�ͨ��ͼ�����ƣ�һ����uchar�����ݣ�һ����double�����ݣ��б�Ҫдһ��ģ�庯����
	//��͸����t
	Mat channel[3];
	split(srcImage, channel); //����//matlab ������˳����R,G,B�� ����opencv�У�����˳����B,G,R��
	Mat srcImage_norm(srcImage.rows, srcImage.cols, CV_64FC3);
	Mat srcImage_norm_r(srcImage.rows, srcImage.cols, CV_64FC1);
	Mat srcImage_norm_g(srcImage.rows, srcImage.cols, CV_64FC1);
	Mat srcImage_norm_b(srcImage.rows, srcImage.cols, CV_64FC1);

	//��һ�㣬���һ������ͨ��ֵ
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

	//�ڶ��㣬����ͨ���е���Сֵ����ͨ������ֵ�ھ�����ͬ�����Ĺ�һ���󣬴�С��ϵ��ԭʼ�Ŀ��ܻᷢ���ı�
	double min = 0, img_r = 0, img_g = 0, img_b = 0;
	Mat g_dstImage(srcImage_norm.rows, srcImage_norm.cols, CV_64FC1);	
	for (int i = 0;i < g_dstImage.rows;i++)
	{
		for (int j = 0;j < g_dstImage.cols;j++)
		{
			//��ʵ֤��,OpenCV�ж�RGBͼ�����ݵĴ洢˳����BGR,
			img_b = srcImage_norm.at<Vec3d>(i, j)[0];
			img_g = srcImage_norm.at<Vec3d>(i, j)[1];
			img_r = srcImage_norm.at<Vec3d>(i, j)[2];
			min = (((img_r < img_g) ? (img_r < img_b ? img_r : img_b) : (img_g < img_b ? img_g : img_b)));
			g_dstImage.at<double>(i, j) = min;
		}
	}
	//imshow("g_dstImage(t)", g_dstImage);


	//�����㣬�ҵ���ͨ���ĸ�����С�󣬶���������ڽ�����Сֵ�˲�
	int ksize = 3; //�����˲�����kernel����ȡ����
	int r = (ksize - 1) / 2;
	double minVal = 0, maxVal = 0;
	double w_omega = 0.95;//�ؾ��������Ե����壬��ֵԽС��ȥ��Ч��Խ������
	Point minPt, maxPt;
	Mat g_dstImage_copy = g_dstImage.clone();
	Mat t_matix(g_dstImage_copy.rows, g_dstImage_copy.cols, CV_64FC1);
	Mat roi_of_image(ksize, ksize, CV_64FC1);  //��������
	Mat dst_ex(g_dstImage.rows + ksize - 1, g_dstImage.cols + ksize - 1, CV_64FC1); //����ͼ��	
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
	//imshow("͸����t��Ԥ��ֵ", t_matix);	
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

			// ��Ͷ��ͼt ��ֵ��Сʱ���ᵼ��restored_img��ֵƫ�󣬴Ӷ�ʹ��ͼ��������׳�����
			t = t_matix.at<double>(i, j) > t_thread ? t_matix.at<double>(i, j) : t_thread;

			restored_img.at<Vec3d>(i, j)[2] = r_value / t + A_Value[0];
			restored_img.at<Vec3d>(i, j)[1] = g_value / t + A_Value[1];
			restored_img.at<Vec3d>(i, j)[0] = b_value / t + A_Value[2];
		}
	}
	imwrite("restored_img.jpg", restored_img);
	return restored_img;

}