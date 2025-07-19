#ifndef DETECTOR_HPP
#define DETECTOR_HPP
#pragma once

#include <vector>
#include <dxcam/dxcam.h>
#include <opencv2/opencv.hpp>
#include "Skull.hpp"
#include "SkullType.hpp"

class Detector
{
	public:
		static void Init();
		static void LoadDetectorModels();
		static void AllocateSkullData();
		static std::vector<Skull> DetectSkulls(cv::Mat& frame, SkullType skullType);
		static DXCam::Region ComputeGameRegion(HWND hwnd);
		static bool IsAgentDead(cv::Mat const& frame);
		static void ComputeXBoundaries(DXCam::Region const& region);
		static int GetRegionArea() { return regionArea; };
		static int GetLeftBoundaryX() { return leftBoundaryX; };
		static int GetMiddleBoundaryX() { return middleBoundaryX; };
		static int GetRightBoundaryX() { return rightBoundaryX; };

	private:
		static cv::dnn::Net net1;
		static cv::dnn::Net net2;
		static int regionArea;
		static int leftBoundaryX;
		static int middleBoundaryX;
		static int rightBoundaryX;
};

#endif // !DETECTOR_HPP
