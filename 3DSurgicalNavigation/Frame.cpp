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
	cout << "���ȷ�Χ " << darkpix << " " << bripix << endl;
}
bool Frame::inimarparam()
{
	imshow("screenwindow", frame);
	waitKey(1);
	//initiate frame
	findpixrange();
	int ftimes = 0;
	glmar.params.filterByColor = true;
	glmar.params.blobColor = 255;
	glmar.params.filterByArea = true;
	glmar.params.filterByCircularity = true;
	glmar.params.minCircularity = 0.6;
	glmar.params.minInertiaRatio = 0.5;
	glmar.params.minConvexity = 0.94;
	glmar.params.minRepeatability = 3;
	glmar.color = Scalar(0, 255, 0);

	glmar.params.minThreshold = bripix - glmar.threoffset - glmar.threrange;
	glmar.params.maxThreshold = bripix - glmar.threoffset;
	glmar.params.thresholdStep = glmar.threstep;
	while (1)
	{
		glmar.params.minThreshold = glmar.params.minThreshold - 20;
		glmar.params.maxThreshold = glmar.params.maxThreshold - 20;
		glmar.params.minThreshold = glmar.params.minThreshold > darkpix ? glmar.params.minThreshold : darkpix;
		if (glmar.params.minThreshold == darkpix) {
			cout << "δ�ܼ�⵽�κ�marker\n";
			return false;
		}
		glmar.detector = SimpleBlobDetector::create(glmar.params);
		glmar.detector->detect(frame, glmar.p2Dlast);
		cout << "��ֵ����ֵ��Χ: " << glmar.params.minThreshold << " ` " << glmar.params.maxThreshold << endl;
		cout << "�ҵ� " << glmar.p2Dlast.size() << " ����\n";
		if (8 == glmar.p2Dlast.size())
		{
			cout << "marker������ʼ���ɹ���\n";
			glmar.sortp();
			break;
		}
		else if (4 == glmar.p2Dlast.size() && registed)
		{
			cout << "marker������ʼ���ɹ���\n";
			glmar.sortp();
			break;
		}

	}
	//unregisted
	if (!registed) {
		//����n�β�ѹ������
		for (size_t i = 0; i < glmar.meanimgnum; i++)
		{
			glmar.p2Dtotal.push_back(glmar.p2Dlast);			
			glmar.detector->detect(frame, glmar.p2Dlast);
			glmar.sortp();
		}
		//����mean
		glmar.calmean();
		seperatemar_p2Dmean();
		calHRpose();
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
		//������������
		rmar.caldist();
		//������ֵ���������
		rmar.calarea();
		//���������Ķ�ֵ����ֵ��Χ
		calbrirange(rmar);
		cout << "markerֱ�� " << rmar.size << " �����أ�ƽ������ " << mpix[0];
		cout << " ,��Χ����ƽ������ " << surpix[0] << endl;
		//����n�β�ѹ������
		rmar.detector = SimpleBlobDetector::create(rmar.params);
		for (size_t i = 0; i < rmar.meanimgnum; i++)
		{
			rmar.detector->detect(frame, rmar.p2Dlast);
			rmar.sortp();
			rmar.p2Dtotal.push_back(rmar.p2Dlast);
		}
		//����mean
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

	//���������������
	//calbestbrirange(mar);
	//�����������ȷ�Χ
	mar.params.minThreshold = (surpix[0] - mpix[0]) / 5 + mpix[0];
	mar.params.maxThreshold = mar.params.minThreshold + (surpix[0] - mpix[0]) / 5;
	mar.params.thresholdStep = (mar.params.maxThreshold - mar.params.minThreshold) / 5;
	//mar.params.minThreshold = 175;
	//mar.params.maxThreshold = 185;
	//mar.params.thresholdStep = 5;
}
bool Frame::trackmarkers(marker& mar)
{
	for (int i = 0; i < mar.p2Dlast.size(); i++) {
		//��������ROI
		Rect box(mar.p2Dlast[i].pt.x - mar.distance / 2, mar.p2Dlast[i].pt.y - mar.distance / 2, mar.distance, mar.distance);
		Mat roi = frame(box);
		mar.detector = SimpleBlobDetector::create(mar.params);
		//ʶ��ROI�ڵ�׷�ٵ㣬�����ݴ�����
		mar.detector->detect(roi, mar.p2Dtemp);
		//�ɹ�ʶ��һ��׷�ٵ�
		if (mar.p2Dtemp.size() == 1) {
			//��׷�ٵ������޸�Ϊȫ������
			mar.p2Dtemp[0].pt.x += box.x;
			mar.p2Dtemp[0].pt.y += box.y;
			//ѹ��洢����
			mar.p2Dlast[i] = mar.p2Dtemp[0];
			break;
		}
		//����ʶ��һ��׷�ٵ�
		else {
			cout << "����ʶ��� " << to_string(i + 1) << " ��׷�ٵ㣬����ȫ������\n";
			return false;
		}
	}
	mar.caldist();
	mar.calarea();
	calbrirange(mar);
	mar.calmean();
	return true;
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

bool Frame::globalsearch()
{
#ifdef webcam
	int i = 0;
	do {
		capframe();
		rmar.detector->detect(frame, rmar.p2Dlast);

		if (3 == i)
		{
			cout << "δ��������marker�����³�ʼ����������\n";
			inimarparam(rmar);
			i = 0;
			continue;
		}
		i++;
	} while (rmar.marnum != rmar.p2Dlast.size() || !rmar.iseqsize());
	cout << "ȫ�������ɹ���\n";
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
		cout << "ȫ�������ɹ���\n";
		return true;
	}
	if (4 == glmar.p2Dlast.size() && glmar.iseqsize()) {
		glmar.sortp();
		rmar.p2Dlast = glmar.p2Dlast;
		rmar.caldist();
		rmar.calarea();
		calbrirange(rmar);
		rmar.calmean();
		cout << "ȫ�������ɹ���\n";
		return true;
	}
	else {
		cout << "ȫ������ʧ�ܣ�����������ʼ������������\n";
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
			cout << "��ֵ����ֵ��Χ: " << mar.params.minThreshold << " ` " << mar.params.maxThreshold <</* " �ҵ� " << mar.p2Dlast.size() << " ���� " <<*/ " ʮ�μ���� " << ratio * 100 << "%" << endl;
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
bool Frame::seperatemar_p2Dlast()
{
	if (refatleft)
	{
		rmar.p2Dlast.assign(glmar.p2Dlast.begin(), glmar.p2Dlast.begin() + 3);
		hmar.p2Dlast.assign(glmar.p2Dlast.begin() + 4, glmar.p2Dlast.begin() + 7);
	}
	else {
		hmar.p2Dlast.assign(glmar.p2Dlast.begin(), glmar.p2Dlast.begin() + 3);
		rmar.p2Dlast.assign(glmar.p2Dlast.begin() + 4, glmar.p2Dlast.begin() + 7);
	}
	return true;
}
bool Frame::seperatemar_p2Dmean()
{
	if (refatleft)
	{
		rmar.p2Dmean.assign(glmar.p2Dmean.begin(), glmar.p2Dmean.begin() + 3);
		hmar.p2Dmean.assign(glmar.p2Dmean.begin() + 4, glmar.p2Dmean.begin() + 7);
	}
	else {
		hmar.p2Dmean.assign(glmar.p2Dmean.begin(), glmar.p2Dmean.begin() + 3);
		rmar.p2Dmean.assign(glmar.p2Dmean.begin() + 4, glmar.p2Dmean.begin() + 7);
	}
	return true;
}