/*
 *  ProjectGenerator: Tool to generate module/project files
 *  Copyright (C) 2002 - 2011.  Dinand Vanvelzen
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

/*-------------------------------------------------------------------------//
//                                                                         //
//      INCLUDES                                                           //
//                                                                         //
//-------------------------------------------------------------------------*/

#ifndef GUCEF_CORE_LOGGING_H
#include "gucefCORE_Logging.h"
#define GUCEF_CORE_LOGGING_H
#endif /* GUCEF_CORE_LOGGING_H ? */

#ifndef GUCEF_CORE_DVCPPSTRINGUTILS_H
#include "dvcppstringutils.h"
#define GUCEF_CORE_DVCPPSTRINGUTILS_H
#endif /* GUCEF_CORE_DVCPPSTRINGUTILS_H ? */

#include "gucefProjectGenerator_CAndroidMakefileGenerator.h"

/*-------------------------------------------------------------------------//
//                                                                         //
//      NAMESPACE                                                          //
//                                                                         //
//-------------------------------------------------------------------------*/

namespace GUCEF {
namespace PROJECTGENERATOR {

/*-------------------------------------------------------------------------//
//                                                                         //
//      GLOBAL VARS                                                        //
//                                                                         //
//-------------------------------------------------------------------------*/

const char* makefileHeader =
    "#-------------------------------------------------------------------\n"
    "# This file has been automatically generated by ProjectGenerator    \n"
    "# which is part of a build system designed for GUCEF                \n"
    "#     (Galaxy Unlimited Framework)                                  \n"
    "# For the latest info, see http://www.VanvelzenSoftware.com/        \n"
    "#                                                                   \n"
    "# The contents of this file are placed in the public domain. Feel   \n"
    "# free to make use of it in any way you like.                       \n"
    "#-------------------------------------------------------------------\n\n\n";

/*-------------------------------------------------------------------------//
//                                                                         //
//      UTILITIES                                                          //
//                                                                         //
//-------------------------------------------------------------------------*/

const TModuleInfo*
FindModuleByName( const TModuleInfoEntryPairVector& mergeLinks ,
                  const CORE::CString& moduleName              )
{GUCEF_TRACE;

    TModuleInfoEntryPairVector::const_iterator i = mergeLinks.begin();
    while ( i != mergeLinks.end() )
    {
        const TModuleInfo& moduleInfo = (*(*i).second);
        if ( moduleInfo.name == moduleName )
        {
            return (*i).second;
        }
        ++i;
    }
    return NULL;
}

/*-------------------------------------------------------------------------*/

CORE::CString
GenerateContentForAndroidMakefile( const TModuleInfoEntryPairVector& mergeLinks ,
                                   const TModuleInfo& moduleInfo                ,
                                   const CORE::CString& moduleRoot              ,
                                   bool addGeneratorCompileTimeToOutput         ,
                                   TStringSet& ndkModulesUsed                   )
{GUCEF_TRACE;

    CORE::CString contentPrefix = makefileHeader;

    if ( addGeneratorCompileTimeToOutput )
    {
        contentPrefix +=
          "#"
          "# The project generator version used was compiled on " __TIME__ __DATE__ "\n"
          "#\n\n";
    }

    contentPrefix +=
      "ifndef MY_MODULE_PATH\n"
      "  MY_MODULE_PATH := $(call my-dir)\n"
      "endif\n"
      "LOCAL_PATH := $(MY_MODULE_PATH)\n\n"
      "include $(CLEAR_VARS)\n\n"
      "@echo Module path: $(MY_MODULE_PATH)\n"
      "LOCAL_MODULE := " + moduleInfo.name + "\n";

    if ( ( MODULETYPE_SHARED_LIBRARY == moduleInfo.moduleType ) ||
         ( MODULETYPE_STATIC_LIBRARY == moduleInfo.moduleType )  )
    {
        contentPrefix += "LOCAL_MODULE_FILENAME := lib" + moduleInfo.name + "\n";
    }
    contentPrefix += "@echo Module name: $(LOCAL_MODULE)\n\n";

    // Generate the source files section
    CORE::CString srcFilesSection = "LOCAL_SRC_FILES := \\\n";
    bool firstLoop = true;
    TStringVectorMap::const_iterator i = moduleInfo.sourceDirs.begin();
    while ( i != moduleInfo.sourceDirs.end() )
    {
        const CORE::CString& srcDir = (*i).first;
        const TStringVector& srcFiles = (*i).second;

        TStringVector::const_iterator n = srcFiles.begin();
        while ( n != srcFiles.end() )
        {
            if ( !firstLoop )
            {
                srcFilesSection += " \\\n";
            }
            firstLoop = false;

            CORE::CString relFilePath = srcDir;
            CORE::AppendToPath( relFilePath, (*n) );

            srcFilesSection += "  " + relFilePath.ReplaceChar( '\\', '/' );

            ++n;
        }
        ++i;
    }

    // Add some spacing for readability
    srcFilesSection += "\n\n";

    // Generate the included files section
    // for android make files we only need the path
    // it will locate the header file on its own
    CORE::CString includeFilesSection = "LOCAL_C_INCLUDES := \\\n";
    i = moduleInfo.includeDirs.begin();
    firstLoop = true;
    while ( i != moduleInfo.includeDirs.end() )
    {
        if ( !firstLoop )
        {
            includeFilesSection += " \\\n";
        }
        firstLoop = false;

        const CORE::CString& dir = (*i).first;
        if ( !dir.IsNULLOrEmpty() )
        {
            includeFilesSection += "  $(MY_MODULE_PATH)/" + dir.ReplaceChar( '\\', '/' );
        }
        else
        {
            // Support the use-case where the include dir is empty because the moduleinfo dir == include dir
            includeFilesSection += "  $(MY_MODULE_PATH)";
        }

        ++i;
    }

    // We also need to add the include paths required to find headers
    // refered to because of dependencies
    TStringSet::const_iterator n = moduleInfo.dependencyIncludeDirs.begin();
    while ( n != moduleInfo.dependencyIncludeDirs.end() )
    {
        if ( !firstLoop )
        {
            includeFilesSection += " \\\n";
        }
        firstLoop = false;

        includeFilesSection += "  $(MY_MODULE_PATH)/" + (*n).ReplaceChar( '\\', '/' );

        ++n;
    }

    // Add some spacing for readability
    includeFilesSection += "\n\n";

    // Now we add the preprocessor definitions
    CORE::CString preprocessorSection;
    if ( !moduleInfo.preprocessorSettings.defines.empty() )
    {
        preprocessorSection = "LOCAL_CFLAGS :=";

        TStringSet::const_iterator m = moduleInfo.preprocessorSettings.defines.begin();
        while ( m != moduleInfo.preprocessorSettings.defines.end() )
        {
            preprocessorSection += " -D" + (*m);
            ++m;
        }

        // Add some spacing for readability
        preprocessorSection += "\n\n";
    }

    // Now we add the compiler flags, if any
    // For Android we only support the GCC compilers
    CORE::CString compilerSection;
    TStringMap::const_iterator p = moduleInfo.compilerSettings.compilerFlags.find( "GCC" );
    if ( p != moduleInfo.compilerSettings.compilerFlags.end() )
    {
        compilerSection = "LOCAL_CFLAGS +=" + (*p).second + "\n\n";
    }
    p = moduleInfo.compilerSettings.compilerFlags.find( "G++" );
    if ( p != moduleInfo.compilerSettings.compilerFlags.end() )
    {
        compilerSection = "LOCAL_CPPFLAGS +=" + (*p).second + "\n\n";
    }

    // Now we will add all the dependency linking instructions.
    // For some reason it matters, at specification time, to Android's build
    // system whether the module you are linking to is a dynamically linked module
    // or a statically linked module. As such we have to figure out which is which.
    //
    // We make an alphabetical list instead of creating the section right away because
    // we dont want the order to vary in the makefile because such changes cause the NDK
    // to build the code again for no reason.
    CORE::CString linkingErrorSection;
    TStringSet linkedSharedLibraries;
    TStringSet linkedStaticLibraries;
    TStringSet linkedRuntimeLibraries;
    TModuleTypeMap::const_iterator m = moduleInfo.linkerSettings.linkedLibraries.begin();
    while ( m != moduleInfo.linkerSettings.linkedLibraries.end() )
    {
        const CORE::CString& linkedLibName = (*m).first;
        TModuleType linkedLibType = (*m).second;
        switch ( linkedLibType )
        {
            case MODULETYPE_EXECUTABLE:
            {
                // This is really nasty but the best option for now...
                // It is possible to link to exported symbols from an executable
                // under linux and as such we will leverage this here
                linkedSharedLibraries.insert( linkedLibName );
                break;
            }
            case MODULETYPE_SHARED_LIBRARY:
            {
                linkedSharedLibraries.insert( linkedLibName );
                break;
            }
            case MODULETYPE_STATIC_LIBRARY:
            {
                linkedStaticLibraries.insert( linkedLibName );
                break;
            }
            case MODULETYPE_CODE_INCLUDE_LOCATION:
            case MODULETYPE_HEADER_INCLUDE_LOCATION:
            {
                // Skip this, no linking required
                break;
            }
            default:
            {
                // Since the depedendency module type was not predefined we will investigate among
                // the other modules to try to determine the nature of the linked module
                const TModuleInfo* linkedDependency = FindModuleByName( mergeLinks, linkedLibName );
                if ( NULL != linkedDependency )
                {
                    // The module we are linking too is part of this project.
                    // As such we can simply check the other module's info
                    // to find out wheter its a dynamically linked module or not
                    // which in turn tells us how to instruct the Android build system
                    // to link.
                    switch( linkedDependency->moduleType )
                    {
                        case MODULETYPE_SHARED_LIBRARY:
                        {
                            linkedSharedLibraries.insert( linkedLibName );
                            break;
                        }
                        case MODULETYPE_STATIC_LIBRARY:
                        {
                            linkedStaticLibraries.insert( linkedLibName );
                            break;
                        }
                        case MODULETYPE_EXECUTABLE:
                        {
                            // This is really nasty but the best option for now...
                            // It is possible to link to exported symbols from an executable
                            // under linux and as such we will leverage this here
                            linkedSharedLibraries.insert( linkedLibName );
                            break;
                        }
                        case MODULETYPE_CODE_INCLUDE_LOCATION:
                        {
                            // Dont do anything.
                            // The files for this 'module' have already been merged into the dependent module
                            break;
                        }
                        case MODULETYPE_HEADER_INCLUDE_LOCATION:
                        {
                            // Don't have to do anything.
                            // Due to the auto-dependency tracking of include paths the header paths will have been added to
                            // whatever module depends on this 'module'
                            break;
                        }
                        default:
                        {
                            linkingErrorSection +=
                              "# *** ERROR *** Finish me\n"
                              "# Unable to determing module type from the source information\n"
                              "# Please edit the line below to manually set the correct linking method for this dependency\n";
                            linkingErrorSection += "#LOCAL_<(LDLIBS???)> += " + moduleInfo.name + "\n\n";

                            GUCEF_ERROR_LOG( CORE::LOGLEVEL_NORMAL, "Error: the module " + moduleInfo.name + " does not have a useable module type set, you will have to manually edit the file to correct the error" );
                            break;
                        }
                    }
                }
                else
                {
                    // If we get here then this dependency is not on a module which is part of the project
                    // As such we cannot build this module thus the only approriote linking method would seem
                    // to be the one where we simply instruct the linker to load this dependency at runtime.
                    // This will typically be the case for any Android NDK modules we have to link to.
                    linkedRuntimeLibraries.insert( linkedLibName );
                }
            }
        }
        ++m;
    }

    CORE::CString linkingSection;
    CORE::CString linkedSharedLibrariesSection = "\nLOCAL_SHARED_LIBRARIES := \\\n";
    CORE::CString linkedStaticLibrariesSection = "\nLOCAL_STATIC_LIBRARIES := \\\n";
    CORE::CString linkedRuntimeLibrariesSection = "\nLOCAL_LDLIBS := \\\n";

    // Based on what was found we will construct the linking section
    bool first = true;
    n = linkedSharedLibraries.begin();
    while ( n != linkedSharedLibraries.end() )
    {
        if ( !first )
        {
             linkedSharedLibrariesSection += " \\\n";
        }
        linkedSharedLibrariesSection += "  " + (*n);
        first = false;
        ++n;
    }
    first = true;
    n = linkedStaticLibraries.begin();
    while ( n != linkedStaticLibraries.end() )
    {
        if ( !first )
        {
             linkedStaticLibrariesSection += " \\\n";
        }
        linkedStaticLibrariesSection += "  " + (*n);
        first = false;
        ++n;
    }
    first = true;
    n = linkedRuntimeLibraries.begin();
    while ( n != linkedRuntimeLibraries.end() )
    {
        if ( !first )
        {
             linkedRuntimeLibrariesSection += " \\\n";
        }
        linkedRuntimeLibrariesSection += "  -l" + (*n);
        first = false;
        ++n;
    }

    if ( !linkedSharedLibraries.empty() )
    {
        linkingSection += linkedSharedLibrariesSection + "\n\n";
    }
    if ( !linkedStaticLibraries.empty() )
    {
        linkingSection += linkedStaticLibrariesSection + "\n\n";
    }
    if ( !linkedRuntimeLibraries.empty() )
    {
        linkingSection += linkedRuntimeLibrariesSection + "\n\n";
    }
    linkingSection += linkingErrorSection;

    // Check if we have a file on disk of information which is to be inserted into
    // our automatically generated make file
    CORE::CString manualContent;
    CORE::CString manualContentFilePath = moduleRoot;
    CORE::AppendToPath( manualContentFilePath, "AndroidAddition.mk" );
    if ( CORE::FileExists( manualContentFilePath ) )
    {
        if ( CORE::LoadTextFileAsString( manualContentFilePath ,
                                         manualContent         ) )
        {
            GUCEF_LOG( CORE::LOGLEVEL_NORMAL, "Successfully loaded manually defined content for module " + moduleInfo.name + " from addition file " + manualContentFilePath );
        }
        else
        {
            manualContent =
              "# *** ERROR *** Finish me\n"
              "# Unable to load manually defined content from detected AndroidAddition.mk file\n"
              "# Please manually insert its contents here\n\n";
            GUCEF_ERROR_LOG( CORE::LOGLEVEL_NORMAL, "Error: the module " + moduleInfo.name + " has manually defined content in a AndroidAddition.mk file but it could not be loaded, you will have to manually edit the file to correct the error" );
        }
    }

    // Now generate the latter part of the file which contains more meta data about the module
    CORE::CString contentSuffix;
    switch ( moduleInfo.moduleType )
    {
        case MODULETYPE_SHARED_LIBRARY:
        {
            contentSuffix += "include $(BUILD_SHARED_LIBRARY)\n\n";
            break;
        }
        case MODULETYPE_STATIC_LIBRARY:
        {
            contentSuffix += "include $(BUILD_STATIC_LIBRARY)\n\n";
            break;
        }
        case MODULETYPE_EXECUTABLE:
        {
            contentSuffix += "include $(BUILD_EXECUTABLE)\n\n";
            break;
        }
        default:
        {
            contentSuffix +=
              "# *** ERROR *** Finish me\n"
              "# Unable to determing module type from the source information\n"
              "# Please edit the line below to manually set the correct module type to build\n"
              "#include $(BUILD_???)\n\n";

            GUCEF_ERROR_LOG( CORE::LOGLEVEL_NORMAL, "Error: the module " + moduleInfo.name + " does not have a useable module type set, you will have to manually edit the file to correct the error" );
            break;
        }
    }

    // Check for NDK static libraries to import
    if ( !linkedStaticLibraries.empty() )
    {
        TStringSet::iterator a = linkedStaticLibraries.find( "android_native_app_glue" );
        if ( a != linkedStaticLibraries.end() )
        {
            ndkModulesUsed.insert( "android_native_app_glue" );
            contentSuffix += "$(call import-module,android/native_app_glue)\n";
        }
        a = linkedStaticLibraries.find( "cpufeatures" );
        if ( a != linkedStaticLibraries.end() )
        {
            ndkModulesUsed.insert( "cpufeatures" );
            contentSuffix += "$(call import-module,android/cpufeatures)\n";
        }
    }

    return contentPrefix + srcFilesSection + includeFilesSection + preprocessorSection + compilerSection + linkingSection + manualContent + contentSuffix;
}

/*-------------------------------------------------------------------------*/

bool
DidMakefileContentChange( const CORE::CString& makefilePath ,
                          const CORE::CString& newContent   )
{GUCEF_TRACE;

    CORE::CString preexistingContent;
    if ( CORE::LoadTextFileAsString( makefilePath, preexistingContent ) )
    {
        GUCEF_LOG( CORE::LOGLEVEL_NORMAL, "Found a pre-existing Android makefile at " + makefilePath + ", comparing against new content..." );

        if ( newContent == preexistingContent )
        {
            return false;
        }
    }
    return true;
}

/*-------------------------------------------------------------------------*/

bool
CreateAndroidMakefileOnDiskForModule( const TModuleInfoEntryPairVector& mergeLinks ,
                                      const TModuleInfo& moduleInfo                ,
                                      const CORE::CString& moduleRoot              ,
                                      bool addGeneratorCompileTimeToOutput         ,
                                      TStringSet& ndkModulesUsed                   )
{GUCEF_TRACE;

    if ( ( MODULETYPE_HEADER_INCLUDE_LOCATION == moduleInfo.moduleType ) ||
         ( MODULETYPE_CODE_INCLUDE_LOCATION == moduleInfo.moduleType )    )
    {
        // this module type does not require processing here
        return true;
    }

    GUCEF_LOG( CORE::LOGLEVEL_NORMAL, "Generating Android makefile content for module " + moduleInfo.name );

    // First we generate the content for the makefile based on the given module information
    CORE::CString makefileContent = GenerateContentForAndroidMakefile( mergeLinks                      ,
                                                                       moduleInfo                      ,
                                                                       moduleRoot                      ,
                                                                       addGeneratorCompileTimeToOutput ,
                                                                       ndkModulesUsed                  );

    // Now we write the makefile to the root location of the module since everything is relative to that
    CORE::CString makefilePath = moduleRoot;
    CORE::AppendToPath( makefilePath, "Android.mk" );

    // Do not change the file on disk unless we have to.
    // the NDK build system will rebuild if the timestamp on the file changed which can take a long time
    if ( DidMakefileContentChange( makefilePath, makefileContent ) )
    {
        GUCEF_LOG( CORE::LOGLEVEL_NORMAL, "Writing Android makefile content for module " + moduleInfo.name + " to " + makefilePath );

        if ( CORE::WriteStringAsTextFile( makefilePath    ,
                                          makefileContent ,
                                          true            ,
                                          "\n"            ) )
        {
            GUCEF_LOG( CORE::LOGLEVEL_NORMAL, "Successfully created Android makefile for module " + moduleInfo.name + " at " + makefilePath );
            return true;
        }
        GUCEF_ERROR_LOG( CORE::LOGLEVEL_NORMAL, "Failed to create an Android makefile for module " + moduleInfo.name + " at " + makefilePath );
        return false;
    }
    else
    {
        GUCEF_LOG( CORE::LOGLEVEL_NORMAL, "Skipping creation of Android makefile for module " + moduleInfo.name + " at " + makefilePath +
            " since there already is a makefile with identical content" );
        return true;
    }
}

/*-------------------------------------------------------------------------*/

bool
CreateAndroidMakefileOnDiskForEachModule( const TModuleInfoEntryPairVector& mergeLinks ,
                                          bool addGeneratorCompileTimeToOutput         ,
                                          TStringSet& ndkModulesUsed                   )
{GUCEF_TRACE;

    TModuleInfoEntryPairVector::const_iterator i = mergeLinks.begin();
    while ( i != mergeLinks.end() )
    {
        const TModuleInfoEntry& moduleInfoEntry = (*(*i).first);
        const TModuleInfo& moduleInfo = (*(*i).second);

        if ( !CreateAndroidMakefileOnDiskForModule( mergeLinks                      ,
                                                    moduleInfo                      ,
                                                    moduleInfoEntry.rootDir         ,
                                                    addGeneratorCompileTimeToOutput ,
                                                    ndkModulesUsed                  ) )
        {
            GUCEF_ERROR_LOG( CORE::LOGLEVEL_NORMAL, "Failed to create an Android makefile for all modules because of the following module " + moduleInfo.name );
            return false;
        }

        ++i;
    }
    return true;
}

/*-------------------------------------------------------------------------*/

const TModuleInfo*
FindFirstModuleAccordingToBuildOrder( const TModuleInfoEntryPairVector& mergeLinks )
{GUCEF_TRACE;

    TModuleInfoEntryPairVector::const_iterator i = mergeLinks.begin();
    while ( i != mergeLinks.end() )
    {
        if ( (*i).second->buildOrder == 0 )
        {
            return (*i).second;
        }
    }
    return NULL;
}

/*-------------------------------------------------------------------------*/

const TModuleInfo*
FindNextModuleAccordingToBuildOrder( const TModuleInfoEntryPairVector& mergeLinks ,
                                     const TModuleInfo& currentModule             )
{GUCEF_TRACE;

    TModuleInfoEntryPairVector::const_iterator i = mergeLinks.begin();
    while ( i != mergeLinks.end() )
    {
        if ( (*i).second->buildOrder == currentModule.buildOrder+1 )
        {
            return (*i).second;
        }
        ++i;
    }
    return NULL;
}

/*-------------------------------------------------------------------------*/

const TModuleInfoEntry*
FindModuleInfoEntryForMergedInfo( const TModuleInfoEntryPairVector& mergeLinks ,
                                  const TModuleInfo& mergedModule              )
{
    TModuleInfoEntryPairVector::const_iterator i = mergeLinks.begin();
    while ( i != mergeLinks.end() )
    {
        if ( (*i).second == &mergedModule )
        {
            return (*i).first;
        }
        ++i;
    }
    return NULL;
}

/*-------------------------------------------------------------------------*/

CORE::CString
GenerateContentForAndroidProjectMakefile( const CORE::CString& projectName             ,
                                          const TModuleInfoEntryPairVector& mergeLinks ,
                                          const CORE::CString& outputDir               ,
                                          bool addGeneratorCompileTimeToOutput         ,
                                          const TStringSet& ndkModulesUsed             )
{GUCEF_TRACE;

    GUCEF_LOG( CORE::LOGLEVEL_NORMAL, "Generating Android makefile content for overall project file regarding project \"" + projectName + "\"" );

    CORE::CString contentPrefix = makefileHeader;

    contentPrefix +=
      "#\n"
      "# This is the project makefile which includes all modules which are part of this project\n"
      "#\n";

    contentPrefix +=
      "# PROJECT: \"" + projectName + "\"\n#\n\n";

    if ( addGeneratorCompileTimeToOutput )
    {
        contentPrefix +=
          "#"
          "# The project generator version used was compiled on " __TIME__ __DATE__ "\n"
          "#\n\n";
    }

    contentPrefix +=
      "ifndef PROJECT_ROOT_PATH\n"
      "  PROJECT_ROOT_PATH := $(call my-dir)\n"
      "endif\n\n"
      "include $(CLEAR_VARS)\n\n";

    // Include makefiles for NDK modules used
    CORE::CString moduleListSection;

/* ->  Not needed
    if ( !ndkModulesUsed.empty() )
    {
        TStringSet::iterator i = ndkModulesUsed.find( "android_native_app_glue" );
        if ( i != ndkModulesUsed.end() ) 
        {
            moduleListSection += "MY_MODULE_PATH := $(ANDROIDNDK)/sources/android/native_app_glue\n";
            moduleListSection += "include $(MY_MODULE_PATH)/Android.mk\n\n";
        }
        i = ndkModulesUsed.find( "cpufeatures" );
        if ( i != ndkModulesUsed.end() ) 
        {
            moduleListSection += "MY_MODULE_PATH := $(ANDROIDNDK)/sources/android/cpufeatures\n";
            moduleListSection += "include $(MY_MODULE_PATH)/Android.mk\n\n";
        }
    }
*/
    // Include each module's makefile in the order listed as their build order
    const TModuleInfo* currentModule = FindFirstModuleAccordingToBuildOrder( mergeLinks );
    while ( NULL != currentModule )
    {
        if ( ( MODULETYPE_HEADER_INCLUDE_LOCATION != currentModule->moduleType ) &&
             ( MODULETYPE_CODE_INCLUDE_LOCATION != currentModule->moduleType )    )
        {
            // Get relative path from the outputDir to the other module
            const TModuleInfoEntry* fullModuleInfo = FindModuleInfoEntryForMergedInfo( mergeLinks, *currentModule );
            CORE::CString relativePathToModule = CORE::GetRelativePathToOtherPathRoot( outputDir, fullModuleInfo->rootDir );
            relativePathToModule = relativePathToModule.ReplaceChar( '\\', '/' );

            // Add entry for this module to the project file
            moduleListSection += "MY_MODULE_PATH := $(PROJECT_ROOT_PATH)/" + relativePathToModule + "\n";
            moduleListSection += "include $(MY_MODULE_PATH)/Android.mk\n\n";
        }

        // Done with this module, go to the next one
        currentModule = FindNextModuleAccordingToBuildOrder( mergeLinks, *currentModule );
    }

    return contentPrefix + moduleListSection;
}

/*-------------------------------------------------------------------------*/

bool
CreateAndroidProjectMakefileOnDisk( const CORE::CString& projectName             ,
                                    const TModuleInfoEntryPairVector& mergeLinks ,
                                    const CORE::CString& outputDir               ,
                                    bool addGeneratorCompileTimeToOutput         ,
                                    const TStringSet& ndkModulesUsed             )
{GUCEF_TRACE;

    GUCEF_LOG( CORE::LOGLEVEL_NORMAL, "Generating Android makefile content for overall project file regarding project \"" + projectName + "\"" );

    // First we generate the content for the makefile based on the given module information
    CORE::CString makefileContent = GenerateContentForAndroidProjectMakefile( projectName                     ,
                                                                              mergeLinks                      ,
                                                                              outputDir                       ,
                                                                              addGeneratorCompileTimeToOutput ,
                                                                              ndkModulesUsed                  );

    // Now we write the makefile to the root location of the project since everything is relative to that
    CORE::CString makefilePath = outputDir;
    CORE::AppendToPath( makefilePath, "Android.mk" );

    // Do not change the file on disk unless we have to.
    // the NDK build system will rebuild if the timestamp on the file changed which can take a long time
    if ( DidMakefileContentChange( makefilePath, makefileContent ) )
    {
        GUCEF_LOG( CORE::LOGLEVEL_NORMAL, "Writing Android makefile content for project \"" + projectName + "\" to " + makefilePath );

        if ( CORE::WriteStringAsTextFile( makefilePath    ,
                                          makefileContent ,
                                          true            ,
                                          "\n"            ) )
        {
            GUCEF_LOG( CORE::LOGLEVEL_NORMAL, "Successfully created Android makefile for project \"" + projectName + "\" at " + makefilePath );
            return true;
        }
        GUCEF_ERROR_LOG( CORE::LOGLEVEL_NORMAL, "Failed to create an Android makefile for project \"" + projectName + "\" at " + makefilePath );
        return false;
    }
    else
    {
        GUCEF_LOG( CORE::LOGLEVEL_NORMAL, "Skipping creation of overall project Android makefile for project \"" + projectName + "\" at " + makefilePath +
            " since there already is a makefile with identical content" );
        return true;
    }
}

/*-------------------------------------------------------------------------*/

CAndroidMakefileGenerator::CAndroidMakefileGenerator( void )
{GUCEF_TRACE;

}

/*-------------------------------------------------------------------------*/

CAndroidMakefileGenerator::~CAndroidMakefileGenerator()
{GUCEF_TRACE;

}

/*-------------------------------------------------------------------------*/

bool
CAndroidMakefileGenerator::GenerateProject( TProjectInfo& projectInfo            ,
                                            const CORE::CString& outputDir       ,
                                            bool addGeneratorCompileTimeToOutput ,
                                            const CORE::CValueList& params       )
{GUCEF_TRACE;

    // Merge all the module info to give us a complete module definition for the Android platform
    // per module. This makes is easy for us to process as we don't care about other platforms
    TModuleInfoVector mergedInfo;
    TModuleInfoEntryPairVector mergeLinks;
    MergeAllModuleInfoForPlatform( projectInfo.modules, "android", mergedInfo, mergeLinks );

    // First we create a makefile per module on disk
    TStringSet ndkModulesUsed;
    if ( CreateAndroidMakefileOnDiskForEachModule( mergeLinks                      ,
                                                   addGeneratorCompileTimeToOutput ,
                                                   ndkModulesUsed                  ) )
    {
        // Now we can create the overall project file which refers to each of the make files
        // we just created per module.
        return CreateAndroidProjectMakefileOnDisk( projectInfo.projectName         ,
                                                   mergeLinks                      ,
                                                   outputDir                       ,
                                                   addGeneratorCompileTimeToOutput ,
                                                   ndkModulesUsed                  );
    }
    return false;
}

/*-------------------------------------------------------------------------//
//                                                                         //
//      NAMESPACE                                                          //
//                                                                         //
//-------------------------------------------------------------------------*/

}; /* namespace PROJECTGENERATOR */
}; /* namespace GUCEF */

/*-------------------------------------------------------------------------*/
