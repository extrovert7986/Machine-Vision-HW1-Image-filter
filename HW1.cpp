#define _CRT_SECURE_NO_WARNINGS

#define WINVER 0x600
#undef UNICODE
#undef _UNICODE
#include <conio.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <time.h>
#include <sstream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

void DarwCanvasLT(Mat target, Size size, Mat &canvas);
void DrawCanvasRT(Mat target, Size size, Mat &canvas);
void DrawCanvasLD(Mat target, Size size, Mat &canvas);
void DrawCanvasRD(Mat target, Size size, Mat &canvas);

int main(int argc, char* argv[])
{

	// open input video
	VideoCapture cap;

	string infilename = "Wildlife.wmv";
	cap.open(infilename);// (arg); //try to open string, this will attempt to open it as a video file
	if (!cap.isOpened()) {
		cerr << "Failed to open a video device or video file!\n" << endl;
		return 1;
	}

	namedWindow("capture", WINDOW_AUTOSIZE);
	Size size = Size((int)cap.get(CV_CAP_PROP_FRAME_WIDTH), (int)cap.get(CV_CAP_PROP_FRAME_HEIGHT));
	cout << size.width << "," << size.height << endl;
	cout << cap.get(CAP_PROP_FPS) << endl;

	// create output video
	string outfilename = "output.wmv";

	VideoWriter videoout;
	videoout.open(outfilename, VideoWriter::fourcc('X', 'V', 'I', 'D'), (int)cap.get(CV_CAP_PROP_FPS), size, true);
	if (!videoout.isOpened()) {
		cap.release();
		return 1;
	}
	Mat frame, img;
	int key = 0;
	int seqno = 0;

	for (; cap.isOpened() && tolower(key) != 'q' && key != 27;) {
		cap >> frame;
		if (frame.empty())  break;
		
		Mat canvas(size.height,size.width,CV_8UC3);

		DarwCanvasLT(frame, size, canvas);
		DrawCanvasRT(frame, size, canvas);
		DrawCanvasLD(frame, size, canvas);
		DrawCanvasRD(frame, size, canvas);

		imshow("capture", canvas);
		videoout << canvas;
		key = waitKey(1);
	}

	videoout.release();

	return 0;
}

/*origin*/
void DarwCanvasLT(Mat target, Size size, Mat &canvas) {
	resize(target, target, Size(size.width / 2, size.height / 2));
	target.copyTo(Mat(canvas, Rect(0, 0, size.width / 2, size.height / 2)));
	putText(canvas, "origin", Point(0, 40), FONT_HERSHEY_TRIPLEX, 1, Scalar(255,0,0));
}

/*gray level*/
void DrawCanvasRT(Mat target, Size size, Mat &canvas) {
	resize(target, target, Size(size.width / 2, size.height / 2));
	cvtColor(target, target, COLOR_BGR2HSV);
	/*create look up table*/
	
	for (int i = 0; i < target.rows; i++) {
		for (int j = 0; j < target.cols; j++) {
			if (target.at<Vec3b>(i, j)[0] >=30 && target.at<Vec3b>(i, j)[0]<=150) {
				target.at<Vec3b>(i, j)[1] = 0;
			}
		}
	}
	cvtColor(target, target, COLOR_HSV2BGR);
	target.copyTo(Mat(canvas, Rect(size.width / 2, 0, size.width / 2, size.height / 2)));
	putText(canvas, "HSV keep H between 0~60 and 300~360", Point(640, 40), FONT_HERSHEY_TRIPLEX, 0.5, Scalar(255, 0, 0));
}

/*line*/
void DrawCanvasLD(Mat target, Size size, Mat &canvas) {
	resize(target, target, Size(size.width / 2, size.height / 2));
	Mat temp = target.clone();
	GaussianBlur(target, temp, Size(3, 3), 7, 7);
	target =target+ 4*(target-temp);

	target.copyTo(Mat(canvas, Rect(0, size.height / 2, size.width / 2, size.height / 2)));
	putText(canvas, "sharp", Point(0, 400), FONT_HERSHEY_TRIPLEX, 1, Scalar(255, 0, 0));
}

/*gaussian pyramid*/

void DrawCanvasRD(Mat target, Size size, Mat &canvas) {
	resize(target, target, Size(size.width / 2, size.height / 2));
	Mat laplacian;
	vector<Mat> temp;
	buildPyramid(target, temp, 3);
	pyrUp(temp[1], laplacian, Size(temp[1].cols*2, temp[1].rows*2));
	temp[0] = temp[0] - laplacian;
	
	temp[0].copyTo(Mat(canvas, Rect(size.width / 2, size.height / 2, size.width / 2, size.height / 2)));
	temp[1].copyTo(Mat(canvas, Rect(size.width / 2, size.height / 2, temp[1].cols, temp[1].rows)));
	temp[2].copyTo(Mat(canvas, Rect(size.width / 2, size.height / 2 + temp[1].rows, temp[2].cols, temp[2].rows)));
	temp[3].copyTo(Mat(canvas, Rect(size.width / 2, size.height / 2 + temp[1].rows + temp[2].rows, temp[3].cols, temp[3].rows)));
	putText(canvas, "Gaussian pyramid", Point(640, 400), FONT_HERSHEY_TRIPLEX, 1, Scalar(255, 0, 0));
	putText(canvas, "Laplacian pyramid", Point(960, 400), FONT_HERSHEY_TRIPLEX, 1, Scalar(255, 255, 255));
	putText(canvas, "(Gau layer1 -Gau layer2 resample)", Point(960, 450), FONT_HERSHEY_TRIPLEX, 0.5, Scalar(255, 255, 255));
}