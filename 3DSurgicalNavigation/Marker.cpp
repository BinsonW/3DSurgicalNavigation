#pragma once
#include "pch.h"
#include "Frame.h"
Frame::marker::marker() {

	params.blobColor = 255;
	detector = SimpleBlobDetector::create(params);
}
Frame::marker::~marker() {}
bool Frame::marker::sortp()
{
	//sort x
	sort(p2Dlast.begin(), p2Dlast.end(), Frame::marker::compx);
	//sort y
	auto ptr = p2Dlast.begin();
	while (p2Dlast.end() - 1 != (ptr + 1)) {
		sort(ptr, ptr + 2, Frame::marker::compy);
		ptr = ptr + 2;
	}
	sort(ptr, ptr + 2, Frame::marker::compy);
	return true;
}
bool Frame::marker::compx(KeyPoint i, KeyPoint j)
{
	if (i.pt.x < j.pt.x) return true;
	else return false;
}
bool Frame::marker::compy(KeyPoint i, KeyPoint j)
{
	if (i.pt.y < j.pt.y) return true;
	else return false;
}
bool Frame::marker::iseqsize()
{
	//最大差值
	float max = 0;
	float temp = 0;
	for (int i = 0; i < p2Dlast.size(); i++) {
		for (int j = i + 1; j < p2Dlast.size(); j++) {
			temp = fabs(p2Dlast[i].size - p2Dlast[j].size);
			max = max > temp ? max : temp;
		}
	}
	//均值
	float mean = 0;
	for (auto i : p2Dlast) {
		mean += i.size;
	}
	mean = mean / marnum;
	if (max < 0.2*mean) return true;
	else return false;
}
bool Frame::marker::caldist()
{
	float t=0,dt=0;
	//calculate min distance
	for (int i = 0; i < p2Dlast.size(); i++) {
		for (int j = i + 1; j < p2Dlast.size(); j++) {
			dt = t;
			t = fabs(p2Dlast[i].pt.x - p2Dlast[j].pt.x) + fabs(p2Dlast[i].pt.y - p2Dlast[j].pt.y);
			if (dt == 0) dt = t;
			dt = dt < t ? dt : t;
		}
	}
	distance = dt*0.5;
	return true;
}
bool Frame::marker::calarea()
{
	float area = 0;
	size = 0;
	for (KeyPoint i : p2Dlast) {
		size += i.size;
	}
	size = size / marnum;
	area = 3.1416*(size / 2)*(size / 2);
	params.minArea = area * 0.5;
	params.maxArea = area * 4;
	return true;
}
bool Frame::marker::calmean()
{

	if (meanimgnum == p2Dtotal.size()) {
		p2Dtotal.erase(p2Dtotal.begin());
		p2Dtotal.push_back(p2Dlast);
		float x = 0;
		float y = 0;
		float size = 0;
		//初始化p2dmean
		p2Dmean = p2Dlast;
		//第几个点
		for (size_t i = 0; i < marnum; i++)
		{
			p2Dmean[i].pt.x = 0;
			p2Dmean[i].pt.y = 0;
			//第几张图
			for (size_t j = 0; j < meanimgnum; j++)
			{
				p2Dmean[i].pt.x += p2Dtotal[j][i].pt.x;
				p2Dmean[i].pt.y += p2Dtotal[j][i].pt.y;
			}
			p2Dmean[i].pt.x = p2Dmean[i].pt.x / meanimgnum;
			p2Dmean[i].pt.y = p2Dmean[i].pt.y / meanimgnum;
		}
	}
	else {
		p2Dtotal.push_back(p2Dlast);
	}
	//cout << p2Dtotal.size();
	return 1;
}