#pragma once
#include <opencv2/core.hpp>
#include <vector>
using namespace cv;
using std::vector;

// Alignment: 
namespace align {

	struct ImageFeatures
	{
		vector<DMatch> matches;
		vector<KeyPoint> keypoints1;
		vector<KeyPoint> keypoints2;
	};
	
	struct ImageCanvas
	{
		Mat leftImage;   // warpedImage
		Mat rightImage;  // placedImage
		Mat translation;
		vector<Point2f> leftCorners; // warpedCorners
		vector<Point2f> rightCorners;
	};

	struct ImageOverlap
	{
		Rect ROI;

		Mat imageMask;
		Mat leftImage;  // warpedImage
		Mat rightImage;
		
		Point seamStart;
		Point seamEnd;
	};

	ImageFeatures detectFeatures(const Mat& img1, const Mat& img2);
	Mat estimateHomography(const ImageFeatures& features);
	ImageCanvas warpImage(const Mat& img1, const Mat& img2, Mat& homography);
	ImageOverlap findOverlap(const ImageCanvas& canvasData);
	ImageOverlap alignImages(const Mat& img1, const Mat& img2);
}