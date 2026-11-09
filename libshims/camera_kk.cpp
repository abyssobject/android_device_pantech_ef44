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

/* C++ symbols the KitKat ef44s camera HAL needs from older frameworks */

#include <new>
#include <stdio.h>
#include <unistd.h>
#include <binder/MemoryHeapBase.h>
#include <camera/CameraParameters.h>

using namespace android;

/*
 * MemoryHeapPmem(const sp<MemoryHeapBase>& pmemHeap, uint32_t flags) is gone;
 * build a plain MemoryHeapBase over the same fd in its place. The caller
 * allocated the (larger) MemoryHeapPmem, so the object fits.
 */
extern "C" void _ZN7android14MemoryHeapPmemC1ERKNS_2spINS_14MemoryHeapBaseEEEj(
        void *self, const sp<MemoryHeapBase> &heap, uint32_t flags)
{
    new (self) MemoryHeapBase(dup(heap->getHeapID()), heap->getSize(), flags, 0);
}

extern "C" void _ZN7android16CameraParameters18setPreviewFpsRangeEii(
        CameraParameters *self, int minFPS, int maxFPS)
{
    char str[32];
    snprintf(str, sizeof(str), "%d,%d", minFPS, maxFPS);
    self->set(CameraParameters::KEY_PREVIEW_FPS_RANGE, str);
}

extern "C" void _ZN7android16CameraParameters23setPreviewFrameRateModeEPKc(
        CameraParameters *self, const char *mode)
{
    self->set("preview-frame-rate-mode", mode);
}
