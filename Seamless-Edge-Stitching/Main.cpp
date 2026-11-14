#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/opencv_modules.hpp>
#include <iostream>
#include <string>
#include <vector>

#include "Align.h"
using namespace cv;
using std::vector;
using std::string;

const int SCALE = 4;


/*
 * @brief Loads images from the working directory and converts them to grayscale
 *
 * @param img1  The first image.
 * @param img2  The second image.
 * @return      True if both images are loaded, false otherwise.
 * @pre         Both images exist in the working directory.
 * @post        If true, img1 and img2 hold grayscale images.
 */
bool loadImages(string& file1, string& file2, Mat& img1, Mat& img2)
{
	img1 = imread(file1, IMREAD_COLOR);
	img2 = imread(file2, IMREAD_COLOR);

	if (img1.empty() || img2.empty())
	{
		std::cout << "Error: An image could not be read or is empty."
			<< std::endl;
		return false;
	}

	return true;
}

// Contains method calls for the full sequence of events
// Takes two images, returns one back
Mat stitchPair(const Mat& img1, const Mat& img2) 
{
	Mat stitchedImg;

	align::alignImages(img1, img2);

	return stitchedImg;
}


void extractImageOverlap()
{
	vector<Point> overlapPoints;
	for (int i = 0; i < overlap)

	Mat mask = zeros()
	// Create an overlap mask, this is the area where the two image overlap, is white Scalar(255).
	fillConvexPoly(mask, poly, Scalar(255));
}

void calculateFeatureMaps() {}

void calculateColorMap() {}

void calculateEdgeMap() {}

void calculateTextureMap() {}

void calculateSaliencyMap() {}

void createCostMap()
{
	// Weighted map, can use 32bit maybe 64bit if needed
	// Created from combining 4 feature maps
	Mat costMap = Mat::zeros(4, 4, CV_32F); // replace 4 with actual size
}

void buildPotentialField() {}

void extractSeam() {}

void optimizeSeam() {}

void blendImages() {}

void outputStitched(Mat& img)
{
	imwrite("output.jpg", img);
}


////////////////////////////////////////  MAIN   ////////////////////////////////////////
int main() 
{
	// Load two images, assumed to be overlapping with large parallax
	string filepath1 = "bonsai.jpeg";
	string filepath2 = "bonsai.jpeg";
	Mat img1, img2, output;
	if (!loadImages(filepath1, filepath2, img1, img2))
	{
		return -1;
	}

	output = stitchPair(img1, img2);
	outputStitched(output);
	imshow("Stitched Image", output);

	return 0;
}

