#pragma once
#include "opencv2/opencv.hpp"
#include <iostream>
#include <vector>
#include <stdexcept>
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
		bool boxselect;
		bool clickselect;
		bool selectdone;
		const int marnum = 4;
		const int meanimgnum=3;
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
		bool clear();
		bool sortp();
		static bool compx(KeyPoint,KeyPoint);
		static bool compy(KeyPoint,KeyPoint);
		bool caldist();
		bool calarea();
		bool calmean();
		bool iseqsize();
	}rmar,hmar,dmar,glmar;
	enum marposition {
		LEFT,RIGHT,UP,DOWN,MID
	};
	class bad_glosearch:public runtime_error
	{
	public:
		int marnum;
		bad_glosearch(const int);
		~bad_glosearch();
	};
	Rect projroi;
	Size projsize;
	bool reseting = false;
	//bool keepregister=true;
	Mat frame,frame_clip,projframe,camframe,undistortmap1,undistortmap2,CameraMatrix,distCoefficients; 
	Size imgsize_Cam;
	//webcam
	VideoCapture cap;
	double darkpix, bripix;
	Scalar mpix, surpix;
	int drawmarkers(marker mar);
	int trackmarkers(marker& mar);
	bool capframe();
	void findpixrange();
	bool inimarparam(marker& mar);
	void calbrirange(Frame::marker & mar);
	bool globalsearch(marker& mar, marposition p);
	float findmanytimes(marker& mar);
	bool calbestbrirange(marker& mar);
public:
	Frame(char* imagemem,const Mat cammat,const Mat camdiscoeff,const Size imgsize_cam);
	Frame(VideoCapture);
	~Frame();
	int reset();
	bool usingref = false;
	bool needtoregist = false;
	bool exposureUp = false;
	bool exposureDown = false;
	// show camera frame before navigation
	int ShowCamFrame();
	
	// calculate camera pose and show navigation frame
	int Navigate(Mat projimg);

	//show camera frame to projector for calibration
	int ShowCamFrameToProjector();
	int run();

	
};


