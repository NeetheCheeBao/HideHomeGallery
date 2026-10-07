#include "Window.h"
#include "Registry.h"
#include "resource.h"

#include <shellapi.h>
#include <uxtheme.h>

#pragma comment(lib, "uxtheme.lib")

namespace
{
    constexpr wchar_t kWindowClassName[] =
        L"HideHomeGalleryWindowClass";

    constexpr wchar_t kWindowTitle[] =
        L"主文件夹和图库隐藏工具v1.0";

    constexpr int kInitialClientWidth  = 420;
    constexpr int kInitialClientHeight = 340;

    constexpr int kMinClientWidth  = 360;
    constexpr int kMinClientHeight = 300;

    constexpr int kMargin       = 16;
    constexpr int kGroupPadding = 14;
    constexpr int kGap          = 12;
    constexpr int kButtonHeight = 34;
    constexpr int kStatusHeight = 22;
    constexpr int kGroupTitleH  = 20;

    enum ControlId : int
    {
        ID_HOME_HIDE = 1001,
        ID_HOME_SHOW,
        ID_GALLERY_HIDE,
        ID_GALLERY_SHOW,
        ID_RESTART_EXPLORER
    };

    // 现代扁平圆角按钮
    HWND MakeModernButton(
        HWND parent,
        HINSTANCE instance,
        const wchar_t* text,
        int id)
    {
        return CreateWindowExW(
            0,
            L"BUTTON",
            text,
            WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_OWNERDRAW,
            0, 0, 0, kButtonHeight,
            parent,
            reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)),
            instance,
            nullptr
        );
    }

    HWND MakeGroupBox(
        HWND parent,
        HINSTANCE instance,
        const wchar_t* text)
    {
        return CreateWindowExW(
            0,
            L"BUTTON",
            text,
            WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
            0, 0, 0, 0,
            parent,
            nullptr,
            instance,
            nullptr
        );
    }

    HWND MakeLabel(
        HWND parent,
        HINSTANCE instance,
        const wchar_t* text)
    {
        return CreateWindowExW(
            0,
            L"STATIC",
            text,
            WS_CHILD | WS_VISIBLE | SS_LEFT | SS_CENTERIMAGE,
            0, 0, 0, kStatusHeight,
            parent,
            nullptr,
            instance,
            nullptr
        );
    }

    // 简单圆角矩形（GDI）
    void FillRoundRect(HDC hdc, const RECT& rc, int radius, HBRUSH brush)
    {
        HRGN rgn = CreateRoundRectRgn(
            rc.left, rc.top,
            rc.right + 1, rc.bottom + 1,
            radius, radius
        );
        FillRgn(hdc, rgn, brush);
        DeleteObject(rgn);
    }

    void FrameRoundRect(HDC hdc, const RECT& rc, int radius, HPEN pen)
    {
        HPEN oldPen = static_cast<HPEN>(SelectObject(hdc, pen));
        HBRUSH oldBrush = static_cast<HBRUSH>(SelectObject(hdc, GetStockObject(NULL_BRUSH)));

        RoundRect(
            hdc,
            rc.left, rc.top,
            rc.right, rc.bottom,
            radius, radius
        );

        SelectObject(hdc, oldBrush);
        SelectObject(hdc, oldPen);
    }
}

MainWindow::~MainWindow()
{
    if (font_)
    {
        DeleteObject(font_);
        font_ = nullptr;
    }
    if (fontBold_)
    {
        DeleteObject(fontBold_);
        fontBold_ = nullptr;
    }
}

void MainWindow::ApplyFont(HWND control)
{
    if (control && font_)
    {
        SendMessageW(control, WM_SETFONT, reinterpret_cast<WPARAM>(font_), TRUE);
    }
}

void MainWindow::DrawModernButton(DRAWITEMSTRUCT* dis)
{
    if (!dis || dis->CtlType != ODT_BUTTON)
        return;

    const HDC hdc = dis->hDC;
    RECT rc = dis->rcItem;

    const bool isPressed  = (dis->itemState & ODS_SELECTED) != 0;
    const bool isFocused  = (dis->itemState & ODS_FOCUS) != 0;
    const bool isDisabled = (dis->itemState & ODS_DISABLED) != 0;
    const bool isHot      = false; // 简化，不跟踪鼠标悬停

    // 颜色（接近 Windows 11 扁平按钮）
    COLORREF bgNormal   = RGB(243, 243, 243);
    COLORREF bgPressed  = RGB(230, 230, 230);
    COLORREF bgDisabled = RGB(249, 249, 249);
    COLORREF borderNormal = RGB(210, 210, 210);
    COLORREF borderFocus  = RGB(0, 103, 192);   // 蓝色焦点环
    COLORREF textColor    = isDisabled ? RGB(160, 160, 160) : RGB(30, 30, 30);

    COLORREF bg = isDisabled ? bgDisabled : (isPressed ? bgPressed : bgNormal);
    COLORREF border = isFocused ? borderFocus : borderNormal;

    // 背景
    HBRUSH bgBrush = CreateSolidBrush(bg);
    FillRoundRect(hdc, rc, 6, bgBrush);
    DeleteObject(bgBrush);

    // 边框（焦点时稍粗一点）
    HPEN pen = CreatePen(PS_SOLID, isFocused ? 2 : 1, border);
    FrameRoundRect(hdc, rc, 6, pen);
    DeleteObject(pen);

    // 文字
    wchar_t text[128]{};
    GetWindowTextW(dis->hwndItem, text, 128);

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, textColor);

    HFONT oldFont = nullptr;
    if (font_)
        oldFont = static_cast<HFONT>(SelectObject(hdc, font_));

    // 按下时文字轻微下移
    RECT textRc = rc;
    if (isPressed)
    {
        textRc.top += 1;
        textRc.left += 1;
    }

    DrawTextW(
        hdc,
        text,
        -1,
        &textRc,
        DT_CENTER | DT_VCENTER | DT_SINGLELINE
    );

    if (oldFont)
        SelectObject(hdc, oldFont);

    // 焦点虚线框（可选，已经用蓝色边框了）
    if (isFocused && !isPressed)
    {
        RECT focusRc = rc;
        InflateRect(&focusRc, -3, -3);
        DrawFocusRect(hdc, &focusRc);
    }
}

bool MainWindow::Create(HINSTANCE instance, int nCmdShow)
{
    instance_ = instance;

    font_ = CreateFontW(
        -15, 0, 0, 0,
        FW_NORMAL,
        FALSE, FALSE, FALSE,
        DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY,
        VARIABLE_PITCH,
        L"Microsoft YaHei UI"
    );

    fontBold_ = CreateFontW(
        -15, 0, 0, 0,
        FW_SEMIBOLD,
        FALSE, FALSE, FALSE,
        DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY,
        VARIABLE_PITCH,
        L"Microsoft YaHei UI"
    );

    WNDCLASSEXW wc{};
    wc.cbSize        = sizeof(wc);
    wc.lpfnWndProc   = &MainWindow::WindowProc;
    wc.hInstance     = instance_;
    wc.hCursor       = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    wc.lpszClassName = kWindowClassName;
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.hIcon         = LoadIconW(instance_, MAKEINTRESOURCEW(IDI_APP_ICON));
    wc.hIconSm       = LoadIconW(instance_, MAKEINTRESOURCEW(IDI_APP_ICON));

    if (!RegisterClassExW(&wc))
    {
        if (GetLastError() != ERROR_CLASS_ALREADY_EXISTS)
            return false;
    }

    RECT rc{0, 0, kInitialClientWidth, kInitialClientHeight};
    AdjustWindowRectEx(
        &rc,
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_THICKFRAME,
        FALSE,
        0
    );

    const int winW = rc.right - rc.left;
    const int winH = rc.bottom - rc.top;

    hwnd_ = CreateWindowExW(
        0,
        kWindowClassName,
        kWindowTitle,
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_THICKFRAME,
        CW_USEDEFAULT, CW_USEDEFAULT,
        winW, winH,
        nullptr,
        nullptr,
        instance_,
        this
    );

    if (!hwnd_)
        return false;

    CreateControls();
    CenterWindow(hwnd_);
    ShowWindow(hwnd_, nCmdShow);
    UpdateWindow(hwnd_);

    return true;
}

void MainWindow::CreateControls()
{
    homeGroup_ = MakeGroupBox(
        hwnd_, instance_,
        L"主文件夹 (Home)"
    );

    homeHide_ = MakeModernButton(
        hwnd_, instance_,
        L"隐藏 (Delete)",
        ID_HOME_HIDE
    );

    homeShow_ = MakeModernButton(
        hwnd_, instance_,
        L"显示 (Restore)",
        ID_HOME_SHOW
    );

    galleryGroup_ = MakeGroupBox(
        hwnd_, instance_,
        L"图库 (Gallery)"
    );

    galleryHide_ = MakeModernButton(
        hwnd_, instance_,
        L"隐藏 (Delete)",
        ID_GALLERY_HIDE
    );

    galleryShow_ = MakeModernButton(
        hwnd_, instance_,
        L"显示 (Restore)",
        ID_GALLERY_SHOW
    );

    restartExplorer_ = MakeModernButton(
        hwnd_, instance_,
        L"重启资源管理器 (立即生效)",
        ID_RESTART_EXPLORER
    );

    status_ = MakeLabel(
        hwnd_, instance_,
        L"准备就绪"
    );

    ApplyFont(homeGroup_);
    ApplyFont(galleryGroup_);
    ApplyFont(status_);

    // 按钮字体在 DrawModernButton 里使用
    if (fontBold_)
    {
        // 重启按钮可以用粗体，但因为 owner-draw，我们在绘制时统一用 font_
    }
}

void MainWindow::LayoutControls(int width, int height)
{
    const int contentW = width - 2 * kMargin;
    const int btnGap   = 10;

    const int groupInnerH = kGroupTitleH + kGroupPadding + kButtonHeight + kGroupPadding;
    const int groupH      = groupInnerH;

    int y = kMargin;

    // ---- 主文件夹 Group ----
    MoveWindow(homeGroup_, kMargin, y, contentW, groupH, TRUE);

    const int btnY1   = y + kGroupTitleH + (kGroupPadding / 2);
    const int btnW    = (contentW - 2 * kGroupPadding - btnGap) / 2;
    const int btnX1   = kMargin + kGroupPadding;
    const int btnX2   = btnX1 + btnW + btnGap;

    MoveWindow(homeHide_, btnX1, btnY1, btnW, kButtonHeight, TRUE);
    MoveWindow(homeShow_, btnX2, btnY1, btnW, kButtonHeight, TRUE);

    y += groupH + kGap;

    // ---- 图库 Group ----
    MoveWindow(galleryGroup_, kMargin, y, contentW, groupH, TRUE);

    const int btnY2 = y + kGroupTitleH + (kGroupPadding / 2);
    MoveWindow(galleryHide_, btnX1, btnY2, btnW, kButtonHeight, TRUE);
    MoveWindow(galleryShow_, btnX2, btnY2, btnW, kButtonHeight, TRUE);

    y += groupH + kGap + 4;

    // ---- 重启按钮 ----
    MoveWindow(
        restartExplorer_,
        kMargin, y,
        contentW, kButtonHeight + 2,
        TRUE
    );

    // ---- 状态栏 ----
    const int statusY = height - kStatusHeight - 8;
    MoveWindow(
        status_,
        kMargin, statusY,
        contentW, kStatusHeight,
        TRUE
    );
}

void MainWindow::SetStatus(const wchar_t* text)
{
    if (status_)
        SetWindowTextW(status_, text);
}

void MainWindow::RestartExplorer()
{
    SetStatus(L"正在重启资源管理器...");
    UpdateWindow(hwnd_);

    // 最简单可靠的方式：强制结束所有 explorer 再重新启动
    STARTUPINFOW si{};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    PROCESS_INFORMATION pi{};

    wchar_t killCmd[] = L"cmd.exe /c taskkill /f /im explorer.exe";
    if (CreateProcessW(
            nullptr, killCmd,
            nullptr, nullptr, FALSE,
            CREATE_NO_WINDOW,
            nullptr, nullptr,
            &si, &pi))
    {
        WaitForSingleObject(pi.hProcess, 5000);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }

    Sleep(500);

    // 重新启动 explorer
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    wchar_t startCmd[] = L"explorer.exe";
    if (CreateProcessW(
            nullptr, startCmd,
            nullptr, nullptr, FALSE,
            0,
            nullptr, nullptr,
            &si, &pi))
    {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        SetStatus(L"资源管理器已重启");
    }
    else
    {
        SetStatus(L"资源管理器启动失败");
    }
}

void MainWindow::CenterWindow(HWND hwnd)
{
    RECT rect{};
    GetWindowRect(hwnd, &rect);

    const int width  = rect.right - rect.left;
    const int height = rect.bottom - rect.top;

    const int screenW = GetSystemMetrics(SM_CXSCREEN);
    const int screenH = GetSystemMetrics(SM_CYSCREEN);

    SetWindowPos(
        hwnd,
        nullptr,
        (screenW - width) / 2,
        (screenH - height) / 2,
        0, 0,
        SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE
    );
}

int MainWindow::Run()
{
    MSG msg{};
    while (GetMessageW(&msg, nullptr, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK MainWindow::WindowProc(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    MainWindow* self = reinterpret_cast<MainWindow*>(
        GetWindowLongPtrW(hwnd, GWLP_USERDATA)
    );

    if (message == WM_NCCREATE)
    {
        const auto* create =
            reinterpret_cast<const CREATESTRUCTW*>(lParam);

        self = static_cast<MainWindow*>(create->lpCreateParams);
        self->hwnd_ = hwnd;

        SetWindowLongPtrW(
            hwnd,
            GWLP_USERDATA,
            reinterpret_cast<LONG_PTR>(self)
        );
    }

    if (self)
        return self->HandleMessage(message, wParam, lParam);

    return DefWindowProcW(hwnd, message, wParam, lParam);
}

LRESULT MainWindow::HandleMessage(
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    switch (message)
    {
    case WM_GETMINMAXINFO:
    {
        auto* info = reinterpret_cast<MINMAXINFO*>(lParam);

        RECT desired{0, 0, kMinClientWidth, kMinClientHeight};
        AdjustWindowRectEx(
            &desired,
            WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU |
            WS_MINIMIZEBOX | WS_THICKFRAME,
            FALSE, 0
        );

        info->ptMinTrackSize.x = desired.right - desired.left;
        info->ptMinTrackSize.y = desired.bottom - desired.top;
        return 0;
    }

    case WM_SIZE:
    {
        LayoutControls(LOWORD(lParam), HIWORD(lParam));
        return 0;
    }

    case WM_DRAWITEM:
    {
        auto* dis = reinterpret_cast<DRAWITEMSTRUCT*>(lParam);
        if (dis && dis->CtlType == ODT_BUTTON)
        {
            DrawModernButton(dis);
            return TRUE;
        }
        break;
    }

    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case ID_HOME_SHOW:
        {
            const bool ok = Registry::SetHomeFolderVisible(true);
            SetStatus(ok ? L"主文件夹：已设置为显示" : L"主文件夹：修改失败");
            break;
        }
        case ID_HOME_HIDE:
        {
            const bool ok = Registry::SetHomeFolderVisible(false);
            SetStatus(ok ? L"主文件夹：已设置为隐藏" : L"主文件夹：修改失败");
            break;
        }
        case ID_GALLERY_SHOW:
        {
            const bool ok = Registry::SetGalleryVisible(true);
            SetStatus(ok ? L"图库：已设置为显示" : L"图库：修改失败");
            break;
        }
        case ID_GALLERY_HIDE:
        {
            const bool ok = Registry::SetGalleryVisible(false);
            SetStatus(ok ? L"图库：已设置为隐藏" : L"图库：修改失败");
            break;
        }
        case ID_RESTART_EXPLORER:
        {
            RestartExplorer();
            break;
        }
        default:
            break;
        }
        return 0;
    }

    case WM_CLOSE:
        DestroyWindow(hwnd_);
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    default:
        return DefWindowProcW(hwnd_, message, wParam, lParam);
    }

    return 0;
}
