/*
 *  gucefANDROIDGLUE: GUCEF module providing glue code for Android
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

//BEGIN_INCLUDE(all)
#include <jni.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>             /* POSIX utilities */
#include <limits.h>             /* Linux OS limits */
#include <string.h>
#include <dlfcn.h>              /* dynamic linking utilities */
#include <stdio.h>

#include <android/log.h>
#include <android/asset_manager.h>
#include <android/native_activity.h>
#include <android_native_app_glue.h>

#include "gucefLOADER.h"

void
android_syslog(int level, const char *format, ...)
{
    va_list arglist;
    va_start( arglist, format );
    __android_log_vprint( level, "GalaxyUnlimitedPlatform", format, arglist );
    va_end( arglist );
    return;
}


#define FLOGI( format, ... ) ( (void) android_syslog( ANDROID_LOG_INFO, format, __VA_ARGS__) )
#define FLOGW( format, ... ) ( (void) android_syslog( ANDROID_LOG_WARN, format, __VA_ARGS__) )
#define FLOGF( format, ... ) ( (void) android_syslog( ANDROID_LOG_FATAL, format, __VA_ARGS__) )
#define FLOGE( format, ... ) ( (void) android_syslog( ANDROID_LOG_ERROR, format, __VA_ARGS__) )
#define FLOGD( format, ... ) ( (void) android_syslog( ANDROID_LOG_DEBUG, format, __VA_ARGS__) )

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "GalaxyUnlimitedPlatform", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "GalaxyUnlimitedPlatform", __VA_ARGS__))
#define LOGF(...) ((void)__android_log_print(ANDROID_LOG_FATAL, "GalaxyUnlimitedPlatform", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "GalaxyUnlimitedPlatform", __VA_ARGS__))
#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, "GalaxyUnlimitedPlatform", __VA_ARGS__))

typedef int ( GUCEF_CALLSPEC_PREFIX *TGUCEFCORECINTERFACE_LoadAndRunGucefPlatformApp ) ( const char* appName, const char* rootDir, const char* resRootDir, const char* libRootDir, int platformArgc, char** platformArgv, int appArgc, char** appArgv );

#define NULLPTR ((void*)(0))

/*-------------------------------------------------------------------------*/

char*
Combine2Strings( const char* str1 ,
                 const char* str2 )
{
    int str1len = strlen( str1 );
    int str2len = strlen( str2 );
    char* strBuffer = (char*) malloc( str1len + str2len + 1 );
    memcpy( strBuffer, str1, str1len );
    memcpy( strBuffer+str1len, str2, str2len+1 );
    return strBuffer;
}

/*-------------------------------------------------------------------------*/

char*
Combine3Strings( const char* str1 ,
                 const char* str2 ,
                 const char* str3 )
{
    int str1len = strlen( str1 );
    int str2len = strlen( str2 );
    int str3len = strlen( str3 );
    char* strBuffer = (char*) malloc( str1len + str2len + str3len + 1 );
    memcpy( strBuffer, str1, str1len );
    memcpy( strBuffer+str1len, str2, str2len );
    memcpy( strBuffer+str1len+str2len, str3, str3len+1 );
    return strBuffer;
}

/*-------------------------------------------------------------------------*/

char*
GetAssetPath( const char* packageDir ,
              const char* fileName   )
{
    return Combine3Strings( packageDir, "/assets/", fileName );
}

/*-------------------------------------------------------------------------*/

char*
GetLibPath( const char* packageDir ,
            const char* moduleName )
{
    return Combine3Strings( packageDir, "/lib/", moduleName );
}

/*-------------------------------------------------------------------------*/

int
InvokeLoadAndRunGucefPlatformApp( const char* appName ,
                                  const char* rootDir ,
                                  int platformArgc    ,
                                  char** platformArgv ,
                                  int appArgc         ,
                                  char** appArgv      )
{
    char* modulePath = GetLibPath( rootDir, "libgucefLOADER.so" );
    void* modulePtr = (void*) dlopen( modulePath, RTLD_NOW );
    if ( NULL == modulePtr )
    {
        free( modulePath );
        LOGF( "Unable to link gucefLOADER module" );
        return 0;
    }
    FLOGI( "Loading loader module from: %s", modulePath );
    free( modulePath );
    modulePath = NULL;

    TGUCEFCORECINTERFACE_LoadAndRunGucefPlatformApp loadAndRunGucefPlatformApp =
        (TGUCEFCORECINTERFACE_LoadAndRunGucefPlatformApp) dlsym( modulePtr, "LoadAndRunGucefPlatformApp" );

    if ( NULL == loadAndRunGucefPlatformApp )
    {
        LOGF( "Unable to link gucefLOADER function: LoadAndRunGucefPlatformApp" );
        dlclose( modulePtr );
        return 0;
    }

    char* libRootDir = GetLibPath( rootDir, "" );
    char* assetRootDir = GetAssetPath( rootDir, "" );

    int returnValue = loadAndRunGucefPlatformApp( appName      ,
                                                  rootDir      ,
                                                  assetRootDir ,
                                                  libRootDir   ,
                                                  platformArgc ,
                                                  platformArgv ,
                                                  appArgc      ,
                                                  appArgv      );

    free( libRootDir );
    libRootDir = NULL;
    free( assetRootDir );
    assetRootDir = NULL;
    dlclose( modulePtr );
    modulePtr = NULL;

    FLOGI( "LoadAndRunGucefPlatformApp returned with code %i", returnValue );
    return returnValue;
}

/*-------------------------------------------------------------------------*/

void
GetPackageDir( struct android_app* state ,
               char* pathBuffer          ,
               int bufferSize            )
{
    ANativeActivity* activity = state->activity;
    int pathLength = strlen( activity->internalDataPath );
    memcpy( pathBuffer, activity->internalDataPath, pathLength+1 );

    int i=pathLength;
    while ( i > 0 )
    {
        if ( pathBuffer[ i ] == '/' )
        {
            pathBuffer[ i ] = '\0';
            break;
        }
        --i;
    }
}

/*-------------------------------------------------------------------------*/

int
FileExists( const char* filename )
{
    struct stat buf;
    return stat( filename, &buf ) == 0;
}

/*-------------------------------------------------------------------------*/

static int
recursive_mkdir( const char* dir, int accessPerms )
{
    char tmp[ PATH_MAX ];
    char *p = NULL;
    size_t len;
    int retValue=0;

    snprintf( tmp, sizeof(tmp), "%s", dir );
    len = strlen( tmp );
    if( tmp[ len-1 ] == '/' )
    {
        tmp[ len-1 ] = 0;
    }
    for( p=tmp+1; *p; ++p )
    {
        if( *p == '/' )
        {
            *p = 0;
            retValue = mkdir( tmp, accessPerms );
            if ( 0 != retValue ) return retValue;
            *p = '/';
        }
    }
    retValue = mkdir( tmp, accessPerms );
    return retValue;
}

/*-------------------------------------------------------------------------*/

int
MakeDir( const char* path, int permissions )
{
    int retValue = recursive_mkdir( path, permissions );

    if ( retValue == 0 )
    {
        FLOGI( "created dir: %s", path );
    }
    else
    {
        if ( EEXIST == errno )
        {
            FLOGI( "found existing dir: %s", path );
        }
        else
        {
            FLOGI( "error %i creating dir: %s", errno, path );
        }
    }
    return retValue == 0;
}

/*-------------------------------------------------------------------------*/

int
MakeResourceDirs( const char* packageDir )
{
    // add more dirs if needed
    char* assetsDir = Combine2Strings( packageDir, "/assets" );
    int retValue = MakeDir( assetsDir, 00777 );
    free( assetsDir );
    return retValue;
}

/*-------------------------------------------------------------------------*/

int
ExtractAsset( AAssetManager* assetManager ,
              const char* assetPath       ,
              const char* destPath        )
{
    AAsset* asset = AAssetManager_open( assetManager, assetPath, AASSET_MODE_BUFFER );
    if ( NULL == asset )
    {
        FLOGI( "Unable to open asset for extraction: %s", assetPath );
        return 0;
    }

    const void* fileBuffer = AAsset_getBuffer( asset );
    if ( NULL == fileBuffer )
    {
        AAsset_close( asset );
        FLOGE( "Unable to get buffer to asset for extraction: %s", assetPath );
        return 0;
    }
    off_t bufferSize = AAsset_getLength( asset );

    FILE* destFile = fopen( destPath, "wb" );
    if ( 1 != fwrite( fileBuffer, bufferSize, 1, destFile ) )
    {
        FLOGE( "Error extracting asset from %s to %s", assetPath, destPath );
        fclose( destFile );
        AAsset_close( asset );
        return 0;
    }
    fclose( destFile );
    AAsset_close( asset );
    FLOGI( "Extracted asset from %s to %s", assetPath, destPath );
    return 1;
}

/*-------------------------------------------------------------------------*/

char*
ReadString( FILE* fptr, char* buffer, int bufferSize )
{
    char* strRead = fgets( buffer, bufferSize, fptr );
    if ( NULL != strRead )
    {
        // fgets() includes \n and \r, we dont want that
        // get rid of them...
        int i = strlen( strRead )-1;
        while ( i>0 )
        {
            if ( buffer[ i ] == '\n' || buffer[ i ] == '\r' )
            {
                buffer[ i ] = '\0';
            }
            else break;
            --i;
        }
    }
    return strRead;
}

/*-------------------------------------------------------------------------*/

void
ExtractAssets( struct android_app* state ,
               const char* packageDir    )
{
    AAssetManager* assetManager = state->activity->assetManager;

    // First extract the index file which tells us which other files to extract
    char* extractionIndexfilePath = GetAssetPath( packageDir, "filestoextract.txt" );
    int retValue = ExtractAsset( assetManager, "filestoextract.txt", extractionIndexfilePath );

    if ( 0 != retValue )
    {
        // Now go trough the list extraction all files listed.
        // each line in the index file is a file to extract
        char entryBuffer[ PATH_MAX ];
        FILE* listFptr = fopen( extractionIndexfilePath, "r" );
        char* bufferPtr = 0;
        if ( NULL == listFptr )
        {
            // Abort, unable to open the file
            FLOGE( "Error %i when opening file %s", errno, extractionIndexfilePath );
        }
        do
        {
            // Read the entry
            bufferPtr = ReadString( listFptr, entryBuffer, PATH_MAX );
            if ( NULL != bufferPtr )
            {
                // Create destination path and extract the asset
                char* destPath = GetAssetPath( packageDir, bufferPtr );
                retValue = ExtractAsset( assetManager, entryBuffer, destPath );
                free( destPath );

                if ( 0 == retValue )
                {
                    // Failed to extract the current entry, abort
                    break;
                }
            }
        }
        while ( NULL != bufferPtr );
        fclose( listFptr );
    }
    else
    {
        LOGI( "No extraction index is available, no assets will be extracted" );
    }

    free( extractionIndexfilePath );
}

/*-------------------------------------------------------------------------*/

int
IsFirstRun( const char* packageDir )
{
    // We know the gucefLOADER relies on a text file named firstrun.completed.txt
    // we will use the same convention here to keep things consistent
    char* firstrunFile = GetAssetPath( packageDir, "firstrun.completed.txt" );
    int existsBool = FileExists( firstrunFile );
    free( firstrunFile );
    return existsBool;
}

/*-------------------------------------------------------------------------*/

void
SetFirstRunCompleted( const char* packageDir )
{
    // We know the gucefLOADER relies on a text file named firstrun.completed.txt
    // we will use the same convention here to keep things consistent
    char* firstrunFile = GetAssetPath( packageDir, "firstrun.completed.txt" );
    int existsBool = FileExists( firstrunFile );
    if ( 0 == existsBool )
    {
        FILE* fptr = fopen( firstrunFile, "wb" );
        fclose( fptr );
    }
    free( firstrunFile );
}

/*-------------------------------------------------------------------------*/

/**
 * This is the main entry point of a native application that is using
 * android_native_app_glue.  It runs in its own thread, with its own
 * event loop for receiving input events and doing other things.
 */
void
android_main( struct android_app* state )
{
    // Make sure glue isn't stripped.
    app_dummy();

    char packageDir[ 512 ];
    GetPackageDir( state, packageDir, 512 );

    // Check if we need to perform first time initialization
    if ( 0 == IsFirstRun( packageDir ) )
    {
        LOGI( "Performing first run initialization" );

        // Make the resource dirs if they do not exist yet
        if ( 0 != MakeResourceDirs( packageDir ) )
        {
            return;
        }

        // Extract to our private storage as desired
        ExtractAssets( state, packageDir );

        LOGI( "Completed first run initialization" );
    }
    else
    {
        LOGI( "Detected previous run, skipping first run initialization" );
    }

    // Start the process of invoking the launch of the platform using the loader
    int appStatus = InvokeLoadAndRunGucefPlatformApp( "gucefPRODMAN", packageDir, 0, NULLPTR, 0, NULLPTR );

    // Check if we had a successfull run
    if ( 0 == appStatus )
    {
        LOGI( "Successfull completed first, setting first run flag to false" );

        // Set the flag that we completed the first run
        SetFirstRunCompleted( packageDir );
    }
}

/*-------------------------------------------------------------------------*/

//END_INCLUDE(all)
