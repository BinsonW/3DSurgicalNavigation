// Image3Dtest.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include <opencv2/viz.hpp>
#include<opencv2/opencv.hpp>
using namespace cv;
int main()
{
	viz::WCloud a(viz::readCloud("tum&inc.stl"), viz::Color::green());
	viz::Viz3d ProjectWindow;
	viz::WCoordinateSystem c(10);

	viz::WSphere s1(Point3d(2.6852, -27.8480, -30.5402), 10);
	viz::WSphere s2(Point3d(-30.2534, 15.3029, -32.1072), 7.5);
	viz::WSphere s3(Point3d(26.0762, 49.7373, -32.6188), 5);
	ProjectWindow.showWidget("s1", s1);
	ProjectWindow.showWidget("s2", s2);
	ProjectWindow.showWidget("s3", s3);

	ProjectWindow.showWidget("c", c);
	ProjectWindow.showWidget("ptumor", a);
	ProjectWindow.spin();

}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
