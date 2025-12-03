# GLM 快速安装脚本 (Windows PowerShell)
# 使用方法: .\install_glm.ps1

$ErrorActionPreference = "Stop"

# 获取项目根目录
$scriptPath = Split-Path -Parent $MyInvocation.MyCommand.Path
$projectRoot = $scriptPath
$glmPath = Join-Path $projectRoot "extern\glm"

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "GLM 安装脚本" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# 检查是否已安装
if (Test-Path $glmPath) {
    Write-Host "GLM 已存在于: $glmPath" -ForegroundColor Yellow
    Write-Host "跳过安装..." -ForegroundColor Yellow
    exit 0
}

# 检查 Git 是否可用
$gitAvailable = $false
try {
    $null = git --version 2>&1
    $gitAvailable = $true
} catch {
    $gitAvailable = $false
}

if ($gitAvailable) {
    Write-Host "检测到 Git，使用 Git Clone 方式下载..." -ForegroundColor Green
    Write-Host ""
    
    # 进入 extern 目录
    $externPath = Join-Path $projectRoot "extern"
    if (-not (Test-Path $externPath)) {
        New-Item -ItemType Directory -Path $externPath | Out-Null
    }
    
    Set-Location $externPath
    
    # 尝试从 GitHub 克隆
    Write-Host "正在从 GitHub 下载 GLM..." -ForegroundColor Yellow
    try {
        git clone https://github.com/g-truc/glm.git
        Write-Host "GLM 安装成功！" -ForegroundColor Green
        exit 0
    } catch {
        Write-Host "GitHub 下载失败，尝试使用 Gitee 镜像..." -ForegroundColor Yellow
        try {
            git clone https://gitee.com/mirrors/glm.git
            Write-Host "GLM 安装成功（使用 Gitee 镜像）！" -ForegroundColor Green
            exit 0
        } catch {
            Write-Host "Git Clone 失败，请使用手动下载方式" -ForegroundColor Red
        }
    }
} else {
    Write-Host "未检测到 Git，请使用手动下载方式" -ForegroundColor Yellow
    Write-Host ""
}

# 如果 Git 不可用，提供手动下载说明
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "手动下载说明" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "1. 访问以下链接下载 GLM:" -ForegroundColor Yellow
Write-Host "   https://github.com/g-truc/glm/archive/refs/heads/master.zip" -ForegroundColor Cyan
Write-Host ""
Write-Host "2. 解压 ZIP 文件到以下目录:" -ForegroundColor Yellow
Write-Host "   $glmPath" -ForegroundColor Cyan
Write-Host ""
Write-Host "3. 确保目录结构为:" -ForegroundColor Yellow
Write-Host "   extern\glm\glm\glm.hpp" -ForegroundColor Cyan
Write-Host ""

# 尝试自动下载（如果 PowerShell 版本支持）
if ($PSVersionTable.PSVersion.Major -ge 5) {
    Write-Host "尝试使用 PowerShell 直接下载..." -ForegroundColor Yellow
    try {
        $zipPath = Join-Path $env:TEMP "glm-master.zip"
        $downloadUrl = "https://github.com/g-truc/glm/archive/refs/heads/master.zip"
        
        Write-Host "正在下载 GLM ZIP 文件..." -ForegroundColor Yellow
        Invoke-WebRequest -Uri $downloadUrl -OutFile $zipPath -UseBasicParsing
        
        Write-Host "正在解压..." -ForegroundColor Yellow
        $externPath = Join-Path $projectRoot "extern"
        if (-not (Test-Path $externPath)) {
            New-Item -ItemType Directory -Path $externPath | Out-Null
        }
        
        Expand-Archive -Path $zipPath -DestinationPath $externPath -Force
        
        # 重命名文件夹
        $extractedPath = Join-Path $externPath "glm-master"
        if (Test-Path $extractedPath) {
            if (Test-Path $glmPath) {
                Remove-Item $glmPath -Recurse -Force
            }
            Rename-Item -Path $extractedPath -NewName "glm"
        }
        
        # 清理临时文件
        Remove-Item $zipPath -Force
        
        if (Test-Path $glmPath) {
            Write-Host "GLM 安装成功！" -ForegroundColor Green
            exit 0
        }
    } catch {
        Write-Host "自动下载失败: $_" -ForegroundColor Red
        Write-Host "请使用上面的手动下载方式" -ForegroundColor Yellow
    }
}

Write-Host ""
Write-Host "安装脚本执行完成" -ForegroundColor Cyan

