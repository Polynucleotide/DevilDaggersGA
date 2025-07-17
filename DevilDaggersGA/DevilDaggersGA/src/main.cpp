#include <iostream>
#include <windows.h>
#include <dxcam/dxcam.h>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <sstream>
#include <iomanip>
#include <chrono>
#include "Utilities.hpp"
#include "Detector.hpp"
#include "Constants.hpp"
#include "InputSimulator.hpp"

void MoveMouseBy(int dx, int dy)
{
	INPUT in = { 0 };
	in.type = INPUT_MOUSE;
	in.mi.dwFlags = MOUSEEVENTF_MOVE;
	in.mi.dx = dx;
	in.mi.dy = dy;
	SendInput(1, &in, sizeof(in));
}

int main()
{
	HWND hwnd = FindWindow(nullptr, L"Devil Daggers");
	//if (!hwnd)
	//{
	//	std::cerr << "Unable to find Devil Daggers\n";
	//	return 1;
	//}

	if (!RegisterHotKey(nullptr, 1, MOD_CONTROL, 'P'))
	{
		std::cerr << "Failed to register hotkey\n";
		return 1;
	}

	InterceptionContext context;
	InterceptionDevice device;
	InterceptionStroke stroke;

	context = interception_create_context();
	InputSimulator::Init();

	// Initialize templates
	// Load .onnx models
	Detector::Init();
	cv::Mat const& gameOverTemplate = Detector::GetGetGameOverTemplate();

	int frameCount = 0;
	MSG msg{};
	std::shared_ptr<DXCam::DXCamera> camera = DXCam::create();
	bool running = false;
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
					Detector::ComputeGameOverROI(region);
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

		if (running)
		{
			// Get frame
			cv::Mat frameBGRA = camera->get_latest_frame();
			cv::Mat frameBGR;
			cv::cvtColor(frameBGRA, frameBGR, cv::COLOR_BGRA2BGR);

			// Check if agent is dead
			cv::Mat roi = frameBGR(cv::Rect(0, 0, 500, 100));
			cv::Mat result;
			cv::matchTemplate(roi, gameOverTemplate, result, cv::TM_CCOEFF_NORMED);
			double maxVal;
			cv::minMaxLoc(result, nullptr, &maxVal);
			if (maxVal > Constants::GAME_OVER_THRESHOLD)
			{
				//InputSimulator::KeyPress('R');
				continue;
			}

			//std::vector<cv::Point> skulls = Detector::DetectSkulls(frameBGR, SkullType::SKULL1);
			//std::vector<cv::Point> skulls2 = Detector::DetectSkulls(frameBGR, SkullType::SKULL2);

#if 1
			// Set number
			std::stringstream ss;
			ss << std::setw(3) << std::setfill('0') << frameCount;
			std::string count = ss.str();

			// Write frame
			cv::imwrite("Frame" + count + ".jpg", frameBGR);
			++frameCount;
#endif
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}

	}
	interception_destroy_context(context);
	UnregisterHotKey(nullptr, 1);
	return 0;
}
