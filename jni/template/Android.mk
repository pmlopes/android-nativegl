# Android NDK v5 Build Script for Tokamak

LOCAL_PATH:= $(call my-dir)

# Build Tokamak as a static lib
include $(CLEAR_VARS)
LOCAL_MODULE := libtokamakp
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_SRC_FILES := tokamak/boxcylinder.cpp \
                   tokamak/collision.cpp \
                   tokamak/collisionbody.cpp \
                   tokamak/constraint.cpp \
                   tokamak/cylinder.cpp \
                   tokamak/dcd.cpp \
                   tokamak/lines.cpp \
                   tokamak/ne_interface.cpp \
                   tokamak/perflinux.cpp \
                   tokamak/region.cpp \
                   tokamak/restcontact.cpp \
                   tokamak/rigidbody.cpp \
                   tokamak/rigidbodybase.cpp \
                   tokamak/scenery.cpp \
                   tokamak/simulator.cpp \
                   tokamak/solver.cpp \
                   tokamak/sphere.cpp \
                   tokamak/stack.cpp \
                   tokamak/tricollision.cpp \
                   tokamak/useopcode.cpp

LOCAL_CXXFLAGS := -g -W --no-exceptions --no-rtti
include $(BUILD_STATIC_LIBRARY)

# final linkage (app files)
#
include $(CLEAR_VARS)

LOCAL_MODULE := na
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_SRC_FILES := na/ugshape.c \
                   na/uglu.c \
                   na/na-glue.c \
                   app.cpp

LOCAL_CFLAGS := -g -W
LOCAL_CXXFLAGS := -g -W --no-exceptions --no-rtti
LOCAL_STATIC_LIBRARIES := libtokamakp
LOCAL_LDLIBS := -lGLESv1_CM -llog

include $(BUILD_SHARED_LIBRARY)
