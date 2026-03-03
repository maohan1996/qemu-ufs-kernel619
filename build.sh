#!/bin/bash

# 第一步：校验参数数量（必须传入1个参数）
if [ $# -ne 1 ]; then
    echo "❌ 错误：请传入1个参数（仅支持[cfg|bd|dtb]）！"
    echo "✅ 用法：$0 [cfg|bd|dtb]"
    exit 1
fi

# 提取参数并清洗（去空格、转数字，避免非法字符）
param=$(echo "$1" | xargs)  # 去首尾空格

if [ "${param}" == "c" ]; then
    make ARCH=arm CROSS_COMPILE=arm-none-linux-gnueabihf- O=/home/maohan/linux/qemu/kernel/buildout vexpress_defconfig

elif [ "${param}" == "b" ]; then
    make ARCH=arm CROSS_COMPILE=arm-none-linux-gnueabihf- O=/home/maohan/linux/qemu/kernel/buildout -j16

elif [ "${param}" == "d" ]; then
    make ARCH=arm CROSS_COMPILE=arm-none-linux-gnueabihf- O=/home/maohan/linux/qemu/kernel/buildout dtbs -j16 

elif [ "${param}" == "ac" ]; then
    make ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- O=/home/maohan/linux/qemu/kernel/arm64_buildout defconfig

elif [ "${param}" == "ab" ]; then
    make ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- O=/home/maohan/linux/qemu/kernel/arm64_buildout -j16
else
    echo "❌ 错误：参数仅支持[cfg|bd|dtb]，你输入了${param}！"
    exit 1
fi

echo -e "\n🎉 所有操作执行完成！"






