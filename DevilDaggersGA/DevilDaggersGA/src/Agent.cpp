#include <fstream>
#include <sstream>
#include <iostream>
#include <exception>
#include "Agent.hpp"
#include "GeneticAlgorithm.hpp"
#include "InputSimulator.hpp"
#include "Time.hpp"
#include "Constants.hpp"
#include "Random.hpp"

float Agent::SCOUT_TIMER = 0.3f;

Agent::Agent() :
	genomeFilename{}, genome{},
	timeSurvived{ 0.f }, totalTimeSurvived{ 0.f }, scoutTimer{ 0.f },
	isScouting{ false }, scoutClockwise{ true }
{
}

Agent::Agent(std::string const& _genomeFilename) :
	genomeFilename{ _genomeFilename }, genome{},
	timeSurvived{ 0.f }, totalTimeSurvived{ 0.f }, scoutTimer{ 0.f },
	isScouting{ false }, scoutClockwise{ true }
{
	std::ifstream ifs(Constants::GENOME_PATH + genomeFilename + Constants::GENOME_EXT);
	if (!ifs) throw std::runtime_error("Failed to open data.txt");
	std::string line;
	while (std::getline(ifs, line))
	{
		std::stringstream ss(line);
		float gene;
		while (ss >> gene)
			genome.push_back(gene);
	}
	ifs.close();
}

Agent::Agent(std::vector<float> const& _genome) :
	genomeFilename{}, genome{ _genome },
	timeSurvived{ 0.f }, totalTimeSurvived{ 0.f }, scoutTimer{ 0.f },
	isScouting{ false }, scoutClockwise{ true }
{
}

void Agent::MoveFront(int milliseconds)
{
	InputSimulator::HoldWS('W', milliseconds);
}

void Agent::MoveBack(int milliseconds)
{
	InputSimulator::HoldWS('S', milliseconds);
}

void Agent::MoveLeft(int milliseconds)
{
	InputSimulator::HoldWS('A', milliseconds);
}

void Agent::MoveRight(int milliseconds)
{
	InputSimulator::HoldWS('D', milliseconds);
}

void Agent::LookUp(int dy, int steps)
{
	InputSimulator::MoveMouseUD(-dy, steps);
}

void Agent::LookDown(int dy, int steps)
{
	InputSimulator::MoveMouseUD(dy, steps);
}

void Agent::LookLeft(int dx, int steps)
{
	InputSimulator::MoveMouseLR(-dx, steps);
}

void Agent::LookRight(int dx, int steps)
{
	InputSimulator::MoveMouseLR(dx, steps);
}

void Agent::Shoot(cv::Point const& aimVector, int milliseconds)
{
	InputSimulator::MoveMouseLR(aimVector.x);
	InputSimulator::MoveMouseUD(aimVector.y);
	InputSimulator::HoldLMB(milliseconds);
}

void Agent::Scout()
{
	scoutTimer -= Time::dt;
	if (scoutTimer <= 0.f)
	{
		if (!isScouting)
			scoutClockwise = static_cast<bool>(Random::RandInt(0, 1));
		isScouting = true;
		int sign = scoutClockwise ? 1 : -1;
		InputSimulator::MoveMouseLR(sign * 500);
	}
}

void Agent::StopScout()
{
	isScouting = false;
	scoutTimer = SCOUT_TIMER;
}

void Agent::Jump()
{
	InputSimulator::KeyPress(' ');
}
