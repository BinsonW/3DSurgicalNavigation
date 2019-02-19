#pragma once
#include<iostream>
#include<opencv2/imgproc.hpp>
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
	double setAutoGain1 = 1;
	double setAutoGain2 = 1;
	
	double ExposureTime_min;
	double ExposureTime_max;
public:
	Camera();
	~Camera();
	char* getImageMemory();
	int run();
	double ExposureTime=33;
	int setexposure(int param);
};

