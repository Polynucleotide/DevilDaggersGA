#ifndef SKULL_HPP
#define SKULL_HPP
#pragma once

#include <opencv2/opencv.hpp>
#include "SkullType.hpp"

struct Skull
{
	Skull();
	Skull(cv::Point const& pos, int area, SkullType skullType);
	cv::Point pos;
	int area;
	SkullType skullType;
};

#endif // !SKULL_HPP
