#include <opencv\cv.h>
#include <opencv\highgui.h>
#include <iostream>
#include <stdio.h>

using namespace std;

#define PI 3.141592
#define MAX_POINT 500	// 그냥 쓰는 배열 최대값

// 설정해야 하는 값
#define VOTE_DIFF 10	// vote 이후 필요한 값만 찾기 위한 임의 범위값
#define PIXEL_DIFF 10	// 여러 원들 위치 차이가 이 값보다 작으면 같은 원으로 취급  -> min_dist
//

struct Pixel
{
	int x, y;
};

int drawCircle(IplImage* img, IplImage* edge,unsigned char ** pixel,int radius, int min_dist);
//void myHoughCircle(IplImage* img, int min_dist, int min_radius, int max_radius);

int main()
{

	int min_dist = 10;
	int min_radius = 30;
	int max_radius = 47;

	IplImage* img = cvLoadImage("coin.jpg", 1);
	IplImage *gray_image = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
	IplImage *edge = cvCreateImage(cvGetSize(gray_image), IPL_DEPTH_8U, 1);


	cvCvtColor(img, gray_image, CV_BGR2GRAY);	// 흑백 변환
	cvSmooth(gray_image, gray_image, CV_GAUSSIAN, 7, 7);	// 가우시안 smoothing
	cvCanny(gray_image, edge, 150, 50, 3);	// canny edge

											// canny 이미지 데이터를 2차원 배열에 동적할당 
	unsigned char** pixel = new unsigned char*[edge->height];
	for (int i = 0; i < edge->height; i++) {
		pixel[i] = new unsigned char[edge->widthStep];
	}
	for (int i = 0; i < edge->height; i++) {
		for (int j = 0; j < edge->widthStep; j++) {
			pixel[i][j] = edge->imageData[i*edge->widthStep + j];
		}
	}
	

	cout << "10원 : " << drawCircle(img, edge, pixel, 31, min_dist) << endl;
	
	cout << "50원 : " << drawCircle(img, edge, pixel, 36, min_dist) << endl;

	//cout << "100원 : " << drawCircle(img, edge, pixel, 41, min_dist) << endl;

	cout << "500원 : " << drawCircle(img, edge, pixel, 46, min_dist) << endl;


	cvNamedWindow("image", CV_WINDOW_AUTOSIZE);

	cvShowImage("image", img);

	cvWaitKey(0);

	cvReleaseImage(&img);
	
	/*
	IplImage* img2 = cvLoadImage("money1.jpg", -1);


																				
	unsigned char *r = new unsigned char[img2->widthStep*img2->height];
	unsigned char *g = new unsigned char[img2->widthStep*img2->height];
	unsigned char *b = new unsigned char[img2->widthStep*img2->height];

	float *h = new float[img2->widthStep*img2->height];

	for (int i = 0; i<img2->height; i++) {
		for (int j = 0; j<img2->width; j++) {
			h[i*img2->widthStep + j] = 0.0f;
			r[i*img2->widthStep + j] = 0;
			g[i*img2->widthStep + j] = 0;
			b[i*img2->widthStep + j] = 0;
		}
	}

	for (int i = 0; i < img2->height*img2->widthStep; i++)
	{
		b[i] = img2->imageData[i * 3];
		g[i] = img2->imageData[i * 3 + 1];
		r[i] = img2->imageData[i * 3 + 2];
	}

	
	//h
	for (int i = 0; i < img2->height; i++) {
		for (int j = 0; j < img2->width; j++) {
			h[i*img2->width + j] = acos(((r[i*img2->width + j] + g[i*img2->width + j] + b[i*img2->width + j] / 2.0)
				/ sqrt((r[i*img2->width + j] - g[i*img2->width + j])*(r[i*img2->width + j] - g[i*img2->width + j])
					*(r[i*img2->width + j] - b[i*img2->width + j])*(g[i*img2->width + j] - b[i*img2->width + j]))) * 180 / PI);

			if (b[i*img2->width + j] > g[i*img2->width + j])
				h[i*img2->width + j] = 360.0 - h[i*img2->width + j];
		}
	}

	float total = 0.0;
	float average;
	for (int i = 0; i < img2->height; i++) {
		for (int j = 0; j < img2->width; j++) {
			total += h[i*img2->width + j];
			average = total / (img2->height * img2->width);
		}
	}
	

	*/


	return 0;

}

int drawCircle(IplImage* img, IplImage* edge, unsigned char ** pixel, int radius,int min_dist)
{


	////// edge 이미지의 흰색 값 픽셀마다 원을 그려 vote 하기
	IplImage *vote = cvCreateImage(cvGetSize(edge), IPL_DEPTH_8U, 1);
	for (int i = 0; i < vote->height; i++) {
		for (int j = 0; j < vote->width; j++) {
			vote->imageData[i*vote->width + j] = 0;
		}
	}

	int a;  // vote값 x좌표
	int b;	// vote값 y좌표
	
	for (int i = 0; i < edge->height; i++)
	{
		for (int j = 0; j < edge->width; j++)
		{
			if (pixel[i][j] == 255)
			{
				for (int d = 0; d < 360; d++)
				{

					a = (int)(i - floor(radius*cos(d*PI / 180)));
					b = (int)(j - floor(radius*sin(d*PI / 180)));
					if (a > 0 && b > 0 && a < edge->height&& b < edge->width
						&& vote->imageData[edge->widthStep*a + b] < 255)
						vote->imageData[edge->widthStep*a + b] ++;
				}

			}

		}
	}
	//////


	///// vote 가장 높은 값을 찾기
	unsigned char max = 0;


	for (int i = 0; i < vote->height; i++)
	{
		for (int j = 0; j < vote->width; j++)
		{
			if (max < vote->imageData[edge->widthStep*i + j])
			{
				max = vote->imageData[edge->widthStep*i + j];
			}
		}
	}
	/////


	////가장 높은곳과 표차이가 VOTE_DIFF 미만인 좌표들만 저장 (thresholding 과정과 비슷)
	Pixel* point = new Pixel[MAX_POINT];
	for (int i = 0; i < MAX_POINT; i++)
	{
		point[i].x = 0;
		point[i].y = 0;
	}

	int k = 0;

	for (int i = 0; i < vote->height; i++)
	{
		for (int j = 0; j < vote->width; j++)
		{
			if (vote->imageData[edge->widthStep*i + j]> max - VOTE_DIFF)
			{
				point[k].x = j;
				point[k].y = i;
				k++;
			}
		}
	}
	//////


	///////// 중점이 비슷하고 같은 크기의 여러 원을 하나의 원으로만 나타내도록 하는 과정
	int* xarray = new int[MAX_POINT];
	int* yarray = new int[MAX_POINT];
	for (int i = 0; i < MAX_POINT; i++)
	{
		xarray[i] = 0;
		yarray[i] = 0;
	}

	int count_diff = 0;
	xarray[count_diff] = point[0].x;
	for (int i = 0; i < MAX_POINT - 1; i++)
	{
		if (abs(xarray[count_diff] - point[i].x) < min_dist)
		{
			continue;
		}
		else
		{
			count_diff++;
			xarray[count_diff] = point[i].x;
		}
	}

	count_diff = 0;
	yarray[count_diff] = point[0].y;
	for (int i = 0; i < MAX_POINT - 1; i++)
	{
		if (abs(yarray[count_diff] - point[i].y) < min_dist)
		{
			continue;
		}
		else
		{
			count_diff++;
			yarray[count_diff] = point[i].y;
		}
	}
	/////////////


	////////// 중점이 같은 여러 원을 하나의 원으로만 나타내도록 하는 과정 
	int **overlap_check = new int*[vote->height];
	for (int i = 0; i < vote->height; i++) {
		overlap_check[i] = new int[vote->widthStep];
	}
	for (int i = 0; i < vote->height; i++)
	{
		for (int j = 0; j < vote->width; j++)
		{
			overlap_check[i][j] = 0;
		}
	}
	//////////


	// 원그리기
	int circle_count = 0;
	for (int i = 0; i < MAX_POINT; i++)
	{
		if (xarray[i] != 0 && yarray[i] != 0 && overlap_check[xarray[i]][yarray[i]] == 0)
		{

			cvCircle(img, cvPoint(xarray[i], yarray[i]), radius, CV_RGB(0, 255, 255), 2, 8, 0);
			overlap_check[point[i].y][point[i].x] = 1;
			circle_count++;

		}
	}

	return circle_count;
}

/*
void tmain()
{
	int width, height;


	
	IplImage* image = cvLoadImage("money1.jpg", 1); //파일 로딩


	IplImage* gray_image = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
	IplImage* threshold = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
	IplImage* edge = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
	IplImage* out = cvCreateImage(cvGetSize(image), IPL_DEPTH_32F, 1);

	cvCvtColor(image, gray_image, CV_BGR2GRAY);

	cvSmooth(gray_image, gray_image, CV_GAUSSIAN, 7, 7);	// 가우시안 smoothing

	cvCanny(gray_image, edge, 150, 50, 3);	// canny edge

												  // Convert to grayscale
	

	// HarrisCorner
	cvCornerHarris(edge, out, 5, 7, 0.09);

	Pixel point[4];
	
	int x_min = 0;
	int x_max = 0;
	int y_min = 0;
	int y_max = 0;
	int count = 0;


	cout << x_min << endl;
	cout << y_min << endl;
	cout << x_max << endl;
	cout << y_max << endl;



	cvNamedWindow("source", 1);  //윈도우 생성
	cvShowImage("source", gray_image);  //이미지 출력


	cvNamedWindow("viewer", 1);  //윈도우 생성
	cvShowImage("viewer", out);  //이미지 출력

	cvWaitKey(0);

	cvReleaseImage(&edge);
	cvReleaseImage(&out);
	cvReleaseImage(&image);



}
*/

