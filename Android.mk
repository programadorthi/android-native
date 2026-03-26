LOCAL_PATH             := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE           := native-application
LOCAL_SRC_FILES        := $(LOCAL_PATH)/jni/main.c

LOCAL_SHARED_LIBRARIES := -llog -landroid -lEGL -lGLESv2

include $(BUILD_SHARED_LIBRARY)
