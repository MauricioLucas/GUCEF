*
*  CMakeListGenerator
*  Copyright (C) 2002 - 2010.  Dinand Vanvelzen
*
*  This software is free software; you can redistribute it and/or
*  modify it under the terms of the GNU Lesser General Public
*  License as published by the Free Software Foundation; either
*  version 2.1 of the License, or (at your option) any later version.
*
*  This software is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*  Lesser General Public License for more details.
*
*  You should have received a copy of the GNU Lesser General Public
*  License along with this library; if not, write to the Free Software
*  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA 
*


About:
------------------------

The CMakeListGenerator tool is meant for automatically generating the bulk 
of the CMakeLists.txt content based on directory structure and file analysis.
This reduces maintenance time for CMake files and makes it easy to generate
CMake files for 3'rd party sources with little effort.
I, Dinand Vanvelzen, created this tool to help me build the Galaxy Unlimited
codebase using CMake without having to manually write all the CMake files for
GU libraries and their dependencies. One of the advantages is being able 
to dump new code releases of dependencies into the archive without having to
go through the code to check which files got added/deleted etc. Just re-run the
generator and you will have up-to-date CMakeList.txt files for those projects.
An additional advantage is the automatic generation of include_directories paths
which can be a pain to get right, especially with a lot of modules in a complicated
directory structure. Using this tool you can just move the modules around and their
include paths will be correct again as soon as you re-run the generator.

Version:
------------------------

This is a release build created on 18'th of March 2010.
It is a 32-bit Microsoft Windows application.
There should be no problem running this application in WINE on linux,
if you encounter any problems let me know.

Features:
------------------------

- Automatically locate and list all header and source file for each module
- Ability to exclude directories from processing
- Automatically generates the include paths for each module's dependencies
- Handles most include/source directory structures
- Special processing for platform specific headers/sources is supported.
- Support for combined processing of multipe source trees



Usage: Overview
------------------------

There are 3 kinds of text files involved in the generation of the CMakeLists.txt 
files, these are:
- CMakeLists.txt:
        Module (project) file which is automatically generated
- CMakeListsSuffix.txt: 
        Like the name says, a suffix that will be added to the automatically 
        generated CMakeLists.txt. It is also used for dependency determination by the
        generator. Directories containing a suffix file will be considered project 
        directories.
- CMakeGenExcludeList.txt:
        A simple text file containing a sub-dir/file name per line that should be 
        excluded from CMakeListGenerator processing

Note that if a CMakeLists.txt file already exists the tool will overwrite it!


Usage: Command line parameters
------------------------

If no command line arguments are given then the tool will start processing 
the directory structure using the current working directory as the root.

The tool accepts the following command line parameters:
- 'rootDir=<path to processing root>'
    Example:
       'rootDir=C:\Dev\MyCodeRoot'

Like most GU applications command line parameters accepted in the form:
'<key>=<value>'

Note that you can pass multiple instances of the rootDir argument in order
to pass multiple root directories.

    Example:
       'rootDir=C:\Dev\MyCodeRoot' 'rootDir=D:\OtherDev\MyCodeRoot'

Passing multiple root dirs will allow include dependencies between the two root dirs
to be resolved thus allowing them to be used as if it's all part of the same project set.

Usage: Platform (O/S) specific processing
------------------------

This feature allows you to include/exclude files automatically
by placing them in one of the supported platform directories. Include-directories for 
the module with platform specific code and every module that depends on it will have
its automatically generated include dirs altered based on the target platform, saving you
the effort of making all kinds of platform checks everywhere. Note that it is still 
recommended to surround your code with a preprocessor platform check, this way if your 
file gets included on the wrong platform it will not break the build!

Example:
  include\mswin\MyWindowsSpecificCode.h
  src\mswin\MyWindowsSpecificCode.cpp

For the example file structure above a special if clause will be generated where
depending on the target platform PLATFORM_HEADER_FILES and PLATFORM_SOURCE_FILES
will be set to the correct list of files. In this case the if clause will check
for platform WIN32. which directory names map to which platform is listed below.
You must name the directories according to one of the supported directory names
in order to make use of this feature to exclude/include files based on the target
platform.

Supported platform directories:
  "mswin" for platform "WIN32"
  "win32" for platform "WIN32"
  "win64" for platform "WIN64"
  "linux" for platform "UNIX"
  "osx" for platform "OSX"
  "android" for platform "ANDROID"
  "iphone" for platform "IPHONEOS"
  "glx" for platform "GLX"
  "gtK" for platform "GTK"
  "symbian" for platform "Symbian"


History:
------------------------

- 18'th March 2010:
     - Drastically improved support for platform directories. Various platforms are now supported
       and any include/source directory structure can be used as long as the platform dir conforms
       to one of the supported names.
- 15'th March 2010:
     - Added include dir determination based on a dependency's dependencies.
     - Added the ability to exclude specific files using the exclude list. It can now  
       be used to exclude both sub-dirs and files. Just place each entry on a new line as before.
     - Fixed a bug with last sub-dir string logic, having a trailing \ will no longer cause 
       the last sub-dir string to be determinded as being ""
     - Changed include/source dir processing so that every sub-dir and the project dir itself will
       have all known include/source file types added to the project except for directories excluded
       using the exclude list or directories who have a suffix file and are thus treated as a project 
       defined in a sub-dir.
- 18'th Jan 2010:
     - The exclude list functionality now properly accounts for different EOL
       characters in the exclude text file if the file has multiple entries
     - In addition the exclude list can now be used to exclude specific files not
       just folders
     - Added support for multiple rootDir command line arguments in order to process
       multiple roots. This is a nice feature for combining multiple source trees into
       a single CMake project because it will automatically figure out the include directories
       etc for you.
- 12'th Jan 2010: First release