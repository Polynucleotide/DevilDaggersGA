#ifndef GENETIC_ALGORITHM_HPP
#define GENETIC_ALGORITHM_HPP
#pragma once

#include <string>
#include <memory>
#include <vector>
#include <functional>
#include <unordered_map>
#include <any>
#include <dxcam/dxcam.h>
#include "Agent.hpp"
#include "Skull.hpp"

class GeneticAlgorithm
{
	public:
		GeneticAlgorithm(std::shared_ptr<DXCam::DXCamera> camera);
		bool RunGeneration(Agent& agent);
		void MutateAgent(Agent& agent);
		void SaveGenome(Agent& agent);
		int GetNumStates() const { return NUM_STATES; };
		int GetNumActions() const { return NUM_ACTIONS; };
		std::vector<float> GenerateGenome();

	private:
		std::vector<float> GetInputVector(Agent const& agent);
		std::vector<float> GetOutputVector(std::vector<float> const& genome, std::vector<float> const& input);
		std::unordered_map<std::string, int> stateToIndexMap;
		std::unordered_map<std::string, int> actionToIndexMap;
		std::vector<Skull> skulls;
		std::shared_ptr<DXCam::DXCamera> camera;
		int NUM_STATES;
		int NUM_ACTIONS;
		float const RESET_TIMER;
		float resetTimer;
};

#endif // !GENETIC_ALGORITHM_HPP
