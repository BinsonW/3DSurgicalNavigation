#include "pch.h"
#include "Frame.h"


Frame::Frame(char * imagemem, const Mat cammat, const Mat camdiscoeff, const Size imgsize_cam) :
	camframe(1024, 1280, CV_8UC1, imagemem),
	//projector calibration 13th
	projroi(0, 160, 1048, 801 - 160),
	projsize(1048 + 95, 801 - 160)
{
	//define camera
	CameraMatrix = cammat;
	distCoefficients=camdiscoeff;
	imgsize_Cam = imgsize_cam;
	//define windows
	namedWindow("screen window", 0);
	namedWindow("project window", WINDOW_NORMAL);
	moveWindow("project window", 1400, 0);
	setWindowProperty("project window", WND_PROP_FULLSCREEN, WINDOW_FULLSCREEN);
	//undistort map
	//initUndistortRectifyMap(CameraMatrix, distCoefficients, noArray(), getOptimalNewCameraMatrix(CameraMatrix, distCoefficients, Size(1280, 1024), 1, Size(1280, 1024), 0), Size(1280, 1024), CV_16SC2, undistortmap1, undistortmap2);
	//point_3D
	//rmar.p3D.push_back(Point3f(100, 100, 0));
	rmar.p3D.push_back(Point3f(125, 125, 0));
	rmar.p3D.push_back(Point3f(125, 0, 0));
	rmar.p3D.push_back(Point3f(0, 125, 0));
	rmar.p3D.push_back(Point3f(0, 25, 0));

	//Calibration board
	//hmar.p3D.push_back(Point3f(193, 142.5, 0.0));
	//hmar.p3D.push_back(Point3f(193, 8, 0.0));
	//hmar.p3D.push_back(Point3f(8, 142.5, 0.0));
	//hmar.p3D.push_back(Point3f(8, 7.5, 0.0));

	//Calibration board translate
	//hmar.p3D.push_back(Point3f(193, 142.5, -184));
	//hmar.p3D.push_back(Point3f(193, 8, -184));
	//hmar.p3D.push_back(Point3f(8, 142.5, -184));
	//hmar.p3D.push_back(Point3f(8, 7.5, -184));

	////Phantom
	hmar.p3D.push_back(Point3f(-30.4300, -27.7672, -1.6042));
	hmar.p3D.push_back(Point3f(-36.7909, 36.8750, -4.5509));
	hmar.p3D.push_back(Point3f(33.6753, -30.5107, 0.6414));
	hmar.p3D.push_back(Point3f(32.2384, 40.1942, -5.7228));

	//Phantom translate
	//hmar.p3D.push_back(Point3f(-59.4855, -76.4799, 34.5232));
	//hmar.p3D.push_back(Point3f(-64.3759, -11.7098, 37.4699));
	//hmar.p3D.push_back(Point3f(4.5409, -80.6794, 32.2777));
	//hmar.p3D.push_back(Point3f(4.7110, -9.9601, 38.6419));



	glmar.color = Scalar(0, 255, 0);
	rmar.color = Scalar(0, 255, 0);
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
	inimarparam();
	globalsearch();
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
bool Frame::inimarparam()
{
	imshow("screen window", frame);
	waitKey(1);
	//initiate frame
	findpixrange();
	int ftimes = 0;
	glmar.params.filterByColor = true;
	glmar.params.blobColor = 255;
	glmar.params.filterByArea = true;
	glmar.params.filterByCircularity = false;
	glmar.params.filterByInertia= false;
	glmar.params.filterByConvexity= false;
	glmar.params.minCircularity = 0.4;
	glmar.params.minInertiaRatio = 0.4;
	glmar.params.minConvexity = 0.9;
	glmar.params.minRepeatability = 3;


	glmar.params.minThreshold = bripix - glmar.threoffset - glmar.threrange;
	glmar.params.maxThreshold = bripix - glmar.threoffset;
	glmar.params.thresholdStep = glmar.threstep;
	while (1)
	{
		glmar.params.minThreshold = glmar.params.minThreshold - 20;
		glmar.params.maxThreshold = glmar.params.maxThreshold - 20;
		glmar.params.minThreshold = glmar.params.minThreshold > darkpix ? glmar.params.minThreshold : darkpix;
		if (glmar.params.minThreshold == darkpix) {
			cout << "未能检测到任何marker\n";
			return false;
		}
		glmar.detector = SimpleBlobDetector::create(glmar.params);
		glmar.detector->detect(frame, glmar.p2Dlast);
		cout << "二值化阈值范围: " << glmar.params.minThreshold << " ` " << glmar.params.maxThreshold << endl;
		cout << "找到 " << glmar.p2Dlast.size() << " 个点\n";
		if (8 == glmar.p2Dlast.size())
		{
			cout << "marker参数初始化成功！\n";
			glmar.sortp();
			break;
		}
		else if (4 == glmar.p2Dlast.size() && registed)
		{
			cout << "marker参数初始化成功！\n";
			glmar.sortp();
			break;
		}

	}
	rmar.params = glmar.params;
	//unregisted
	if (!registed) {
		//搜索n次并压入容器
		glmar.p2Dtotal.clear();
		for (size_t i = 0; i < glmar.meanimgnum; i++)
		{
			glmar.p2Dtotal.push_back(glmar.p2Dlast);			
			glmar.detector->detect(frame, glmar.p2Dlast);
			glmar.sortp();
		}
		//计算mean
		glmar.calmean();
		seperatemar_p2Dmean();		
		calHRpose();
		seperatemar_p2Dlast();
		//计算搜索距离
		rmar.caldist();
		//计算阈值面积并设置
		rmar.calarea();
		//计算搜索的二值化阈值范围
		calbrirange(rmar);
		cout << "marker直径 " << rmar.size << " 个像素，平均亮度 " << mpix[0];
		cout << " ,周围像素平均亮度 " << surpix[0] << endl;
		return true;
	}
	//registed
	else {
		if (8 == glmar.p2Dlast.size()) {
			seperatemar_p2Dlast();
		}
		else {
			rmar.p2Dlast = glmar.p2Dlast;
		}
		//计算搜索距离
		rmar.caldist();
		//计算阈值面积并设置
		rmar.calarea();
		//计算搜索的二值化阈值范围
		calbrirange(rmar);
		cout << "marker直径 " << rmar.size << " 个像素，平均亮度 " << mpix[0];
		cout << " ,周围像素平均亮度 " << surpix[0] << endl;
		//搜索n次并压入容器
		rmar.detector = SimpleBlobDetector::create(rmar.params);
		for (size_t i = 0; i < rmar.meanimgnum; i++)
		{
			if (!trackmarkers(rmar)) {
				cout << "局部搜索失败！\n" << endl;
				return false;
			}
		}
		//计算mean
		rmar.calmean();
		return true;
	}
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
	mar.params.minThreshold = (mpix[0]- surpix[0]) / 3 + surpix[0];
	mar.params.maxThreshold = mar.params.minThreshold + (mpix[0] - surpix[0]) / 3;
	mar.params.thresholdStep = (mar.params.maxThreshold - mar.params.minThreshold) / 3;
}
bool Frame::trackmarkers(marker& mar)
{
	for (int i = 0; i < mar.p2Dlast.size(); i++) {
		//定义搜索ROI
		Rect box(mar.p2Dlast[i].pt.x - mar.distance / 2, mar.p2Dlast[i].pt.y - mar.distance / 2, mar.distance, mar.distance);
		if (box.x <= 0 || box.y <= 0 || (box.x + box.width) > imgsize_Cam.width || (box.y + box.height) > imgsize_Cam.height) {
			cout << "超出边界，全局搜索！\n";
			return false;
		}
		Mat roi = frame(box);
		mar.detector = SimpleBlobDetector::create(mar.params);
		//识别ROI内的追踪点，放入暂存容器
		mar.detector->detect(roi, mar.p2Dtemp);
		//成功识别一个追踪点
		if (mar.p2Dtemp.size() == 1) {
			//将追踪点坐标修改为全局坐标
			mar.p2Dtemp[0].pt.x += box.x;
			mar.p2Dtemp[0].pt.y += box.y;
			//压入存储容器
			mar.p2Dlast[i] = mar.p2Dtemp[0];
		}
		//不能识别一个追踪点
		else {
			cout << "不能识别第 " << to_string(i + 1) << " 个追踪点，尝试全局搜索\n";
			return false;
		}
	}
	mar.caldist();
	mar.calarea();
	calbrirange(mar);
	mar.calmean();
	return true;
}
void Frame::moveframe(Mat& projimg)
{
	//hori>0,move to right;vert>0,move to down side
	//down
	if (vert > 0) {
		projimg = projimg(Range(vert, projimg.rows), Range(0, projimg.cols));
		copyMakeBorder(projimg, projimg, 0, vert, 0, 0, BORDER_ISOLATED, Scalar::all(0));
	}
	if (vert < 0) {
		projimg = projimg(Range(0, projimg.rows+vert), Range(0, projimg.cols));
		copyMakeBorder(projimg, projimg, -(vert), 0, 0, 0, BORDER_ISOLATED, Scalar::all(0));
	}
	if (hori > 0) {
		projimg = projimg(Range(0, projimg.rows), Range(hori, projimg.cols));
		copyMakeBorder(projimg, projimg, 0, 0, 0, hori, BORDER_ISOLATED, Scalar::all(0));
	}
	if (hori < 0) {
		projimg = projimg(Range(0, projimg.rows), Range(0, projimg.cols+hori));
		copyMakeBorder(projimg, projimg, 0, 0, -(hori), 0, BORDER_ISOLATED, Scalar::all(0));
	}
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
	camframe.copyTo(frame);
#endif // webcam
	return 1;
}

bool Frame::globalsearch()
{
#ifdef webcam
	int i = 0;
	do {
		capframe();
		rmar.detector->detect(frame, rmar.p2Dlast);

		if (3 == i)
		{
			cout << "未能搜索到marker，重新初始化搜索参数\n";
			inimarparam(rmar);
			i = 0;
			continue;
		}
		i++;
	} while (rmar.marnum != rmar.p2Dlast.size() || !rmar.iseqsize());
	cout << "全局搜索成功！\n";
	return true;
#else
	glmar.detector = SimpleBlobDetector::create(rmar.params);
	glmar.detector->detect(frame, glmar.p2Dlast);
	if (8 == glmar.p2Dlast.size()) {
		glmar.sortp();		
		seperatemar_p2Dlast();
		rmar.caldist();
		rmar.calarea();
		calbrirange(rmar);
		rmar.calmean();
		cout << "全局搜索成功！\n";
		return true;
	}
	if (4 == glmar.p2Dlast.size() && glmar.iseqsize()) {
		glmar.sortp();
		rmar.p2Dlast = glmar.p2Dlast;
		rmar.caldist();
		rmar.calarea();
		calbrirange(rmar);
		rmar.calmean();
		cout << "全局搜索成功！\n";
		return true;
	}
	else {
		cout << "全局搜索失败，搜索参数初始化并重新搜索\n";
		return false;
	}
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
	//cout << CameraMatrix << distCoefficients;
	solvePnP(hmar.p3D, hmar.p2Dm_P2f, CameraMatrix, distCoefficients, hmar.rvec, hmar.tvec,0, SOLVEPNP_P3P);
	solvePnP(rmar.p3D, rmar.p2Dm_P2f, CameraMatrix, distCoefficients, rmar.rvec, rmar.tvec);
	Affine3d h((Vec3d)hmar.rvec, (Vec3d)hmar.tvec);
	Affine3d r((Vec3d)rmar.rvec, (Vec3d)rmar.tvec);
	headpose = h;
	refpose = r;
	return true;
}
bool Frame::calRpose()
{
	KeyPoint::convert(rmar.p2Dmean, rmar.p2Dm_P2f);
	solvePnP(rmar.p3D, rmar.p2Dm_P2f, CameraMatrix, distCoefficients, rmar.rvec, rmar.tvec, !rmar.tvec.empty());
	//cout << rmar.tvec;
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
	projimg = projimg(Range(116, 580), Range(0, 760));
	projimg.copyTo(projimg_add);
	resize(projimg_add, projimg_add, Size(1048, 641));
	//projframe = Mat(bottomedge - topedge, rightedge, CV_8UC3, Scalar(0, 0, 0));
	frame_clip += projimg_add;
	flip(projimg, projimg, 1);
	moveframe(projimg);
	copyMakeBorder(projimg, projimg, 0, 0, 0, 68, BORDER_ISOLATED, Scalar::all(0));
	rectangle(frame, projroi, Scalar(0, 255, 0));
	imshow("screen window", frame);
	imshow("project window", projimg);
	int c = waitKey(1);
	switch (c)
	{
	case 'w':
		vert--;
		cout << "up 1 pix\n";
		break;
	case 'a':
		hori--;
		cout << "left 1 pix\n";
		break;
	case 's':
		vert++;
		cout << "down 1 pix\n";
		break;
	case 'd':
		hori++;
		cout << "right 1 pix\n";
		break;
	default:
		break;
	}
	cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
	return true;
}
int Frame::ShowCamFrameToProjector()
{
	frame.copyTo(frame_clip);
	frame_clip = frame_clip(projroi);

	//resize(frame_clip, frame_clip, Size(759, 464));

	flip(frame_clip, frame_clip, 1);
	//copyMakeBorder(frame_clip, frame_clip, 0, 0, 0,69, BORDER_ISOLATED, Scalar::all(0));
	copyMakeBorder(frame_clip, frame_clip, 0, 0, 0, projsize.width - projroi.width, BORDER_ISOLATED, Scalar::all(0));
	cvtColor(frame, frame, cv::COLOR_GRAY2BGR);
	rectangle(frame, projroi, Scalar(0, 255, 0),5);
	imshow("screen window", frame);
	imshow("project window", frame_clip);
	char c = (char)waitKey(1);
	switch (c) {
	case 32:
		waitKey(0);
		break;
	}
	return 1;
}
bool Frame::ShowCamFrameToScreen()
{
	imshow("screen window", frame);
	char c = static_cast<char>(waitKey(1));
	if(c!=32)	return true;
	else return false;
}
bool Frame::seperatemar_p2Dlast()
{
	if (refatleft)
	{
		hmar.p2Dlast.assign(glmar.p2Dlast.begin(), glmar.p2Dlast.begin() + 4);
		rmar.p2Dlast.assign(glmar.p2Dlast.begin() + 4, glmar.p2Dlast.begin() + 8);
	}
	else {
		rmar.p2Dlast.assign(glmar.p2Dlast.begin(), glmar.p2Dlast.begin() + 4);
		hmar.p2Dlast.assign(glmar.p2Dlast.begin() + 4, glmar.p2Dlast.begin() + 8);
	}
	return true;
}
bool Frame::seperatemar_p2Dmean()
{
	if (refatleft)
	{
		hmar.p2Dmean.assign(glmar.p2Dmean.begin(), glmar.p2Dmean.begin() + 4);
		rmar.p2Dmean.assign(glmar.p2Dmean.begin() + 4, glmar.p2Dmean.begin() + 8);
	}
	else {
		rmar.p2Dmean.assign(glmar.p2Dmean.begin(), glmar.p2Dmean.begin() + 4);
		hmar.p2Dmean.assign(glmar.p2Dmean.begin() + 4, glmar.p2Dmean.begin() + 8);
	}
	return true;
}
