#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>

#include <ctime>
#include <random>

#include "BinaryThresholding.h"

int main(int argc, char ** argv) {
	if (argc < 2) {
		printf("Need a input file.\n");
		return -1;
	}

	cv::Mat inMat = cv::imread(argv[1]);
	cv::Mat gMat, bMat, show;
	cv::cvtColor(inMat, gMat, CV_RGB2GRAY);

	cv::resize(gMat, show, gMat.size() / 4);
	//cv::imshow("GrayImage", show);
	//cv::waitKey(0);

	//for (int i = 0; i < 25; i++)
	//{
	//	cv::threshold(gMat, bMat, i * 10, 255, cv::THRESH_BINARY);
	//	cv::imwrite("result.png", bMat);
	//	std::string thres = std::to_string(i);
	//	cv::imwrite("results\\" + thres + ".png", bMat);
	//
	//}

	double thres = cv::threshold(gMat, bMat, 68, 255, cv::THRESH_BINARY);
	//double thres = cv::threshold(gMat, bMat, 10, 255, cv::THRESH_OTSU);
	std::string outName = std::to_string(thres) + "result.png";
	cv::imwrite(outName, bMat);
}