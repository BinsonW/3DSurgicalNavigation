#pragma once
#include "opencv2/core/traits.hpp"
#include "opencv2/core/types.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/calib3d.hpp"
#include <iostream>
using namespace std;
using namespace cv;
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
	
	Mat frame;
	Mat projframe;
	int inimarkers(int event, int x,int y);
	int topedge = 159;
	int bottomedge = 804;
	int rightedge = 1048;
	int blackedge = 97;
private:
	int run();

	bool refboxselect;
	bool headboxselect;
	bool refclickselect;
	bool headclickselect;
	bool headselectdone;
	bool refselectdone;
	Point2f refboxorigin;
	Point2f headboxorigin;
	Rect refbox;
	Rect headbox;
	Mat refROI;
	Mat headROI;
	bool reseting = false;
	Mat camframe;
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
	
	Mat CameraMatrix = (cv::Mat_<float>(3, 3) << 2.4512e+03, 0.,611.5857, 0.,
		2.4526e+03, 379.1207, 0., 0., 1.);
	Mat distCoefficients = (cv::Mat_<float>(4, 1) << -0.0435,-1.8628,0.,0.);
public:
	// show camera frame before navigation
	int ShowCamFrame();
	
	// calculate camera pose and show navigation frame
	int Navigate(Mat projimg);

	//show camera frame to projector for calibration
	int ShowCamFrameToProjector();
};


