#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2\imgproc\types_c.h>
#include <opencv2/highgui/highgui_c.h>
/*------------------------------------------------------------------------
	OpenCV边缘检测：Canny算子，Sobel算子，Laplace算子，Scharr滤波器合辑
----------------------------------------------------------------------------*/
using namespace cv;
using namespace std;

// 全局变量声明
Mat g_srcImage, g_srcGrayImage, g_dstImage;       //原图，原图的灰度版，目标图

//Canny边缘检测相关变量
Mat g_cannyDetectedEdges;
int g_cannyLowThreshold = 1; //trackbar 位置参数

//Sobel边缘检测相关变量
Mat g_sobelGradient_X, g_sobelGradient_Y;
Mat g_sobelAbsGradient_X, g_sobelAbsGradient_Y;
int g_sobelKernelSize = 1;   //trackbar位置参数

//scharr滤波器相关变量
Mat g_scharrGradient_X, g_scharrGradient_Y;
Mat g_scharrAbsGradient_X, g_scharrAbsGradient_Y;



//全局函数声明
static void ShowHelpText();
static void on_Canny(int, void*);    //Canny边缘检测窗口滚动条的回调函数
static void on_Sobel(int, void*);     //Sobel边缘检测窗口滚动条的回调函数
void Scharr();      //封装了Scharr边缘检测相关代码的函数


int main(int argc, char** argv)
{
	//改变console字体颜色
	//system("color 5F");

	//显示欢迎语
	ShowHelpText();

	//载入原图
	g_srcImage = imread("background.jpg");
	if (!g_srcImage.data)
	{
		printf("Oh，no，读取srcImage错误~！ \n");
		return false;
	}

	//显示原始图
	namedWindow("【原始图】");
	imshow("【原始图】", g_srcImage);

	//创建于src同类型和大小的矩阵（dst）
	g_dstImage.create(g_srcImage.size(), g_srcImage.type());

	//将原始图转换为灰度图像
	cvtColor(g_srcImage, g_srcGrayImage, CV_BGR2GRAY);

	//创建显示窗口
	namedWindow("【效果图】Canny边缘检测", CV_WINDOW_AUTOSIZE);
	namedWindow("【效果图】Sobel边缘检测", CV_WINDOW_AUTOSIZE);

	//创建trackbar
	createTrackbar("参数值：", "【效果图】Canny边缘检测", &g_cannyLowThreshold, 120, on_Canny);
	createTrackbar("参数值：", "【效果图】Sobel边缘检测", &g_sobelKernelSize, 3, on_Sobel);

	//调用回调函数
	on_Canny(0, 0);
	on_Sobel(0, 0);

	//调用封装了Scharr边缘检测代码的函数
	Scharr();

	//轮询获取按键信息，若按下Q，程序退出
	while ((char(waitKey(1)) != 'q')) {}

	return 0;
}

static void ShowHelpText()
{
	// 输出一些帮助信息
	printf("\n\n\t今天还是悲惨的一天~\n\n"
		"\t按下“q”键时，程序退出~!\n"
		"\n\n\t\t\t\t by  Washburn");
}

//Canny边缘检测窗口滚动条的回调函数
void on_Canny(int, void*)
{
	//先使用3*3内核来降噪
	blur(g_srcGrayImage, g_cannyDetectedEdges, Size(3, 3));

	//运行我们的Canny算子
	Canny(g_cannyDetectedEdges, g_cannyDetectedEdges, g_cannyLowThreshold, g_cannyLowThreshold * 3, 3);

	//先将g_dstImage内的所有元素设置为0
	g_dstImage = Scalar::all(0);

	//使用Canny算子输出的边缘图g_cannyDetectedEdges作为掩码，来将原图g_srcImage拷到目标图g_dstImage中
	g_srcImage.copyTo(g_dstImage, g_cannyDetectedEdges);

	//显示效果图
	imshow("【效果图】Canny边缘检测", g_dstImage);
}

//Sobel边缘检测窗口滚动条的回调函数
void on_Sobel(int, void*)
{
	//求X方向梯度
	Sobel(g_srcImage, g_sobelGradient_X, CV_16S, 1, 0, (2 * g_sobelKernelSize + 1), 1, 1, BORDER_DEFAULT);
	convertScaleAbs(g_sobelGradient_X, g_sobelAbsGradient_X);   //计算绝对值，并将结果转换成8位

	//求Y方向梯度
	Sobel(g_srcImage, g_sobelGradient_Y, CV_16S, 0, 1, (2 * g_sobelKernelSize + 1), 1, 1, BORDER_DEFAULT);
	convertScaleAbs(g_sobelGradient_Y, g_sobelAbsGradient_Y);   //计算绝对值，并将结果转换成8位

	//合并梯度
	addWeighted(g_sobelAbsGradient_X, 0.5, g_sobelAbsGradient_Y, 0.5, 0, g_dstImage);
	//addWeighted(g_sobelAbsGradient_X, 0.5, g_sobelAbsGradient_Y, 0.5, 0, g_dstImage);

	//显示效果图
	imshow("【效果图】Sobel边缘检测", g_dstImage);
}

//封装了Scharr边缘检测相关代码的函数
void Scharr()
{
	//求X方向梯度
	Scharr(g_srcImage, g_scharrGradient_X, CV_16S, 1, 0, 1, 0, BORDER_DEFAULT);
	convertScaleAbs(g_scharrGradient_X, g_scharrAbsGradient_X); //计算绝对值，并将结果转换成8位

	//求Y方向梯度
	Scharr(g_srcImage, g_scharrGradient_Y, CV_16S, 0, 1, 1, 0, BORDER_DEFAULT);
	convertScaleAbs(g_scharrGradient_Y, g_scharrAbsGradient_Y); //计算绝对值，并将结果转换成8位

	//合并梯度
	addWeighted(g_scharrAbsGradient_X, 0.5, g_scharrAbsGradient_Y, 0.5, 0, g_dstImage);

	//显示效果图
	imshow("【效果图】Scharr滤波器", g_dstImage);
}
