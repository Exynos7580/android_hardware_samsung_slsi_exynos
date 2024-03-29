/*
 * Copyright (C) 2012 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <ion.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <cutils/log.h>

typedef unsigned long ion_handle;

struct ion_allocation_data {
    size_t len;
    size_t align;
    unsigned int heap_mask;
    unsigned int flags;
    ion_handle handle;
};

struct ion_fd_data {
    ion_handle handle;
    int fd;
};

struct ion_handle_data {
    ion_handle handle;
};

struct ion_custom_data {
    unsigned int cmd;
    unsigned long arg;
};

struct ion_preload_data {
    unsigned int heap_id_mask;
    unsigned int flags;
    unsigned int count;
    struct ion_preload_object *obj; /* ion.h */
};

#define ION_IOC_MAGIC   'I'
#define ION_IOC_ALLOC   _IOWR(ION_IOC_MAGIC, 0, struct ion_allocation_data)
#define ION_IOC_FREE    _IOWR(ION_IOC_MAGIC, 1, struct ion_handle_data)
#define ION_IOC_MAP     _IOWR(ION_IOC_MAGIC, 2, struct ion_fd_data)
#define ION_IOC_SHARE   _IOWR(ION_IOC_MAGIC, 4, struct ion_fd_data)
#define ION_IOC_IMPORT  _IOWR(ION_IOC_MAGIC, 5, struct ion_fd_data)
#define ION_IOC_CUSTOM  _IOWR(ION_IOC_MAGIC, 6, struct ion_custom_data)
#define ION_IOC_SYNC	_IOWR(ION_IOC_MAGIC, 7, struct ion_fd_data)
#define ION_IOC_PRELOAD _IOW (ION_IOC_MAGIC, 8, struct ion_preload_data)

struct ion_msync_data {
    long flags;
    ion_buffer buf;
    size_t size;
    off_t offset;
};

enum ION_EXYNOS_CUSTOM_CMD {
    ION_EXYNOS_CUSTOM_MSYNC
};

ion_client ion_client_create(void)
{
    return open("/dev/ion", O_RDWR);
}

void ion_client_destroy(ion_client client)
{
    close(client);
}

ion_buffer ion_alloc(ion_client client, size_t len, size_t align,
                     unsigned int heap_mask, unsigned int flags)
{
    int ret;
    struct ion_handle_data arg_free;
    struct ion_fd_data arg_share;
    struct ion_allocation_data arg_alloc;

    arg_alloc.len = len;
    arg_alloc.align = align;
    arg_alloc.heap_mask = heap_mask;
    arg_alloc.flags = flags;

    ret = ioctl(client, ION_IOC_ALLOC, &arg_alloc);
    if (ret < 0)
        return ret;

    arg_share.handle = arg_alloc.handle;
    ret = ioctl(client, ION_IOC_SHARE, &arg_share);

    if ((ret >= 0) && (!arg_share.fd)) {
        ret = ioctl(client, ION_IOC_SHARE, &arg_share);
        if (ret >= 0)
            close(0);
        else
            ret = 0;
    }

    arg_free.handle = arg_alloc.handle;
    ioctl(client, ION_IOC_FREE, &arg_free);

    if (ret < 0)
        return ret;

    return arg_share.fd;
}

void ion_free(ion_buffer buffer)
{
    close(buffer);
}

void *ion_map(ion_buffer buffer, size_t len, off_t offset)
{
    return mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED,
                buffer, offset);
}

int ion_unmap(void *addr, size_t len)
{
    return munmap(addr, len);
}

int ion_sync(ion_client client, ion_buffer buffer)
{
    struct ion_fd_data data;

    data.fd = buffer;

    return ioctl(client, ION_IOC_SYNC, &data);
}

int ion_preload(ion_client client, unsigned int heap_mask, unsigned int flags,
                int count, struct ion_preload_object objs[])
{
    struct ion_preload_data data;

    data.heap_id_mask = heap_mask;
    data.flags = flags;
    data.count = count;
    data.obj = objs;

    return ioctl(client, ION_IOC_PRELOAD, &data);
}

#define ION_EXYNOS_SYNC_BY_HANDLE       0x01 /* otherwise, by fd */
#define ION_EXYNOS_SYNC_INV             0x10 /* otherwise, clean */

struct ion_exynos_sync_data {
    unsigned int flags;
    union {
        int dmabuf_fd;
        ion_handle handle;
    };
    void *addr;
    size_t size;
};
#define ION_IOC_EXYNOS_MAGIC 'E'

#define ION_IOC_EXYNOS_SYNC  _IOW(ION_IOC_EXYNOS_MAGIC, 0, struct ion_exynos_sync_data)

int ion_sync_range(ion_client client, int dmabuf_fd, void *addr, size_t size)
{
    ion_exynos_sync_data data;
    ion_custom_data custom;

    data.flags = 0;
    data.dmabuf_fd = dmabuf_fd;
    data.addr = addr;
    data.size = size;

    custom.cmd = ION_IOC_EXYNOS_SYNC;
    custom.arg = reinterpret_cast<unsigned long>(&data);

    return ioctl(client, ION_IOC_CUSTOM, &custom);
}

int ion_incRef(int fd, int share_fd, unsigned long **handle)
{
    struct ion_fd_data data;

    data.fd = share_fd;

    int ret = ioctl(fd, ION_IOC_IMPORT, &data);
    if (ret < 0)
        return ret;

    *handle = (unsigned long *)(data.handle);
    return ret;
}

int ion_decRef(int fd, unsigned long *handle)
{
    struct ion_handle_data data;
    data.handle = (ion_handle)handle;

    return ioctl(fd, ION_IOC_FREE, &data);
}
