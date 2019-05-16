#include "pch.h"
#include "Frame.h"


Frame::Frame(char * imagemem, const Mat cammat, const Mat camdiscoeff, const Size imgsize_cam) :
	frame(1024, 1280, CV_8UC1, imagemem),
	//projector calibration 13th
	projroi(0, 161, 1048, 802 - 161),
	projsize(1048 + 95, 802 - 161)
{
	//define camera
	CameraMatrix = cammat;
	distCoefficients = camdiscoeff;
	imgsize_Cam = imgsize_cam;
	//define windows
	namedWindow("screen window", 0);
	namedWindow("project window", WINDOW_NORMAL);
	moveWindow("project window", 1400, 0);
	setWindowProperty("project window", WND_PROP_FULLSCREEN, WINDOW_FULLSCREEN);
	//undistort map
	initUndistortRectifyMap(CameraMatrix, distCoefficients, noArray(), getOptimalNewCameraMatrix(CameraMatrix, distCoefficients, Size(1280, 1024), 1, Size(1280, 1024), 0), Size(1280, 1024), CV_16SC2, undistortmap1, undistortmap2);
	//point_3D
	rmar.p3D.push_back(Point3f(0.0f, 0.0f, 0));
	rmar.p3D.push_back(Point3f(100, 0, 0));
	rmar.p3D.push_back(Point3f(100, 100, 0));
	rmar.p3D.push_back(Point3f(0, 100, 0));

	hmar.p3D.push_back(Point3f(0.0, 0.0, 0.0));
	hmar.p3D.push_back(Point3f(191, 0, 0.0));
	hmar.p3D.push_back(Point3f(191, 141, 0.0));
	hmar.p3D.push_back(Point3f(0.0, 141, 0.0));

	glmar.marnum = 8;
}
Frame::Frame(VideoCapture capture) {
	cap = capture;
	namedWindow("screen window", 1);
	rmar.p3D.push_back(Point3f(0.0f, 0.0f, 0));
	rmar.p3D.push_back(Point3f(100, 0, 0));
	rmar.p3D.push_back(Point3f(100, 100, 0));
	rmar.p3D.push_back(Point3f(0, 100, 0));

	hmar.p3D.push_back(Point3f(0.0, 0.0, 0.0));
	hmar.p3D.push_back(Point3f(191, 0, 0.0));
	hmar.p3D.push_back(Point3f(191, 141, 0.0));
	hmar.p3D.push_back(Point3f(0.0, 141, 0.0));


	//initiate frame
	findpixrange();
	inimarparam(dmar);
	globalsearch(dmar);
}
Frame::~Frame()
{
}
void Frame::findpixrange()
{
	capframe();
	minMaxIdx(frame, &darkpix, &bripix);
	cout << "亮度范围 " << darkpix << " " << bripix << endl;
}
bool Frame::inimarparam(marker& mar)
{
	imshow("screenwindow", frame);
	waitKey(1);
	//initiate frame
	findpixrange();
	int ftimes = 0;
	mar.params.filterByColor = true;
	mar.params.blobColor = 255;
	mar.params.filterByArea = true;
	mar.params.filterByCircularity = true;
	mar.params.minCircularity = 0.6;
	mar.params.minInertiaRatio = 0.5;
	mar.params.minConvexity = 0.94;
	mar.params.minRepeatability = 3;
	mar.color = Scalar(0, 255, 0);

	mar.params.minThreshold = bripix - mar.threoffset - mar.threrange;
	mar.params.maxThreshold = bripix - mar.threoffset;
	mar.params.thresholdStep = mar.threstep;
	while (1)
	{
		mar.params.minThreshold = mar.params.minThreshold - 20;
		mar.params.maxThreshold = mar.params.maxThreshold - 20;
		mar.params.minThreshold = mar.params.minThreshold > darkpix ? mar.params.minThreshold : darkpix;
		if (mar.params.minThreshold == darkpix) {
			cout << "未能检测到任何marker\n";
			return false;
		}
		mar.detector = SimpleBlobDetector::create(mar.params);
		mar.detector->detect(frame, mar.p2Dlast);
		cout << "二值化阈值范围: " << mar.params.minThreshold << " ` " << mar.params.maxThreshold << endl;
		cout << "找到 " << mar.p2Dlast.size() << " 个点\n";
		//连续找到多次
		if (mar.marnum == mar.p2Dlast.size() /*&& mar.iseqsize()*/)
		{
			if (findmanytimes(mar) == true) {
				cout << "marker参数初始化成功！\n";
				break;
			}
		}
	}
	//计算搜索距离
	mar.caldist();
	//计算阈值面积并设置
	mar.calarea();
	//计算搜索的二值化阈值范围
	calbrirange(mar);
	cout << "marker直径 " << mar.size << " 个像素，平均亮度 " << mpix[0];
	cout << " ,周围像素平均亮度 " << surpix[0] << endl;
	//搜索n次并压入容器
	mar.detector = SimpleBlobDetector::create(mar.params);
	for (size_t i = 0; i < mar.meanimgnum; i++)
	{
		mar.detector->detect(frame, mar.p2Dlast);
		mar.sortp();
		mar.p2Dtotal.push_back(mar.p2Dlast);
	}
	//计算mean
	mar.calmean();
	//8个marker？
	if (mar.marnum == 8) {
		seperatemar();
		calHRpose();
	}
	//4个marker
	else {
		calRpose();
	}
	return true;
}
void Frame::calbrirange(Frame::marker & mar)
{
	Mat marmask = Mat::zeros(frame.size(), CV_8UC1);
	circle(marmask, mar.p2Dlast[0].pt, mar.size / 2, Scalar(255, 255, 255), -1);
	mpix = mean(frame, marmask);

	Mat surmask = Mat::zeros(frame.size(), CV_8UC1);
	circle(surmask, mar.p2Dlast[0].pt, mar.size *1.5, Scalar(255, 255, 255), -1);
	circle(surmask, mar.p2Dlast[0].pt, mar.size, Scalar(0, 0, 0), -1);
	surpix = mean(frame, surmask);

	//计算最佳搜索亮度
	//calbestbrirange(mar);
	//设置搜索亮度范围
	mar.params.minThreshold = (surpix[0] - mpix[0]) / 5 + mpix[0];
	mar.params.maxThreshold = mar.params.minThreshold + (surpix[0] - mpix[0]) / 5;
	mar.params.thresholdStep = (mar.params.maxThreshold - mar.params.minThreshold) / 5;
	//mar.params.minThreshold = 175;
	//mar.params.maxThreshold = 185;
	//mar.params.thresholdStep = 5;
}
int Frame::trackmarkers(marker& mar)
{
	mar.caldist();
	for (int i = 0; i < mar.p2Dlast.size(); i++) {
		//定义搜索ROI
		Rect box(mar.p2Dlast[i].pt.x - mar.distance / 2, mar.p2Dlast[i].pt.y - mar.distance / 2, mar.distance, mar.distance);
		Mat roi = frame(box);
		//多次识别ROI内的追踪点，放入暂存容器
		for (size_t j = 0; j < 3; j++)
		{
			mar.detector->detect(roi, mar.p2Dtemp);
			//成功识别一个追踪点
			if (mar.p2Dtemp.size() == 1) {
				//将追踪点坐标修改为全局坐标
				mar.p2Dtemp[0].pt.x += box.x;
				mar.p2Dtemp[0].pt.y += box.y;
				//压入存储容器
				mar.p2Dlast[i] = mar.p2Dtemp[0];
				break;
			}
			//不能识别一个追踪点
			else {
				continue;
			}
		}
	}
	mar.caldist();
	mar.calarea();
	calbrirange(mar);
	mar.calmean();
	return 1;

}
int Frame::drawmarkers(marker mar) {
	int m = 1;
	for (KeyPoint i : mar.p2Dlast) {
		circle(frame, i.pt, 3, mar.color);
		rectangle(frame, Rect(Point2f((i.pt.x - mar.distance / 2), (i.pt.y - mar.distance / 2)), Size(mar.distance, mar.distance)), mar.color);
		putText(frame, to_string(m++), i.pt, FONT_HERSHEY_PLAIN, 1, mar.color);
	}
	return 1;
}
bool Frame::capframe()
{
#ifdef webcam
	cap >> frame;
	cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
#else

#endif // webcam
	return 1;
}

bool Frame::globalsearch(marker& mar)
{
#ifdef webcam
	int i = 0;
	do {
		capframe();
		mar.detector->detect(frame, mar.p2Dlast);

		if (3 == i)
		{
			cout << "未能搜索到marker，重新初始化搜索参数\n";
			inimarparam(mar);
			i = 0;
			continue;
		}
		i++;
	} while (mar.marnum != mar.p2Dlast.size() || !mar.iseqsize());
	cout << "全局搜索成功！\n";
	return true;
#else
	mar.detector->detect(frame, mar.p2Dlast);
	if (mar.marnum == mar.p2Dlast.size() && mar.iseqsize()) {
		cout << "全局搜索成功！\n";
		return true;
	}
	else return false;
#endif // webcam

	return true;
}

bool Frame::findmanytimes(marker& mar)
{
	for (size_t i = 0; i < 5; i++)
	{
		mar.detector->detect(frame, mar.p2Dlast);
		if (mar.marnum != mar.p2Dlast.size() || !mar.iseqsize())return false;
	}
	return true;
}

bool Frame::calbestbrirange(marker & mar)
{

	for (size_t i = mpix[0]; i < surpix[0]; i = i + 10)
	{
		for (size_t j = i; j < surpix[0]; j = j + 10)
		{
			mar.params.minThreshold = i;
			mar.params.maxThreshold = j;
			mar.params.thresholdStep = (j - i) / 3;
			mar.detector = SimpleBlobDetector::create(mar.params);
			int m = 0;
			for (size_t j = 0; j < 10; j++)
			{
				mar.detector->detect(frame, mar.p2Dlast);
				if (mar.marnum == mar.p2Dlast.size()/* && mar.iseqsize()*/)
					m++;
			}
			float ratio = static_cast<float>(m) / 10;
			cout << "二值化阈值范围: " << mar.params.minThreshold << " ` " << mar.params.maxThreshold <</* " 找到 " << mar.p2Dlast.size() << " 个点 " <<*/ " 十次检出率 " << ratio * 100 << "%" << endl;
		}
	}

	return false;
}
bool Frame::calHRpose()
{
	KeyPoint::convert(hmar.p2Dmean, hmar.p2Dm_P2f);
	KeyPoint::convert(rmar.p2Dmean, rmar.p2Dm_P2f);
	solvePnPRansac(hmar.p3D, hmar.p2Dm_P2f, CameraMatrix, distCoefficients, hmar.rvec, hmar.tvec, !hmar.tvec.empty());
	solvePnPRansac(rmar.p3D, rmar.p2Dm_P2f, CameraMatrix, distCoefficients, rmar.rvec, rmar.tvec, !rmar.tvec.empty());
	Affine3d h((Vec3d)hmar.rvec, (Vec3d)hmar.tvec);
	Affine3d r((Vec3d)rmar.rvec, (Vec3d)rmar.tvec);
	headpose = h;
	refpose = r;
	return true;
}
bool Frame::calRpose()
{
	KeyPoint::convert(rmar.p2Dmean, rmar.p2Dm_P2f);
	solvePnP(rmar.p3D, rmar.p2Dm_P2f, CameraMatrix, distCoefficients, rmar.rvec, rmar.tvec, !rmar.tvec.empty(), cv::SOLVEPNP_P3P);
	Affine3d r((Vec3d)rmar.rvec, (Vec3d)rmar.tvec);
	refpose = r;
	return true;
}
bool Frame::Navigate(Mat projimg)
{
	//track markers
	if (!trackmarkers(rmar)) return false;
	calRpose();
	//convert to colored image
	cvtColor(frame, frame, cv::COLOR_GRAY2BGR);
	//draw markers
	drawmarkers(rmar);

	//construct projact frame
	frame_clip = frame(projroi);
	projimg = projimg(Range(117, 581), Range(0, 759));
	projimg.copyTo(projimg_add);
	resize(projimg_add, projimg_add, Size(1048, 641));
	//projframe = Mat(bottomedge - topedge, rightedge, CV_8UC3, Scalar(0, 0, 0));
	frame_clip += projimg_add;
	flip(projimg, projimg, 1);
	copyMakeBorder(projimg, projimg, 0, 0, 0, 69, BORDER_ISOLATED, Scalar::all(0));
	rectangle(frame, projroi, Scalar(0, 255, 0));
	imshow("screen window", frame);
	imshow("project window", projimg);
	int c = waitKey(1);
	return true;
}
int Frame::ShowCamFrameToProjector()
{
	flip(frame, frame, 1);
	copyMakeBorder(frame, frame, 0, 0, 0, projsize.width - projroi.width, BORDER_ISOLATED, Scalar::all(0));
	cv::imshow("project window", frame);
	char c = (char)waitKey(1);
	switch (c) {
	case 32:
		waitKey(0);
		break;
	}
	return 1;
}
bool Frame::seperatemar()
{
	if (refatleft)
	{
		rmar.p2Dmean.assign(glmar.p2Dmean.begin(), glmar.p2Dmean.begin() + 3);
		rmar.p2Dtotal.assign(glmar.p2Dtotal.begin(), glmar.p2Dtotal.begin() + 3);
		hmar.p2Dmean.assign(glmar.p2Dmean.begin() + 4, glmar.p2Dmean.begin() + 7);
		hmar.p2Dtotal.assign(glmar.p2Dtotal.begin() + 4, glmar.p2Dtotal.begin() + 7);
	}
	else {
		hmar.p2Dmean.assign(glmar.p2Dmean.begin(), glmar.p2Dmean.begin() + 3);
		hmar.p2Dtotal.assign(glmar.p2Dtotal.begin(), glmar.p2Dtotal.begin() + 3);
		rmar.p2Dmean.assign(glmar.p2Dmean.begin() + 4, glmar.p2Dmean.begin() + 7);
		rmar.p2Dtotal.assign(glmar.p2Dtotal.begin() + 4, glmar.p2Dtotal.begin() + 7);
	}
	return true;
}
