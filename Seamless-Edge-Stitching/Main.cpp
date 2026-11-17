#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/opencv_modules.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <queue>
#include <functional>

#include "Align.h"
#include "FeatureMap.h"
using namespace cv;
using std::vector;
using std::string;

const int SCALE = 4;
constexpr float INF = std::numeric_limits<float>::infinity();

enum PixelStatus { 
	Far     = 0,
	Narrow  = 1,
	Alive   = 2,
	Blocked = 3
};

struct QueuePixel
{
	float time;
	Point pixel;

	bool operator>(const QueuePixel& other) const
	{
		return time > other.time;
	}
};

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

float solveArrivalTime()
{
}

// Take the pixel with the smallest arrival time out of the queue. 
// If it’s already Alive, skip it
// Mark it Alive.
// For each of its neighbours that isn’t Alive or Blocked
//	T1 = the smaller arrival time of that neighbor’s left and right neighbours, 
//	T2 = the smaller of its up and down neighbors.
//	Calculate its arrival time (Eq. 14), use the cost map value 
//	as 1 / V. If either T1 or T2 is still infinity, the formula becomes the  
//	one not infinity plus the cost map value.
//	If the result of that is smaller than the neighbor’s current
//  arrival time, hold that result, mark the neighbor as Narrow,
//  and add it to the queue.
//  Repeat until the queue is empty

void buildPotentialField(Mat& costMap, Mat& imageMask, Point seamStart)
{
	Mat arrivalTime(costMap.size(), CV_32F, Scalar(INF));
	Mat statusMap(costMap.size(), CV_8UC1, Scalar(Far));
	statusMap.setTo(Blocked, imageMask == 0);

	// A min-heap priority queue
	std::priority_queue<QueuePixel, vector<QueuePixel>, std::greater<QueuePixel>> narrowBand;

	// Wave starts at the seam start at Time 0
	arrivalTime.at<float>(seamStart) = 0.0f;
	statusMap.at<uchar>(seamStart) = Narrow;
	narrowBand.push({ 0.0, seamStart });

	int dx[4] = { 1, -1, 0, 0 }; // right, left neighbors
	int dy[4] = { 0, 0, 1, -1 }; // top, bottom neighbors

	while (!narrowBand.empty())
	{
		QueuePixel smallest = narrowBand.top();
		narrowBand.pop();
		Point p = smallest.pixel;

		if (statusMap.at<uchar>(p) == Alive) { continue; }

		statusMap.at<uchar>(p) = Alive;

		for (int i = 0; i < 4; i++)
		{
			Point neighbor(p.x + dx[i], p.y + dy[i]);

			if (neighbor.x < 0 || neighbor.y < 0 || neighbor.x >= costMap.cols || neighbor.y >= costMap.rows)
			{
				continue;
			}

			uchar status = statusMap.at<uchar>(neighbor);
			if (status == Alive || status == Blocked) { continue; }

			float newTime = solveArrivalTime(arrivalTime, statusMap, costMap, neighbor);
			if (newTime < arrivalTime.at<float>(neighbor))
			{
				arrivalTime.at<float>(neighbor) = newTime;
				statusMap.at<uchar>(n) = Narrow;
				narrowBand.push({ newTime, neighbor });				
			}

		}
	}
}

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

