#include "Window.h"

#include <windows.h>
#include <commctrl.h>

int APIENTRY wWinMain(
    HINSTANCE hInstance,
    HINSTANCE,
    LPWSTR,
    int nCmdShow)
{
    // 初始化通用控件，配合 manifest 启用视觉样式
    INITCOMMONCONTROLSEX controls{};
    controls.dwSize = sizeof(controls);
    controls.dwICC  = ICC_STANDARD_CLASSES | ICC_WIN95_CLASSES;
    InitCommonControlsEx(&controls);

    MainWindow window;

    if (!window.Create(hInstance, nCmdShow))
    {
        MessageBoxW(
            nullptr,
            L"程序窗口创建失败。",
            L"主文件夹和图库隐藏工具",
            MB_OK | MB_ICONERROR
        );
        return 1;
    }

    return window.Run();
}
