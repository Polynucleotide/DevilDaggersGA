#ifndef SKULL_DETECTIONS_HPP
#define SKULL_DETECTIONS_HPP
#pragma once

#include <vector>
#include <opencv2/opencv.hpp>

struct SkullDetections
{
	std::vector<cv::Point> positions;
	std::vector<float> areas;
};

#endif // !SKULL_DETECTIONS_HPP
