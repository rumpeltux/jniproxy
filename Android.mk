LOCAL_PATH:= $(call my-dir)
#include $(CLEAR_VARS)

LOCAL_SRC_FILES := jniproxy.c table.s
LOCAL_DISABLE_FATAL_LINKER_WARNINGS := true
LOCAL_MODULE    := jniproxy
LOCAL_MODULE_FILENAME := jniproxy
LOCAL_LDLIBS :=  -llog
include $(BUILD_SHARED_LIBRARY)
