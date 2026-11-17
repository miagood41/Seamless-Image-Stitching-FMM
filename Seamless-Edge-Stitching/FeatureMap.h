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

	int otsuThreshold(const Mat& image, const Mat& imageMask, int maxLevel);

	FeatureMaps calculateFeatureMaps(Mat& leftImage, Mat& rightImage, Mat& imageMask);

	Mat calculateColorMap(Mat& leftImage, Mat& rightImage, Mat& imageMask);
	Mat calculateEdgeMap(Mat& leftImage, Mat& rightImage, Mat& imageMask);
	Mat calculateTextureMap();
	Mat calculateSaliencyMap();
	Mat createCostMap();

}