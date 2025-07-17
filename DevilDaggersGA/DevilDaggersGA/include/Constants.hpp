#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP
#pragma once

namespace Constants
{
	inline constexpr int CAPTURE_FRAMERATE = 60;
	inline constexpr int I_YOLO_IMGSZ = 640;
	inline constexpr float F_YOLO_IMGSZ = 640.f;
	inline constexpr float CONFIDENCE_THRESHOLD = 0.4f;
	inline constexpr float NMS_THRESHOLD = 0.3f;
	inline constexpr float SCORE_SUBMITTED_THRESHOLD = 0.5f;
	inline constexpr float GAME_OVER_RED_THRESHOLD = 0.8f;
	inline constexpr float GAME_OVER_ROI_WIDTH_RATIO  = 500.f / 2560.f;
	inline constexpr float GAME_OVER_ROI_HEIGHT_RATIO = 100.f / 1494.f;
}

#endif // !CONSTANTS_HPP
