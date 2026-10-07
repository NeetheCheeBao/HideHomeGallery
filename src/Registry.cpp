#include "Registry.h"

#include <windows.h>

namespace
{
    constexpr wchar_t kClsidRoot[] =
        L"Software\\Classes\\CLSID\\";

    // Home  = {f874310e-b6b7-47dc-bc84-b9e6b38f5903}
    // Gallery = {e88865ea-0e1c-4e20-9aa6-edcd0212c87c}
    constexpr wchar_t kHomeFolderClsid[] =
        L"{f874310e-b6b7-47dc-bc84-b9e6b38f5903}";

    constexpr wchar_t kGalleryClsid[] =
        L"{e88865ea-0e1c-4e20-9aa6-edcd0212c87c}";

    constexpr wchar_t kValueName[] =
        L"System.IsPinnedToNameSpaceTree";

    // 写入指定 CLSID 下的显示状态
    bool SetPinnedValue(const wchar_t* clsid, bool visible)
    {
        HKEY key = nullptr;

        wchar_t subKey[256]{};

        wsprintfW(
            subKey,
            L"%s%s",
            kClsidRoot,
            clsid
        );

        LONG result = RegCreateKeyExW(
            HKEY_CURRENT_USER,
            subKey,
            0,
            nullptr,
            REG_OPTION_NON_VOLATILE,
            KEY_SET_VALUE,
            nullptr,
            &key,
            nullptr
        );

        if (result != ERROR_SUCCESS)
        {
            return false;
        }

        const DWORD value = visible ? 1u : 0u;

        result = RegSetValueExW(
            key,
            kValueName,
            0,
            REG_DWORD,
            reinterpret_cast<const BYTE*>(&value),
            sizeof(value)
        );

        RegCloseKey(key);

        return result == ERROR_SUCCESS;
    }
}

// 注册表操作
namespace Registry
{
    bool SetHomeFolderVisible(bool visible)
    {
        return SetPinnedValue(
            kHomeFolderClsid,
            visible
        );
    }

    bool SetGalleryVisible(bool visible)
    {
        return SetPinnedValue(
            kGalleryClsid,
            visible
        );
    }

    bool SetAllVisible(bool visible)
    {
        const bool homeOk =
            SetPinnedValue(
                kHomeFolderClsid,
                visible
            );

        const bool galleryOk =
            SetPinnedValue(
                kGalleryClsid,
                visible
            );

        return homeOk && galleryOk;
    }
}
