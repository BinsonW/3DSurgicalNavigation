#pragma once
#include "pch.h"
#include <iostream>
#include <opencv2/viz.hpp>
#include <fstream>
using namespace std;
using namespace cv;
class Model3D
{
private:
	viz::Camera ProjectCamera;
	viz::WCloud tumor;
	viz::WCylinder cylinder;
	viz::WSphere sphere;
	ifstream widgetposein;
	ofstream widgetposeout;
	//store pose if readed
	Affine3d pose;
public:
	bool hasposedata;
	viz::Viz3d ProjectWindow;
	Model3D();
	~Model3D();
	int run(Affine3d campose);
	int regist(Affine3d& scalpose,Affine3d& refpose);
	Mat Projectwindow;
};

