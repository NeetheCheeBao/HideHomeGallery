<div align="center">

<h1>主文件夹和图库隐藏工具</h1>

<img src="resources/app.ico" width="128" alt="logo" />

[![Platform](https://img.shields.io/badge/Platform-Windows%2011-blue.svg)](https://www.microsoft.com/windows)
[![Language](https://img.shields.io/badge/Language-C%2B%2B-00599C.svg)](https://isocpp.org/)
[![UI](https://img.shields.io/badge/UI-Win32-purple.svg)](https://learn.microsoft.com/windows/win32/)
[![Build](https://img.shields.io/badge/Build-CMake-064F8C.svg)](https://cmake.org/)
[![License](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

用于在资源管理器导航窗格中快速隐藏或显示 **主文件夹（Home）** 和 **图库（Gallery）**

</div>

## 📸 工具截图

![img](/screenshot/demo1.png)

## 🛠️ 原理

工具通过修改以下注册表项控制显示状态：

| 项目       | CLSID                                      | 值名称                          |
|------------|--------------------------------------------|---------------------------------|
| 主文件夹   | `{f874310e-b6b7-47dc-bc84-b9e6b38f5903}`   | `System.IsPinnedToNameSpaceTree` |
| 图库       | `{e88865ea-0e1c-4e20-9aa6-edcd0212c87c}`   | `System.IsPinnedToNameSpaceTree` |

路径：`HKEY_CURRENT_USER\Software\Classes\CLSID\{CLSID}`

- 值设为 `1` → 显示
- 值设为 `0` → 隐藏

## ⬇️ 下载使用

[![Releases](https://img.shields.io/badge/Download%20Releases-7C25FF?style=for-the-badge&logoColor=white")](https://github.com/NeetheCheeBao/HideHomeGallery/releases)

## 本地编译

1. 克隆仓库

```bash
git clone https://github.com/NeetheCheeBao/HideHomeGallery.git
```

或

```bash
gh repo clone NeetheCheeBao/HideHomeGallery
```

2. 一键编译
```bash
.\build.bat
```

3. 产物路径：

```text
build\Release\HideHomeGallery.exe
```

## 📁 项目结构

```text
HideHomeGallery
|   .gitignore
|   build.bat               # 一键编译脚本
|   CMakeLists.txt          # CMake 构建配置
|   LICENSE                 # 许可证
|   README.md               # 项目说明
|   
+---resources
|       app.ico             # 程序图标
|       app.manifest        # 应用程序清单
|       app.rc              # 资源脚本
|       resource.h          # 资源 ID 定义
|       
+---screenshot
|       demo1.png           # 截图
|       
\---src
        main.cpp            # 程序入口
        Registry.cpp        # 注册表操作实现
        Registry.h          # 注册表操作接口
        Window.cpp          # 主窗口与界面逻辑
        Window.h            # 主窗口类声明
```

## ⚖️ 许可证

本项目采用 MIT 许可证 - 详情请参阅 [LICENSE](LICENSE) 文件