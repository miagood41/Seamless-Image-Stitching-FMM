#include "Seam.h"

namespace seam
{
	vector<Point> extractSeam(Mat& arrivalTime, Point start, Point end)
	{
		return vector<Point> { (0, 0) };
	}
	
	vector<Point> findSeam(Mat& leftImage, Mat& rightImage, Mat& imageMask, Point start, Point end)
	{
		return vector<Point> { (0, 0) };
	}
	
	QualityIndicators evalSeam(Mat& leftImage, Mat& rightImage, vector<Point> seam)
	{
		QualityIndicators quality;
		return quality;
	}
	
	int truncateSeam(QualityIndicators& indicators, int window)
	{
		return 0;
	}
	
	Mat remainingImage(Mat& imageMask, Point truncation, Point start, Point end)
	{
		Mat img;
		return img;
	}

	vector<Point> optimizeSeam(Mat& leftImage, Mat& rightImage, Mat& imageMask, 
		vector<Point>& seam, Point start, Point end)
	{
		return vector<Point> { (0, 0) };
	}
}