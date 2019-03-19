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

int main(int argc, char* argv[])
{

	// open input video
	VideoCapture cap;
	cout << "Input Video:";
	string infilename;
	cin >> infilename;

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
	cout << "Output Video:";
	string outfilename;
	cin >> outfilename;

	if (outfilename == "") {
		cap.release();
		return 1;
	}

	VideoWriter videoout;

	videoout.open(outfilename, VideoWriter::fourcc('X', 'V', 'I', 'D'), (int)cap.get(CV_CAP_PROP_FPS), size, true);

	if (!videoout.isOpened()) {
		cap.release();
		return 1;
	}

	

	Mat frame,img;

	int key = 0;
	int seqno = 0;

	for (; cap.isOpened() && tolower(key) != 'q' && key != 27;) {

		cap >> frame;
		if (frame.empty())  break;

		frame.copyTo(img);
		
		//capture.grab();
		//capture.retrieve(frame);		
		Mat img0,img1,img2,img3;
		resize(img,img0,Size(size.width/2,size.height/2));
		// apply some image processing algorithms to img0
		
		// place img0,img1,img2,img3 on img
		img0.copyTo(Mat(img,Rect(0,0,size.width/2,size.height/2)));
		img1.copyTo(Mat(img,Rect(size.width/2,0,size.width/2,size.height/2)));
		img2.copyTo(Mat(img,Rect(0,size.height/2,size.width/2,size.height/2)));
		img3.copyTo(Mat(img,Rect(size.width/2,size.height/2,size.width/2,size.height/2)));
		
		// show caption for each image
		putText(img,"origin",Point(0,8),FONT_HERSHEY_PLAIN,3,Scale(255,255,255),3);
		//....
		//...
		
		imshow("capture", img);
		videoout << img;
		key = waitKey(1);
	}

	videoout.release();
	
	return 0;
}

