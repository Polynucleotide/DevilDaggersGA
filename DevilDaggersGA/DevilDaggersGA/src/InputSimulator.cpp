#include <iostream>
#include <algorithm>
#include <memory>
#include <chrono>
#include "InputSimulator.hpp"
#include "Time.hpp"

InterceptionContext InputSimulator::context;
InterceptionDevice InputSimulator::keyboard;
InterceptionDevice InputSimulator::mouse;

std::thread InputSimulator::holdWSThread;
std::thread InputSimulator::holdADThread;
std::thread InputSimulator::mouseMoveUDThread;
std::thread InputSimulator::mouseMoveLRThread;
std::thread InputSimulator::holdLMBThread;

std::atomic<bool> InputSimulator::cancelHoldWSFlag = false;
std::atomic<bool> InputSimulator::cancelHoldADFlag = false;
std::atomic<bool> InputSimulator::cancelMouseMoveUDFlag = false;
std::atomic<bool> InputSimulator::cancelMouseMoveLRFlag = false;
std::atomic<bool> InputSimulator::cancelHoldLMBFlag = false;

float InputSimulator::upAngle = 0.f;
float const InputSimulator::MAX_UP_ANGLE = 800.f;

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
    for (int device = 11; device <= INTERCEPTION_MAX_DEVICE; ++device)
    {
        if (interception_is_mouse(device) && !interception_is_invalid(device))
        {
            mouse = device;
            break;
        }
    }
}

void InputSimulator::Free()
{
    interception_destroy_context(context);
}

void InputSimulator::KeyPress(char c)
{
    std::thread([=]()
    {
        SHORT vkCombo = VkKeyScanEx(c, GetKeyboardLayout(0));
        BYTE vk = LOBYTE(vkCombo);
        UINT scanCode = MapVirtualKeyEx(vk, MAPVK_VK_TO_VSC, GetKeyboardLayout(0));

        InterceptionStroke stroke;
        InterceptionKeyStroke& ks = *(InterceptionKeyStroke*)&stroke;

        ks.code = static_cast<short>(scanCode);
        ks.state = INTERCEPTION_KEY_DOWN;
        interception_send(context, keyboard, &stroke, 1);

        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        ks.state = INTERCEPTION_KEY_UP;
        interception_send(context, keyboard, &stroke, 1);
    }).detach();
}

void InputSimulator::HoldWS(char c, int milliseconds)
{
    cancelHoldWSFlag.store(true);
    if (holdWSThread.joinable()) holdWSThread.join();

    cancelHoldWSFlag.store(false);
    holdWSThread = std::thread([=]()
    {
        SHORT vkCombo = VkKeyScanEx(c, GetKeyboardLayout(0));
        BYTE vk = LOBYTE(vkCombo);
        UINT scanCode = MapVirtualKeyEx(vk, MAPVK_VK_TO_VSC, GetKeyboardLayout(0));

        InterceptionStroke stroke;
        InterceptionKeyStroke& ks = *(InterceptionKeyStroke*)&stroke;
        ks.code = static_cast<short>(scanCode);
        ks.state = INTERCEPTION_KEY_DOWN;
        interception_send(context, keyboard, &stroke, 1);

        // Hold the key until time is up or we cancel this thread
        float elapsed = 0.f;
        while (true)
        {
            if (cancelHoldWSFlag.load()) break;

            elapsed += Time::dt;
            if (elapsed >= milliseconds) break;

            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }

        ks.state = INTERCEPTION_KEY_UP;
        interception_send(context, keyboard, &stroke, 1);
    });
}

void InputSimulator::HoldAD(char c, int milliseconds)
{
    cancelHoldADFlag.store(true);
    if (holdADThread.joinable()) holdADThread.join();

    cancelHoldADFlag.store(false);
    holdADThread = std::thread([=]()
    {
        SHORT vkCombo = VkKeyScanEx(c, GetKeyboardLayout(0));
        BYTE vk = LOBYTE(vkCombo);
        UINT scanCode = MapVirtualKeyEx(vk, MAPVK_VK_TO_VSC, GetKeyboardLayout(0));

        InterceptionStroke stroke;
        InterceptionKeyStroke& ks = *(InterceptionKeyStroke*)&stroke;
        ks.code = static_cast<short>(scanCode);
        ks.state = INTERCEPTION_KEY_DOWN;
        interception_send(context, keyboard, &stroke, 1);

        // Hold the key until time is up or we cancel this thread
        float elapsed = 0.f;
        while (true)
        {
            if (cancelHoldADFlag.load()) break;

            elapsed += Time::dt;
            if (elapsed >= milliseconds) break;

            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }

        ks.state = INTERCEPTION_KEY_UP;
        interception_send(context, keyboard, &stroke, 1);
    });
}

void InputSimulator::MoveMouseUD(int dy, int steps)
{
    // Cancel the previous mouse move thread
    cancelMouseMoveUDFlag.store(true);
    if (mouseMoveUDThread.joinable()) mouseMoveUDThread.join();

    cancelMouseMoveUDFlag.store(false);
    mouseMoveUDThread = std::thread([=]()
    {
        for (int i = 0; i < steps; ++i)
        {
            if (cancelMouseMoveUDFlag.load()) break;

            int stepDy = dy / steps;
            InterceptionMouseStroke stroke = {};
            stroke.flags = INTERCEPTION_MOUSE_MOVE_RELATIVE;
            stroke.y = stepDy;
            upAngle += static_cast<float>(stepDy) / MAX_UP_ANGLE;
            upAngle = std::clamp(upAngle, -1.f, 1.f);
            interception_send(context, mouse, (InterceptionStroke*)&stroke, 1);
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    });
}

void InputSimulator::MoveMouseLR(int dx, int steps)
{
    // Cancel the previous mouse move thread
    cancelMouseMoveLRFlag.store(true);
    if (mouseMoveLRThread.joinable()) mouseMoveLRThread.join();

    cancelMouseMoveLRFlag.store(false);
    mouseMoveLRThread = std::thread([=]()
    {
        for (int i = 0; i < steps; ++i)
        {
            if (cancelMouseMoveLRFlag.load()) break;

            InterceptionMouseStroke stroke = {};
            stroke.flags = INTERCEPTION_MOUSE_MOVE_RELATIVE;
            stroke.x = dx / steps;
            interception_send(context, mouse, (InterceptionStroke*)&stroke, 1);
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    });
}

void InputSimulator::HoldLMB(int milliseconds)
{
    // Cancel the previous thread
    cancelHoldLMBFlag.store(true);
    if (holdLMBThread.joinable()) holdLMBThread.join();

    cancelHoldLMBFlag.store(false);
    holdLMBThread = std::thread([=]()
    {
        // Simulate left button down
        InterceptionMouseStroke stroke = {};
        stroke.state = INTERCEPTION_MOUSE_LEFT_BUTTON_DOWN;
        interception_send(context, mouse, (const InterceptionStroke*)&stroke, 1);

        // Hold the key until time is up or we cancel this thread
        float elapsed = 0.f;
        while (true)
        {
            if (cancelHoldLMBFlag.load()) break;

            elapsed += Time::dt;
            if ((elapsed += Time::dt) >= milliseconds) break;

            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }

        stroke.state = INTERCEPTION_MOUSE_LEFT_BUTTON_UP;
        interception_send(context, mouse, (const InterceptionStroke*)&stroke, 1);
    });
}

void InputSimulator::ReleaseWS()
{
    cancelHoldWSFlag.store(true);
    if (holdWSThread.joinable()) holdWSThread.join();
    cancelHoldWSFlag.store(false);
}

void InputSimulator::ReleaseAD()
{
    cancelHoldADFlag.store(true);
    if (holdADThread.joinable()) holdADThread.join();
    cancelHoldADFlag.store(false);
}

void InputSimulator::StopMovingMouse()
{
    // Set dx and dy to 0
    MoveMouseUD(0, 1);
    MoveMouseLR(0, 1);
    cancelMouseMoveUDFlag.store(false);
    cancelMouseMoveLRFlag.store(false);
}

void InputSimulator::ReleaseLMB()
{
    cancelHoldLMBFlag.store(true);
    if (holdLMBThread.joinable()) holdLMBThread.join();
    cancelHoldLMBFlag.store(false);
}

void InputSimulator::Reset()
{
    upAngle = 0.f;
    ReleaseWS();
    ReleaseAD();
    StopMovingMouse();
    ReleaseLMB();
}
