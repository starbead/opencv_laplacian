#include "pch.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;
using namespace std;

int main()
{	// declare two original images and mask images
	Mat apple = imread("apple.jpg");
	Mat orange = imread("orange.jpg");
	Mat temp_a = imread("mask_a.jpg");
	Mat temp_o = imread("mask_o.jpg");
	//convert
	apple.convertTo(apple, CV_32F, 1 / 255.f);
	orange.convertTo(orange, CV_32F, 1 / 255.f);
	temp_a.convertTo(temp_a, CV_32F, 1 / 255.f);
	temp_o.convertTo(temp_o, CV_32F, 1 / 255.f);
	//make roi
	Mat mask_a = apple + temp_a;
	Mat mask_o = orange + temp_o;
	
	//laplacian pyramid
	vector<Mat> a;
	a.push_back(apple);

	for (int i = 0; i < 6; i++) {
		Mat dst;
		pyrDown(a[i], dst);
		Mat scaled;
		pyrUp(dst, scaled, a[i].size());
		a[i] = a[i] - scaled;
		a.push_back(dst);
	}

	vector<Mat> o;
	o.push_back(orange);

	for (int i = 0; i < 6; i++) {
		Mat dst;
		pyrDown(o[i], dst);
		Mat scaled;
		pyrUp(dst, scaled, o[i].size());
		o[i] = o[i] - scaled;
		o.push_back(dst);
	}

	//gaussian pyramid
	vector<Mat> m_a;
	m_a.push_back(mask_a);

	for (int i = 0; i < 6; i++) {
		Mat dst;
		pyrDown(m_a[i], dst);
		Mat scaled;
		pyrUp(dst, scaled, m_a[i].size());
		m_a[i] = scaled;
		m_a.push_back(dst);
	}

	vector<Mat> m_o;
	m_o.push_back(mask_o);

	for (int i = 0; i < 6; i++) {
		Mat dst;
		pyrDown(m_o[i], dst);
		Mat scaled;
		pyrUp(dst, scaled, m_o[i].size());
		m_o[i] = scaled;
		m_o.push_back(dst);
	}

	//add two things
	vector<Mat> r_a;
	vector<Mat> r_o;

	for (int i = 0; i < 6; i++) {
		Mat temp_a;
		Mat temp_o;
		temp_a = 0.7*a[i] + 0.3*m_a[i];
		temp_o = 0.7*o[i] + 0.3*m_o[i];
		r_a.push_back(temp_a);
		r_o.push_back(temp_o);
	}
	
	for (int i = 4; i >= 0; i--) {
		Mat scaled_a;
		Mat scaled_o;
		pyrUp(r_a[i + 1], scaled_a, r_a[i].size());
		pyrUp(r_o[i + 1], scaled_o, r_o[i].size());
		r_a[i] = 0.7*r_a[i] + 0.3*scaled_a;
		r_o[i] = 0.7*r_o[i] + 0.3*scaled_o;
	}

	Mat result;

	result = 0.5*r_a[0] + 0.5*r_o[0];

	imshow("result", result*2-0.3);

	waitKey();
}
