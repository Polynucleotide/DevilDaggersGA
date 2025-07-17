#ifndef DETECTOR_HPP
#define DETECTOR_HPP
#pragma once

#include <vector>
#include <dxcam/dxcam.h>
#include <opencv2/opencv.hpp>

enum class SkullType
{
	SKULL1 = 0,
	SKULL2
};

class Detector
{
	public:
		static void Init();
		static void LoadDetectorModels();
		static void AllocateSkullData();
		static std::vector<cv::Point> DetectSkulls(cv::Mat& frame, SkullType skullType);
		static DXCam::Region ComputeGameRegion(HWND hwnd);
		static bool IsAgentDead(cv::Mat const& frame);

	private:
		static cv::dnn::Net net1;
		static cv::dnn::Net net2;
		//static std::vector<SkullDetections>
};

#endif // !DETECTOR_HPP
