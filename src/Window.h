#pragma once

#include <windows.h>

class MainWindow
{
public:
    MainWindow() = default;
    ~MainWindow();

    bool Create(HINSTANCE instance, int nCmdShow);
    int Run();

private:
    static LRESULT CALLBACK WindowProc(
        HWND hwnd,
        UINT message,
        WPARAM wParam,
        LPARAM lParam
    );

    LRESULT HandleMessage(
        UINT message,
        WPARAM wParam,
        LPARAM lParam
    );

    void CreateControls();
    void LayoutControls(int width, int height);
    void SetStatus(const wchar_t* text);
    void RestartExplorer();
    static void CenterWindow(HWND hwnd);

    void ApplyFont(HWND control);
    void DrawModernButton(DRAWITEMSTRUCT* dis);

private:
    HINSTANCE instance_ = nullptr;
    HWND hwnd_ = nullptr;
    HFONT font_ = nullptr;
    HFONT fontBold_ = nullptr;

    // Group boxes
    HWND homeGroup_ = nullptr;
    HWND galleryGroup_ = nullptr;

    // Home buttons
    HWND homeHide_ = nullptr;
    HWND homeShow_ = nullptr;

    // Gallery buttons
    HWND galleryHide_ = nullptr;
    HWND galleryShow_ = nullptr;

    // Bottom
    HWND restartExplorer_ = nullptr;
    HWND status_ = nullptr;
};
