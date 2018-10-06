#pragma once
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


class ColorGenerator {
public:
	ColorGenerator() : rng(12985) {};
	cv::Scalar HSV2RGB(const float h, const float s, const float v) {
		const int h_i = static_cast<int>(h * 6);
		const float f = h * 6 - h_i;
		const float p = v * (1 - s);
		const float q = v * (1 - f * s);
		const float t = v * (1 - (1 - f) * s);
		float r, g, b;
		switch (h_i) {
		case 0:
			r = v; g = t; b = p;
			break;
		case 1:
			r = q; g = v; b = p;
			break;
		case 2:
			r = p; g = v; b = t;
			break;
		case 3:
			r = p; g = q; b = v;
			break;
		case 4:
			r = t; g = p; b = v;
			break;
		case 5:
			r = v; g = p; b = q;
			break;
		default:
			r = 1; g = 1; b = 1;
			break;
		}
		return cv::Scalar(r * 255, g * 255, b * 255);
	}
	std::vector<cv::Scalar> GetColors(const int n) {
		std::vector<cv::Scalar> colors;
		
		const float golden_ratio_conjugate = 0.618033988749895;
		const float s = 0.8;
		for (int i = 0; i < n; ++i) {
			float v = rng.uniform(0.3f, 1.f);
			const float h = std::fmod(rng.uniform(0.f, 1.f) + golden_ratio_conjugate, 1.f);
			colors.push_back(HSV2RGB(h, s, v));
		}
		return colors;
	}

	void setSeed(int seed) {
		rng = cv::RNG(seed);
	}
private:
	cv::RNG rng;
};
