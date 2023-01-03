#ifndef BINARYMASK_H
#define BINARYMASK_H

#define BINARY_MASK_32BIT(start, end)               \
    ((((uint32_t)0xffffffff) >> (32 - ((end)+1))) & \
        (((uint32_t)0xffffffff) << (start)))


#endif // BINARYMASK_H

