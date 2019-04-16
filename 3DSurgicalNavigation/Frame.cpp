#include "pch.h"
#include "Frame.h"


Frame::Frame(char * imagemem, const Mat cammat, const Mat camdiscoeff, const Size imgsize_cam) :
	camframe(1024, 1280, CV_8UC1, imagemem),
	projroi(0, 159, 1048, 804 - 259),
	projsize(1048 + 97, 804 - 259)
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

}
Frame::Frame(VideoCapture capture) {
	cap = capture;
	namedWindow("screen window", 0);
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
	globalsearch(dmar, MID);
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
	int ftimes=0;
	//找黑点
		//marker params
	mar.params.filterByColor = true;
	mar.params.blobColor = 0;
	mar.params.filterByArea = true;
	mar.params.filterByCircularity = true;
	mar.params.minCircularity = 0.6;
	mar.params.minInertiaRatio = 0.5;
	mar.params.minConvexity = 0.94;
	mar.params.minRepeatability = 3;
	mar.color = Scalar(0, 255, 0);
	if (0 == mar.params.blobColor) {
		mar.params.minThreshold = darkpix + mar.threoffset;
		mar.params.maxThreshold = darkpix + mar.threoffset + mar.threrange;
		mar.params.thresholdStep = mar.threstep;
		while(1)
		{
			capframe();
			mar.params.minThreshold=mar.params.minThreshold+20;
			mar.params.maxThreshold =mar.params.maxThreshold + 20;
			mar.params.maxThreshold = mar.params.maxThreshold< bripix ? mar.params.maxThreshold : bripix;
			if (mar.params.maxThreshold == bripix) {
				cout << "未能检测到任何marker，退出程序\n";
				abort();
			}
			mar.detector = SimpleBlobDetector::create(mar.params);
			mar.detector->detect(frame, mar.p2Dlast);
			cout << "二值化阈值范围: " << mar.params.minThreshold << " ` " << mar.params.maxThreshold << endl;
			cout << "找到 " << mar.p2Dlast.size() << " 个点\n";
			//连续找到多次
			if (mar.marnum == mar.p2Dlast.size() /*&& mar.iseqsize()*/)
			{
				if (findmanytimes(mar)>=0.9) {
					cout << "marker参数初始化成功！\n";
					break;
				}
			}
		}

	}
	//找白点
	else if (255 == mar.params.blobColor) {

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
		mar.p2Dtotal.push_back(mar.p2Dlast);
	}
	//计算mean，准备注册
	mar.calmean();
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
int Frame::reset()
{
	rmar.clear();
	usingref = false;
	reseting = true;
	return 0;
}
int Frame::trackmarkers(marker& mar)
{
	mar.caldist();
	for (int i = 0; i < mar.p2Dlast.size(); i++) {
		//定义搜索ROI
		Rect box(mar.p2Dlast[i].pt.x - mar.distance / 2, mar.p2Dlast[i].pt.y - mar.distance / 2, mar.distance, mar.distance);
		Mat roi = frame(box);
		//多次识别ROI内的追踪点，放入暂存容器
		for (size_t j = 0; j < 3 ; j++)
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
				return 0;
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
		rectangle(frame, Rect(Point2f((i.pt.x-mar.distance/2),(i.pt.y - mar.distance / 2)), Size(mar.distance, mar.distance)), mar.color);
		putText(frame, to_string(m++), i.pt, FONT_HERSHEY_PLAIN, 1, mar.color);
	}
	return 1;
}
int Frame::run()
{

	capframe();
	//track markers
	if (!trackmarkers(dmar)) {
		globalsearch(dmar, MID);
		return 2;
	}

	//convert to colored image
	cvtColor(frame, frame, cv::COLOR_GRAY2BGR);
	//draw markers
	drawmarkers(dmar);
	if (reseting&&rmar.selectdone) {
		reseting = false;
		usingref = true;
		cout << "重新识别参考架追踪点成功！继续导航\n\n";
	}
	imshow("screen window", frame);
	waitKey(1);
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

int Frame::ShowCamFrame()
{
	run();
	//register scalp and ref
	if (hmar.selectdone&&rmar.selectdone && !usingref) {
		//solve scalp-camera pose
		KeyPoint::convert(hmar.p2Dmean, hmar.p2Dm_P2f);
		solvePnPRansac(hmar.p3D, hmar.p2Dm_P2f, CameraMatrix, distCoefficients, hmar.rvec, hmar.tvec);
		//solve ref-camera pose
		KeyPoint::convert(rmar.p2Dmean, rmar.p2Dm_P2f);
		solvePnPRansac(rmar.p3D, rmar.p2Dm_P2f, CameraMatrix, distCoefficients, rmar.rvec, rmar.tvec);
		//compute affine matirx
		Affine3d s((Vec3d)hmar.rvec, (Vec3d)hmar.tvec);
		Affine3d r((Vec3d)rmar.rvec, (Vec3d)rmar.tvec);
		hmar.pose = s;
		rmar.pose = r;
		cout << "目标点全部选取完毕，已取消头部追踪\n\n";
		cout << "已关闭选取窗口\n\n";

		//markers2D_scalp.clear();
		needtoregist = true;

	}
	cv::imshow("screen window", frame);
	char c = (char)waitKey(1);
	if (c == 27) return 0;
	switch (c) {
	case 'c':

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
//
//
//
bool Frame::globalsearch(marker& mar, marposition p)
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

#endif // webcam

	return true;
	}

float Frame::findmanytimes(marker& mar)
{
	for (size_t i = 0; i < 5; i++)
	{
		mar.detector->detect(frame, mar.p2Dlast);
		if(mar.marnum != mar.p2Dlast.size() || !mar.iseqsize())return false;
	}
	return true;
}

bool Frame::calbestbrirange(marker & mar)
{

	for (size_t i = mpix[0]; i < surpix[0]; i=i+10)
	{
		for (size_t j = i; j < surpix[0]; j=j+10)
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
			cout << "二值化阈值范围: " << mar.params.minThreshold << " ` " << mar.params.maxThreshold <</* " 找到 " << mar.p2Dlast.size() << " 个点 " <<*/ " 十次检出率 " << ratio*100 << "%"<<endl;
		}
	}

	return false;
}
int Frame::ShowCamFrameToProjector()
{
	run();
	projframe = frame(projroi);
	flip(projframe, projframe, 1);
	copyMakeBorder(projframe, projframe, 0, 0, 0, projsize.width-projroi.width, BORDER_ISOLATED, Scalar::all(0));
	cv::imshow("project window", projframe);
	char c = (char)waitKey(1);
	switch (c) {
	case 32:
		waitKey(0);
		break;
	}
	return 1;
}

Frame::bad_glosearch::bad_glosearch(const int num) :runtime_error("全局搜索失败！")
{
	marnum = num;
}

Frame::bad_glosearch::~bad_glosearch()
{
}
