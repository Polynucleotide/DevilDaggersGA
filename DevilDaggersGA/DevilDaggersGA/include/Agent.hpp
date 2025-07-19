#ifndef AGENT_HPP
#define AGENT_HPP
#pragma once

#include <string>
#include <vector>
#include <map>
#include <thread>
#include <opencv2/opencv.hpp>

class Agent
{
	public:
		Agent();
		Agent(std::string const& genomeFile);
		Agent(std::vector<float> const& genome);
		void MoveFront(int milliseconds = 500);
		void MoveBack(int milliseconds = 500);
		void MoveLeft(int milliseconds = 500);
		void MoveRight(int milliseconds = 500);
		void LookUp(int dy, int steps = 20);
		void LookDown(int dy, int steps = 20);
		void LookLeft(int dx, int steps = 20);
		void LookRight(int dx, int steps = 20);
		void Shoot(cv::Point const& aimVector, int milliseconds = 1000);
		void Scout();
		void StopScout();
		void Jump();
		std::string genomeFilename;
		std::vector<float> genome;
		float timeSurvived;
		float totalTimeSurvived;
		float scoutTimer;
		bool isScouting;
		bool scoutClockwise;
		static float SCOUT_TIMER;
};

#endif // !AGENT_HPP
