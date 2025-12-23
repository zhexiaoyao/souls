# Souls Engine - Souls-Like 游戏渲染引擎

## 项目概述

Souls Engine 是一个基于 OpenGL 的 Souls-Like 游戏渲染引擎，使用 C++17 开发。项目已完成基础的渲染系统、场景图系统、变换系统和交互式编辑器功能。

## 项目结构

```
Souls-Engine/
├── CMakeLists.txt          # CMake构建配置
├── src/
│   ├── main.cpp            # 程序入口
│   ├── core/               # 核心系统
│   │   ├── Window.h/cpp     # 窗口管理
│   │   ├── OpenGLContext.h/cpp  # OpenGL上下文
│   │   ├── Shader.h/cpp     # Shader管理
│   │   ├── Camera.h/cpp     # 相机系统
│   │   ├── Node.h/cpp       # 场景节点
│   │   ├── Scene.h/cpp      # 场景图
│   │   ├── SceneNode.h/cpp  # 可渲染节点
│   │   ├── Transform.h/cpp  # 变换系统
│   │   ├── ObjectManager.h/cpp  # 对象管理器
│   │   └── SelectionSystem.h/cpp # 选择系统
│   └── geometry/           # 几何体
│       ├── Mesh.h/cpp      # 网格基类
│       ├── Cube.h/cpp      # 立方体
│       ├── Sphere.h/cpp    # 球体
│       ├── Cylinder.h/cpp  # 圆柱体
│       ├── Cone.h/cpp      # 圆锥体
│       ├── Prism.h/cpp     # 棱柱
│       └── Frustum.h/cpp   # 棱台
├── include/
│   ├── glad/               # GLAD头文件
│   └── KHR/                # Khronos平台头文件
├── extern/
│   ├── glfw/               # GLFW库（子项目）
│   ├── glm/                # GLM数学库
│   └── stb/                # stb_image库
└── assets/                 # 资源文件
    └── shaders/            # Shader文件
```

## 技术栈

- **语言**: C++17
- **渲染API**: OpenGL 3.3 Core Profile
- **窗口库**: GLFW 3.3+
- **OpenGL加载**: GLAD
- **数学库**: GLM
- **构建系统**: CMake 3.15+
- **图像加载**: stb_image

## 前置要求

### 必需软件

1. **CMake** 3.15 或更高版本
   - Windows: 从 [cmake.org](https://cmake.org/download/) 下载安装
   - Linux: `sudo apt-get install cmake` (Ubuntu/Debian)
   - macOS: `brew install cmake`

2. **C++17 兼容的编译器**
   - Windows: Visual Studio 2019/2022 (MSVC) 或 MinGW-w64
   - Linux: GCC 7+ 或 Clang 5+
   - macOS: Xcode Command Line Tools (Clang)

3. **Git** (用于下载依赖，如果使用子项目)

### 依赖库状态

项目已包含以下依赖：
- ✅ **GLFW**: 已作为子项目包含在 `extern/glfw`
- ✅ **GLAD**: 头文件和加载器已实现
- ⚠️ **GLM**: 需要手动配置（见下方说明）
- ✅ **stb_image**: 占位文件已创建

## 构建步骤

### Windows 构建（Visual Studio）

```powershell
# 进入项目目录
cd C:\code\computer_graphics\final_cursor1.0\Souls-Engine

# 创建构建目录
mkdir build
cd build

# 配置项目（使用 Visual Studio 2022）
cmake .. -G "Visual Studio 17 2022" -A x64

# 编译项目
cmake --build . --config Debug

# 运行程序
.\bin\Debug\SoulsEngine.exe
```

### Linux 构建

```bash
# 进入项目目录
cd ~/code/computer_graphics/final_cursor1.0/Souls-Engine

# 创建构建目录
mkdir build
cd build

# 配置项目
cmake .. -DCMAKE_BUILD_TYPE=Debug

# 编译项目
make -j4

# 运行程序
./bin/SoulsEngine
```

### macOS 构建

```bash
# 进入项目目录
cd ~/code/computer_graphics/final_cursor1.0/Souls-Engine

# 创建构建目录
mkdir build
cd build

# 配置项目
cmake .. -DCMAKE_BUILD_TYPE=Debug

# 编译项目
make -j4

# 运行程序
./bin/SoulsEngine
```

## GLM 配置

### 方法1: 使用 Git（推荐）

```powershell
cd extern
git clone https://github.com/g-truc/glm.git
cd ..
```

### 方法2: 使用系统包管理器

**Linux (Ubuntu/Debian)**:
```bash
sudo apt-get install libglm-dev
```

**macOS**:
```bash
brew install glm
```

### 方法3: 使用 vcpkg (Windows)

```powershell
# 安装 vcpkg（如果还没有）
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat

# 安装 GLM
.\vcpkg install glm:x64-windows

# 配置 CMake 使用 vcpkg
cmake .. -DCMAKE_TOOLCHAIN_FILE=[vcpkg目录]/scripts/buildsystems/vcpkg.cmake
```

## 运行说明

### 快速开始

如果已经构建过：

```powershell
cd build
cmake --build . --config Debug
.\bin\Debug\SoulsEngine.exe
```

### 运行效果

运行成功后，你应该看到：

1. **控制台输出**：
   ```
   OpenGL Info:
     Vendor: [你的显卡厂商]
     Renderer: [你的显卡型号]
     Version: OpenGL [版本号]
     GLSL Version: [GLSL版本]
   Souls Engine initialized successfully!
   Window size: 1280x720
   ```

2. **窗口显示**：
   - 一个 1280x720 的窗口
   - 标题: "Souls Engine"
   - 窗口中可以渲染和操作几何体

## 操作指南

### 键盘控制

#### 创建几何体
- `1`: 创建立方体（Cube，红色）
- `2`: 创建球体（Sphere，绿色）
- `3`: 创建圆柱体（Cylinder，蓝色）
- `4`: 创建圆锥体（Cone，黄色）
- `5`: 创建棱柱（Prism，紫色）
- `6`: 创建棱台（Frustum，青色）

#### 交互控制
- `R`: 切换旋转模式（ON/OFF）
- `ESC`: 关闭窗口

#### 相机控制
- `W/A/S/D`: 移动相机（前后左右）
- `Q/E`: 移动相机（上下）

### 鼠标交互

- **左键点击**: 选择对象
- **点击空白处**: 取消选择
- **拖拽鼠标**: 
  - 默认模式：平移选中的对象
  - 旋转模式（按R键后）：旋转选中的对象

## 核心功能

### 1. 窗口系统
- 创建和管理 OpenGL 窗口
- 窗口事件处理
- 缓冲区交换

### 2. OpenGL 上下文管理
- GLAD 初始化
- OpenGL 状态配置
- 错误检查机制

### 3. Shader 系统
- 从文件加载 GLSL 着色器
- Shader 编译和链接
- Uniform 变量设置接口
- 完整的错误处理机制

### 4. 相机系统
- 视图矩阵计算
- 投影矩阵计算
- MVP 矩阵计算
- WASD 键盘控制
- 鼠标控制和滚轮缩放

### 5. 几何体渲染
- 立方体（Cube）
- 球体（Sphere）
- 圆柱体（Cylinder）
- 圆锥体（Cone）
- 棱柱（Prism）
- 棱台（Frustum）

### 6. 场景图系统
- 节点层次结构
- 父子节点关系
- 场景遍历和渲染
- 对象管理器

### 7. 变换系统
- 位置、旋转、缩放变换
- 局部/世界坐标转换
- 变换矩阵计算
- 绝对变换和相对变换

### 8. 交互式编辑器
- 对象选择和操作
- 鼠标拖拽平移/旋转
- 动态创建几何体

## 常见问题

### 问题1: CMake 找不到 GLM

**解决方案**:
```powershell
cd extern
git clone https://github.com/g-truc/glm.git
cd ../build
cmake ..
```

### 问题2: 编译错误

**解决方案**:
- 确保所有依赖都已正确配置
- 检查 CMake 版本是否 >= 3.15
- 检查编译器是否支持 C++17
- 清理构建目录后重新配置

### 问题3: 运行时错误 - 找不到 Shader 文件

**解决方案**:
- 确保 `assets` 目录已复制到 `build/bin/Debug/` 目录
- CMakeLists.txt 应该自动复制，如果没有，手动复制：
  ```powershell
  Copy-Item -Recurse ..\assets build\bin\Debug\assets
  ```

### 问题4: OpenGL 版本不支持

**解决方案**:
- 更新显卡驱动
- 确保支持 OpenGL 3.3+
- 检查显卡是否支持硬件加速

## 开发环境推荐

- **Windows**: Visual Studio 2019/2022 或 CLion
- **Linux**: CLion 或 VS Code + CMake 插件
- **macOS**: Xcode 或 CLion

## 项目状态

✅ **已完成功能**:
- 项目搭建和构建系统
- 基础渲染系统
- Shader 管理系统
- 相机系统
- 基础几何体渲染
- 场景图系统
- 变换系统
- 交互式编辑器

📋 **详细进度**: 请查看 `项目进度报告.md`

## 许可证

本项目遵循开发计划表中的要求，使用 OpenGL + C++17 技术栈。

## 获取帮助

如果遇到问题：
1. 检查控制台错误信息
2. 查看 CMake 配置输出
3. 验证所有依赖是否正确安装
4. 检查文件路径是否正确
5. 确保 OpenGL 驱动是最新的

## 导入外部模型（OBJ / Blender / Assimp）

- 支持的导入方式：
  - 内置轻量 OBJ 导入器（无需额外依赖）：通过 `src/io/OBJLoader.cpp` 实现，导入后会把顶点转换为引擎当前的顶点格式（position(3) + color(3)），颜色会根据法线映射为可视化颜色。
  - Assimp（可选）：如果系统上安装并且 CMake 能找到 `assimp`，构建时会启用 `AssimpLoader`（`src/io/AssimpLoader.cpp`），Assimp 能读取更多格式（如 `.blend`, `.fbx`, `.dae` 等）。

- 运行时加载模型：
  - 命令行参数传入模型路径即可在启动时加载并显示模型，例如：

```powershell
.\bin\Debug\SoulsEngine.exe assets/models/examples/triangle.obj
```

  - 如果不传入参数，程序会尝试加载内置示例 `assets/models/examples/triangle.obj`（如果存在）。

- Blender 导出脚本（批量转换）：
  - 附带示例脚本 `tools/blender_export.py`，可在命令行调用 Blender 将 `.blend` 导出为 OBJ 或 glTF：

```bash
blender -b input.blend --python tools/blender_export.py -- output.obj
```

- 在 Windows 上启用 Assimp（可选）：
  - 推荐使用 `vcpkg` 安装：在 `vcpkg` 环境中运行 `.\vcpkg install assimp:x64-windows`，然后在 CMake 配置时传入 `-DCMAKE_TOOLCHAIN_FILE=[vcpkg]/scripts/buildsystems/vcpkg.cmake`。
  - 如果 CMake 找到 assimp，会自动把 `AssimpLoader` 加入构建并链接 `assimp::assimp`，否则项目仍可正常构建（使用内置 OBJ 导入）。


祝开发顺利！🚀
