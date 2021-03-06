--------------------------------------------------------------------
-- This file was automatically generated by ProjectGenerator
-- which is tooling part the build system designed for GUCEF
--     (Galaxy Unlimited Framework)
-- For the latest info, see http://www.VanvelzenSoftware.com/
--
-- The contents of this file are placed in the public domain. Feel
-- free to make use of it in any way you like.
--------------------------------------------------------------------
--

-- Configuration for module: vfspluginDVP


project( "vfspluginDVP" )

configuration( {} )
  location( os.getenv( "PM4OUTPUTDIR" ) )

configuration( {} )
  targetdir( os.getenv( "PM4TARGETDIR" ) )

configuration( {} )
language( "C++" )



configuration( {} )
kind( "SharedLib" )

configuration( {} )
links( { "DVPACKSYS", "gucefCORE", "gucefMT", "gucefVFS" } )
links( { "DVPACKSYS", "gucefCORE", "gucefMT", "gucefVFS" } )


configuration( {} )
defines( { "GUCEF_VFSPLUGIN_DVP_BUILD_MODULE" } )


configuration( {} )
vpaths { ["Headers"] = { "**.h", "**.hpp", "**.hxx" } }
files( {
  "include/vfspluginDVP.h",
  "include/vfspluginDVP_CDVPArchive.h",
  "include/vfspluginDVP_config.h",
  "include/vfspluginDVP_macros.h"
 } )



configuration( {} )
vpaths { ["Source"] = { "**.c", "**.cpp", "**.cs", "**.asm" } }
files( {
  "src/vfspluginDVP.cpp",
  "src/vfspluginDVP_CDVPArchive.cpp"
 } )


configuration( {} )
includedirs( { "../../../common/include", "../../../dependencies/DVPACKSYS/include", "../../../gucefCORE/include", "../../../gucefMT/include", "../../../gucefVFS/include", "include" } )

configuration( { "ANDROID" } )
includedirs( { "../../../gucefCORE/include/android" } )

configuration( { "LINUX" } )
includedirs( { "../../../gucefCORE/include/linux" } )

configuration( { "WIN32" } )
includedirs( { "../../../gucefCORE/include/mswin" } )

configuration( { "WIN64" } )
includedirs( { "../../../gucefCORE/include/mswin" } )
