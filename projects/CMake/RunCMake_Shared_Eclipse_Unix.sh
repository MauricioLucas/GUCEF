#!/bin/bash
#


# Turn on tracing, we want to see what's going on
#set -x

clear
echo "*** Invoking CMakeCommon ***"
. CMakeCommon.sh
echo "*** Invoking CMake ***"
ENIX_OUTPUTDIR="$OUTPUTDIR/ENIX"
echo "Output dir for this CMake target = $ENIX_OUTPUTDIR"
echo "CMake will use source root: $SRCROOTDIR"
cmake -DBUILD_SHARED_LIBS=ON -G"Eclipse CDT4 - Unix Makefiles" -H"$SRCROOTDIR" -B$ENIX_OUTPUTDIR
echo "Press enter to continue..."
line=""
read line
