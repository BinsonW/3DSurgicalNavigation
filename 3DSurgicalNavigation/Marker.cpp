#include "pch.h"
#include "Marker.h"
#include <vector>

Marker::Marker(char* imagemem) :
	camframe(1024, 1280, CV_8UC1, imagemem)
{
	markers3D_ref.push_back(Point3f(0.0f, 0.0f, 0));
	markers3D_ref.push_back(Point3f(150, 37.5, 0));
	markers3D_ref.push_back(Point3f(75, 75, 0));
	markers3D_ref.push_back(Point3f(0.0f, 75, 0));
	markers3D_scalp.push_back(Point3f(0.0, 0.0, 0.0));
	markers3D_scalp.push_back(Point3f(191, 0, 0.0));
	markers3D_scalp.push_back(Point3f(191, 141, 0.0));
	markers3D_scalp.push_back(Point3f(0.0, 141, 0.0));
	namedWindow("screen window");
	namedWindow("project window", WINDOW_NORMAL);
	moveWindow("project window", 1400, 0);
	setWindowProperty("project window", WND_PROP_FULLSCREEN, WINDOW_FULLSCREEN);

	//set scalpmarker detector
	//headmarkerparams.blobColor = 0;
	//set refmarker detector
	//refmarkerparams.blobColor = 255;
	refmarkerparams.minRepeatability = 1;
	headmarkerparams.minRepeatability = 1;
	refmarkerparams.blobColor = 255;
	headmarkerparams.blobColor = 0;

	refdetector = SimpleBlobDetector::create(refmarkerparams);
	headdetector = SimpleBlobDetector::create(headmarkerparams);
}
Marker::~Marker()
{
}
int Marker::reset()
{
	markers2D_ref.clear();
	refboxselect = false;
	refclickselect = false;
	usingref = false;
	refselectdone = false;
	reseting = true;
	return 0;
}
// initiate markers
int Marker::inimarkers(int event, int x, int y) {
	//正在框选参考架第一点
	if (refboxselect && !refselectdone) {
		refbox.x = MIN(x, refboxorigin.x);
		refbox.y = MIN(y, refboxorigin.y);
		refbox.width = std::abs(x - refboxorigin.x);
		refbox.height = std::abs(y - refboxorigin.y);
		refbox &= Rect(0, 0, frame.cols, frame.rows);
	}
	//正在框选头部第一点
	if (headboxselect && !headselectdone) {
		headbox.x = MIN(x, headboxorigin.x);
		headbox.y = MIN(y, headboxorigin.y);
		headbox.width = std::abs(x - headboxorigin.x);
		headbox.height = std::abs(y - headboxorigin.y);
		headbox &= Rect(0, 0, frame.cols, frame.rows);
	}
	switch (event) {
	case EVENT_LBUTTONDOWN:
		//参考架框选开始
		if (!refboxselect && !refselectdone && !refclickselect) {
			refboxselect = true;
			refboxorigin = Point2f(x, y);
			refbox = Rect(x, y, 0, 0);
		}
		break;
	case EVENT_LBUTTONUP:
		//参考架框选结束
		if (refboxselect) {
			refROI = frame(refbox);
			//识别ROI内的追踪点
			refdetector->detect(refROI, refkeypoints);
			KeyPoint::convert(refkeypoints, markers2D_ref_ini);
			//成功识别一个追踪点
			if (markers2D_ref_ini.size() == 1) {
				//将追踪点坐标修改为全局坐标
				markers2D_ref_ini[0].x += refbox.x;
				markers2D_ref_ini[0].y += refbox.y;
				//压入存储容器
				markers2D_ref.push_back(markers2D_ref_ini[0]);
				cout << "参考架追踪点框选结束\n已识别参考架第1个追踪点\n开始点选参考架追踪点\n";
				refclickselect = true;
				refboxselect = false;
				return 1;
			}
			//不能识别一个追踪点
			else {

				cout << "参考架追踪点框选结束\n未能识别参考架第1个追踪点\n请重新框选参考架第1个追踪点\n\n";
				return 0;
			}
		}
		//参考架点选结束
		if (refclickselect) {
			//构建搜索ROI
			refbox.x = x - refbox.width / 2;
			refbox.y = y - refbox.height / 2;
			refROI = frame(refbox);
			//识别ROI内的追踪点，放入暂存容器
			refdetector->detect(refROI, refkeypoints);
			KeyPoint::convert(refkeypoints, markers2D_ref_ini);
			//成功识别一个追踪点
			if (markers2D_ref_ini.size() == 1) {
				cout << "参考架追踪点点选结束\n已识别参考架第 " << markers2D_ref.size() + 1 << "个追踪点\n";
				//将追踪点坐标修改为全局坐标
				markers2D_ref_ini[0].x += refbox.x;
				markers2D_ref_ini[0].y += refbox.y;
				//压入存储容器
				markers2D_ref.push_back(markers2D_ref_ini[0]);
			}
			//不能识别一个追踪点
			else {
				cout << "参考架追踪点点选结束\n未能识别参考架追踪点\n请重新点选\n";
			}
			//若全部选完
			if (markers2D_ref.size() == 4) {
				cout << "已选择全部参考架追踪点\n";
				refclickselect = false;
				refselectdone = true;
			}
		}
		break;
	case EVENT_RBUTTONDOWN:
		//头部框选开始
		if (!headboxselect && !headselectdone && !headclickselect) {
			headboxselect = true;
			headboxorigin = Point2f(x, y);
			headbox = Rect(x, y, 0, 0);
		}
		break;
	case EVENT_RBUTTONUP:
		//头部框选结束
		if (headboxselect) {
			headROI = frame(headbox);


			//识别ROI内的追踪点
			headdetector->detect(headROI, headkeypoints);
			KeyPoint::convert(headkeypoints, markers2D_scalp_ini);
			//成功识别一个追踪点
			if (markers2D_scalp_ini.size() == 1) {
				//将追踪点坐标修改为全局坐标
				markers2D_scalp_ini[0].x += headbox.x;
				markers2D_scalp_ini[0].y += headbox.y;
				//压入存储容器
				markers2D_scalp.push_back(markers2D_scalp_ini[0]);
				cout << "头部追踪点框选结束\n已识别头部第一个追踪点\n开始点选头部追踪点\n";
				headclickselect = true;
				headboxselect = false;
				return 1;
			}
			//不能识别一个追踪点
			else {

				cout << "头部追踪点框选结束\n未能识别头部第一个追踪点\n请重新框选头部第一个追踪点\n";
				return 0;
			}
		}

		//头部点选结束
		if (headclickselect) {
			//构建搜索ROI
			headbox.x = x - headbox.width / 2;
			headbox.y = y - headbox.height / 2;
			headROI = frame(headbox);
			//识别ROI内的追踪点，放入暂存容器
			headdetector->detect(headROI, headkeypoints);
			KeyPoint::convert(headkeypoints, markers2D_scalp_ini);
			//成功识别一个追踪点
			if (markers2D_scalp_ini.size() == 1) {
				cout << "头部追踪点点选结束\n已识别头部第 " << markers2D_scalp.size() + 1 << "个追踪点\n";
				//将追踪点坐标修改为全局坐标
				markers2D_scalp_ini[0].x += headbox.x;
				markers2D_scalp_ini[0].y += headbox.y;
				//压入存储容器
				markers2D_scalp.push_back(markers2D_scalp_ini[0]);
			}
			//不能识别一个追踪点
			else {
				cout << "头部追踪点框选结束\n未能识别头部第一个追踪点\n请重新框选头部第一个追踪点\n";
			}
			//若全部选完
			if (markers2D_scalp.size() == 4) {
				cout << "已选择全部头部追踪点\n";
				headclickselect = false;
				headselectdone = true;
			}
		}
		break;
	}
	return 1;
}
int Marker::trackmarkers(vector<Point2f>& pointsini, vector<Point2f>& points, vector<KeyPoint> keypoints, Rect box, Ptr<SimpleBlobDetector> detector)
{
	int i;
	for (i = 0; i < points.size(); i++) {
		//定义搜索ROI
		box.x = points[i].x - box.width / 2;
		box.y = points[i].y - box.height / 2;
		Mat roi = frame(box);
		//识别ROI内的追踪点，放入暂存容器
		detector->detect(roi, keypoints);
		KeyPoint::convert(keypoints, pointsini);
		//成功识别一个追踪点
		if (pointsini.size() == 1) {
			//将追踪点坐标修改为全局坐标
			pointsini[0].x += box.x;
			pointsini[0].y += box.y;
			//压入存储容器
			points[i] = pointsini[0];
		}
		//不能识别一个追踪点
		else {

		}
	}
	return 1;

}
int Marker::drawmarkers(vector<Point2f>& points, Mat & image, Scalar color, Rect box) {
	int m;
	for (m = 0; m < points.size(); m++)
	{

		String cordi;
		stringstream c;
		c << points[m] << m + 1 << "th Point";
		cordi = c.str();
		//putText(image, cordi, points[m], cv::FONT_HERSHEY_COMPLEX, 0.5, color);
		circle(image, points[m], 3, color, -1, 8);
		rectangle(image, Point(points[m].x - box.width / 2, points[m].y - box.width / 2), Point(points[m].x + box.width / 2, points[m].y + box.width / 2), color);
	}
	return 0;
}
// basic run function
int Marker::run()
{
	camframe.copyTo(frame);
	frame = frame(Range(topedge, bottomedge), Range(0, rightedge));
	//equalizeHist(frame, frame);
	//track markers
	if (!markers2D_ref.empty()) {
		trackmarkers(markers2D_ref_ini, markers2D_ref, refkeypoints, refbox, refdetector);
	}
	if (!markers2D_scalp.empty()) {
		trackmarkers(markers2D_scalp_ini, markers2D_scalp, headkeypoints, headbox, headdetector);
	}
	//convert to colored image
	cvtColor(frame, frame, cv::COLOR_GRAY2BGR);
	//draw markers
	if (!markers2D_ref.empty()) {
		drawmarkers(markers2D_ref, frame, Scalar(0, 0, 255), refbox);
	}
	if (!markers2D_scalp.empty() && !usingref) {
		drawmarkers(markers2D_scalp, frame, Scalar(0, 255, 0), headbox);
	}
	if (reseting&&refselectdone) {
		reseting = false;
		usingref = true;
		cout << "重新识别参考架追踪点成功！继续导航\n\n";
	}
	return 1;


}






// show camera frame before navigation
int Marker::ShowCamFrame()
{
	run();
	//register scalp and ref
	if (headselectdone&&refselectdone && !usingref) {
		//solve scalp-camera pose
		solvePnP(markers3D_scalp, markers2D_scalp, CameraMatrix, distCoefficients, rvec_scalmar, tvec_scalmar);
		//solve ref-camera pose
		solvePnP(markers3D_ref, markers2D_ref, CameraMatrix, distCoefficients, rvec_refmar, tvec_refmar);
		//compute affine matirx
		Affine3d s((Vec3d)rvec_scalmar, (Vec3d)tvec_scalmar);
		Affine3d r((Vec3d)rvec_refmar, (Vec3d)tvec_refmar);
		scalp_pose = s;
		ref_pose = r;

		markers2D_scalp.clear();
		cout << "目标点全部选取完毕，已取消头部追踪\n\n";

		destroyWindow("2D window");
		cout << "已关闭选取窗口\n\n";

		needtoregist = true;
	}
	cv::imshow("screen window", frame);
	char c = (char)waitKey(1);
	if (c == 27) return 0;
	switch (c) {
	case 'c':
		markers2D_scalp.clear();
		markers2D_ref.clear();
		refboxselect = false;
		headboxselect = false;
		refclickselect = false;
		headclickselect = false;
		headselectdone = false;
		refselectdone = false;
		break;
	case 61:
		exposureUp = true;
		break;
	case 45:
		exposureDown = true;
		break;
	}
	return 1;
}


// calculate camera pose and show navigation frame
int Marker::Navigate(Mat projimg)
{
	run();
	
	//solve ref-camera pose
	solvePnP(markers3D_ref, markers2D_ref, CameraMatrix, distCoefficients, rvec_refmar, tvec_refmar,!tvec_refmar.empty());
	//compute affine matirx
	Affine3d r((Vec3d)rvec_refmar, (Vec3d)tvec_refmar);
	ref_pose = r;

	//construct projact frame
	projframe = Mat(bottomedge - topedge, rightedge, CV_8UC3, Scalar(0, 0, 0));
	frame += projimg;
	projframe += projimg;

	flip(projframe, projframe, 1);
	copyMakeBorder(projframe, projframe, 0, 0, 0, blackedge, BORDER_ISOLATED, Scalar::all(0));
	imshow("screen window", frame);
	imshow("project window", projframe);
	int c =waitKeyEx(1);
	if (c != -1) {
		cout << c << endl;
	}

	if (c == 27) return 0;
	switch (c) {
	case 'r':
		reset();
	}
	return 1;
}

int Marker::ShowCamFrameToProjector()
{
	run();
	projframe = frame;
	flip(projframe, projframe, 1);
	copyMakeBorder(projframe, projframe, 0, 0, 0, blackedge, BORDER_ISOLATED, Scalar::all(0));
	cv::imshow("project window", projframe);
	char c = (char)waitKey(1);
	if (c == 27) return 0;
	switch (c) {
	case 'q':
		cout << topedge++ << endl;
		break;
	case 'w':
		cout << topedge-- << endl;
		break;
	case 'a':
		cout << bottomedge++ << endl;
		break;
	case 's':
		cout << bottomedge-- << endl;
		break;
	case 'e':
		cout << blackedge++ << endl;
		break;
	case 'r':
		cout << blackedge-- << endl;
		break;
	case 'd':
		cout << rightedge++ << endl;
		break;
	case 'f':
		cout << rightedge-- << endl;
		break;
	}
	return 1;
}
