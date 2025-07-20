#include <iostream>
#include <windows.h>
#include <dxcam/dxcam.h>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <algorithm>
#include "Utilities.hpp"
#include "Detector.hpp"
#include "Constants.hpp"
#include "InputSimulator.hpp"
#include "Agent.hpp"
#include "GeneticAlgorithm.hpp"
#include "Time.hpp"
#include "Random.hpp"

int main()
{
	HWND hwnd = FindWindow(nullptr, L"Devil Daggers");
	if (!hwnd)
	{
		std::cerr << "Unable to find Devil Daggers\n";
		return 1;
	}

	if (!RegisterHotKey(nullptr, 1, MOD_CONTROL, 'P'))
	{
		std::cerr << "Failed to register hotkey\n";
		return 1;
	}

#if 0
	cv::Mat frame = cv::imread("Frame075.jpg", cv::IMREAD_GRAYSCALE);
	cv::GaussianBlur(frame, frame, cv::Size(3, 3), 0.8);
	cv::Mat edge;
	cv::Canny(frame, edge, 80, 240);
	cv::imwrite("edge.jpg", edge);
	cv::waitKey(0);
#endif

	// Create interception context
	// Find active keyboard and mouse
	InputSimulator::Init();

	// Initialize templates
	// Load .onnx models
	Detector::Init();

	// Load agents
	std::vector<std::string> agentNames{ "agent0", "agent1", "agent2", "agent3", "agent4", "agent5" };
	std::vector<Agent> agents;
	for (int i = 0; i < agentNames.size(); ++i)
		agents.push_back(Agent{ agentNames[i] });
	int eliteCount = 3;
	int replaceCount = static_cast<int>(agents.size()) - eliteCount;
	int currentAgent = 0;
	int const TRIAL_COUNT = 5;
	int trialCount  = 0;

	std::shared_ptr<DXCam::DXCamera> camera = DXCam::create();
	GeneticAlgorithm ga = GeneticAlgorithm(camera);

#if 0
	// Generate a random genome
	std::vector<float> genome = ga.GenerateGenome();
#endif

	MSG msg{};
	bool running = false;
	auto lastTime = std::chrono::steady_clock::now();
	while (true)
	{
		// Starts capturing when Ctrl + P is pressed
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_HOTKEY)
			{
				running = !running;
				if (running)
				{
					DXCam::Region region = Detector::ComputeGameRegion(hwnd);
					camera->start(region, Constants::CAPTURE_FRAMERATE, true);
				}
				else
				{
					camera->stop();
				}
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		bool generationFinished = false;
		if (running)
		{
			auto currentTime = std::chrono::steady_clock::now();
			std::chrono::duration<float> delta = currentTime - lastTime;
			Time::dt = delta.count();
			bool isAgentDead = ga.RunGeneration(agents[currentAgent]);
			lastTime = currentTime;

			// Post-generation processing
			if (isAgentDead && !generationFinished)
			{
				std::cout
					<< "agent" << currentAgent << " total time: "
					<< agents[currentAgent].totalTimeSurvived << '\n';

				generationFinished = true;
				++currentAgent;

				// If we finished running all agents
				if (currentAgent >= agents.size())
				{
					++trialCount;
					if (trialCount % TRIAL_COUNT == 0)
					{
						// Sort agents by survival time
						std::sort(std::begin(agents), std::end(agents), [](Agent const& a, Agent const& b) {
							return a.timeSurvived > b.timeSurvived;
						});

						// Pick the top 3 agents
						// Replace the rest with mutated version of the top 3
						for (int i = 0; i < replaceCount; ++i)
						{
							Agent& replacedAgent = agents[agents.size() - i - 1];
							std::string replacedFilename = replacedAgent.genomeFilename;
							replacedAgent = agents[i % eliteCount];
							replacedAgent.genomeFilename = std::move(replacedFilename);
							ga.MutateAgent(replacedAgent);
						}

						// Reset all agent state
						for (int i = 0; i < agents.size(); ++i)
						{
							Agent& agent = agents[i];
							agent.timeSurvived = 0.f;
							agent.totalTimeSurvived = 0.f;
							agent.scoutTimer = 0.f;
							agent.isScouting = false;
						}

						trialCount = 0;
						std::cout << "Population evaluated\n";
					}
				}

				// Run the next agent
				currentAgent %= agents.size();
			}

			if (!isAgentDead)
			{
				generationFinished = false;
			}
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}
	}
	InputSimulator::Free();
	UnregisterHotKey(nullptr, 1);
	return 0;
}
