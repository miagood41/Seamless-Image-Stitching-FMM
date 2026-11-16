#include "Align.h"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <stdexcept>
#include <string>
using std::cout;
using std::endl;
using std::string;
using std::runtime_error;

void testAlign(const string& leftPath, const string& rightPath)
{
	Mat leftImage = imread(leftPath);
	Mat rightImage = imread(rightPath);
	if (leftImage.empty() || rightImage.empty())
	{
		cout << "Could not load images." << endl;
		return;
	}

	try
	{
		align::ImageOverlap overlap = align::alignImages(leftImage, rightImage);
		cout << "ROI: " << overlap.ROI << endl;
		cout << "Seam start: " << overlap.seamStart << ", seam end: " << overlap.seamEnd << endl;

		Mat view = overlap.leftImage.clone();
		circle(view, overlap.seamStart, 6, Scalar(0, 255, 255), 2);
		circle(view, overlap.seamEnd, 6, Scalar(0, 255, 255), 2);
		imshow("Left overlap", view);
		imshow("Right overlap", overlap.rightImage);
		imshow("Mask", overlap.imageMask);
		waitKey(0);
	}
	catch (const runtime_error& e)
	{
		cout << "Stopped: " << e.what() << endl;
	}
}

void testFeatures(Mat& leftImage, Mat& rightImage, Mat& imageMask) 
{
	Mat diffMap;
	absdiff(leftImage, rightImage, diffMap);
	imshow("Difference Map", diffMap);
	imshow("Left Image", leftImage);
	imshow("Right Image", rightImage);
	waitKey(0);
}