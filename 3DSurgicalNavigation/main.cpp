#include "pch.h"
#include <stdlib.h>
#include <iostream>
#include "Camera.h"
#include "Frame.h"
#include"Model3D.h"
#include"Webcam.h"
#include <String>
#include "SerialPort.h"
using namespace std;
using namespace cv;

int main() {
#ifdef webcam
	Webcam cam;
	Frame frame(cam.cap);
	while (1) {
		if (2 == frame.run()) continue;
	}
#else
	Model3D model;
	Camera cam("3240N_MatlabCalib.yml");
	Frame frame(cam.getImageMemory(), cam.getcammat(), cam.getcamdiscoeff(), cam.getimgsize());
	////co-axial
	//while (1) {
	//	cam.run();
	//	frame.capframe();
	//	if (!frame.ShowCamFrameToScreen()) break;
	//}
	//show img
	while (1) {
		cam.run();
		frame.capframe();
		if (!frame.ShowCamFrameToScreen()) break;
	}
	//regist
	while (1) {
		cam.run();
		frame.capframe();
		//check if has pose data
		if (model.hasposedata) {
			model.hasposedata = false;
			frame.registed = true;
			if (frame.inimarparam()) {
				break;
			}
		}		
		//need to regist
		if (frame.inimarparam()) {
			model.regist(frame.headpose, frame.refpose);
			frame.registed = true;
			cout << "����λ��ע��ɹ��� ��ʼʹ�òο��ܵ�����\n\n";
			break;
		}
	}
	//navigate
	while (1) {		
		cam.run();
		frame.capframe();
		model.run(frame.refpose);
		if (!frame.Navigate(model.Projectwindow)) {
			//�ֲ�����ʧ�ܣ�ȫ������
			cam.run();
			frame.capframe();
			if (!frame.globalsearch())
			{
				//ȫ������ʧ�ܣ���ʼ��������������
				while (!frame.inimarparam()) {
					//Sleep(3000);
					cam.run();
					frame.capframe();
				}
			}
		}
	}
#endif

}