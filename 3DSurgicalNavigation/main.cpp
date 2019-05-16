#include "pch.h"
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
	Camera cam("3240N_MatlabCalib.yml");
	Frame frame(cam.getImageMemory(), cam.getcammat, cam.getcamdiscoeff, cam.getimgsize);
	Model3D model;
	//regist
	while (1) {
		//check if has pose data
		if (model.hasposedata) {
			frame.reset();
			model.hasposedata = false;
			break;
		}
		
		//need to regist
		if (frame.inimarparam(frame.glmar)) {
			model.regist(frame.headpose, frame.refpose);
			cout << "患者位置注册成功！ 开始使用参考架导航！\n\n";
			break;
		}
	}
	//navigate
	while (1) {
		cam.run();
		model.run(frame.refpose);
		if (!frame.Navigate(model.Projectwindow)) {
			
		}
	}
#endif

}