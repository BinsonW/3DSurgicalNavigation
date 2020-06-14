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
			cout << "患者位置注册成功！ 开始使用参考架导航！\n\n";
			break;
		}
	}
	//navigate
	while (1) {		
		cam.run();
		frame.capframe();
		model.run(frame.refpose);
		if (!frame.Navigate(model.Projectwindow)) {
			//局部搜索失败，全局搜索
			cam.run();
			frame.capframe();
			if (!frame.globalsearch())
			{
				//全局搜索失败，初始化参数重新搜索
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