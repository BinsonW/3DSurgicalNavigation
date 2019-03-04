#include "pch.h"
#include <iostream>
#include "Camera.h"
#include "Marker.h"
#include"Model3D.h"
using namespace std;
using namespace cv;

void onMouse(int event, int x, int y, int /*flags*/, void* marker) {
	Marker* marker_ = (Marker*)marker;
	cout << "test";
	marker_->inimarkers(event, x, y);
}
int main() {
	Camera cam;
	Model3D model;
	Marker marker(cam.getImageMemory());	
	setMouseCallback("screen window", onMouse, &marker);
	while (1) {
		cam.run();
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
					marker.usingref = true;
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

}