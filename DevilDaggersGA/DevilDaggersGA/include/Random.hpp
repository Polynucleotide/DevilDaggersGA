#ifndef RANDOM_HPP
#define RANDOM_HPP
#pragma once

#include <random>

class Random
{
	public:
		static int RandInt(int min, int max);
		static float RandFloat(float min, float max);

	private:
		static std::mt19937 gen;
};

#endif // !RANDOM_HPP
