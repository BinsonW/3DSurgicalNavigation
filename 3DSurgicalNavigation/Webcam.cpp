#include "pch.h"
#include "Webcam.h"


Webcam::Webcam()
{
	cap.open(0);
	cap.set(CAP_PROP_FRAME_WIDTH, 1920);
	cap.set(CAP_PROP_FRAME_HEIGHT, 1080);
}


Webcam::~Webcam()
{
}
