#include "pch.h"
#include "Camera.h"


Camera::Camera(const String path)
{
	readcamparams(path);
	if (inicamera()) {
		cout << "InitCamera complete!\n";
	}
	else
		cout << "fail to InitCamera!\n";
	//set auto gain
	//if (is_SetAutoParameter(hcam, IS_SET_ENABLE_AUTO_GAIN, &setAutoGain1, &setAutoGain2) == IS_SUCCESS)cout << "AutoGain success!\n";
	//get exposure info
	is_Exposure(hcam, IS_EXPOSURE_CMD_GET_EXPOSURE_RANGE_MIN, &ExposureTime_min, sizeof(ExposureTime_min));
	cout << "Minimum Exposure time " << ExposureTime_min << endl;
	is_Exposure(hcam, IS_EXPOSURE_CMD_GET_EXPOSURE_RANGE_MAX, &ExposureTime_max, sizeof(ExposureTime_max));
	cout << "Maximum Exposure time " << ExposureTime_max << endl;
	//set default exposure
	is_Exposure(hcam, IS_EXPOSURE_CMD_SET_EXPOSURE, &ExposureTime, sizeof(ExposureTime));
	cout << "Default exposure time is set to " << ExposureTime << endl;
}


Camera::~Camera()
{
	is_ExitCamera(hcam);
}


char * Camera::getImageMemory()
{
	return ImageMemory;
}


// initiate camera
int Camera::inicamera()
{
	int result = 0;
	result = is_InitCamera(&hcam, NULL);
	if (result == IS_SUCCESS)
		cout << "InitCamera Success!" << endl;
	else {
		cerr << "Can't Initcamera";
		return 0;
	}
	result = is_AllocImageMem(hcam, 1280, 1024, 8, &ImageMemory, &MemoryId);
	if (result == IS_SUCCESS)
		cout << "Allocate Memory Success!" << endl;
	else {
		cerr << "Can't AllocateMemory";
		return 0;
	}
	result = is_SetImageMem(hcam, ImageMemory, MemoryId);
	if (result == IS_SUCCESS)
		cout << "Activate Memory Success!" << endl;
	else {
		cerr << "Can't Activate Memory";
		return 0;
	}
	is_SetColorMode(hcam, IS_CM_MONO8);
	is_SetDisplayMode(hcam, IS_SET_DM_DIB);
	return 1;
}
int Camera::readcamparams(const String path)
{
	FileStorage fs(path, FileStorage::READ);
	if (!fs.isOpened()) {
		cout << "Can't open camera intrinsic file!";
	}
	fs["camera_matrix"] >> cammat;
	cammat.convertTo(cammat1, CV_64FC1);
	fs["distortion_coefficients"] >> camdiscoeff;
	camdiscoeff.convertTo(camdiscoeff, CV_64FC1);
	fs["image_width"] >> imgsize.width;
	fs["image_height"] >> imgsize.height;
	return 1;
}
int Camera::run()
{
	is_FreezeVideo(hcam, IS_WAIT);
	return 0;
}

int Camera::setexposure(int param)
{
	if (param == 1) {
		ExposureTime++;
		is_Exposure(hcam, IS_EXPOSURE_CMD_SET_EXPOSURE, &ExposureTime, sizeof(ExposureTime));
		cout << "increase exposure time to " << ExposureTime << endl;
	}
	if (param == -1) {
		ExposureTime--;
		is_Exposure(hcam, IS_EXPOSURE_CMD_SET_EXPOSURE, &ExposureTime, sizeof(ExposureTime));
		cout << "decrease exposure time to " << ExposureTime << endl;
	}
	return 0;
}
