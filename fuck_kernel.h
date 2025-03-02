#pragma once

#include "fuck_common.h"

struct sbiret {
    long error;
    long value;
};

/**
 * �ں�ջ��������� CPU �Ĵ��������ص�ַ���Ӻδ����ã��;ֲ�������
 * ͨ��Ϊÿ������׼��һ���ں�ջ�����ǿ���ͨ������ͻָ� CPU �Ĵ������л�ջָ����ʵ���������л���
 */
struct process{
    int pid;// ���� ID
    int state;// ����״̬: PROC_UNUSED �� PROC_RUNNABLE
    vaddr_t sp;// ջָ��
    uint32_t *page_table;
    uint8_t stack[8192];// �ں�ջ
};

// ����ҳ��
// �������� Sv32 �й���ҳ�����ȣ����Ƕ���һЩ�ꡣSATP_SV32 �� satp �Ĵ����б�ʾ���� Sv32 ģʽ�����÷�ҳ���ĵ���λ��
//�� PAGE_* ��Ҫ��ҳ���������õı�־��
#define SATP_SV32 (1u << 31)
#define PAGE_V    (1 << 0)   // "Valid" λ�����������ã�
#define PAGE_R    (1 << 1)   // �ɶ�
#define PAGE_W    (1 << 2)   // ��д
#define PAGE_X    (1 << 3)   // ��ִ��
#define PAGE_U    (1 << 4)   // �û����û�ģʽ�ɷ��ʣ�

#define SSTATUS_SPIE (1 << 5)

#define SCAUSE_ECALL 8 // ����ϵͳ����

#define PROC_EXITED 2

// ������virtio�����
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

// Virtqueque Descriptor �ṹ��
struct virtq_desc {
  uint64_t addr;
  uint32_t len;
  uint16_t flags;
  uint16_t next;
} __attribute__((packed));

// Virtqueue Available Ring �ṹ��
struct virtq_avail {
  uint16_t flags;
  uint16_t idx;
  uint16_t ring[VIRTQ_ENTRY_NUM];
} __attribute__((packed));

// Virtqueue Used Ring ��Ŀ
struct virtq_used_elem{
  uint32_t id;
  uint32_t len;
} __attribute__((packed));

// Virtqueue Used Ring �ṹ��
struct virtq_used{
  uint16_t flags;
  uint16_t index;
  struct virtq_used_elem ring[VIRTQ_ENTRY_NUM];
} __attribute__((packed));


// Virtqueue �ṹ��
/**
 * �ṹ�� virtio_queque ������һ�� Virtio ���У������������Ϳͻ���֮���Ч�������ݡ�
 * �ýṹ������������������û������û����ֱ������������ݿ顢�ṩ���ݿ��ȷ�����ݿ�Ľ��ա�
 */
struct virtio_queque{
  // descs ����洢���������ÿ����������������һ�����ݿ��λ�úʹ�С��
  struct virtq_desc descs[VIRTQ_ENTRY_NUM];

  // avail �ṹ���ǿ��û��������˷��ͷ����ÿ��������������ĵط���
  struct virtq_avail avail;

  // used �ṹ�������û������շ��ڴ˱���ѽ��յ���������PAGE_SIZE ����ȷ���������Ż���
  struct virtq_used used __attribute__((aligned(PAGE_SIZE)));

  // queue_index ��ʾ��ǰ���е�������Virtio �豸�����ж�����С�
  int queue_index;

  // used_index ָ��ǰ���û���������volatile �ؼ���ȷ���˶��̻߳�ദ���������µ��ڴ����һ���ԡ�
  volatile uint16_t *used_index;

  // last_used_index ��¼����һ��������������������������ڼ���Ƿ����µ����ݰ������ա�
  uint16_t  last_used_index;
} __attribute__((packed)); // packed ����ȷ���˽ṹ�����ڴ��еĽ��ղ��֣������������ɵĿռ��˷ѡ�


// Virtio-blk ����
struct virtio_blk_req{
  uint32_t type;
  uint32_t reserved;
  uint32_t sector;
  uint8_t data[512];
  uint8_t status;
} __attribute__((packed));


// ����Ϊ�꣬��������ԭ����Ϊ����ȷ��ʾԴ�ļ�����__FILE__�����кţ�__LINE__����
// ������ǽ��䶨��Ϊ������__FILE__ �� __LINE__ ����ʾ PANIC ��������ļ������кţ��������������õ�λ�á�
#define PANIC(fmt, ...)                                                        \
    do {                                                                       \
        printf("PANIC: %s:%d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__);  \
        while (1) {}                                                           \
    } while (0)
// ��һ���������� do-while ��䡣�������� while (0)�����ѭ��ִֻ��һ�Ρ����Ƕ����ɶ�������ɵĺ�ĳ�����ʽ��
//�򵥵��� { ...} ��װ���ܻ����� if ��������ʱ�����������Ϊ���μ�������������ӣ���
// ���⣬ע��ÿ��ĩβ�ķ�б�ܣ�\������Ȼ�����ڶ����϶���ģ�����չ��ʱ���з��ᱻ���ԡ�

// �ڶ����������� ##__VA_ARGS__������һ�����ڶ�����ܿɱ����������ĺ�����ñ�������չ���ο���GCC �ĵ�����
// ���ɱ����Ϊ��ʱ��## ��ɾ��ǰ��� ,����ʹ�ü�ʹֻ��һ���������� PANIC("booted!")������Ҳ�ܳɹ���


// trap_frame �ṹ���ʾ�� kernel_entry �б���ĳ���״̬
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


//READ_CSR �� WRITE_CSR �������ڶ�д CSR �Ĵ����ı�ݺꡣ
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

