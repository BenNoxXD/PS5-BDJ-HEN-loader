/*
 * Copyright (c) 2025 BenNox_XD
 *
 * This file is part of PS5-BDJ-HEN-loader and is licensed under the MIT License.
 * See the LICENSE file in the root of the project for full license information.
 */
#include "lib_get_pid.h"
#include <sys/sysctl.h>
#include <sys/user.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

pid_t get_pid(const char *process_name) {
    int mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_PROC, 0};
    size_t buf_size;
    void *buf = NULL;

    if (sysctl(mib, 4, NULL, &buf_size, NULL, 0)) return -1;

    buf = malloc(buf_size);
    if (!buf) return -1;

    if (sysctl(mib, 4, buf, &buf_size, NULL, 0)) {
        free(buf);
        return -1;
    }

    pid_t pid = -1;
    for (void *ptr = buf; ptr < (buf + buf_size); ptr += ((struct kinfo_proc *)ptr)->ki_structsize) {
        struct kinfo_proc *ki = (struct kinfo_proc *)ptr;
        if (ki->ki_structsize < sizeof(struct kinfo_proc)) break;
        if (strncmp(ki->ki_comm, process_name, sizeof(ki->ki_comm)) == 0) {
            pid = ki->ki_pid;
            break;
        }
    }

    free(buf);
    return pid;
}
