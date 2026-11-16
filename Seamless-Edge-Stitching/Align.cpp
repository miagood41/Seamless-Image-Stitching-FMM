#include "Align.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/features2d.hpp>
#include <iostream>
#include <stdexcept>
#include <string>

namespace align
{
	// Matching and feature detection using SIFT
	ImageFeatures detectFeatures(const Mat& leftImage, const Mat& rightImage)
	{
		ImageFeatures keyFeatures;
		Mat descriptor1, descriptor2;

		Ptr<Feature2D> detector = SIFT::create();
		detector->detectAndCompute(leftImage, noArray(), keyFeatures.keypoints1, descriptor1);
		detector->detectAndCompute(rightImage, noArray(), keyFeatures.keypoints2, descriptor2);
		
		Ptr<BFMatcher> matcher = BFMatcher::create(detector->defaultNorm(), true);
		matcher->match(descriptor1, descriptor2, keyFeatures.matches);
		
		return keyFeatures;
	}

	Mat estimateHomography(const ImageFeatures& keyFeatures)
	{
		vector<Point2f> leftPoints;
		vector<Point2f> rightPoints;
	
		// Check all the matches, add the keypoint1 position to the leftPoint list
		// and the position of the keypoint2 match to the rightPoint list.
		for (int i = 0; i < keyFeatures.matches.size(); i++)
		{
			DMatch match = keyFeatures.matches[i];
			leftPoints.push_back(keyFeatures.keypoints1[match.queryIdx].pt);
			rightPoints.push_back(keyFeatures.keypoints2[match.trainIdx].pt);
		}

		Mat homography = findHomography(leftPoints, rightPoints, RANSAC);
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

	ImageCanvas warpImage(const Mat& leftImage, const Mat& rightImage, Mat& homography)
	{
		ImageCanvas canvasData;

		canvasData.leftCorners = findCorners(leftImage);
		canvasData.rightCorners  = findCorners(rightImage);

		// Warp the left image's corners into the right image's frame
		perspectiveTransform(canvasData.leftCorners, canvasData.leftCorners, homography);

		vector<Point2f> combinedCorners;
		combinedCorners.reserve(canvasData.leftCorners.size() + canvasData.rightCorners.size() );
		combinedCorners.insert( combinedCorners.end(), canvasData.leftCorners.begin(), canvasData.leftCorners.end() );
		combinedCorners.insert( combinedCorners.end(), canvasData.rightCorners.begin(),  canvasData.rightCorners.end() );

		// Calculates the smallest rectangle that contains all 8 corner points
		Rect canvasBoundary = boundingRect(combinedCorners);
		Size canvasSize = canvasBoundary.size();

		// Determine the Translation matrix to shift images into canvas space
		canvasData.translation = Mat::eye(3, 3, CV_64F);
		canvasData.translation.at<double>(0, 2) = -canvasBoundary.x;
		canvasData.translation.at<double>(1, 2) = -canvasBoundary.y;

		// Warp and/or shift both images into postive canvas space to be canvas size images
		warpPerspective(leftImage, canvasData.leftImage, canvasData.translation * homography, canvasSize);
		warpPerspective(rightImage, canvasData.rightImage, canvasData.translation, canvasSize);

		// Update warped and right image's corners
		perspectiveTransform(canvasData.leftCorners, canvasData.leftCorners, canvasData.translation);
		perspectiveTransform(canvasData.rightCorners, canvasData.rightCorners, canvasData.translation);

		return canvasData;
	}

	// Returns the points where the overlap's verticies cross the two image's boundaries.
	vector<Point2f> findCrossPoints(vector<Point2f> overlap, vector<Point2f> leftCorners, vector<Point2f> rightCorners)
	{
		vector<Point2f> crossPoints;

		// Check each vertex of the overlap quad
		for (int i = 0; i < overlap.size(); i++)
		{
			bool isCorner = false;

			// Compare this vertex against all 8 corners of the images
			// If its less than half a pixel away from a corner then its a corner, not a crosspoint.
			for (int j = 0; j < 4; j++)
			{
				if (norm(overlap[i] - leftCorners[j]) < 0.5 || norm(overlap[i] - rightCorners[j]) < 0.5)
				{
					isCorner = true;
				}
			}

			// Add the vertices that aren't corners
			if (!isCorner) {
				crossPoints.push_back(overlap[i]);
			}
		}
		return crossPoints;
	}

	// overlapVertices are the corners of the overlap shape as decimals
	// pixelVertices are the corners of the overlap shape as pixels
	ImageOverlap findOverlap(const ImageCanvas& canvasData)
	{
		// Finds the shape created by the two overlapping sets of corner points
		vector<Point2f> overlapVertices;
		if (isContourConvex(canvasData.leftCorners) && isContourConvex(canvasData.rightCorners))
		{
			float overlapArea = intersectConvexConvex(canvasData.leftCorners, canvasData.rightCorners, overlapVertices);
			if (overlapArea <= 0)
			{
				throw std::runtime_error("Images do not overlap.");
			}
			std::cout << "Overlap coordinates: " << Mat(overlapVertices) << std::endl;
		} 
		else
		{
			throw std::runtime_error("Input images must be convex.");
		}
		
		// The overlap points are pixels, which require integer values so
		// round to the nearest integer.
		vector<Point> pixelVertices;
		for (int i = 0; i < overlapVertices.size(); i++)
		{
			int x = cvRound(overlapVertices[i].x);
			int y = cvRound(overlapVertices[i].y);
			pixelVertices.push_back(Point(x, y));
		}

		vector<Point2f> crossPoints;
		crossPoints = findCrossPoints(overlapVertices, canvasData.leftCorners, canvasData.rightCorners);
		if (crossPoints.size() != 2)
		{
			throw std::runtime_error("Exactly 2 cross points are allowed, " + std::to_string(crossPoints.size()) + " were found.");
		}

		// Draws a white shape where the boundaries overlap, on a new canvas sized Mat 
		Mat regionMask = Mat::zeros(canvasData.leftImage.size(), CV_8UC1);
		fillConvexPoly(regionMask, pixelVertices, Scalar(255));

		// Copy the overlap region of each image to a new canvas size Mat
		Mat leftImageCrop, rightImageCrop;
		canvasData.leftImage.copyTo(leftImageCrop, regionMask);
		canvasData.rightImage.copyTo(rightImageCrop, regionMask);

		// Crop to just the region of interest
		Rect ROI = boundingRect(regionMask);
		leftImageCrop  = leftImageCrop(ROI).clone();
		rightImageCrop = rightImageCrop(ROI).clone();
		regionMask = regionMask(ROI).clone();

		ImageOverlap result;
		result.ROI         = ROI;
		result.imageMask   = regionMask;
		result.leftImage   = leftImageCrop;
		result.rightImage  = rightImageCrop;

		Point2f origin = Point2f(ROI.tl());
		result.seamStart   = crossPoints[0] - origin;
		result.seamEnd     = crossPoints[1] - origin;

		return result;
	}

	// Coordinator
	ImageOverlap alignImages(const Mat& leftImage, const Mat& rightImage)
	{
		ImageFeatures features;
		Mat homography;
		ImageCanvas canvasData;
		vector<Point2f> crossPoints;
		ImageOverlap croppedImages;

		features      = detectFeatures(leftImage, rightImage);
		homography    = estimateHomography(features);
		canvasData    = warpImage(leftImage, rightImage, homography);
		croppedImages = findOverlap(canvasData);

		return croppedImages;
	}
}