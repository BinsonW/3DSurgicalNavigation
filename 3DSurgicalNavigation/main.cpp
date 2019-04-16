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
		if(2==frame.run()) continue;
	}
#else
	Camera cam("3240N_MatlabCalib.yml");
	Frame frame(cam.getImageMemory(),cam.getcammat,cam.getcamdiscoeff,cam.getimgsize);
	Model3D model;

	while (1) {
		//check if has pose data
		if (model.hasposedata) {
			marker.reset();
			model.hasposedata = false;
			continue;
		}
		//show camera frame before navigation
		if (!marker.usingref) {
			marker.ShowCamFrame();
			//set exposure
			if (marker.exposureUp) {
				cam.setexposure(1);
				marker.exposureUp = false;
			}
			if (marker.exposureDown) {
				cam.setexposure(-1);
				marker.exposureDown = false;
			}
			//need to regist
			if (marker.needtoregist) {
				if (model.regist(marker.scalp_pose, marker.ref_pose)) {
					marker.needtoregist = false;
					//marker.usingref = true;
					cout << "参考架与头部配准成功！\n开始使用参考架导航！\n\n";
					continue;
				}
				else {
					cout << "参考架与头部配准失败！\n\n";
					return 0;
				}
			}
		}
		//navigate use reference
		else {
			model.run(marker.ref_pose);
			marker.Navigate(model.Projectwindow);
		}
	}
#endif

}