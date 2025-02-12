#!/bin/bash
set -xue

# QEMU 鏂囦欢璺緞
QEMU=qemu-system-riscv32

# clang 路径和编译器标志
CC=clang  # Ubuntu 用户：使用 CC=clang
CFLAGS="-std=c11 -O2 -g3 -Wall -Wextra --target=riscv32-unknown-elf -fno-stack-protector -ffreestanding -nostdlib"

# 构建内核
$CC $CFLAGS -Wl,-Tfuck_kernel.ld -Wl,-Map=fuck_kernel.map -B/usr/riscv64-linux-gnu/bin -o fuck_kernel.elf \
    fuck_kernel.c fuck_common.c

# 启动 QEMU
$QEMU -machine virt -bios default -nographic -serial mon:stdio --no-reboot \
    -kernel fuck_kernel.elf