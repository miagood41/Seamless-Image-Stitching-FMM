#pragma once
#include <opencv2/core.hpp>
#include <vector>
#include <limits>
#include <queue>
#include <functional>
using namespace cv;
using std::vector;

constexpr float INF = std::numeric_limits<float>::infinity();

namespace seam {

	enum PixelStatus {
		Far = 0,
		Narrow = 1,
		Alive = 2,
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

	class PotentialField
	{
	public:
		PotentialField(Mat& costMap, Mat& imageMask);
		Mat buildPotentialField(Point start);

	private:
		Mat costMap;
		Mat arrivalTime;
		Mat statusMap;

		std::priority_queue< QueuePixel, vector<QueuePixel>, std::greater<QueuePixel> > narrowBand;

		float getKnownTime(int x, int y) const;
		float calculateArrivalTime(Point pixel) const;
	};

}