#include "InputSimulator.hpp"
#include <iostream>
#include <thread>

InterceptionContext InputSimulator::context;
InterceptionDevice InputSimulator::keyboard;
InterceptionDevice InputSimulator::mouse;

void InputSimulator::Init()
{
    context = interception_create_context();

    // Find active keyboard
    for (int device = 1; device <= INTERCEPTION_MAX_KEYBOARD; ++device)
    {
        if (interception_is_keyboard(device) && !interception_is_invalid(device))
        {
            keyboard = device;
            break;
        }
    }

    // Find active mouse
    for (int device = 11; device <= INTERCEPTION_MAX_MOUSE; ++device)
    {
        if (interception_is_mouse(device) && !interception_is_invalid(device))
        {
            mouse = device;
            break;
        }
    }
}

void InputSimulator::KeyPress(char c)
{
    std::thread([=]()
    {
        InterceptionStroke stroke;
        InterceptionKeyStroke& ks = *(InterceptionKeyStroke*)&stroke;
        try
        {
            SHORT vkCombo = VkKeyScanEx(c, GetKeyboardLayout(0));
            BYTE vk = LOBYTE(vkCombo);
            UINT scanCode = MapVirtualKeyEx(vk, MAPVK_VK_TO_VSC, GetKeyboardLayout(0));

            ks.code = scanCode;

            ks.state = INTERCEPTION_KEY_DOWN;
            interception_send(context, keyboard, &stroke, 1);

            std::this_thread::sleep_for(std::chrono::milliseconds(50));

            ks.state = INTERCEPTION_KEY_UP;
            interception_send(context, keyboard, &stroke, 1);
        }
        catch (const std::exception& e)
        {
            ks.state = INTERCEPTION_KEY_UP;
            interception_send(context, keyboard, &stroke, 1);
        }
    }).detach();
}

void InputSimulator::HoldLMB()
{
    std::thread([=]()
    {
        InterceptionMouseStroke stroke = {};
        try
        {
            // Simulate left button down
            stroke.state = INTERCEPTION_MOUSE_LEFT_BUTTON_DOWN;
            interception_send(context, 12, (const InterceptionStroke*)&stroke, 1);
        }
        catch (const std::exception& e)
        {
            stroke.state = INTERCEPTION_MOUSE_LEFT_BUTTON_UP;
            interception_send(context, 12, (const InterceptionStroke*)&stroke, 1);
        }
    }).detach();
}
