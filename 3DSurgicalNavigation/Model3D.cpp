#include "pch.h"
#include "Model3D.h"




Model3D::Model3D() :
	tumor(viz::readCloud("E:\\wubxprogram\\Debug\\Skull.stl"), viz::Color::green()),
	ProjectWindow("3D Project Window"),
	ProjectCamera(2.4188753857320721e+03*0.724609375, 2.4188753857320721e+03*0.724609375, 5.7164331421986378e+02*0.724609375, 5.5079334958877484e+02*0.724609375, Size(928, 742)),
	cylinder(Point3d(95, 70, 0), Point3d(95, 70, 100), 10, 30, viz::Color::green()),
	sphere(Point3d(95.5, 57.4, -141.91), 25, 50, viz::Color::green()),
	circlegrid3D(circlegrid,Size(240,240),Vec3d(95.5,90.5,0),Vec3d(0,0,1),Vec3d(0,-1,0))
{
	//initiate 3D project window
	ProjectWindow.showWidget("ptumor", circlegrid3D);
	ProjectWindow.spinOnce(1, true);
	ProjectWindow.setCamera(ProjectCamera);
	/*ProjectWindow.setRenderingProperty("ptumor", viz::LINE_WIDTH, viz::REPRESENTATION_SURFACE);
	Vec3d skullr(0.0, 0.0, 5.3977871437821381673096259207391);
	Vec3d skullt(75.0, 75.0, 0.0);
	Affine3d skullpose(skullr, skullt);
	ProjectWindow.setWidgetPose("ptumor", skullpose);
	ProjectWindow.setRenderingProperty("ptumor", viz::POINT_SIZE, 1.3);*/
	ProjectWindow.setBackgroundColor();
	ProjectWindow.spinOnce();
	ProjectWindow.setOffScreenRendering();
	// check if widgetpose has been stored
	widgetposein.open("widgetpose.dat", ios_base::in | ios_base::binary);
	if (widgetposein.is_open()) {
		cout << "已成功读取位置数据，请选择参考架追踪点\n\n";
		widgetposein.read((char *)&pose, sizeof pose);
		ProjectWindow.setWidgetPose("ptumor", pose);
		hasposedata = true;
	}
	else {
		cout << "未检测到位置数据，请选择参考架和头部追踪点\n\n";
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
	ProjectWindow.setWidgetPose("ptumor", pose);
	//save pose data
	//widgetposeout.open("widgetpose.dat", ios_base::out | ios_base::binary);
	//widgetposeout.write((char *)&pose, sizeof pose);
	widgetposeout << flush;
	cout << "已成功存储位置数据！\n\n";
	return 1;
}
