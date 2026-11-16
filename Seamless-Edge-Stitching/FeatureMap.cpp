#include "FeatureMap.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>
#include <iostream>
#include <stdexcept>
#include <string>

namespace features
{

	void calculateFeatureMaps(Mat& leftImage, Mat& rightImage, Mat& imageMask) 
	{
		calculateColorMap(leftImage, rightImage, imageMask);
	}

	Mat calculateColorMap(Mat& leftImage, Mat& rightImage, Mat& imageMask) 
	{
		// Compute the local mean color, using a 5x5 neighborhood, for each channel.
		// Convert the left and right images to 32 bit floating point values
		Mat leftMean, rightMean;
		boxFilter(leftImage, leftMean, CV_32F, Size(5, 5));
		boxFilter(rightImage, rightMean, CV_32F, Size(5, 5));

		Mat channelDiff;
		absdiff(leftMean, rightMean, channelDiff);

		Mat sqrdDiff;
		multiply(channelDiff, channelDiff, sqrdDiff);

		vector<Mat> sqrdChannels;
		split(sqrdDiff, sqrdChannels);

		Mat sumOfSquares = sqrdChannels[0] + sqrdChannels[1] + sqrdChannels[2];

		Mat colorDist;
		sqrt(sumOfSquares, colorDist);

		// Scale only the pixels in the mask area to 0-255
		Mat colorMap;
		normalize(colorDist, colorMap, 0, 255, NORM_MINMAX, CV_8U, imageMask);

		return colorMap;
	}

	void calculateEdgeMap() {}

	void calculateTextureMap() {}

	void calculateSaliencyMap() {}

	void createCostMap()
	{
		// Weighted map, can use 32bit maybe 64bit if needed
		// Created from combining 4 feature maps
		Mat costMap = Mat::zeros(4, 4, CV_32F); // replace 4 with actual size
	}



}