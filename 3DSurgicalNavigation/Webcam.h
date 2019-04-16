#pragma once
#include "opencv2/opencv.hpp"
using namespace std;
using namespace cv;

class Webcam
{
public:
	VideoCapture cap;

	Webcam();
	~Webcam();
};

