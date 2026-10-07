#pragma once

namespace Registry
{
    // 设置主文件夹显示或隐藏
    bool SetHomeFolderVisible(bool visible);

    // 设置图库显示或隐藏
    bool SetGalleryVisible(bool visible);

    // 同时设置主文件夹和图库
    bool SetAllVisible(bool visible);
}