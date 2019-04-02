LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := getpixel
LOCAL_SRC_FILES := \
	main.cpp 	\
	log.c
LOCAL_LDLIBS    := -llog -lGLESv2
include $(BUILD_EXECUTABLE)
