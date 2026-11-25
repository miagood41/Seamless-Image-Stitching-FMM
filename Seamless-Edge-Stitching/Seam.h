#pragma once
#include <opencv2/core.hpp>
#include <vector>
using namespace cv;
using std::vector;

namespace seam {

	struct QualityIndicators
	{
		vector<float> sdsim;
		vector<float> mae;
		float meanSDSIM;
		float meanMAE;
	};

	vector<Point> extractSeam(Mat& arrivalTime, Point start, Point end);
	vector<Point> findSeam(Mat& leftImage, Mat& rightImage, Mat& imageMask, Point start, Point end);

	QualityIndicators evalSeam(Mat& leftImage, Mat& rightImage, vector<Point> seam);

	int truncateSeam(QualityIndicators& indicators, int window);

	Mat remainingImage(Mat& imageMask, Point truncation, Point start, Point end);

	vector<Point> optimizeSeam(
		Mat& leftImage, 
		Mat& rightImage, 
		Mat& imageMask, 
		vector<Point>& seam, 
		Point start, 
		Point end
	);

}