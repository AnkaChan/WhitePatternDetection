#pragma once
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"

#include <deque>
#include <vector>
#include <list>

typedef std::vector<cv::Point> DepthComponent;
typedef std::list<DepthComponent> DepthComponents;


template<typename T = uint8_t>
class ComponentMarker {
public:
	void setInputBinaryImage(cv::Mat & inImg) {
		img = inImg;
		pixelComponentId = cv::Mat(img.size(), CV_32S, cv::Scalar(-1));
	}

	enum SmallComponentDetermination
	{
		MinSize,
		RatioOfTotalPts
	};

	enum ComponentColor {
		Black,
		White
	};

	void setSmallComponentDetermination(SmallComponentDetermination newDetermination) {
		determination = newDetermination;
	};

	void setComponentColor(ComponentColor color) {
		componetColor = color;
	}

	void componentRemovement() {
		markComponent();
		removerSmallComponent();
	}

	void removerSmallComponent() {
		cv::Mat & img = pDepth->getDepth();
		for (DepthComponent & depthComponent : depthComponents)
		{
			if (isSmallComponent(depthComponent.size())) {
				for (int j = 0; j < depthComponent.size(); ++j)
				{
					img.at<T>(depthComponent[j]) = 0;
				}
			}
		}
	}

	void markComponent() {
		neighborPoints.clear();
		depthComponents.clear();

		int nRows = img.rows;
		int nCols = img.cols;

		int currentComponentId = 0;
		for (int i = 0; i < nRows; ++i)
		{
			T * pRow = img.ptr<T>(i);
			int32_t * pComponentIdRow = pixelComponentId.ptr<int32_t>(i);
			for (int j = 0; j < nCols; ++j)
			{
				//We do not need to consider the component of given color
				if (!isComponentColor(*pRow))
				{
					++pComponentIdRow;
					++pRow;
					continue;
				}
				//Already in a component
				if (*pComponentIdRow >= 0)
				{
					++pComponentIdRow;
					++pRow;
					continue;
				}
				//set as a component seed 
				neighborPoints.push_back(cv::Point(j, i));
				depthComponents.push_back(std::move(DepthComponent()));
				DepthComponent & depthComponent = depthComponents.back();
				//search component
				while (!neighborPoints.empty())
				{
					cv::Point & p = neighborPoints.front();
					pixelComponentId.at<int32_t>(p) = currentComponentId;
					depthComponent.push_back(p);
					int depthVal = (int)img.at<T>(p);

					for (int d = 1; d <= connectionDistance; d++)
					{
						cv::Point px_p = p + cv::Point(0, d);
						cv::Point px_m = p + cv::Point(0, -d);
						cv::Point py_p = p + cv::Point(d, 0);
						cv::Point py_m = p + cv::Point(-d, 0);

						//int val_px_p = (int)img.at<T>(px_p) - depthVal;
						if (px_p.x < nCols
							&& px_p.y < nRows
							&& isComponentColor(img.at<T>(px_p))
							&& pixelComponentId.at<int32_t>(px_p) == -1)
						{
							pixelComponentId.at<int32_t>(px_p) = -2;
							neighborPoints.push_back(px_p);
						}

						//T val_px_m = img.at<T>(px_m) - depthVal;
						if (px_m.x >= 0
							&& px_m.y >= 0
							&& isComponentColor(img.at<T>(px_m))
							&& pixelComponentId.at<int32_t>(px_m) == -1)
						{
							pixelComponentId.at<int32_t>(px_m) = -2;
							neighborPoints.push_back(px_m);
						}

						//T val_py_p = img.at<T>(py_p) - depthVal;
						if (py_p.x < nCols
							&& py_p.y < nRows
							&& isComponentColor(img.at<T>(py_p))
							&& pixelComponentId.at<int32_t>(py_p) == -1)
						{
							pixelComponentId.at<int32_t>(py_p) = -2;
							neighborPoints.push_back(py_p);
						}

						//T val_py_m = img.at<T>(py_m) - depthVal;
						if (py_m.x >= 0
							&& py_m.y >= 0
							&& isComponentColor(img.at<T>(py_m))
							&& pixelComponentId.at<int32_t>(py_m) == -1)
						{
							pixelComponentId.at<int32_t>(py_m) = -2;
							neighborPoints.push_back(py_m);
						}
					}

					neighborPoints.pop_front();
				}

				++currentComponentId;
				++pComponentIdRow;
				++pRow;
			}
		}

		std::vector<int> numPtsInComponents(depthComponents.size());
		numTotalPts = 0;
		int i = 0;
		for (DepthComponent & depthComponent : depthComponents)
		{
			numPtsInComponents[i] = depthComponent.size();
			numTotalPts += depthComponent.size();
			++i;
		}

		std::sort(numPtsInComponents.begin(), numPtsInComponents.end());
		numMaxComponentPts = numPtsInComponents.back();
	}

	void setMaxNeiDepthDis(float newMaxNeiDepthDis) {
		maxNeiDepthDis = newMaxNeiDepthDis;
	}

	void setMinComponentSize(int newMinComponentSize) {
		minComponentSize = newMinComponentSize;
	}

	void setMinComponentRatio(float ratio) {
		minComponentRatio = ratio;
	}

	void setConnectionDistance(int newConnectionDistance) {
		connectionDistance = newConnectionDistance;
	}

	bool isSmallComponent(int numComponentPts) {
		switch (determination)
		{
		case MinSize:
			if (numComponentPts > minComponentSize)
			{
				return false;
			}
			else {
				return true;
			}
			break;
		case RatioOfTotalPts:
			if (numComponentPts > numTotalPts * minComponentRatio)
			{
				return false;
			}
			else {
				return true;
			}
			break;
		default:
			break;
		}
	}

	DepthComponents & getDepthComponents() { return depthComponents; }
private:
	bool isComponentColor(T color) {
		if (componetColor == White) {
			return (color != 0);
		}
		else
		{
			return (color == 0);
		}
	}

	cv::Mat img;
	std::deque<cv::Point> neighborPoints;
	cv::Mat pixelComponentId;
	DepthComponents depthComponents;
	int minComponentSize = 400;
	float minComponentRatio = 0.1;
	int numTotalPts = 0;
	int numMaxComponentPts;

	SmallComponentDetermination determination = MinSize;
	ComponentColor componetColor = White;

	int connectionDistance = 1;
};