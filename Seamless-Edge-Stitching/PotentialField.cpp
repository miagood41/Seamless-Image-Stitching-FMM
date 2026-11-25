#include "Seam.h"
#include "PotentialField.h"

namespace seam
{
	PotentialField::PotentialField(Mat& costMap, Mat& imageMask)
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
	Mat PotentialField::buildPotentialField(Point seamStart)
	{
		Mat potentialField; // temp

		Mat arrivalTime(costMap.size(), CV_32F, Scalar(INF));
		Mat statusMap(costMap.size(), CV_8UC1, Scalar(Far));
		//statusMap.setTo(Blocked, imageMask == 0);

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

				float newTime = calculateArrivalTime(neighbor);
				if (newTime < arrivalTime.at<float>(neighbor))
				{
					arrivalTime.at<float>(neighbor) = newTime;
					statusMap.at<uchar>(neighbor) = Narrow;
					narrowBand.push({ newTime, neighbor });
				}

			}
		}
		return potentialField;
	}

	float PotentialField::getKnownTime(int x, int y) const
	{
		return 0.0f;
	}
	
	float PotentialField::calculateArrivalTime(Point neighbor) const
	{
		return 0.0f;
	}
}