/*
 * Copyright Samsung Electronics Co.,LTD.
 * Copyright (C) 2015 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// To prevent build conflict with the previous libhwjpeg
#ifdef USES_UNIVERSAL_LIBHWJPEG

#ifndef __HARDWARE_EXYNOS_EXYNOS_JPEG_API_H__
#define __HARDWARE_EXYNOS_EXYNOS_JPEG_API_H__

// Exynos HAL defines another version of videodev2 apart from the original one
// This prevents conflict from the Exynos HAL from the original videodev2.h
/*
#ifndef v4l2_fourcc
#include <videodev2.h>
#endif
*/

#include <exynos-hwjpeg.h>

#ifndef JPEG_CACHE_ON
#define JPEG_CACHE_ON 1
#endif

#define JPEG_BUF_TYPE_USER_PTR 1
#define JPEG_BUF_TYPE_DMA_BUF  2

// CUSTOM V4L2 4CC FORMATS FOR LEGACY JPEG LIBRARY AND DRIVERS
#ifndef V4L2_PIX_FMT_JPEG_444
#define V4L2_PIX_FMT_JPEG_444 v4l2_fourcc('J', 'P', 'G', '4')
#endif
#ifndef V4L2_PIX_FMT_JPEG_422
#define V4L2_PIX_FMT_JPEG_422 v4l2_fourcc('J', 'P', 'G', '2')
#endif
#ifndef V4L2_PIX_FMT_JPEG_420
#define V4L2_PIX_FMT_JPEG_420 v4l2_fourcc('J', 'P', 'G', '0')
#endif
#ifndef V4L2_PIX_FMT_JPEG_GRAY
#define V4L2_PIX_FMT_JPEG_GRAY v4l2_fourcc('J', 'P', 'G', 'G')
#endif
#ifndef V4L2_PIX_FMT_JPEG_422V
#define V4L2_PIX_FMT_JPEG_422V v4l2_fourcc('J', 'P', 'G', '5')
#endif
#ifndef V4L2_PIX_FMT_JPEG_411
#define V4L2_PIX_FMT_JPEG_411 v4l2_fourcc('J', 'P', 'G', '1')
#endif

class ExynosJpegEncoder {
    /*
     * ExynosJpedgEncoder class is actually a derived class of
     * CHWJpegV4L2Compressor. But it is not derived from CHWJpegV4L2Compressor
     * because it has a lot of virtual functions which require extra memory for
     * vtables. Moreover, ExynosJpegEncoder class implements no virtual function
     * of CHWJpegV4L2Compressor.
     */
    CHWJpegV4L2Compressor m_hwjpeg;

    char m_iInBufType;
    char m_iOutBufType;

    unsigned int m_uiState;

    int m_nQFactor;
    int m_nWidth;
    int m_nHeight;
    int m_v4l2Format;
    int m_jpegFormat;
    int m_nStreamSize;

    bool __EnsureFormatIsApplied();
protected:
    enum {
        STATE_SIZE_CHANGED      = 1 << 0,
        STATE_PIXFMT_CHANGED    = 1 << 1,
        STATE_BASE_MAX          = 1 << 16,
    };

    unsigned int GetDeviceCapabilities() { return m_hwjpeg.GetDeviceCapabilities(); }
    CHWJpegCompressor &GetCompressor() { return m_hwjpeg; }
    unsigned int GetHWDelay() { return m_hwjpeg.GetHWDelay(); }

    void SetState(unsigned int state) { m_uiState |= state; }
    void ClearState(unsigned int state) { m_uiState &= ~state; }
    bool TestState(unsigned int state) { return (m_uiState & state) == state; }
    bool TestStateEither(unsigned int state) { return (m_uiState & state) != 0; }

    virtual bool EnsureFormatIsApplied() { return __EnsureFormatIsApplied(); }
public:
    ExynosJpegEncoder();
    virtual ~ExynosJpegEncoder() { destroy(); }


    // Return 0 on success, -1 on error
    int flagCreate();
    virtual int create(void);
    virtual int destroy(void);
    int updateConfig(void);
    int setCache(int val);

    void *getJpegConfig() { return reinterpret_cast<void *>(this); }

    int setJpegConfig(void* pConfig);

    int checkInBufType(void);
    int checkOutBufType(void);

    int getInBuf(int *piBuf, int *piInputSize, int iSize);
    int getOutBuf(int *piBuf, int *piOutputSize);
    int getInBuf(char **pcBuf, int *piInputSize, int iSize);
    int getOutBuf(char **pcBuf, int *piOutputSize);

    int setInBuf(int *piBuf, int *iSize);
    int setOutBuf(int iBuf, int iSize);
    int setInBuf(char **pcBuf, int *iSize);
    int setOutBuf(char *pcBuf, int iSize);

    int getSize(int *piWidth, int *piHeight);
    int setSize(int iW, int iH);
    int setJpegFormat(int iV4l2JpegFormat);
    int getColorFormat(void);
    int setColorFormat(int iV4l2ColorFormat);
    int setQuality(int iQuality);

    int setQuality(const unsigned char q_table[]);

    int setColorBufSize(int *piBufSize, int iSize);
    int getJpegSize(void);

    int encode(void);
};

#endif //__HARDWARE_EXYNOS_EXYNOS_JPEG_API_H__

#endif //USES_UNIVERSAL_LIBHWJPEG
