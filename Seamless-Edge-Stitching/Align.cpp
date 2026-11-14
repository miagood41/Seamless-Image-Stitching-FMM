

#include "Align.h"

namespace align
{
	// Matching and feature detection using SIFT
	ImageFeatures detectFeatures(const Mat& img1, const Mat& img2)
	{
		ImageFeatures keyFeatures;

		// Initialize SIFT detector:  SIFT::create()
		Ptr<Feature2D> detector = SIFT::create();

		// Detect keypoints and compute descriptors
		detector->detectAndCompute(img1, noArray(), keyFeatures.keypoints1, keyFeatures.descriptor1);
		detector->detectAndCompute(img2, noArray(), keyFeatures.keypoints2, keyFeatures.descriptor2);
		
		// Match descriptors with Brute-Forch Matcher
		Ptr<BFMatcher> matcher = BFMatcher::create(detector->defaultNorm(), true);
		matcher->match(keyFeatures.descriptor1, keyFeatures.descriptor2, keyFeatures.matches);
		
		// return a Mat that stitchPair can send to estimateHomography
		return keyFeatures;

	}

	Mat estimateHomography(ImageFeatures keyFeatures)
	{
		Mat homography = findHomography(keyFeatures.descriptor1, keyFeatures.descriptor2, RANSAC);
		return homography;
	}

	vector<Point2f> findCorners(const Mat& img)
	{
		vector<Point2f> corners = {
			Point2f(0.0, 0.0),
			Point2f(img.cols, 0.0),
			Point2f(img.cols, img.rows),
			Point2f(0.0, img.rows)
		};

		return corners;
	}

	ImageCanvas warpImage(const Mat& img1, const Mat& img2, Mat homography)
	{
		ImageCanvas canvasData;

		canvasData.placedRight;
		canvasData.warpedLeft;

		canvasData.rightCorners;
		canvasData.warpedCorners;
		canvasData.translation;

		vector<Point2f> leftImageCorners;

		leftImageCorners  = findCorners(img1);
		canvasData.rightCorners = findCorners(img2);

		// Warp the left image's corners into the right image's frame
		perspectiveTransform(leftImageCorners, canvasData.warpedCorners, homography);

		vector<Point2f> combinedCorners;
		combinedCorners.reserve(canvasData.warpedCorners.size() + canvasData.rightCorners.size() );
		combinedCorners.insert( combinedCorners.end(), canvasData.warpedCorners.begin(), canvasData.warpedCorners.begin() );
		combinedCorners.insert( combinedCorners.end(), canvasData.rightCorners.begin(),  canvasData.rightCorners.begin() );

		// Calculate the canvas size
		Rect canvasBoundary = boundingRect(combinedCorners);
		Size canvasSize = canvasBoundary.size();

		// Determine the Translation matrix to shift images into canvas space
		canvasData.translation = Mat::eye(3, 3, CV_64F);
		canvasData.translation.at<double>(0, 2) = -canvasBoundary.x;
		canvasData.translation.at<double>(1, 2) = -canvasBoundary.y;

		// Warp and/or shift both images into postive canvas space to be canvas size images
		warpPerspective(img1, canvasData.warpedLeft, canvasData.translation * homography, canvasBoundary.size());
		warpPerspective(img2, canvasData.placedRight, homography, canvasSize);

		// Update warped and left image's corners
		perspectiveTransform(canvasData.warpedCorners, canvasData.warpedCorners, canvasData.translation);
		perspectiveTransform(canvasData.rightCorners, canvasData.rightCorners, canvasData.translation);

		return canvasData;
	}

	vector<Point2f> findOverlapPoly(ImageCanvas canvasData)
	{
		// Finds the polygon created by the two overlapping sets of corner points
		vector<Point2f> overlapPoly;
		if (isContourConvex(canvasData.warpedCorners) && isContourConvex(canvasData.rightCorners))
		{
			intersectConvexConvex(canvasData.warpedCorners, canvasData.rightCorners, overlapPoly);
		}
		
		// The overlap points are pixels, which require integer values
		vector<Point> overlapPoints;
		for (int i = 0; i < overlapPoly.size(); i++)
		{
			int x = cvRound(overlapPoly[i].x);
			int y = cvRound(overlapPoly[i].y);
			overlapPoints.push_back(Point(x, y));
		}

		// Draws a white polygon (onto ?)
		Mat mask = Mat::zeros(canvasData.warpedLeft.size(), CV_8UC1);
		fillConvexPoly(mask, overlapPoints, Scalar(255));

		// Copy
		Mat leftOverlap, rightOverlap;
		canvasData.warpedLeft.copyTo(leftOverlap, mask);
		canvasData.placedRight.copyTo(rightOverlap, mask);

		// Crop each canvas size image to just the region of interest rectangle
		Rect ROI = boundingRect(mask);
		leftOverlap = leftOverlap(ROI).clone();
		rightOverlap = rightOverlap(ROI).clone();
		mask = mask(ROI).clone();
	}

	// returns the 4 points that make up the overlap region
	vector<Point2f> findCrossPoints(vector<Point2f> overlapPoly)
	{
		vector<Point2f> crossPoints;

		return crossPoints;
	}

	// Coordinator
	void alignImage(const Mat& img1, const Mat& img2)
	{
		ImageFeatures keyFeatures;
		Mat homography;
		ImageCanvas canvasData;
		vector<Point2f> crossPoints;

		keyFeatures = detectFeatures(img1, img2);
		homography  = estimateHomography(keyFeatures);
		canvasData  = warpImage(img1, img2, homography);
		findOverlapPoly();
		crossPoints = findCrossPoints(vector<Point2f> overlap);
	}
}