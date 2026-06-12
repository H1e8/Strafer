#include <iostream>
#include <string>
#include <thread>
#include <functional>
#include <windows.h>
#include <random>
#include <chrono>

#include "KeyboardHook.h"

using std::string;
using Clock = std::chrono::steady_clock;

void holdKey(char keyCode, int time) {
    INPUT input = {};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = keyCode;
    input.ki.dwFlags = 0;

    SendInput(1, &input, sizeof(INPUT));

    Sleep(time);

    if (!isRealKeyDown(keyCode))
    {
        input.ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(1, &input, sizeof(INPUT));
    }
}

int humanizeMove(int min, int max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());

    std::uniform_int_distribution<int> dist(min, max);
    return dist(gen);
}

void leftStrafe(bool& active, bool& isStrafing, bool& isWalking, int& strafedTimes) {
    bool wasPressed = false;
    Clock::time_point pressTime;

    while (true) {
        bool isHold = isRealA();

        if (isHold && !wasPressed) {
            wasPressed = true;
            pressTime = Clock::now();
        }

        if (isStrafing) {
            wasPressed = false;
            Sleep(10);
            continue;
        }

        Sleep(10);

        if (!isHold && wasPressed) {
            wasPressed = false;

            auto strafeDuration = Clock::now() - pressTime;
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(strafeDuration).count();

            int time = humanizeMove(90, 100);
            if (ms <= 200) {
                time = humanizeMove(30, 40);
            }
            else if (ms > 200 && ms <= 500) {
                time = humanizeMove(70, 80);
            }

            if (active && !isStrafing && !isWalking) {
                strafedTimes++;
                isStrafing = true;
                holdKey(68, time);
                isStrafing = false;
            }
        }
    }
}

void rightStrafe(bool& active, bool& isStrafing, bool& isWalking, int& strafedTimes) {
    bool wasPressed = false;
    Clock::time_point pressTime;

    while (true) {
        bool isHold = isRealD();

        if (isHold && !wasPressed) {
            wasPressed = true;
            pressTime = Clock::now();
        }

        if (isStrafing) {
            wasPressed = false;
            Sleep(10);
            continue;
        }

        Sleep(10);

        if (!isHold && wasPressed) {
            wasPressed = false;

            auto strafeDuration = Clock::now() - pressTime;
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(strafeDuration).count();

            int time = humanizeMove(90, 100);
            if (ms <= 200) {
                time = humanizeMove(30, 40);
            }
            else if (ms > 200 && ms <= 500) {
                time = humanizeMove(70, 80);
            }

            if (active && !isStrafing && !isWalking) {
                strafedTimes++;
                isStrafing = true;
                holdKey(65, time);
                isStrafing = false;
            }
        }
    }
}

int main()
{
    bool isWalking = false;
    bool wasPressed = false;
    bool active = false;
    bool isStrafing = false;
    const string certificateID = "hleb";
    int strafedTimes = 0;

    // Logo
    std::cout << "Autostrafer by\n";
    std::cout << " __    __  __            __                           \n/  |  /  |/  |          /  |                          \n$$ |  $$ |$$ |  ______  $$ |____   _______    ______  \n$$ |__$$ |$$ | /      \\ $$      \\ /       \\  /      \\ \n$$    $$ |$$ |/$$$$$$  |$$$$$$$  |$$$$$$$  |/$$$$$$  |\n$$$$$$$$ |$$ |$$    $$ |$$ |  $$ |$$ |  $$ |$$ |  $$ |\n$$ |  $$ |$$ |$$$$$$$$/ $$ |__$$ |$$ |  $$ |$$ \\__$$ |\n$$ |  $$ |$$ |$$       |$$    $$/ $$ |  $$ |$$    $$/ \n$$/   $$/ $$/  $$$$$$$/ $$$$$$$/  $$/   $$/  $$$$$$/ \n\n";
    std::cout << "|---> To activate or deactivate press END\n\n";

    Sleep(10);

    // Requiered functions
    startKeyboardHook();

    std::thread(leftStrafe, std::ref(active), std::ref(isStrafing), std::ref(isWalking), std::ref(strafedTimes)).detach();
    std::thread(rightStrafe, std::ref(active), std::ref(isStrafing), std::ref(isWalking), std::ref(strafedTimes)).detach();

    // Main loop
    while (true) {
        bool isActive = GetAsyncKeyState(VK_END) & 0x8000;

        isWalking = (GetAsyncKeyState(87) & 0x8000) || (GetAsyncKeyState(83) & 0x8000);

        if (isActive && !wasPressed) {
            wasPressed = true;

            if (!active) {
                active = true;
                std::cout << "\n|$$$| - Activated - |$$$|\n    - Time to move! -\n";
            }
            else {
                active = false;
                std::cout << "\n- You have strafed " << strafedTimes << " times! -\n";
                std::cout << "\n|$$$| - Deactivated - |$$$|\n     - Be Right Back -\n";
                strafedTimes = 0;
            }
        }

        Sleep(25);

        if (!isActive) {
            wasPressed = false;
        }
    }
}