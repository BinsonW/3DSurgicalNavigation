#include "pch.h"
#include <iostream>
#include "Camera.h"
#include "Frame.h"
using namespace std;
using namespace cv;

int main() {
	Camera cam("3240N_MatlabCalib.yml");
	Frame marker(cam.getImageMemory(),  cam.getcammat(), cam.getcamdiscoeff(), cam.getimgsize());
	while (1) {
		cam.run();
		marker.capframe();
		marker.ShowCamFrameToProjector();
		//marker.Showgreen();
	}
}