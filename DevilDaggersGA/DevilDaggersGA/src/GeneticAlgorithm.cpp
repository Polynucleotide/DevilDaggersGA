#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <any>
#include <utility>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iostream>
#include "GeneticAlgorithm.hpp"
#include "InputSimulator.hpp"
#include "Detector.hpp"
#include "Constants.hpp"
#include "Time.hpp"
#include "SkullType.hpp"
#include "Random.hpp"

GeneticAlgorithm::GeneticAlgorithm(std::shared_ptr<DXCam::DXCamera> _camera) :
	camera{ _camera }, RESET_TIMER{ 4.f }, resetTimer{ RESET_TIMER },
	MIN_WEIGHT{ -5.f }, MAX_WEIGHT{ 5.f }
{
	using namespace Constants;

	// State and actions
	static std::vector<std::string> states{ SKULLS_FRONT, SKULLS_BACK, SKULLS_LEFT, SKULLS_RIGHT, NO_SKULLS, IS_SCOUTING, UP_ANGLE, AVERAGE_DISTANCE };
	static std::vector<std::string> actions{ MOVE_FRONT, MOVE_BACK, MOVE_LEFT, MOVE_RIGHT, LOOK_UP, LOOK_DOWN, LOOK_LEFT, LOOK_RIGHT, SHOOT, SCOUT, JUMP };

	// Set state indices
	NUM_STATES = static_cast<int>(states.size());
	for (int i = 0; i < NUM_STATES; ++i)
		stateToIndexMap[states[i]] = i;

	// Set action indices
	NUM_ACTIONS = static_cast<int>(actions.size());
	for (int i = 0; i < NUM_ACTIONS; ++i)
		actionToIndexMap[actions[i]] = i;
}

bool GeneticAlgorithm::RunGeneration(Agent& agent)
{
	// Get frame
	cv::Mat frameBGRA = camera->get_latest_frame();
	cv::Mat frameBGR;
	cv::cvtColor(frameBGRA, frameBGR, cv::COLOR_BGRA2BGR);

	// Check if agent is dead
	// Make sure we can only press 'R' once every RESET_TIMER seconds
	resetTimer -= Time::dt;
	if (resetTimer <= 0.f && Detector::IsAgentDead(frameBGR))
	{
		InputSimulator::KeyPress('R');
		InputSimulator::Reset();
		resetTimer = RESET_TIMER;
		agent.totalTimeSurvived += agent.timeSurvived;
		agent.timeSurvived = 0.f;
		return true;
	}

	// Merge skull detections
	// Sort them by area
	skulls = Detector::DetectSkulls(frameBGR, SkullType::SKULL1);
	std::vector<Skull> skull2s = Detector::DetectSkulls(frameBGR, SkullType::SKULL2);
	for (Skull& skull : skull2s)
	{
		skulls.push_back(std::move(skull));
	}
	std::sort(std::begin(skulls), std::end(skulls), [](const Skull& a, const Skull& b) {
		if (a.skullType != b.skullType)
			return a.skullType < b.skullType; // Skull 1 first
		return a.area > b.area; // Largest first
	});

	std::vector<float> input = GetInputVector(agent);
	std::vector<float> output = GetOutputVector(agent.genome, input);

	// Save these in the stack so we prevent multiple lookups
	using namespace Constants;
	int moveFront = actionToIndexMap[MOVE_FRONT];
	int moveBack = actionToIndexMap[MOVE_BACK];
	int moveLeft = actionToIndexMap[MOVE_LEFT];
	int moveRight = actionToIndexMap[MOVE_RIGHT];
	int lookUp = actionToIndexMap[LOOK_UP];
	int lookDown = actionToIndexMap[LOOK_DOWN];
	int shoot = actionToIndexMap[SHOOT];
	int scout = actionToIndexMap[SCOUT];
	int jump = actionToIndexMap[JUMP];

	// Shoot
	if (output[shoot] > 0.f)
	{
		cv::Point center = cv::Point(frameBGR.cols / 2, frameBGR.rows / 2);
		cv::Point aimVector;
		if (skulls.size() <= 0)
		{
			aimVector = cv::Point(0, 0);
		}
		else
		{
			aimVector = skulls[0].pos - center;
		}
		agent.Shoot(aimVector);
	}

	// Look parallel to the ground if staring on the sky or floor
	else if (output[lookUp] > 2.f)
	{
		float dy = std::abs(InputSimulator::GetUpAngle());
		agent.LookUp(static_cast<int>(dy * InputSimulator::MAX_UP_ANGLE));
	}
	else if (output[lookDown] > 2.f)
	{
		float dy = std::abs(InputSimulator::GetUpAngle());
		agent.LookDown(static_cast<int>(dy * InputSimulator::MAX_UP_ANGLE));
	}

	// Scout
	if (output[scout] > 0.f)
	{
		agent.Scout(static_cast<int>(output[scout]));
	}
	else
	{
		agent.StopScout();
	}

	// Movement
	if (output[moveFront] > output[moveBack])
	{
		if (output[moveFront] > 5.f)
		{
			agent.MoveFront(500);
		}
	}
	else
	{
		if (output[moveBack] > 5.f)
		{
			agent.MoveBack(500);
		}
	}

	if (output[moveLeft] > output[moveRight])
	{
		if (output[moveLeft] > 5.f)
		{
			agent.MoveLeft(500);
		}
	}
	else
	{
		if (output[moveRight] > 5.f)
		{
			agent.MoveRight(500);
		}
	}

	if (output[jump] > 5.f)
	{
		agent.Jump();
	}

	agent.timeSurvived += Time::dt;

	return false;
}

void GeneticAlgorithm::MutateAgent(Agent& agent)
{
	for (float& gene : agent.genome)
	{
		if (Random::RandFloat(0.0f, 1.0f) <= 0.1f) // 10% chance per gene
		{
			gene += Random::RandFloat(-0.5f, 0.5f);
			gene = std::clamp(gene, MIN_WEIGHT, MAX_WEIGHT);
		}
	}
	SaveGenome(agent);
}

void GeneticAlgorithm::SaveGenome(Agent& agent)
{
	std::string filename = agent.genomeFilename;
	if (filename.empty())
	{
		filename = "random";
	}

	std::ofstream ofs(Constants::GENOME_PATH + filename + Constants::GENOME_EXT);
	if (!ofs.is_open())
	{
		std::cerr << "Failed to open file: " << filename << "\n";
		return;
	}

	// 6 dp
	ofs << std::fixed << std::setprecision(6);
	for (int i = 0; i < NUM_ACTIONS; ++i)
	{
		for (int j = 0; j < NUM_STATES; ++j)
		{
			ofs << agent.genome[i * NUM_STATES + j] << ' ';
		}
		ofs << '\n';
	}
	ofs.close();
}

std::vector<float> GeneticAlgorithm::GenerateGenome()
{
	std::vector<float> genome;
	int size = NUM_ACTIONS * NUM_STATES;
	for (int i = 0; i < size; ++i)
		genome.push_back(Random::RandFloat(MIN_WEIGHT, MAX_WEIGHT));

#if 0
	// Print genome
	for (int i = 0; i < NUM_ACTIONS; ++i)
	{
		for (int j = 0; j < NUM_STATES; ++j)
		{
			std::cout << genome[i * NUM_STATES + j] << ' ';
		}
		std::cout << '\n';
	}
	std::cout << '\n';
	std::cout << '\n';
#endif

	return genome;
}

std::vector<float> GeneticAlgorithm::GetInputVector(Agent const& agent)
{
	using namespace Constants;

	std::vector<float> input(NUM_STATES, 0.f);

	float totalArea = 0.f;
	for (Skull const& skull : skulls)
	{
		// Labelling the regions of skulls
		if (skull.pos.x < Detector::GetLeftBoundaryX())
		{
			++input[stateToIndexMap[SKULLS_LEFT]];
		}
		else if (skull.pos.x < Detector::GetRightBoundaryX())
		{
			++input[stateToIndexMap[SKULLS_FRONT]];
		}
		else
		{
			++input[stateToIndexMap[SKULLS_RIGHT]];
		}

		totalArea += skull.area;
	}

	// Up Angle (If looking too far up or down, snap back to head level)
	input[stateToIndexMap[UP_ANGLE]] = InputSimulator::GetUpAngle();
	input[stateToIndexMap[IS_SCOUTING]] = static_cast<float>(agent.isScouting);
	input[stateToIndexMap[NO_SKULLS]] = (skulls.size() <= 0);
	input[stateToIndexMap[AVERAGE_DISTANCE]] = totalArea / Detector::GetRegionArea() - 0.2f;

	return input;
}

std::vector<float> GeneticAlgorithm::GetOutputVector(std::vector<float> const& genome, std::vector<float> const& input)
{
	std::vector<float> output;

	for (int i = 0; i < NUM_ACTIONS; ++i)
	{
		float score = 0.f;
		for (int j = 0; j < NUM_STATES; ++j)
		{
			score += genome[i * NUM_STATES + j] * input[j];
		}
		output.push_back(score * Random::RandFloat(0.8f, 1.f));
	}

	return output;
}
