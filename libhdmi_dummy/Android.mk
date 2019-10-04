# Copyright (C) 2008 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_PRELINK_MODULE := false
LOCAL_SHARED_LIBRARIES := liblog libutils libcutils libexynosutils \
			  libsync libexynosv4l2 libhwcutils libexynosdisplay \
			  libmpp

LOCAL_C_INCLUDES := \
	$(TOP)/hardware/samsung_slsi/$(TARGET_SOC)/include \
	$(TOP)/hardware/samsung_slsi/$(TARGET_BOARD_PLATFORM)/include \
	$(LOCAL_PATH)/../include \
	$(LOCAL_PATH)/../libhwcutils \
	$(LOCAL_PATH)/../libhwc \
	$(TOP)/hardware/samsung_slsi/exynos/libexynosutils \
	$(TOP)/hardware/samsung_slsi/$(TARGET_SOC)/libhwcmodule \
	$(TOP)/hardware/samsung_slsi/$(TARGET_SOC)/libhwcutilsmodule \
	$(TOP)/hardware/samsung_slsi/exynos/libmpp

LOCAL_HEADER_LIBRARIES := generated_kernel_headers

ifeq ($(BOARD_USES_VPP), true)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../libvppdisplay \
	$(TOP)/hardware/samsung_slsi/$(TARGET_SOC)/libdisplaymodule
else
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../libdisplay
endif

LOCAL_SRC_FILES := \
	ExynosExternalDisplay.cpp

include $(TOP)/hardware/samsung_slsi/$(TARGET_SOC)/libhdmimodule/Android.mk

LOCAL_CFLAGS += -Wno-unused-variable -Wno-parentheses-equality -Wno-reorder -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-format -Wno-gnu-designator

LOCAL_MODULE_TAGS := eng
LOCAL_MODULE := libhdmi

include $(TOP)/hardware/samsung_slsi/exynos/BoardConfigCFlags.mk
include $(BUILD_SHARED_LIBRARY)

