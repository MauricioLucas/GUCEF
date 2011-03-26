#-------------------------------------------------------------------
# This file has been automatically generated by ProjectGenerator    
# which is part of a build system designed for GUCEF                
#     (Galaxy Unlimited Framework)                                  
# For the latest info, see http://www.VanvelzenSoftware.com/        
#                                                                   
# The contents of this file are placed in the public domain. Feel   
# free to make use of it in any way you like.                       
#-------------------------------------------------------------------


ifndef $(MY_MODULE_PATH)
  MY_MODULE_PATH := $(call my-dir)
endif
LOCAL_PATH := $(MY_MODULE_PATH)

include $(CLEAR_VARS)

@echo Module path: $(MY_MODULE_PATH)
LOCAL_MODULE := gucefINPUT
@echo Module name: $(LOCAL_MODULE)

LOCAL_SRC_FILES := \
  src/CGUCEFINPUTModule.cpp \
  src/CInputActionMap.cpp \
  src/CInputContext.cpp \
  src/CInputController.cpp \
  src/CInputDriverPlugin.cpp \
  src/gucefINPUT_CActionEventData.cpp \
  src/gucefINPUT_CInputDriver.cpp \
  src/gucefINPUT_CInputObserverSwitch.cpp \
  src/gucefINPUT_CKeyboard.cpp \
  src/gucefINPUT_CKeyModStateChangedEventData.cpp \
  src/gucefINPUT_CKeyStateChangedEventData.cpp \
  src/gucefINPUT_CMouse.cpp \
  src/gucefINPUT_CMouseButtonEventData.cpp \
  src/gucefINPUT_CMouseMovedEventData.cpp \
  src/gucefINPUT_keyboard.cpp

LOCAL_C_INCLUDES := \
  $(MY_MODULE_PATH)/include \
  $(MY_MODULE_PATH)/../gucefCORE/include \
  $(MY_MODULE_PATH)/../gucefMT/include


LOCAL_SHARED_LIBRARIES := \
  gucefCORE \
  gucefMT

include $(BUILD_SHARED_LIBRARY)

