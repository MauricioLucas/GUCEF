#-------------------------------------------------------------------
# This file has been automatically generated by ProjectGenerator    
# which is part of a build system designed for GUCEF                
#     (Galaxy Unlimited Framework)                                  
# For the latest info, see http://www.VanvelzenSoftware.com/        
#                                                                   
# The contents of this file are placed in the public domain. Feel   
# free to make use of it in any way you like.                       
#-------------------------------------------------------------------


ifndef MY_MODULE_PATH
  MY_MODULE_PATH := $(call my-dir)
endif
LOCAL_PATH := $(MY_MODULE_PATH)

include $(CLEAR_VARS)

@echo Module path: $(MY_MODULE_PATH)
LOCAL_MODULE := DVPPackTool
@echo Module name: $(LOCAL_MODULE)

LOCAL_SRC_FILES := \
  src/DVP_Pack_Tool.c

LOCAL_C_INCLUDES := \
  $(MY_MODULE_PATH)/../../common/include \
  $(MY_MODULE_PATH)/../../gucefCORE/include \
  $(MY_MODULE_PATH)/../../gucefCORE/include/android \
  $(MY_MODULE_PATH)/../../gucefMT/include \
  $(MY_MODULE_PATH)/../../gucefVFS/plugins/vfspluginDVP/dependencies/DVPACKSYS/include


LOCAL_SHARED_LIBRARIES := \
  DVPACKSYS \
  gucefCORE \
  gucefMT

include $(BUILD_EXECUTABLE)

