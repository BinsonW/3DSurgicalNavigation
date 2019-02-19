#pragma once
#include "pch.h"
#include <iostream>
#include <opencv2/viz.hpp>
using namespace std;
using namespace cv;
class Model3D
{
private:
	viz::Camera ProjectCamera;
	viz::WCloud tumor;
	viz::WCylinder cylinder;
public:
	viz::Viz3d ProjectWindow;
	Model3D();
	~Model3D();
	int run(Affine3d campose);
	int regist(Affine3d& scalpose,Affine3d& refpose);
	Mat Projectwindow;
};

