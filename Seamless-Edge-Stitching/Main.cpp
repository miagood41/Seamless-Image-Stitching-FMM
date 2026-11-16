#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/opencv_modules.hpp>
#include <iostream>
#include <string>
#include <vector>

#include "Align.h"
#include "FeatureMap.h"
using namespace cv;
using std::vector;
using std::string;

const int SCALE = 4;

namespace tests {
	void testAlign(const string& leftPath, const string& rightPath);
}

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

	align::ImageOverlap alignImg;
	alignImg = align::alignImages(img1, img2);

	features::FeatureMaps costMap;
	features::calculateFeatureMaps(alignImg.leftImage, alignImg.rightImage, alignImg.imageMask);

	return stitchedImg;
}


void buildPotentialField() {}

void extractSeam() {}

void optimizeSeam() {}

void blendImages() {}

void outputStitched(Mat& img)
{
	//imwrite("output.jpg", img);
}

////////////////////////////////////////  MAIN   ////////////////////////////////////////
int main() 
{
	// Load two images, assumed to be overlapping with large parallax
	string filepath1 = "001.jpg";
	string filepath2 = "002.jpg";
	Mat img1, img2, output;
	if (!loadImages(filepath1, filepath2, img1, img2))
	{
		return -1;
	}

	output = stitchPair(img1, img2);
	
	// Test Alignment step
	//tests::testAlign(filepath1, filepath2);

	// Output result
	//outputStitched(output);
	//imshow("Stitched Image", output);

	return 0;
}

