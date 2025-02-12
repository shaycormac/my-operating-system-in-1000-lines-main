#!/bin/bash
set -xue

# QEMU 文件路径
QEMU=qemu-system-riscv32

# clang ·���ͱ�������־
CC=clang  # Ubuntu �û���ʹ�� CC=clang
CFLAGS="-std=c11 -O2 -g3 -Wall -Wextra --target=riscv32-unknown-elf -fno-stack-protector -ffreestanding -nostdlib"

# �����ں�
$CC $CFLAGS -Wl,-Tfuck_kernel.ld -Wl,-Map=fuck_kernel.map -B/usr/riscv64-linux-gnu/bin -o fuck_kernel.elf \
    fuck_kernel.c fuck_common.c

# ���� QEMU
$QEMU -machine virt -bios default -nographic -serial mon:stdio --no-reboot \
    -kernel fuck_kernel.elf