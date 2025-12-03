#!/bin/bash
# GLM 快速安装脚本 (Linux/macOS)
# 使用方法: ./install_glm.sh

set -e

# 获取脚本所在目录
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$SCRIPT_DIR"
GLM_PATH="$PROJECT_ROOT/extern/glm"

echo "========================================"
echo "GLM 安装脚本"
echo "========================================"
echo ""

# 检查是否已安装
if [ -d "$GLM_PATH" ]; then
    echo "GLM 已存在于: $GLM_PATH"
    echo "跳过安装..."
    exit 0
fi

# 检查 Git 是否可用
if command -v git &> /dev/null; then
    echo "检测到 Git，使用 Git Clone 方式下载..."
    echo ""
    
    # 进入 extern 目录
    EXTERN_PATH="$PROJECT_ROOT/extern"
    mkdir -p "$EXTERN_PATH"
    cd "$EXTERN_PATH"
    
    # 尝试从 GitHub 克隆
    echo "正在从 GitHub 下载 GLM..."
    if git clone https://github.com/g-truc/glm.git 2>/dev/null; then
        echo "GLM 安装成功！"
        exit 0
    else
        echo "GitHub 下载失败，尝试使用 Gitee 镜像..."
        if git clone https://gitee.com/mirrors/glm.git 2>/dev/null; then
            echo "GLM 安装成功（使用 Gitee 镜像）！"
            exit 0
        else
            echo "Git Clone 失败，请使用手动下载方式"
        fi
    fi
else
    echo "未检测到 Git，尝试使用 curl/wget 下载..."
fi

# 如果 Git 不可用，尝试使用 curl 或 wget 下载
EXTERN_PATH="$PROJECT_ROOT/extern"
mkdir -p "$EXTERN_PATH"
cd "$EXTERN_PATH"

DOWNLOAD_URL="https://github.com/g-truc/glm/archive/refs/heads/master.zip"
ZIP_FILE="glm-master.zip"

if command -v curl &> /dev/null; then
    echo "使用 curl 下载 GLM..."
    curl -L -o "$ZIP_FILE" "$DOWNLOAD_URL"
elif command -v wget &> /dev/null; then
    echo "使用 wget 下载 GLM..."
    wget -O "$ZIP_FILE" "$DOWNLOAD_URL"
else
    echo "========================================"
    echo "手动下载说明"
    echo "========================================"
    echo ""
    echo "1. 访问以下链接下载 GLM:"
    echo "   $DOWNLOAD_URL"
    echo ""
    echo "2. 解压 ZIP 文件到以下目录:"
    echo "   $GLM_PATH"
    echo ""
    echo "3. 确保目录结构为:"
    echo "   extern/glm/glm/glm.hpp"
    echo ""
    exit 1
fi

# 解压文件
if command -v unzip &> /dev/null; then
    echo "正在解压..."
    unzip -q "$ZIP_FILE"
    
    # 重命名文件夹
    if [ -d "glm-master" ]; then
        if [ -d "glm" ]; then
            rm -rf glm
        fi
        mv glm-master glm
    fi
    
    # 清理 ZIP 文件
    rm -f "$ZIP_FILE"
    
    if [ -d "$GLM_PATH" ]; then
        echo "GLM 安装成功！"
        exit 0
    else
        echo "解压失败，请检查文件"
        exit 1
    fi
else
    echo "未找到 unzip 命令，请手动解压 $ZIP_FILE"
    exit 1
fi

