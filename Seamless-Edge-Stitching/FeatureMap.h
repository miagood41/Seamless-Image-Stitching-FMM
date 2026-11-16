#pragma once
#include <opencv2/core.hpp>
#include <vector>
using namespace cv;
using std::vector;

 
namespace features {

	struct FeatureMaps
	{
		Mat colorMap;
		Mat edgeMap;
		Mat textureMap;
		Mat saliencyMap;
	};

	void calculateFeatureMaps(Mat& leftImage, Mat& rightImage, Mat& imageMask); // called from stitchPair in main

	Mat calculateColorMap(Mat& leftImage, Mat& rightImage, Mat& imageMask);

	void calculateEdgeMap();

	void calculateTextureMap();

	void calculateSaliencyMap();

	void createCostMap();

}