/*
 * Copyright (C) 2026 The LineageOS Project
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

/*
 * The JB rmt_storage maps the modem's EFS shared memory at physical
 * 0x8fb00000 through /dev/mem with a 32-bit off_t. Nougat's bionic mmap()
 * sign-extends that into a negative off64_t and the kernel returns EINVAL,
 * so the modem never gets its EFS and crashes. Treat the offset as unsigned,
 * as JB's mmap2-based implementation did. Preloaded into rmt_storage only.
 */

#include <stdint.h>
#include <sys/mman.h>
#include <sys/types.h>

void *mmap(void *addr, size_t size, int prot, int flags, int fd, off_t offset)
{
    return mmap64(addr, size, prot, flags, fd, (off64_t)(uint32_t)offset);
}
