#include "pch.h"
#include "Model3D.h"




Model3D::Model3D() :
	tumor(viz::readCloud("tum&inc.stl"), viz::Color::green()),
	ProjectWindow("3D Project Window"),
	ProjectCamera(2.419128343195550e+03*0.724609375, 2.418354228183528e+03*0.724609375, 5.764543077579378e+02*0.724609375, 5.526957147621707e+02*0.724609375, Size(928, 742)),
	cylinder(Point3d(95, 70, 0), Point3d(95, 70, 100), 10, 30, viz::Color::green()),
	sphere(Point3d(95.5, 57.4, -141.91), 25, 50, viz::Color::green()),
	circlegrid3D(circlegrid,Size(240,240),Vec3d(100,95,0),Vec3d(0,0,-1),Vec3d(0,-1,0))
{
	//initiate 3D project window
	ProjectWindow.showWidget("ptumor", tumor);
	ProjectWindow.spinOnce(1, true);
	ProjectWindow.setCamera(ProjectCamera);

	viz::WCoordinateSystem a(100);
	//ProjectWindow.showWidget("cor", a);
	//ProjectWindow.setRenderingProperty("ptumor", viz::POINT_SIZE, 1.3);
	ProjectWindow.setBackgroundColor();
	ProjectWindow.spinOnce();
	// check if widgetpose has been stored
	widgetposein.open("widgetpose.dat", ios_base::in | ios_base::binary);
	if (widgetposein.is_open()) {
		cout << "已成功读取位置数据\n";
		widgetposein.read((char *)&pose, sizeof pose);
		ProjectWindow.setWidgetPose("ptumor", pose);
		hasposedata = true;
	}
	else {
		cout << "未检测到位置数据\n";
	}
}


Model3D::~Model3D()
{
}

int Model3D::run(Affine3d campose)
{
	//set project window
	ProjectWindow.setViewerPose(campose.inv());
	ProjectWindow.spinOnce(1, true);
	Projectwindow = ProjectWindow.getScreenshot();
	return 1;
}

int Model3D::regist(Affine3d & scalpose, Affine3d & refpose)
{
	pose = refpose.inv()*scalpose;
	Matx33d r = pose.rotation();
	Matx33d y (1, 0, 0, 0, 1, 0, 0, 0, -1);
	Matx33d r_ = r * y;
	Vec3d t = pose.translation();
	Affine3d p(r_, t);
	cout << "注册位置 "<<pose.translation()<<endl;
	ProjectWindow.setWidgetPose("ptumor", p);
	//ProjectWindow.setWidgetPose("cor", p);
	//save pose data
	//widgetposeout.open("widgetpose.dat", ios_base::out | ios_base::binary);
	//widgetposeout.write((char *)&pose, sizeof pose);
	//widgetposeout << flush;
	//cout << "已成功存储位置数据！\n\n";

	return 1;
}
