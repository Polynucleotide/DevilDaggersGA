#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP
#pragma once

namespace Constants
{
	static constexpr int CAPTURE_FRAMERATE = 60;
	static constexpr int I_YOLO_IMGSZ = 640;
	static constexpr float F_YOLO_IMGSZ = 640.f;
	static constexpr float CONFIDENCE_THRESHOLD = 0.4f;
	static constexpr float NMS_THRESHOLD = 0.3f;
	static constexpr double GAME_OVER_THRESHOLD = 0.3;
}

#endif // !CONSTANTS_HPP
