#include "KeyboardHook.h"

#include <windows.h>
#include <iostream>
#include <thread>

bool realA = false;
bool realD = false;

HHOOK keyboardHook = nullptr;

bool isRealA()
{
    return realA;
}

bool isRealD()
{
    return realD;
}

bool isRealKeyDown(char keyCode)
{
    if (keyCode == 65) return realA;
    if (keyCode == 68) return realD;

    return false;
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION)
    {
        KBDLLHOOKSTRUCT* kb = (KBDLLHOOKSTRUCT*)lParam;

        bool isInjected = (kb->flags & LLKHF_INJECTED) != 0;

        if (!isInjected)
        {
            bool isDown = wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN;
            bool isUp = wParam == WM_KEYUP || wParam == WM_SYSKEYUP;

            if (kb->vkCode == 65)
            {
                if (isDown) realA = true;
                if (isUp) realA = false;
            }

            if (kb->vkCode == 68)
            {
                if (isDown) realD = true;
                if (isUp) realD = false;
            }
        }
    }

    return CallNextHookEx(keyboardHook, nCode, wParam, lParam);
}

void hookLoop()
{
    keyboardHook = SetWindowsHookEx(
        WH_KEYBOARD_LL,
        KeyboardProc,
        GetModuleHandle(nullptr),
        0
    );

    if (!keyboardHook)
    {
        std::cout << "Keyboard hook failed\n";
        return;
    }

    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(keyboardHook);
}

void startKeyboardHook()
{
    std::thread(hookLoop).detach();
}