#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP
#pragma once

#include <string>

namespace Constants
{
	inline constexpr int CAPTURE_FRAMERATE = 60;
	inline constexpr int I_YOLO_IMGSZ = 640;
	inline constexpr float F_YOLO_IMGSZ = 640.f;
	inline constexpr float SKULL1_CONFIDENCE_THRESHOLD = 0.3f;
	inline constexpr float SKULL2_CONFIDENCE_THRESHOLD = 0.4f;
	inline constexpr float NMS_THRESHOLD = 0.3f;
	inline constexpr float SCORE_SUBMITTED_THRESHOLD = 0.5f;
	inline constexpr float GAME_OVER_RED_THRESHOLD = 0.3f;
	inline constexpr float GAME_OVER_ROI_WIDTH_RATIO  = 500.f / 2560.f;
	inline constexpr float GAME_OVER_ROI_HEIGHT_RATIO = 100.f / 1494.f;

	inline constexpr char const* SKULLS_FRONT = "Skulls Front";
	inline constexpr char const* SKULLS_BACK  = "Skulls Back";
	inline constexpr char const* SKULLS_LEFT  = "Skulls Left";
	inline constexpr char const* SKULLS_RIGHT = "Skulls Right";
	inline constexpr char const* NO_SKULLS = "No Skull";
	inline constexpr char const* UP_ANGLE = "Up Angle";
	inline constexpr char const* IS_SCOUTING = "Is Scouting";
	inline constexpr char const* AVERAGE_DISTANCE = "Average Distance";

	inline constexpr char const* MOVE_FRONT = "Move Front";
	inline constexpr char const* MOVE_BACK = "Move Back";
	inline constexpr char const* MOVE_LEFT = "Move Left";
	inline constexpr char const* MOVE_RIGHT = "Move Right";
	inline constexpr char const* LOOK_UP= "Look Up";
	inline constexpr char const* LOOK_DOWN = "Look Down";
	inline constexpr char const* LOOK_LEFT = "Look Left";
	inline constexpr char const* LOOK_RIGHT = "Look Right";
	inline constexpr char const* SHOOT = "Shoot";
	inline constexpr char const* SCOUT = "Scout";
	inline constexpr char const* JUMP = "Jump";

	inline constexpr char const* GENOME_PATH = "genomes/";
	inline constexpr char const* GENOME_EXT = ".txt";
}

#endif // !CONSTANTS_HPP
