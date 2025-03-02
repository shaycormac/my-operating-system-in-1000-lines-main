#pragma once

#include "fuck_common.h"

struct sbiret {
    long error;
    long value;
};

/**
 * 内核栈包含保存的 CPU 寄存器、返回地址（从何处调用）和局部变量。
 * 通过为每个进程准备一个内核栈，我们可以通过保存和恢复 CPU 寄存器并切换栈指针来实现上下文切换。
 */
struct process{
    int pid;// 进程 ID
    int state;// 进程状态: PROC_UNUSED 或 PROC_RUNNABLE
    vaddr_t sp;// 栈指针
    uint32_t *page_table;
    uint8_t stack[8192];// 内核栈
};

// 构建页表
// 让我们在 Sv32 中构建页表。首先，我们定义一些宏。SATP_SV32 是 satp 寄存器中表示“在 Sv32 模式下启用分页”的单个位，
//而 PAGE_* 是要在页表项中设置的标志。
#define SATP_SV32 (1u << 31)
#define PAGE_V    (1 << 0)   // "Valid" 位（表项已启用）
#define PAGE_R    (1 << 1)   // 可读
#define PAGE_W    (1 << 2)   // 可写
#define PAGE_X    (1 << 3)   // 可执行
#define PAGE_U    (1 << 4)   // 用户（用户模式可访问）

#define SSTATUS_SPIE (1 << 5)

#define SCAUSE_ECALL 8 // 调用系统调用

#define PROC_EXITED 2

// 定义与virtio的相关
#define SECTOR_SIZE 512
#define VIRTQ_ENTRY_NUM 16
#define VIRTIO_DEVICE_BLK 2
#define VIRTIO_BLK_PADDR 0x10001000
#define VIRTIO_REG_MAGIC 0x00
#define VIRTIO_REG_VERSION       0x04
#define VIRTIO_REG_DEVICE_ID     0x08
#define VIRTIO_REG_QUEUE_SEL     0x30
#define VIRTIO_REG_QUEUE_NUM_MAX 0x34
#define VIRTIO_REG_QUEUE_NUM     0x38
#define VIRTIO_REG_QUEUE_ALIGN   0x3c
#define VIRTIO_REG_QUEUE_PFN     0x40
#define VIRTIO_REG_QUEUE_READY   0x44
#define VIRTIO_REG_QUEUE_NOTIFY  0x50
#define VIRTIO_REG_DEVICE_STATUS 0x70
#define VIRTIO_REG_DEVICE_CONFIG 0x100
#define VIRTIO_STATUS_ACK       1
#define VIRTIO_STATUS_DRIVER    2
#define VIRTIO_STATUS_DRIVER_OK 4
#define VIRTIO_STATUS_FEAT_OK   8
#define VIRTQ_DESC_F_NEXT          1
#define VIRTQ_DESC_F_WRITE         2
#define VIRTQ_AVAIL_F_NO_INTERRUPT 1
#define VIRTIO_BLK_T_IN  0
#define VIRTIO_BLK_T_OUT 1

// Virtqueque Descriptor 结构体
struct virtq_desc {
  uint64_t addr;
  uint32_t len;
  uint16_t flags;
  uint16_t next;
} __attribute__((packed));

// Virtqueue Available Ring 结构体
struct virtq_avail {
  uint16_t flags;
  uint16_t idx;
  uint16_t ring[VIRTQ_ENTRY_NUM];
} __attribute__((packed));

// Virtqueue Used Ring 条目
struct virtq_used_elem{
  uint32_t id;
  uint32_t len;
} __attribute__((packed));

// Virtqueue Used Ring 结构体
struct virtq_used{
  uint16_t flags;
  uint16_t index;
  struct virtq_used_elem ring[VIRTQ_ENTRY_NUM];
} __attribute__((packed));


// Virtqueue 结构体
/**
 * 结构体 virtio_queque 描述了一个 Virtio 队列，用于在主机和客户机之间高效传输数据。
 * 该结构体包含描述符环、可用环和已用环，分别用于描述数据块、提供数据块和确认数据块的接收。
 */
struct virtio_queque{
  // descs 数组存储了描述符项，每个描述符项描述了一个数据块的位置和大小。
  struct virtq_desc descs[VIRTQ_ENTRY_NUM];

  // avail 结构体是可用环，包含了发送方放置可用描述符索引的地方。
  struct virtq_avail avail;

  // used 结构体是已用环，接收方在此标记已接收的描述符。PAGE_SIZE 对齐确保了性能优化。
  struct virtq_used used __attribute__((aligned(PAGE_SIZE)));

  // queue_index 表示当前队列的索引，Virtio 设备可以有多个队列。
  int queue_index;

  // used_index 指向当前已用环的索引，volatile 关键字确保了多线程或多处理器环境下的内存访问一致性。
  volatile uint16_t *used_index;

  // last_used_index 记录了上一个处理的已用描述符索引，用于检测是否有新的数据包被接收。
  uint16_t  last_used_index;
} __attribute__((packed)); // packed 属性确保了结构体在内存中的紧凑布局，避免因对齐造成的空间浪费。


// Virtio-blk 请求
struct virtio_blk_req{
  uint32_t type;
  uint32_t reserved;
  uint32_t sector;
  uint8_t data[512];
  uint8_t status;
} __attribute__((packed));


// 定义为宏，这样做的原因是为了正确显示源文件名（__FILE__）和行号（__LINE__）。
// 如果我们将其定义为函数，__FILE__ 和 __LINE__ 将显示 PANIC 被定义的文件名和行号，而不是它被调用的位置。
#define PANIC(fmt, ...)                                                        \
    do {                                                                       \
        printf("PANIC: %s:%d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__);  \
        while (1) {}                                                           \
    } while (0)
// 第一个惯用语是 do-while 语句。由于它是 while (0)，这个循环只执行一次。这是定义由多个语句组成的宏的常见方式。
//简单地用 { ...} 封装可能会在与 if 等语句组合时导致意外的行为（参见这个清晰的例子）。
// 另外，注意每行末尾的反斜杠（\）。虽然宏是在多行上定义的，但在展开时换行符会被忽略。

// 第二个惯用语是 ##__VA_ARGS__。这是一个用于定义接受可变数量参数的宏的有用编译器扩展（参考：GCC 文档）。
// 当可变参数为空时，## 会删除前面的 ,。这使得即使只有一个参数，如 PANIC("booted!")，编译也能成功。


// trap_frame 结构体表示在 kernel_entry 中保存的程序状态
struct trap_frame
{
    uint32_t ra;
    uint32_t gp;
    uint32_t tp;
    uint32_t t0;
    uint32_t t1;
    uint32_t t2;
    uint32_t t3;
    uint32_t t4;
    uint32_t t5;
    uint32_t t6;
    uint32_t a0;
    uint32_t a1;
    uint32_t a2;
    uint32_t a3;
    uint32_t a4;
    uint32_t a5;
    uint32_t a6;
    uint32_t a7;
    uint32_t s0;
    uint32_t s1;
    uint32_t s2;
    uint32_t s3;
    uint32_t s4;
    uint32_t s5;
    uint32_t s6;
    uint32_t s7;
    uint32_t s8;
    uint32_t s9;
    uint32_t s10;
    uint32_t s11;
    uint32_t sp;
} __attribute__((packed));


//READ_CSR 和 WRITE_CSR 宏是用于读写 CSR 寄存器的便捷宏。
#define READ_CSR(reg) \
({ \
    unsigned long __tmp; \
    __asm__ __volatile__("csrr %0, " #reg : "=r"(__tmp)); \
    __tmp; \
})


#define WRITE_CSR(reg, value)                                                  \
    do {                                                                       \
        uint32_t __tmp = (value);                                              \
        __asm__ __volatile__("csrw " #reg ", %0" ::"r"(__tmp));                \
    } while (0)

