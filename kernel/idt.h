#ifndef __KERNEL_IDT_H__
#define __KERNEL_IDT_H__

#include <types.h>

#define IDT_NUM_ENTRIES         256
#define IDT_FLAG_TYPE32TASK     (0x05 << 0)
#define IDT_FLAG_TYPE16INT      (0x06 << 0)
#define IDT_FLAG_TYPE16TRAP     (0x07 << 0)
#define IDT_FLAG_TYPE32INT      (0x0e << 0)
#define IDT_FLAG_TYPE32TRAP     (0x0f << 0)
#define IDT_FLAG_STORAGE        (0x01 << 4)
#define IDT_FLAG_RING0          (0x00 << 5)
#define IDT_FLAG_RING1          (0x01 << 5)
#define IDT_FLAG_RING2          (0x02 << 5)
#define IDT_FLAG_RING3          (0x03 << 5)
#define IDT_FLAG_PRESENT        (0x01 << 7)

typedef struct
{
        u16_t base_low;
        u16_t selector;
        u8_t  zero;
        u8_t  flags; /* 0-3: gate type, 4: storage segment (0 for interrupts), 5-6: ring, 7: present */
        u16_t base_high;
} __attribute__((packed)) idt_entry_t;

typedef struct
{
        u16_t limit;
        idt_entry_t *base;
} __attribute__((packed)) idt_ptr_t;

void idt_set_entry(idt_ptr_t *p, u8_t index, void (*callback)(), u16_t selector, u8_t flags);
idt_ptr_t *idt_setup_pointer();

#endif
