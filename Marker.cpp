#include "pch.h"
#include "Marker.h"
#include <vector>

Marker::Marker(char* imagemem) :
	camframe(1024, 1280, CV_8UC1, imagemem)
{
	markers3D_ref.push_back(Point3f(0.0f, 0.0f, 0));
	markers3D_ref.push_back(Point3f(100, 0, 0));
	markers3D_ref.push_back(Point3f(100, 100, 0));
	markers3D_ref.push_back(Point3f(0.0f, 100, 0));
	markers3D_scalp.push_back(Point3f(0.0, 0.0, 0.0));
	markers3D_scalp.push_back(Point3f(191, 0, 0.0));
	markers3D_scalp.push_back(Point3f(191, 141, 0.0));
	markers3D_scalp.push_back(Point3f(0.0, 141, 0.0));
	namedWindow("2D window");
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
	refmarkerparams.blobColor= 255;
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
	destroyWindow("screen window");
	destroyWindow("project window");
	return 0;
}
// initiate markers
int Marker::inimarkers(int event, int x, int y) {
	//���ڿ�ѡ�ο��ܵ�һ��
	if (refboxselect && !refselectdone) {
		refbox.x = MIN(x, refboxorigin.x);
		refbox.y = MIN(y, refboxorigin.y);
		refbox.width = std::abs(x - refboxorigin. x);
		refbox.height = std::abs(y - refboxorigin.y);
		refbox &= Rect(0, 0, frame.cols, frame.rows);
	}
	//���ڿ�ѡͷ����һ��
	if (headboxselect && !headselectdone) {
		headbox.x = MIN(x, headboxorigin.x);
		headbox.y = MIN(y, headboxorigin.y);
		headbox.width = std::abs(x - headboxorigin.x);
		headbox.height = std::abs(y - headboxorigin.y);
		headbox &= Rect(0, 0, frame.cols, frame.rows);
	}
	switch (event) {
	case EVENT_LBUTTONDOWN:
		//�ο��ܿ�ѡ��ʼ
		if (!refboxselect && !refselectdone&&!refclickselect) {
			refboxselect = true;
			refboxorigin = Point2f(x, y);
			refbox = Rect(x, y, 0, 0);
		}
		break;
	case EVENT_LBUTTONUP:
		//�ο��ܿ�ѡ����
		if (refboxselect) {
			refROI = frame(refbox);
			//ʶ��ROI�ڵ�׷�ٵ�
			refdetector->detect(refROI, refkeypoints);
			KeyPoint::convert(refkeypoints, markers2D_ref_ini);
			//�ɹ�ʶ��һ��׷�ٵ�
			if (markers2D_ref_ini.size() == 1) {
				//��׷�ٵ������޸�Ϊȫ������
				markers2D_ref_ini[0].x += refbox.x;
				markers2D_ref_ini[0].y += refbox.y;
				//ѹ��洢����
				markers2D_ref.push_back(markers2D_ref_ini[0]);
				cout << "�ο���׷�ٵ��ѡ����\n��ʶ��ο��ܵ�1��׷�ٵ�\n��ʼ��ѡ�ο���׷�ٵ�\n";
				refclickselect = true;
				refboxselect = false;
				return 1;
			}
			//����ʶ��һ��׷�ٵ�
			else {
				
				cout << "�ο���׷�ٵ��ѡ����\nδ��ʶ��ο��ܵ�1��׷�ٵ�\n�����¿�ѡ�ο��ܵ�1��׷�ٵ�\n\n";
				return 0;
			}
		}
		//�ο��ܵ�ѡ����
		if (refclickselect) {
			//��������ROI
			refbox.x = x - refbox.width / 2;
			refbox.y = y - refbox.height / 2;
			refROI = frame(refbox);
			//ʶ��ROI�ڵ�׷�ٵ㣬�����ݴ�����
			refdetector->detect(refROI, refkeypoints);
			KeyPoint::convert(refkeypoints, markers2D_ref_ini);
			//�ɹ�ʶ��һ��׷�ٵ�
			if (markers2D_ref_ini.size() == 1) {
				cout << "�ο���׷�ٵ��ѡ����\n��ʶ��ο��ܵ� "<<markers2D_ref.size()+1<<"��׷�ٵ�\n";
				//��׷�ٵ������޸�Ϊȫ������
				markers2D_ref_ini[0].x += refbox.x;
				markers2D_ref_ini[0].y += refbox.y;
				//ѹ��洢����
				markers2D_ref.push_back(markers2D_ref_ini[0]);
			}
			//����ʶ��һ��׷�ٵ�
			else {
				cout << "�ο���׷�ٵ��ѡ����\nδ��ʶ��ο���׷�ٵ�\n�����µ�ѡ\n";
			}
			//��ȫ��ѡ��
			if (markers2D_ref.size() == 4) {
				cout << "��ѡ��ȫ���ο���׷�ٵ�\n";
				refclickselect = false;
				refselectdone = true;
			}
		}
		break;
	case EVENT_RBUTTONDOWN:
		//ͷ����ѡ��ʼ
		if (!headboxselect && !headselectdone&&!headclickselect) {
			headboxselect = true;
			headboxorigin = Point2f(x, y);
			headbox = Rect(x, y, 0, 0);
		}
		break;
	case EVENT_RBUTTONUP:
		//ͷ����ѡ����
		if (headboxselect) {
			headROI = frame(headbox);


			//ʶ��ROI�ڵ�׷�ٵ�
			headdetector->detect(headROI, headkeypoints);
			KeyPoint::convert(headkeypoints, markers2D_scalp_ini);
			//�ɹ�ʶ��һ��׷�ٵ�
			if (markers2D_scalp_ini.size() == 1) {
				//��׷�ٵ������޸�Ϊȫ������
				markers2D_scalp_ini[0].x += headbox.x;
				markers2D_scalp_ini[0].y += headbox.y;
				//ѹ��洢����
				markers2D_scalp.push_back(markers2D_scalp_ini[0]);
				cout << "ͷ��׷�ٵ��ѡ����\n��ʶ��ͷ����һ��׷�ٵ�\n��ʼ��ѡͷ��׷�ٵ�\n";
				headclickselect = true;
				headboxselect = false;
				return 1;
			}
			//����ʶ��һ��׷�ٵ�
			else {
				
				cout << "ͷ��׷�ٵ��ѡ����\nδ��ʶ��ͷ����һ��׷�ٵ�\n�����¿�ѡͷ����һ��׷�ٵ�\n";
				return 0;
			}
		}

		//ͷ����ѡ����
		if (headclickselect) {
			//��������ROI
			headbox.x = x - headbox.width / 2;
			headbox.y = y - headbox.height / 2;
			headROI = frame(headbox);
			//ʶ��ROI�ڵ�׷�ٵ㣬�����ݴ�����
			headdetector->detect(headROI, headkeypoints);
			KeyPoint::convert(headkeypoints, markers2D_scalp_ini);
			//�ɹ�ʶ��һ��׷�ٵ�
			if (markers2D_scalp_ini.size() == 1) {
				cout << "ͷ��׷�ٵ��ѡ����\n��ʶ��ͷ���� " << markers2D_scalp.size() + 1 << "��׷�ٵ�\n";
				//��׷�ٵ������޸�Ϊȫ������
				markers2D_scalp_ini[0].x += headbox.x;
				markers2D_scalp_ini[0].y += headbox.y;
				//ѹ��洢����
				markers2D_scalp.push_back(markers2D_scalp_ini[0]);
			}
			//����ʶ��һ��׷�ٵ�
			else {
				cout << "ͷ��׷�ٵ��ѡ����\nδ��ʶ��ͷ����һ��׷�ٵ�\n�����¿�ѡͷ����һ��׷�ٵ�\n";
			}
			//��ȫ��ѡ��
			if (markers2D_scalp.size() == 4) {
				cout << "��ѡ��ȫ��ͷ��׷�ٵ�\n";
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
	for (i = 0 ; i < points.size(); i++) {
		//��������ROI
		box.x = points[i].x - box.width/2;
		box.y = points[i].y - box.height / 2;
		Mat roi = frame(box);
		//ʶ��ROI�ڵ�׷�ٵ㣬�����ݴ�����
		detector->detect(roi, keypoints);
		KeyPoint::convert(keypoints, pointsini);
		//�ɹ�ʶ��һ��׷�ٵ�
		if (pointsini.size() == 1) {
			//��׷�ٵ������޸�Ϊȫ������
			pointsini[0].x += box.x;
			pointsini[0].y += box.y;
			//ѹ��洢����
			points[i] = pointsini[0];
		}
		//����ʶ��һ��׷�ٵ�
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
// run thread
int Marker::run()
{
	camframe.copyTo(frame);
	frame = frame(Range(160, 804), Range(0, 1048));
	//equalizeHist(frame, frame);
	frame.copyTo(preframe);
	//track markers
	if (!markers2D_ref.empty()) {
		trackmarkers(markers2D_ref_ini, markers2D_ref, refkeypoints, refbox, refdetector);
	}
	if (!markers2D_scalp.empty()) {
		trackmarkers(markers2D_scalp_ini, markers2D_scalp, headkeypoints ,headbox, headdetector);
	}
	//convert to colored image
	cvtColor(frame, frame, cv::COLOR_GRAY2BGR);
	//draw markers
	if (!markers2D_ref.empty()) {
		drawmarkers(markers2D_ref, frame, Scalar(0, 0, 255),refbox);
	}
	if (!markers2D_scalp.empty() && !usingref) {
		drawmarkers(markers2D_scalp, frame, Scalar(0, 255, 0),headbox);
	}
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
		cout << "Ŀ���ȫ��ѡȡ��ϣ���ȡ��ͷ��׷��\n\n";

		destroyWindow("2D window");
		cout << "�ѹر�ѡȡ����\n\n";

		needtoregist = true;
	}
	if (reseting&&refselectdone) {
		reseting = false;
		usingref = true;
		cout << "����ʶ��ο���׷�ٵ�ɹ�����������\n\n";
	}
	//solve camera pose using ref
	if (usingref) {
		//solve ref-camera pose
		solvePnP(markers3D_ref, markers2D_ref, CameraMatrix, distCoefficients, rvec_refmar, tvec_refmar);
		//compute affine matirx
		Affine3d r((Vec3d)rvec_refmar, (Vec3d)tvec_refmar);
		ref_pose = r;
		//construct projact frame
		projframe = Mat(644, 1048, CV_8UC3, Scalar(0, 0, 0));
		//destroyWindow("2D window");
	}
	/*if (!usingref) */cv::imshow("2D window", frame);
	char c = (char)waitKey(1);
	if (c == 27) return 0;
	switch (c) {
	case 'c':
		markers2D_scalp.clear();
		markers2D_ref.clear();
		refboxselect=false;
		headboxselect=false;
		refclickselect=false;
		headclickselect=false;
		headselectdone=false;
		refselectdone=false;
		break;
	case 61:
		exposureUp = true;
		break;
	case 45:
		exposureDown = true;
		break;
	case 'r':
		reset();
	}
	return 1;
}

int Marker::show(Mat projimg)
{
	frame += projimg;
	projframe += projimg;
	
	flip(projframe, projframe, 1);
	copyMakeBorder(projframe, projframe, 0, 0, 0, 96, BORDER_ISOLATED, Scalar::all(0));
	imshow("screen window", frame);
	imshow("project window", projframe);
	waitKey(1);
	return 0;
}


