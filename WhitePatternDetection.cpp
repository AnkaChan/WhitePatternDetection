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

#include "ComponentMarker.h"
#include "ColorGenerator.h"

#define THRES_TYPE 0

class Demo {};


int main(int argc, char ** argv) {
	if (argc < 2) {
		printf("Need a input file.\n");
		return -1;
	}

	cv::Mat inMat = cv::imread(argv[1]);
	cv::Mat gMat, bMat;
	cv::cvtColor(inMat, gMat, CV_RGB2GRAY);

	cv::imshow("GrayImage", gMat);
	cv::waitKey(0);

	cv::Mat imgFiltered;
	cv::bilateralFilter(gMat, imgFiltered, 10, 50, 75);

	cv::imshow("After Filter", imgFiltered);
	cv::waitKey(0);

	cv::threshold(imgFiltered, bMat, 105, 255, 0);

	cv::imshow("BinaryImage", bMat);
	cv::waitKey(0);

	//Erosion
	int erosionType = cv::MORPH_RECT;
	//int erosionType = cv::MORPH_CROSS;
	int erosionSize = 2;
	cv::Mat element = getStructuringElement(erosionType,
		cv::Size(2 * erosionSize + 1, 2 * erosionSize + 1),
		cv::Point(erosionSize, erosionSize));

	// Apply the erosion operation
	erode(bMat, bMat, element);
	cv::imshow("ErosionDemo", bMat);
	cv::waitKey(0);

	ComponentMarker<uint8_t> cMarker;
	cMarker.setInputBinaryImage(bMat);
	cMarker.markComponent();

	DepthComponents & depthComponents = cMarker.getDepthComponents();

	ColorGenerator cGen;
	std::vector<cv::Scalar> colors = cGen.GetColors(depthComponents.size());

	printf("Number of components:%d\n", depthComponents.size());
	int i = 0;
	for (DepthComponent & depthComponent : depthComponents)
	{
		//if (depthComponent.size() < 250) {
		//	continue;
		//}
		for (int j = 0; j < depthComponent.size(); ++j) {
			inMat.at<cv::Vec3b>(depthComponent[j]) = cv::Vec3b(colors[i][0], colors[i][1], colors[i][2]);
		}
		++i;
	}

	cv::imshow("Components", inMat);
	cv::waitKey(0);

	
}