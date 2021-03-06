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
LOCAL_MODULE := miniupnpc
LOCAL_MODULE_FILENAME := libminiupnpc
@echo Module name: $(LOCAL_MODULE)

LOCAL_SRC_FILES := \
  igd_desc_parse.c \
  minisoap.c \
  minissdpc.c \
  miniupnpc.c \
  miniwget.c \
  minixml.c \
  upnpc.c \
  upnpcommands.c \
  upnperrors.c \
  upnpreplyparse.c

LOCAL_C_INCLUDES := \
  $(MY_MODULE_PATH)

LOCAL_CFLAGS := -DMINIUPNP_EXPORTS -D__sun

include $(BUILD_SHARED_LIBRARY)

