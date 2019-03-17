#include "pch.h"
#include <iostream>
#include "Camera.h"
#include "Marker.h"
#include"Model3D.h"
#include <String>
#include "SerialPort.h"
using namespace std;
using namespace cv;

void onMouse(int event, int x, int y, int /*flags*/, void* marker) {
	Marker* marker_ = (Marker*)marker;
	marker_->inimarkers(event, x, y);
}
wchar_t *multiByteToWideChar(const string& pKey)
{
	char* pCStrKey = const_cast<char*>(pKey.c_str());
	//��һ�ε��÷���ת������ַ������ȣ�����ȷ��Ϊwchar_t*���ٶ����ڴ�ռ�
	int pSize = MultiByteToWideChar(CP_OEMCP, 0, pCStrKey, strlen(pCStrKey) + 1, NULL, 0);
	wchar_t *pWCStrKey = new wchar_t[pSize];
	//�ڶ��ε��ý����ֽ��ַ���ת����˫�ֽ��ַ���
	MultiByteToWideChar(CP_OEMCP, 0, pCStrKey, strlen(pCStrKey) + 1, pWCStrKey, pSize);
	return pWCStrKey;
}
int setserialport() {
	String comname("COM1");
	wchar_t* comport = multiByteToWideChar(comname);
	HANDLE port = CreateFile(comport, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
	if (port == INVALID_HANDLE_VALUE) {
		cout << "serialport open failed!\n" << GetLastError();
		return 0;
	}
	BOOL errorflag = false;
	char message[] = "test";
	DWORD dwBytesToWrite = (DWORD)strlen(message);
	DWORD dwBytesWritten = 0;
	
	errorflag = WriteFile(port, message, dwBytesToWrite, &dwBytesWritten, NULL);
	return 1;
}
int main() {
	Camera cam;
	Model3D model;
	Marker marker(cam.getImageMemory());	
	setMouseCallback("screen window", onMouse, &marker);
	//PHCOMDB com;
	//ComDBOpen(com);
	//if (!setserialport()) {
	//	return 0;
	//}
	//wchar_t comname[] = "COM1";  
	//SerialPort com("COM1")
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
					cout << "�ο�����ͷ����׼�ɹ���\n��ʼʹ�òο��ܵ�����\n\n";
					continue;
				}
				else {
					cout << "�ο�����ͷ����׼ʧ�ܣ�\n\n";
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