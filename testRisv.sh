#!/bin/bash
set -xue

# QEMU 模拟器
QEMU=qemu-system-riscv32

# clang 路径和编译器标志
CC="clang -v" # Ubuntu 用户：使用 CC=clang
CFLAGS="-std=c11 -O2 -g3 -Wall -Wextra --target=riscv32-unknown-elf -fno-stack-protector -ffreestanding -nostdlib"
OBJCOPY=llvm-objcopy

# Build the shell (application)
$CC $CFLAGS -Wl,-Tfuck_user.ld -Wl,-Map=fuck_shell.map -B/usr/riscv64-linux-gnu/bin -o fuck_shell.elf fuck_shell.c fuck_user.c fuck_common.c
$OBJCOPY --set-section-flags .bss=alloc,contents -O binary fuck_shell.elf fuck_shell.bin
$OBJCOPY -Ibinary -Oelf32-littleriscv fuck_shell.bin fuck_shell.bin.o

# 构建内核
$CC $CFLAGS -Wl,-Tfuck_kernel.ld -Wl,-Map=fuck_kernel.map -B/usr/riscv64-linux-gnu/bin -o fuck_kernel.elf \
    fuck_kernel.c fuck_common.c fuck_shell.bin.o

#新添加的选项解释如下：

# -drive id=drive0：定义一个名为 drive0 的磁盘，使用 lorem.txt 作为磁盘镜像。磁盘镜像格式为 raw(将文件内容按原样作为磁盘数据处理)。
# -device virtio-blk-device：添加一个带有 drive0 磁盘的 virtio-blk 设备。bus=virtio-mmio-bus.0 将设备映射到 virtio-mmio 总线(通过内存映射 I/O 的 virtio)。
# 启动 QEMU
$QEMU -machine virt -bios default -nographic -serial mon:stdio --no-reboot \
    -d unimp,guest_errors,int,cpu_reset -D qemu.log \
    -drive id=drive0,file=lorem.txt,format=raw,if=none \
    -device virtio-blk-device,drive=drive0,bus=virtio-mmio-bus.0 \
    -kernel fuck_kernel.elf