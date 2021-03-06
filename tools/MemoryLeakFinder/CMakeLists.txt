#-------------------------------------------------------------------
# This file was automatically generated by ProjectGenerator
# which is tooling part the build system designed for GUCEF
#     (Galaxy Unlimited Framework)
# For the latest info, see http://www.VanvelzenSoftware.com/
#
# The contents of this file are placed in the public domain. Feel
# free to make use of it in any way you like.
#-------------------------------------------------------------------
#

# Configuration for module: MemoryLeakFinder



if (LINUX)
  set( PLATFORM_HEADER_FILES 
    include/DynMemoryManagerLoader.h
    include/gucefMLF_config.h
    include/gucefMLF_macros.h
    include/gucef_dynnewoff.h
    include/gucef_dynnewon.h
    include/MemoryManager.h
  )

  set( PLATFORM_HEADER_INSTALL "LINUX" )
  source_group( "Platform Header Files" FILES ${PLATFORM_HEADER_FILES} )

  set( PLATFORM_SOURCE_FILES 
    src/MemoryManager.cpp
  )

  set( PLATFORM_SOURCE_INSTALL "LINUX" )
  source_group( "Platform Source Files" FILES ${PLATFORM_SOURCE_FILES} )

elseif (WIN32)
  set( PLATFORM_HEADER_FILES 
    include/DynMemoryManagerLoader.h
    include/gucefMLF_config.h
    include/gucefMLF_macros.h
    include/gucef_dynnewoff.h
    include/gucef_dynnewon.h
    include/MemoryManager.h
  )

  set( PLATFORM_HEADER_INSTALL "WIN32" )
  source_group( "Platform Header Files" FILES ${PLATFORM_HEADER_FILES} )

  set( PLATFORM_SOURCE_FILES 
    src/MemoryManager.cpp
  )

  set( PLATFORM_SOURCE_INSTALL "WIN32" )
  source_group( "Platform Source Files" FILES ${PLATFORM_SOURCE_FILES} )

elseif (WIN64)
  set( PLATFORM_HEADER_FILES 
    include/DynMemoryManagerLoader.h
    include/gucefMLF_config.h
    include/gucefMLF_macros.h
    include/gucef_dynnewoff.h
    include/gucef_dynnewon.h
    include/MemoryManager.h
  )

  set( PLATFORM_HEADER_INSTALL "WIN64" )
  source_group( "Platform Header Files" FILES ${PLATFORM_HEADER_FILES} )

  set( PLATFORM_SOURCE_FILES 
    src/MemoryManager.cpp
  )

  set( PLATFORM_SOURCE_INSTALL "WIN64" )
  source_group( "Platform Source Files" FILES ${PLATFORM_SOURCE_FILES} )

endif ()

# For ease of use make a variable that has all files for this module
set( ALL_FILES ${HEADER_FILES} ${SOURCE_FILES} ${PLATFORM_HEADER_FILES} ${PLATFORM_SOURCE_FILES} )


if( LINUX )
  set( MODULE_NAME "MemoryLeakFinder" )
elseif( WIN32 )
  set( MODULE_NAME "MemoryLeakFinder" )
elseif( WIN64 )
  set( MODULE_NAME "MemoryLeakFinder" )
endif()
if( LINUX )
  add_library( ${MODULE_NAME} ${ALL_FILES} )
elseif( WIN32 )
  add_library( ${MODULE_NAME} ${ALL_FILES} )
elseif( WIN64 )
  add_library( ${MODULE_NAME} ${ALL_FILES} )
endif()

if ( LINUX )
  add_dependencies( ${MODULE_NAME} gucefCORE gucefMT )
  target_link_libraries( ${MODULE_NAME} gucefMT )
  set_target_properties( ${MODULE_NAME} PROPERTIES COMPILE_DEFINITIONS "MEMORYLEAKFINDER_BUILD_MODULE" )
endif( LINUX )

if ( WIN32 )
  add_dependencies( ${MODULE_NAME} gucefCORE gucefMT )
  target_link_libraries( ${MODULE_NAME} gucefMT )
  set_target_properties( ${MODULE_NAME} PROPERTIES COMPILE_DEFINITIONS "MEMORYLEAKFINDER_BUILD_MODULE" )
endif( WIN32 )

if ( WIN64 )
  add_dependencies( ${MODULE_NAME} gucefCORE gucefMT )
  target_link_libraries( ${MODULE_NAME} gucefMT )
  set_target_properties( ${MODULE_NAME} PROPERTIES COMPILE_DEFINITIONS "MEMORYLEAKFINDER_BUILD_MODULE" )
endif( WIN64 )

include_directories( ../../common/include ../../gucefCORE/include ../../gucefMT/include )

if ( LINUX )
  include_directories( ../../gucefCORE/include/linux include )
endif( LINUX )

if ( WIN32 )
  include_directories( ../../gucefCORE/include/mswin include )
endif( WIN32 )

if ( WIN64 )
  include_directories( ../../gucefCORE/include/mswin include )
endif( WIN64 )



#******** START OF AUTOGENERATED TEMPLATED ADDITION CONTENT ********




if (LINUX)
GUCEF_config_lib( ${MODULE_NAME} )
elseif (WIN32)
GUCEF_config_lib( ${MODULE_NAME} )
elseif (WIN64)
GUCEF_config_lib( ${MODULE_NAME} )
endif()



#******** END OF AUTOGENERATED TEMPLATED ADDITION CONTENT ********


