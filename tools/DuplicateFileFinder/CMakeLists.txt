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

# Configuration for module: DuplicateFileFinder

set( SOURCE_FILES 
  src/DuplicateFileFinder_main.cpp
)

# Make sure the PLATFORM_HEADER_FILES variable is always defined
 set( PLATFORM_HEADER_FILES "" )

# Make sure the PLATFORM_SOURCE_FILES variable is always defined
 set( PLATFORM_SOURCE_FILES "" )

# For ease of use make a variable that has all files for this module
set( ALL_FILES ${HEADER_FILES} ${SOURCE_FILES} ${PLATFORM_HEADER_FILES} ${PLATFORM_SOURCE_FILES} )




#******** END OF AUTOGENERATED CONTENT ********



if (WIN32)
  add_executable( DuplicateFileFinder WIN32 ${HEADER_FILES} ${SOURCE_FILES} )
else()
  add_executable( DuplicateFileFinder ${HEADER_FILES} ${SOURCE_FILES} )
endif()
add_dependencies( DuplicateFileFinder gucefCORE gucefMT )
target_link_libraries( DuplicateFileFinder gucefCORE gucefMT )
set_target_properties( DuplicateFileFinder PROPERTIES COMPILE_DEFINITIONS DUPLICATEFILEFINDER_BUILD_MODULE )
GUCEF_config_tool( DuplicateFileFinder )



#******** START OF AUTOGENERATED CONTENT ********



include_directories( ../../common/include ../../gucefCORE/include ../../gucefMT/include )

if ( ANDROID )
  include_directories( ../../gucefCORE/include/android )
endif( ANDROID )

if ( LINUX )
  include_directories( ../../gucefCORE/include/linux )
endif( LINUX )

if ( WIN32 )
  include_directories( ../../gucefCORE/include/mswin )
endif( WIN32 )

if ( WIN64 )
  include_directories( ../../gucefCORE/include/mswin )
endif( WIN64 )
