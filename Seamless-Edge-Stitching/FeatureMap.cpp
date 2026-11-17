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
	// Focused on a range of grey levels rather than everything from black to white. 
	int otsuThreshold(const Mat& image, const Mat& imageMask, int maxLevel)
	{
		int histSize = 256;
		float lvlRange[] = { 0.0f, 256.0f };
		const float* ranges = { lvlRange };
		Mat histogram;
		calcHist(&image, 1, 0, imageMask, histogram, 1, &histSize, &ranges);

		double totalPixels = 0;
		double totalIntensity = 0;
		for (int level = 0; level <= maxLevel; level++)
		{
			double count = histogram.at<float>(level);
			totalPixels += count;
			totalIntensity += level * count;
		}

		// Background
		double bgPixels = 0;
		double bgIntensity = 0;
		double bestVariance = -1;
		int threshold = 0;

		for (int level = 0; level <= maxLevel; level++)
		{
			double count = histogram.at<float>(level);

			bgPixels += count;
			if (bgPixels == 0) { continue; } // background empty

			double fgPixels = totalPixels - bgPixels;
			if (fgPixels == 0) { break; } // background empty

			bgIntensity += level * count;

			double bgMean = bgIntensity / bgPixels;
			double fgMean = (totalIntensity - bgIntensity) / fgPixels;
			double diff   = bgMean - fgMean;

			double betweenVariance = bgPixels * fgPixels * diff * diff;

			if (betweenVariance > bestVariance)
			{
				bestVariance = betweenVariance;
				threshold = level + 1;
			}
		}
		return threshold;
	}
	
	FeatureMaps calculateFeatureMaps(Mat& leftImage, Mat& rightImage, Mat& imageMask)
	{
		FeatureMaps features;
		features.colorMap = calculateColorMap(leftImage, rightImage, imageMask);
		features.edgeMap = calculateEdgeMap(leftImage, rightImage, imageMask);
		// features.texMap = calculateTextureMap(leftImage, rightImage, imageMask);
		// features.saliMap = calculateSaliencyMap(leftImage, rightImage, imageMask);
		// features.costMap = createCostMap(
		return features;
	}

	Mat calculateColorMap(Mat& leftImage, Mat& rightImage, Mat& imageMask) 
	{
		// Compute the local mean color, using a 5x5 neighborhood, for each channel.
		// Convert the left and right images to 32 bit floating point values
		Mat leftMean, rightMean;
		boxFilter(leftImage,  leftMean,  CV_32F, Size(5, 5));
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

	Mat calculateEdgeMap(Mat& leftImage, Mat& rightImage, Mat& imageMask)
	{
		// Convert to grayscale
		Mat leftGray, rightGray;
		cvtColor(leftImage,  leftGray,  COLOR_BGR2GRAY);
		cvtColor(rightImage, rightGray, COLOR_BGR2GRAY);

		// Acyclic lateral inhibition: R = I - k * sum(w_ij * I_neighbors)
		int radius = 3;          // radius from neighbor pixels
		double spread = 1.5;     // distribution spread
		double strength = 0.8;   // total strength

		Mat gaussian = getGaussianKernel(2 * radius + 1, spread, CV_64F);
		std::cout << "gaussian: " << gaussian << std::endl;
		Mat weights = gaussian * gaussian.t();
		weights.at<double>(radius, radius) = 0.0;
		weights *= strength / sum(weights)[0];

		Mat kernel = -weights;
		kernel.at<double>(radius, radius) = 1.0;

		Mat leftEnhanced, rightEnhanced;
		filter2D(leftGray,  leftEnhanced,  CV_8U, kernel, Point(-1, -1), 0, BORDER_REFLECT);
		filter2D(rightGray, rightEnhanced, CV_8U, kernel, Point(-1, -1), 0, BORDER_REFLECT);

		// Gausian smoothing
		Mat leftSmoothed, rightSmoothed;
		GaussianBlur(leftEnhanced,  leftSmoothed,  Size(3, 3), 1);
		GaussianBlur(rightEnhanced, rightSmoothed, Size(3, 3), 1);

		int leftHighThreshold  = otsuThreshold(leftSmoothed,  imageMask, 255);
		int rightHighThreshold = otsuThreshold(rightSmoothed, imageMask, 255);
		int leftLowThreshold   = otsuThreshold(leftSmoothed,  imageMask, leftHighThreshold);
		int rightLowThreshold  = otsuThreshold(rightSmoothed, imageMask, rightHighThreshold);

		Mat leftEdges, rightEdges;
		Canny(leftSmoothed,  leftEdges,  leftLowThreshold,  leftHighThreshold,  3, true);
		Canny(rightSmoothed, rightEdges, rightLowThreshold, rightHighThreshold, 3, true);

		// Merge the left and right edge maps per pixel by taking the maximum
		Mat edgeMap;
		max(leftEdges, rightEdges, edgeMap);
		imshow("Edge Map", edgeMap);
		waitKey(0);
		return edgeMap;
	}

	Mat calculateTextureMap()
	{
		Mat texMap;
		return texMap;
	}

	Mat calculateSaliencyMap()
	{
		Mat saliMap;
		return saliMap;
	}

	Mat createCostMap()
	{
		// Weighted map, can use 32bit maybe 64bit if needed
		// Created from combining 4 feature maps
		Mat costMap = Mat::zeros(4, 4, CV_32F); // replace 4 with actual size
		return costMap;
	}



}