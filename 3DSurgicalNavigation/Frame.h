#pragma once
#include "opencv2/opencv.hpp"
#include <iostream>
#include <vector>
#include <stdexcept>
#include "Camera.h"
#define rotateCPI
using namespace std;
using namespace cv;
class Frame
{
private:
	//nested class
	class marker {
	public:
		marker();
		~marker();
		int marnum = 4;
		const int meanimgnum=5;
		float distance=1000;
		float size = 0;
		Scalar color;
		vector<Point3f> p3D;
		//mean location
		vector<KeyPoint> p2Dmean;
		vector<Point2f> p2Dm_P2f;
		vector<KeyPoint> p2Dlast;
		vector<KeyPoint> p2Dtemp;
		vector<vector<KeyPoint>> p2Dtotal;
		SimpleBlobDetector::Params params;
		int threrange = 30;
		int threstep=5;
		int threoffset = 30;
		Ptr<SimpleBlobDetector> detector;
		//register params
		Mat tvec;
		Mat rvec;
		Affine3d pose;
		bool sortp();
		static bool compx(KeyPoint,KeyPoint);
		static bool compy(KeyPoint,KeyPoint);
		static bool compx_rev(KeyPoint i, KeyPoint j);
		bool caldist();
		bool calarea();
		bool calmean();
		bool iseqsize();
	};
	Rect projroi;
	Size projsize;
	bool refinlatter = true;
	//bool keepregister=true;
	Mat frame,camframe,frame_clip,projimg_add,undistortmap1,undistortmap2,CameraMatrix,distCoefficients; 
	Size imgsize_Cam;
	//webcam
	VideoCapture cap;
	double darkpix, bripix;
	Scalar mpix, surpix;
	int hori=0, vert=0;
	int drawmarkers(marker mar);
	bool trackmarkers(marker& mar);
	void moveframe(Mat& projimg);
	void findpixrange();
	void calbrirange(marker & mar);
	bool findmanytimes(marker& mar);
	bool calbestbrirange(marker& mar);
	bool calHRpose();
	bool calRpose();
	bool seperatemar_p2Dlast();
	bool seperatemar_p2Dmean();
public:
	Frame(char* imagemem,const Mat cammat,const Mat camdiscoeff,const Size imgsize_cam);
	Frame(VideoCapture);
	~Frame();
	Affine3d headpose;
	Affine3d refpose;
	marker rmar, hmar, dmar, glmar;
	bool registed = false;
	// calculate camera pose and show navigation frame
	bool Navigate(Mat projimg);
	//show camera frame to projector for calibration
	int ShowCamFrameToProjector();
	bool Showgreen();
	bool ShowCamFrameToScreen();
	int ShowCamFrameToProjectorcoaxial();
	bool inimarparam();
	bool globalsearch();
	bool capframe();
};


