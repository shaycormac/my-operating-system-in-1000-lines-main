#!/bin/bash
set -xue

# QEMU 文件路径
QEMU=qemu-system-riscv32

# clang ·���ͱ�������־
CC="clang -v" # Ubuntu �û���ʹ�� CC=clang
CFLAGS="-std=c11 -O2 -g3 -Wall -Wextra --target=riscv32-unknown-elf -fno-stack-protector -ffreestanding -nostdlib"
OBJCOPY=llvm-objcopy

# Build the shell (application)
$CC $CFLAGS -Wl,-Tfuck_user.ld -Wl,-Map=fuck_shell.map -B/usr/riscv64-linux-gnu/bin -o fuck_shell.elf fuck_shell.c fuck_user.c fuck_common.c
$OBJCOPY --set-section-flags .bss=alloc,contents -O binary fuck_shell.elf fuck_shell.bin
$OBJCOPY -Ibinary -Oelf32-littleriscv fuck_shell.bin fuck_shell.bin.o

# �����ں�
$CC $CFLAGS -Wl,-Tfuck_kernel.ld -Wl,-Map=fuck_kernel.map -B/usr/riscv64-linux-gnu/bin -o fuck_kernel.elf \
    fuck_kernel.c fuck_common.c fuck_shell.bin.o

# ���� QEMU
$QEMU -machine virt -bios default -nographic -serial mon:stdio --no-reboot \
    -kernel fuck_kernel.elf