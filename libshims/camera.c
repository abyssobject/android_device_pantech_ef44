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
 * Symbols the JB ef44 camera HAL (camera.vendor.msm8960.so) links against
 * that Nougat's libutils/libbinder no longer export.
 */

/* Padding virtuals of VectorImpl/SortedVectorImpl, never called */
#define RESERVED(cls, n) void _ZN7android##cls##n##Ev(void) {}
#define RESERVED8(cls) RESERVED(cls, 1) RESERVED(cls, 2) RESERVED(cls, 3) \
	RESERVED(cls, 4) RESERVED(cls, 5) RESERVED(cls, 6) RESERVED(cls, 7) \
	RESERVED(cls, 8)

RESERVED8(10VectorImpl19reservedVectorImpl)
RESERVED8(16SortedVectorImpl25reservedSortedVectorImpl)

/*
 * MemoryBase(const sp<IMemoryHeap>&, long, unsigned int): the JB HAL was built
 * with ssize_t as long; on 32-bit ARM it is the same as today's int overload.
 */
extern void _ZN7android10MemoryBaseC1ERKNS_2spINS_11IMemoryHeapEEEij(void *self,
		const void *heap, int offset, unsigned int size);

void _ZN7android10MemoryBaseC1ERKNS_2spINS_11IMemoryHeapEEElj(void *self,
		const void *heap, long offset, unsigned int size)
{
	_ZN7android10MemoryBaseC1ERKNS_2spINS_11IMemoryHeapEEEij(self, heap,
			(int)offset, size);
}
