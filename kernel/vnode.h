#ifndef __KERNEL_VNODE_H__
#define __KERNEL_VNODE_H__

#include <types.h>

enum vtype { VREG, VDIR, VCHR, VBLK, VLNK, VFIFO, VBAD };

struct vfs;
struct vn_ops;

typedef struct vnode {
    enum vtype          type;       /* type of node */
    uint32_t            ref_count;  /* reference count for this node */
    struct vfs          *vfs;       /* vfs that owns this node */
    void                *data;      /* implementation-specific data */
    const struct vn_ops *ops;       /* implementation-defined operations */
} vnode_t;

struct vn_ops
{
    int (*open)(vnode_t *node, uint16_t flags);
    int (*close)(vnode_t *node);
    int (*reclaim)(vnode_t *node);

    int (*read)(vnode_t *node, uint32_t off, uint32_t size, uint8_t *buf);
    int (*write)(vnode_t *node, uint32_t off, uint32_t size, uint8_t *buf);

    int (*lookup)(vnode_t *node, char *pathname, vnode_t **result);
};

#define VOP_SAFE(vn, func) \
    (vnode_check(vn, #func, (uintptr_t)((vn)->ops->func)), (vn)->ops->func)

#define VOP_OPEN(vn, flags)         (VOP_SAFE(vn, open)(vn, flags))
#define VOP_CLOSE(vn)               (VOP_SAFE(vn, close)(vn))
#define VOP_RECLAIM(vn)             (VOP_SAFE(vn, reclaim)(vn))

#define VOP_READ(vn, off, sz, buf)  (VOP_SAFE(vn, read)(vn, off, sz, buf))
#define VOP_WRITE(vn, off, sz, buf) (VOP_SAFE(vn, write)(vn, off, sz, buf))

#define VOP_LOOKUP(vn, name, res)   (VOP_SAFE(vn, lookup)(vn, name, res))

#define VN_INCREF(vn)               vnode_incref(vn)
#define VN_DECREF(vn)               vnode_decref(vn)

#define VN_INIT(vn, ops, fs, data)  vnode_init(vn, ops, fs, data)
#define VN_KILL(vn)                 vnode_kill(vn)

void vnode_check(vnode_t *node, const char *name, uintptr_t address);

void vnode_incref(vnode_t *node);
void vnode_decref(vnode_t *node);

int vnode_init(vnode_t *node, const struct vn_ops *ops, struct vfs *vfs, void *data);
void vnode_kill(vnode_t *node);

#endif
