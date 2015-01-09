#include <system.h>
#include <paging.h>
#include <logging.h>
#include <kheap.h>
#include <mem_alloc.h>

extern uint32_t kernel_end;
extern uint32_t kernel_voffset;
extern page_dir_t *kernel_directory; 

uintptr_t placement_address = (uintptr_t)&kernel_end;
allocator_t *kheap = 0;

static uint8_t volatile mem_lock = 0;

static uintptr_t kmalloc_int(uint32_t size, uint32_t alignment, uintptr_t *phys)
{
    //spin_lock(&mem_lock);
    irq_state_t irq_state = irq_save();

    uintptr_t addr;
    if (kheap != 0) {
        //kprintf(INFO, "\n------------------ alloc(%x) -------------------\n", size);
        addr = (uintptr_t)alloc(size, alignment, kheap);
        if (phys != 0) {
            pte_t *page = get_page((uintptr_t)addr, 0, kernel_directory);
            *phys = page->frame * FRAME_SIZE + ((uintptr_t)addr & 0xfff);
        }
    }
    else {
        if (alignment != 0 && (placement_address % alignment)) {
            placement_address -= (placement_address % alignment);
            placement_address += alignment;
        }
        if (phys) {
            *phys = placement_address - (uintptr_t)&kernel_voffset;
        }
        addr = placement_address;
        placement_address += size;
    }

    irq_restore(irq_state);
    //spin_unlock(&mem_lock);
    return addr;
}

inline void *kmalloc(uint32_t size)
{
    return (void *)kmalloc_int(size, 0, 0);
}

inline void *kmalloc_a(uint32_t size)
{
    return (void *)kmalloc_int(size, FRAME_SIZE, 0);
}

inline void *kmalloc_p(uint32_t size, uintptr_t *phys)
{
    return (void *)kmalloc_int(size, 0, phys);
}

inline void *kmalloc_ap(uint32_t size, uintptr_t *phys)
{
    return (void *)kmalloc_int(size, FRAME_SIZE, phys);
}

inline void kfree(void *p)
{
    //spin_lock(&mem_lock);
    irq_state_t irq_state = irq_save();

    //kprintf(INFO, "\n--------------- free(%x) ---------------\n", p);
    free(p, kheap);

    irq_restore(irq_state);
    //spin_unlock(&mem_lock);
}
