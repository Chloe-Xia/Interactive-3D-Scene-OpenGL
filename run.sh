#!/bin/bash

# 获取当前文件的完整路径
current_file="$1"
if [ -z "$current_file" ]; then
    echo "Please provide a file path"
    exit 1
fi

# 获取目录名（不含文件名）
dir_path=$(dirname "$current_file")
dir_name=$(basename "$dir_path")

# 构建目标名称（直接使用目录名，不添加额外下划线）
target="${dir_name}"

echo "Current file: $current_file"
echo "Directory: $dir_path"
echo "Directory name: $dir_name"
echo "Target: $target"

# 确保 build 目录存在
if [ ! -d "build" ]; then
    echo "Creating build directory..."
    mkdir build
    cd build
    cmake ..
    cd ..
fi

# 编译
echo "Building target: $target"
cmake --build build --target "$target"

# 如果编译成功，运行程序
if [ $? -eq 0 ]; then
    echo "Running program..."
    # 在 bin/GL-Project 目录下找
    if [ -f "bin/${dir_name}/${target}" ]; then
        (cd "bin/${dir_name}" && "./${target}")
    else
        echo "Executable not found in bin/${dir_name}/${target}!"
    fi
else
    echo "Build failed"
fi 