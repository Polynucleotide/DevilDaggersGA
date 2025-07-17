#ifndef INPUT_SIMULATOR_HPP
#define INPUT_SIMULATOR_HPP
#pragma once

#include <windows.h>
#include <interception/interception.h>

class InputSimulator
{
	public:
		static void Init();
		static void KeyPress(char c);
		static void HoldLMB();

	private:
		static InterceptionContext context;
		static InterceptionDevice keyboard;
		static InterceptionDevice mouse;
};

#endif // !INPUT_SIMULATOR_HPP
