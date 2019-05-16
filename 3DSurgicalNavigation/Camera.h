#pragma once
#include<fstream>
#include<iostream>
#include"opencv2/opencv.hpp"
#include"uc480.h"
using namespace std;
using namespace cv;
class Camera
{
private:
	HCAM hcam = (HCAM)0;
	char* ImageMemory = NULL;
	int MemoryId = 0;
	int inicamera();
	int readcamparams(const String path);
	double setAutoGain1 = 1;
	double setAutoGain2 = 1;	
	double ExposureTime_min;
	double ExposureTime_max;
	Mat cammat;
	Mat camdiscoeff;
	Size imgsize;
public:
	Camera(const String path);
	~Camera();
	char* getImageMemory();
	Mat getcammat() { return cammat; }
	Mat getcamdiscoeff() { return camdiscoeff; }
	Size getimgsize() { return imgsize; } 
	int run();
	double ExposureTime=33;
	int setexposure(int param);
	friend  class Frame;
};

