#include "pch.h"
#include <iostream>
#include "Camera.h"
#include "Marker.h"
using namespace std;
using namespace cv;

int main() {
	Camera cam;
	Marker marker(cam.getImageMemory());
	while (1) {
		cam.run();
		marker.ShowCamFrameToProjector();
	}

}