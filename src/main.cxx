#include "GameApp.h"
#include "windows.h"
#include <iostream>

int WINAPI wWinMain(
    HINSTANCE hInstance,
    HINSTANCE prev,
    PWSTR cmdline,
    int arg
) {
    wchar_t dir[260];
    GetCurrentDirectoryW(260, dir);
    std::wstring cwd = dir;
    GameApp app(hInstance, L"DX", 400, 400);
    app.Init();
    app.Run();

    return 0;
}