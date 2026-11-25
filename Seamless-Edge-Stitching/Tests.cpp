#include "Align.h"
#include "FeatureMap.h"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/photo.hpp>
#include <opencv2/saliency.hpp>
#include <iostream>
#include <stdexcept>
#include <string>
using std::cout;
using std::endl;
using std::string;
using std::runtime_error;

namespace tests
{
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

	// Builds the color map, then shows and saves it
	void testColorMap(align::ImageOverlap& overlap)
	{
		Mat colorMap = features::calculateColorMap(overlap.leftImage, overlap.rightImage, overlap.imageMask);
		imwrite("OutputImages/color_map.png", colorMap);
		imshow("Color map", colorMap);
	}

	// Builds the edge map, then shows and saves it
	void testEdgeMap(align::ImageOverlap& overlap)
	{
		Mat edgeMap = features::calculateEdgeMap(overlap.leftImage, overlap.rightImage, overlap.imageMask);
		imwrite("OutputImages/edge_map.png", edgeMap);
		imshow("Edge map", edgeMap);
	}

	// Builds the texture map, then shows and saves it
	void testTextureMap(align::ImageOverlap& overlap)
	{
	}

	// Builds the saliency map, then shows and saves it
	void testSaliencyMap(align::ImageOverlap& overlap)
	{
	}

	void testFeatures(const string& leftPath, const string& rightPath)
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
			align::ImageOverlap img = align::alignImages(leftImage, rightImage);

			Mat colorMap = features::calculateColorMap(img.leftImage, img.rightImage, img.imageMask);
			Mat edgeMap  = features::calculateEdgeMap(img.leftImage, img.rightImage, img.imageMask);

			imwrite("OutputImages/left_overlap.png", img.leftImage);
			imwrite("OutputImages/right_overlap.png", img.rightImage);

			testColorMap(img);
			testEdgeMap(img);
			testTextureMap(img);
			testSaliencyMap(img);
			waitKey(0);
		}
		catch (const runtime_error& e)
		{
			cout << "Stopped: " << e.what() << endl;
		}
	}

}
