#include "pch.h"
#include "Model3D.h"




Model3D::Model3D() :
	tumor(viz::readCloud("E:\\wubxprogram\\Debug\\Skull.stl"), viz::Color::green()),
	ProjectWindow("3D Project Window"),
	ProjectCamera(2.4512e+03, 2.4526e+03, 611.5857, 379.1207, Size(1048, 644)),
	cylinder(Point3d(95, 70, 0), Point3d(95, 70, 100), 10, 30, viz::Color::green())
{
	//initiate 3D project window
	ProjectWindow.showWidget("ptumor", cylinder);
	ProjectWindow.spinOnce(1, true);
	ProjectWindow.setCamera(ProjectCamera);
	ProjectWindow.setRenderingProperty("ptumor", viz::LINE_WIDTH, viz::REPRESENTATION_SURFACE);
	Vec3d skullr(0.0, 0.0, 5.3977871437821381673096259207391);
	Vec3d skullt(75.0, 75.0, 0.0);
	Affine3d skullpose(skullr, skullt);
	ProjectWindow.setWidgetPose("ptumor", skullpose);
	ProjectWindow.setRenderingProperty("ptumor", viz::POINT_SIZE, 1.3);
	ProjectWindow.setBackgroundColor();
	// check if widgetpose has been stored
	widgetposein.open("widgetpose.dat", ios_base::in | ios_base::binary);
	if (widgetposein.is_open()) {
		cout << "�ѳɹ���ȡλ�����ݣ���ѡ��ο���׷�ٵ�\n\n";
		widgetposein.read((char *)&pose, sizeof pose);
		ProjectWindow.setWidgetPose("ptumor", pose);
		hasposedata = true;
	}
	else {
		cout << "δ��⵽λ�����ݣ���ѡ��ο��ܺ�ͷ��׷�ٵ�\n\n";
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
	widgetposeout.open("widgetpose.dat", ios_base::out | ios_base::binary);
	widgetposeout.write((char *)&pose, sizeof pose);
	widgetposeout.close();
	cout << "�ѳɹ��洢λ�����ݣ�\n\n";
	return 1;
}
