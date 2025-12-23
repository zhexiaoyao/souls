# 3D收集游戏 - 独立项目

这是一个基于Souls Engine开发的独立3D收集游戏项目。

## 项目结构

```
game/
├── CMakeLists.txt          # 游戏独立构建配置
├── main.cpp                # 游戏主程序入口
├── README.md               # 本文件
└── assets/                 # 游戏资源（符号链接到父目录）
    └── shaders/            # Shader文件
```

## 编译和运行

### 方法1：作为Souls Engine的一部分编译

在 `Souls-Engine/build` 目录下：
```bash
cmake --build . --config Debug --target SoulsEngine_Game
```

运行：
```bash
cd bin\Debug
SoulsEngine_Game.exe
```

### 方法2：独立编译（需要先编译引擎）

1. 确保Souls Engine已经编译过
2. 在 `game` 目录下创建 `build` 目录：
```bash
cd game
mkdir build
cd build
cmake ..
cmake --build . --config Debug
```

## 游戏说明

- **目标**: 在60秒内收集尽可能多的黄色立方体
- **控制**: WASD移动，鼠标旋转视角，R重新开始，ESC退出
- **得分**: 每个收集物10分，撞到障碍物扣5分

## 依赖

游戏依赖以下Souls Engine组件：
- 核心系统（Window, OpenGLContext, Shader, Camera）
- 场景系统（ObjectManager, SceneNode, Node）
- 几何体（Cube, Sphere, Cylinder）
- 游戏管理器（GameManager）
- 光源系统（Light, LightManager）
- UI系统（ImGui）





