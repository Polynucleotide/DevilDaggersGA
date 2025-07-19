#ifndef INPUT_SIMULATOR_HPP
#define INPUT_SIMULATOR_HPP
#pragma once

#include <windows.h>
#include <thread>
#include <atomic>
#include <interception/interception.h>

class InputSimulator
{
	public:
		static void Init();
		static void Free();
		static void Reset();
		static void KeyPress(char c);
		static void HoldWS(char c, int milliseconds);
		static void HoldAD(char c, int milliseconds);
		static void MoveMouseUD(int dy, int steps = 20);
		static void MoveMouseLR(int dx, int steps = 20);
		static void HoldLMB(int milliseconds);
		static void ReleaseWS();
		static void ReleaseAD();
		static void StopMovingMouse();
		static void ReleaseLMB();
		static const float MAX_UP_ANGLE;
		static float GetUpAngle() { return upAngle; };

	private:
		static std::thread holdWSThread;
		static std::thread holdADThread;
		static std::thread mouseMoveUDThread;
		static std::thread mouseMoveLRThread;
		static std::thread holdLMBThread;
		static InterceptionContext context;
		static InterceptionDevice keyboard;
		static InterceptionDevice mouse;
		static float upAngle;
		static std::atomic<bool> cancelHoldWSFlag;
		static std::atomic<bool> cancelHoldADFlag;
		static std::atomic<bool> cancelMouseMoveUDFlag;
		static std::atomic<bool> cancelMouseMoveLRFlag;
		static std::atomic<bool> cancelHoldLMBFlag;
};

#endif // !INPUT_SIMULATOR_HPP
