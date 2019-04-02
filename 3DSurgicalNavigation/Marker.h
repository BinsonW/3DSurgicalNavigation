#pragma once
#include "opencv2/opencv.hpp"
#include <iostream>
using namespace std;
using namespace cv;
using std::vector;
class Marker
{
public:
	Marker(char* imagemem);
	~Marker();
	int reset();
	bool usingref = false;
	bool needtoregist = false;
	bool exposureUp = false;
	bool exposureDown = false;
	Affine3d scalp_pose;
	Affine3d ref_pose;
	

	int inimarkers(int event, int x,int y); 
	int topedge = 159;
	int bottomedge = 804;
	int rightedge = 1048;
	int blackedge = 97;
private:
	int run();
	bool reseting = false;
	bool refboxselect;
	bool headboxselect;
	bool refclickselect;
	bool headclickselect;
	bool headselectdone;
	bool refselectdone;
	//bool keepregister=true;
	Point2f refboxorigin;
	Point2f headboxorigin;
	Rect refbox;
	Rect headbox;
	Mat frame;
	Mat frame_clip;
	Mat projframe;
	Mat refROI;
	Mat headROI;	
	Mat camframe;
	Mat undistortmap1;
	Mat undistortmap2;
	Mat tvec_scalmar;
	Mat tvec_refmar;
	Mat rvec_scalmar;
	Mat rvec_refmar;
	vector<Point2f> markers2D_scalp;
	vector<Point2f> markers2D_scalp_ini;
	vector<Point2f> markers2D_ref;
	vector<Point2f> markers2D_ref_ini;
	vector<Point3f> markers3D_scalp;
	vector<Point3f> markers3D_ref;
	//暂存识别结果
	vector<KeyPoint> refkeypoints;
	vector<KeyPoint> headkeypoints;

	SimpleBlobDetector::Params headmarkerparams;
	SimpleBlobDetector::Params refmarkerparams;
	Ptr<SimpleBlobDetector> refdetector;
	Ptr<SimpleBlobDetector> headdetector;
	int drawmarkers(vector<Point2f>& points, Mat & image, Scalar color, Rect box);
	int trackmarkers(vector<Point2f>& pointsini, vector<Point2f>& points, vector<KeyPoint> keypoints, Rect box, Ptr<SimpleBlobDetector> detector);
	
	Mat CameraMatrix = (cv::Mat_<float>(3, 3) << 2.4183e+3, 0.,575.76, 0.,
		2.4174e+03, 554.6358, 0., 0., 1.);
	Mat distCoefficients = (cv::Mat_<float>(4, 1) << -0.0805,-0.3996,/*3.6551,*/9.5535e-4,-0.005);
public:
	// show camera frame before navigation
	int ShowCamFrame();
	
	// calculate camera pose and show navigation frame
	int Navigate(Mat projimg);

	//show camera frame to projector for calibration
	int ShowCamFrameToProjector();
};


