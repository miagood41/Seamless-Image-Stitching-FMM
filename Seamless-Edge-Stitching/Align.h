#pragma once
#include <opencv2/core.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/opencv_modules.hpp>
#include <iostream>
#include <vector>
using namespace cv;
using std::vector;
using std::string;

namespace align {

	struct ImageFeatures
	{
		vector<DMatch> matches;
		vector<KeyPoint> keypoints1, keypoints2;

		Mat descriptor1, descriptor2;
	};
	// two sets of corners, two image placed in canvas space, translation matrix
	struct ImageCanvas
	{
		Mat warpedLeft;  // on canvas, warped
		Mat placedRight; // on canvas, unwarped
		Mat translation;
		vector<Point2f> warpedCorners; 
		vector<Point2f> rightCorners;
	};

	Mat estimateHomography(ImageFeatures keyFeatures);

	ImageFeatures detectFeatures(const Mat& img1, const Mat& img2);
	ImageCanvas warpImage(const Mat& img1, const Mat& img2, Mat homography);
	
	void alignImages(const Mat& img1, const Mat& img2);
	vector<Point2f> findCrossPoints();

}