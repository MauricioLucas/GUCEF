/*
 *  ArchiveDiffLib: library with some archive diff util functions
 *
 *  Copyright (C) 2002 - 2010.  Dinand Vanvelzen
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

#include "ArchiveDiffLib.h"

/*-------------------------------------------------------------------------*/

using namespace GUCEF;

/*-------------------------------------------------------------------------//
//                                                                         //
//      NAMESPACE                                                          //
//                                                                         //
//-------------------------------------------------------------------------*/

namespace ARCHIVEDIFF {

/*-------------------------------------------------------------------------//
//                                                                         //
//      CONSTANTS                                                          //
//                                                                         //
//-------------------------------------------------------------------------*/

#define VERSION_NUMBER  1.1

/*-------------------------------------------------------------------------//
//                                                                         //
//      UTILITIES                                                          //
//                                                                         //
//-------------------------------------------------------------------------*/

void
InitializeFileStatus( TFileStatus& fileStatus )
{GUCEF_TRACE;

    fileStatus.resourceState = RESOURCESTATE_UNKNOWN;
    
    fileStatus.templateArchiveInfo.name.Clear();
    fileStatus.templateArchiveInfo.hash.Clear();
    fileStatus.templateArchiveInfo.sizeInBytes = 0;
    fileStatus.templateArchiveInfo.fileLocations.clear();
    
    fileStatus.mainSvnArchiveInfo.name.Clear();
    fileStatus.mainSvnArchiveInfo.hash.Clear();
    fileStatus.mainSvnArchiveInfo.sizeInBytes = 0;
    fileStatus.mainSvnArchiveInfo.fileLocations.clear();
}

/*---------------------------------------------------------------------------*/

CORE::CString
CreatePathToUnchangedFileDiff( const CORE::CString& dir )
{GUCEF_TRACE;
    
    CORE::CString path = dir;
    CORE::AppendToPath( path, "UnchangedFiles.xml" );
    return path;
}

/*---------------------------------------------------------------------------*/

CORE::CString
CreatePathToChangedFileDiff( const CORE::CString& dir )
{GUCEF_TRACE;
    
    CORE::CString path = dir;
    CORE::AppendToPath( path, "ChangedFiles.xml" );
    return path;
}

/*---------------------------------------------------------------------------*/

CORE::CString
CreatePathToAddedFileDiff( const CORE::CString& dir )
{GUCEF_TRACE;
    
    CORE::CString path = dir;
    CORE::AppendToPath( path, "AddedFiles.xml" );
    return path;
}

/*---------------------------------------------------------------------------*/

CORE::CString
CreatePathToMissingFileDiff( const CORE::CString& dir )
{GUCEF_TRACE;
    
    CORE::CString path = dir;
    CORE::AppendToPath( path, "MissingFiles.xml" );
    return path;

}

/*---------------------------------------------------------------------------*/

CORE::CDStoreCodecRegistry::TDStoreCodecPtr
GetXmlDStoreCodec( void )
{GUCEF_TRACE;

    static CORE::CDStoreCodecRegistry::TDStoreCodecPtr codecPtr;
    if ( codecPtr.IsNULL() )
    {
        CORE::CDStoreCodecRegistry* registry = CORE::CDStoreCodecRegistry::Instance();
        if ( !registry->TryLookup( "XML", codecPtr, false ) )
        {
            // No codec is registered to handle XML, try and load a plugin for it

            CORE::CPluginManager::TPluginPtr codecPlugin =

                #ifdef GUCEF_CORE_DEBUG_MODE
                CORE::CDStoreCodecPluginManager::Instance()->LoadPlugin( "$MODULEDIR$/dstorepluginPARSIFALXML_d" );
                #else
                CORE::CDStoreCodecPluginManager::Instance()->LoadPlugin( "$MODULEDIR$/dstorepluginPARSIFALXML" );
                #endif

            if ( NULL != codecPlugin )
            {
                // Now try and get the codec again
                registry->TryLookup( "XML", codecPtr, false );
                GUCEF_LOG( CORE::LOGLEVEL_IMPORTANT, "Request for data storage codec for xml file, succesfully loaded plugin to handle request" );
            }
            else
            {
                GUCEF_ERROR_LOG( CORE::LOGLEVEL_IMPORTANT, "Request for data storage codec for xml file but no plugin for it could be loaded!" );
                CORE::ShowErrorMessage( "Missing codec support",
                                        "Request for data storage codec for xml file but no plugin for it could be loaded!" );
            }
        }
    }
    return codecPtr;
}

/*---------------------------------------------------------------------------*/

void
LocateFileEntriesUsingName( const PATCHER::CPatchSetParser::TFileEntry& patchsetFileEntry ,
                            const CORE::CString& relMainArchivePath                       ,
                            PATCHER::CPatchSetParser::TFileEntryList& fileList            ,
                            const CORE::CString& name                                     )
{GUCEF_TRACE;

    if ( patchsetFileEntry.name.Equals( name, false ) )
    {        
        CORE::CString relFilePath = relMainArchivePath;
        CORE::AppendToPath( relFilePath, patchsetFileEntry.name );
        
        PATCHER::CPatchSetParser::TFileEntry entry = patchsetFileEntry;
        PATCHER::CPatchSetParser::TFileLocation diskLocation;
        diskLocation.codec = "FILE"; //<- so we can differentiate from the original location item
        diskLocation.URL = relFilePath;
        entry.fileLocations.push_back( diskLocation );
        
        GUCEF_LOG( CORE::LOGLEVEL_BELOW_NORMAL, "Found file entry with the same name at " + relFilePath );
        
        fileList.push_back( entry );
    }
}

/*---------------------------------------------------------------------------*/

void
LocateFileEntriesUsingName( const PATCHER::CPatchSetParser::TDirEntry& patchsetDirEntry ,
                            const CORE::CString& relMainArchivePath                     ,
                            PATCHER::CPatchSetParser::TFileEntryList& fileList          ,
                            const CORE::CString& name                                   )
{GUCEF_TRACE;

    PATCHER::CPatchSetParser::TFileEntryList::const_iterator i = patchsetDirEntry.files.begin();
    while ( i != patchsetDirEntry.files.end() )       
    {
        LocateFileEntriesUsingName( (*i)               ,
                                    relMainArchivePath ,
                                    fileList           ,
                                    name               );
        
        ++i;
    }
    
    PATCHER::CPatchSetParser::TPatchSet::const_iterator n = patchsetDirEntry.subDirs.begin(); 
    while ( n != patchsetDirEntry.subDirs.end() )        
    {
        CORE::CString subDirPath = relMainArchivePath;
        CORE::AppendToPath( subDirPath, (*n).name );
        
        LocateFileEntriesUsingName( (*n)       ,
                                    subDirPath ,
                                    fileList   ,
                                    name       );
        
        ++n;
    }
}

/*---------------------------------------------------------------------------*/

void
LocateFileEntriesUsingName( const PATCHER::CPatchSetParser::TPatchSet& patchSet ,
                            const CORE::CString& relMainArchivePath             ,
                            PATCHER::CPatchSetParser::TFileEntryList& fileList  ,
                            const CORE::CString& name                           )
{GUCEF_TRACE;

    PATCHER::CPatchSetParser::TPatchSet::const_iterator i = patchSet.begin();
    while ( i != patchSet.end() )        
    {
        CORE::CString subDirPath = relMainArchivePath;
        CORE::AppendToPath( subDirPath, (*i).name );

        LocateFileEntriesUsingName( (*i)       ,
                                    subDirPath ,
                                    fileList   ,
                                    name       );
        
        ++i;
    }
}

/*---------------------------------------------------------------------------*/

CORE::CString
GetPathForFile( const PATCHER::CPatchSetParser::TFileEntry& fileEntry )
{GUCEF_TRACE;

    PATCHER::CPatchSetParser::TFileLocations::const_iterator i = fileEntry.fileLocations.begin();
    while ( i != fileEntry.fileLocations.end() )
    {
        const PATCHER::CPatchSetParser::TFileLocation& location = (*i);
        if ( "FILE" == location.codec )
        {
            return location.URL;
        }
    }
    return CORE::CString();
}

/*---------------------------------------------------------------------------*/

bool
PerformArchiveDiff( const PATCHER::CPatchSetParser::TFileEntry& templatePatchsetFileEntry ,
                    const CORE::CString& relTemplatePatchsetDirPath                       ,
                    const PATCHER::CPatchSetParser::TPatchSet& mainArchivePatchset        ,
                    TFileStatusVector& fileStatusList                                     )
{GUCEF_TRACE;

    CORE::CString templateFilePath = relTemplatePatchsetDirPath;
    CORE::AppendToPath( templateFilePath, templatePatchsetFileEntry.name );
       
    // First locate all files in the main archive that have the same name as
    // our template archive file
    GUCEF_LOG( CORE::LOGLEVEL_BELOW_NORMAL, "Commencing search for files with the same name as template file: " + templateFilePath );
    PATCHER::CPatchSetParser::TFileEntryList filesWithSameName;
    LocateFileEntriesUsingName( mainArchivePatchset            , 
                                CORE::CString()                ,
                                filesWithSameName              ,
                                templatePatchsetFileEntry.name );

    GUCEF_LOG( CORE::LOGLEVEL_BELOW_NORMAL, "Found " + CORE::UInt32ToString( filesWithSameName.size() ) + " files with the same name as our template file in the main archive" );                                
    
    // Now check the resource state for each of those files
    PATCHER::CPatchSetParser::TFileEntryList::iterator i = filesWithSameName.begin();
    while ( i != filesWithSameName.end() )
    {        
        PATCHER::CPatchSetParser::TFileEntry& fileEntry = (*i);
        
        if ( ( templatePatchsetFileEntry.hash == fileEntry.hash )               &&
             ( templatePatchsetFileEntry.sizeInBytes == fileEntry.sizeInBytes )  )
        {
            // File content is identical
            CORE::CString relPathForMainArchiveFile = GetPathForFile( fileEntry );
            if ( templateFilePath.Equals( relPathForMainArchiveFile, false ) )
            {                
                TFileStatus fileStatus;
                InitializeFileStatus( fileStatus );
                fileStatus.resourceState = RESOURCESTATE_FILE_UNCHANGED;
                fileStatus.templateArchiveInfo = templatePatchsetFileEntry;
                fileStatus.mainSvnArchiveInfo = fileEntry;
                
                PATCHER::CPatchSetParser::TFileLocation templateLocation;
                templateLocation.codec = "FILE";
                templateLocation.URL = templateFilePath;
                fileStatus.templateArchiveInfo.fileLocations.push_back( templateLocation );
                
                GUCEF_LOG( CORE::LOGLEVEL_NORMAL, "Determined that file \"" + templateFilePath + "\" was unchanged" );
                
                fileStatusList.push_back( fileStatus );
            }
            else
            {
                TFileStatus fileStatus;
                InitializeFileStatus( fileStatus );
                fileStatus.resourceState = RESOURCESTATE_FILE_UNCHANGED_BUT_MOVED;
                fileStatus.templateArchiveInfo = templatePatchsetFileEntry;
                fileStatus.mainSvnArchiveInfo = fileEntry;
                
                PATCHER::CPatchSetParser::TFileLocation templateLocation;
                templateLocation.codec = "FILE";
                templateLocation.URL = templateFilePath;
                fileStatus.templateArchiveInfo.fileLocations.push_back( templateLocation );

                GUCEF_LOG( CORE::LOGLEVEL_NORMAL, "Determined that file \"" + templateFilePath + "\" was unchanged but moved to \"" +  + "\"" );
                
                fileStatusList.push_back( fileStatus );
            }
        }
        else
        {
            // File content is different
            TFileStatus fileStatus;
            InitializeFileStatus( fileStatus );
            fileStatus.resourceState = RESOURCESTATE_FILE_CHANGED;
            fileStatus.templateArchiveInfo = templatePatchsetFileEntry;
            fileStatus.mainSvnArchiveInfo = fileEntry;

            PATCHER::CPatchSetParser::TFileLocation templateLocation;
            templateLocation.codec = "FILE";
            templateLocation.URL = templateFilePath;
            fileStatus.templateArchiveInfo.fileLocations.push_back( templateLocation );
            
            GUCEF_LOG( CORE::LOGLEVEL_NORMAL, "Determined that file \"" + templateFilePath + "\" was changed at location \"" +  + "\"" );
            
            fileStatusList.push_back( fileStatus );
        }
        
        ++i;
    }
    
    if ( 0 == filesWithSameName.size() )
    {
        // Unable to locate such a file in the other archive
        TFileStatus fileStatus;
        InitializeFileStatus( fileStatus );
        fileStatus.resourceState = RESOURCESTATE_FILE_ADDED;
        fileStatus.templateArchiveInfo = templatePatchsetFileEntry;

        PATCHER::CPatchSetParser::TFileLocation templateLocation;
        templateLocation.codec = "FILE";
        templateLocation.URL = templateFilePath;
        fileStatus.templateArchiveInfo.fileLocations.push_back( templateLocation );
        
        GUCEF_LOG( CORE::LOGLEVEL_NORMAL, "Determined that file \"" + templateFilePath + "\" was added in the template archive" );
        
        fileStatusList.push_back( fileStatus );
    }
        
    return true;
}

/*---------------------------------------------------------------------------*/

bool
PerformArchiveDiff( const PATCHER::CPatchSetParser::TDirEntry& templatePatchsetDirEntry ,
                    const CORE::CString& relTemplatePatchsetDirPath                     ,
                    const PATCHER::CPatchSetParser::TPatchSet& mainArchivePatchset      ,
                    TFileStatusVector& fileStatusList                                   )
{GUCEF_TRACE;

    // process all files in this dir
    PATCHER::CPatchSetParser::TFileEntryList::const_iterator i = templatePatchsetDirEntry.files.begin();
    while ( i != templatePatchsetDirEntry.files.end() )
    {
        PerformArchiveDiff( (*i)                          ,
                            relTemplatePatchsetDirPath    ,
                            mainArchivePatchset           ,
                            fileStatusList                );
        
        ++i;
    }

    PATCHER::CPatchSetParser::TDirEntryList::const_iterator n = templatePatchsetDirEntry.subDirs.begin();
    while ( n != templatePatchsetDirEntry.subDirs.end() )        
    {
        // create relative path to sub-dir
        CORE::CString fullPathToSubDir = relTemplatePatchsetDirPath;
        CORE::AppendToPath( fullPathToSubDir, (*n).name );
        
        // do the same for the sub-dir
        PerformArchiveDiff( (*n)                ,
                            fullPathToSubDir    ,
                            mainArchivePatchset ,
                            fileStatusList      );
        
        ++n;
    }

    return true;
}

/*---------------------------------------------------------------------------*/

bool
PerformArchiveDiff( const PATCHER::CPatchSetParser::TPatchSet& templatePatchset    ,
                    const PATCHER::CPatchSetParser::TPatchSet& mainArchivePatchset ,
                    TFileStatusVector& fileStatusList                              )
{GUCEF_TRACE;

    PATCHER::CPatchSetParser::TPatchSet::const_iterator i = templatePatchset.begin();
    while ( i != templatePatchset.end() )        
    {
        PerformArchiveDiff( (*i)                ,
                            (*i).name           ,
                            mainArchivePatchset ,
                            fileStatusList      );
        
        ++i;
    }
    return true;
}

/*---------------------------------------------------------------------------*/

bool
LoadPatchSet( const CORE::CString& filePath  ,
              CORE::CDataNode& patchSet      )
{GUCEF_TRACE;

    if ( CORE::FileExists( filePath ) )
    {
        GUCEF_LOG( CORE::LOGLEVEL_NORMAL, "Successfully located file: " + filePath );
        
        CORE::CDStoreCodecRegistry::TDStoreCodecPtr codecPtr = GetXmlDStoreCodec();
        if ( !codecPtr.IsNULL() )
        {
            GUCEF_LOG( CORE::LOGLEVEL_NORMAL, "Successfully located codec: " + codecPtr->GetTypeName() );
            
            if ( codecPtr->BuildDataTree( &patchSet ,
                                          filePath  ) )
            {
                return true;
            }
        }
    }
    return false;
}

/*---------------------------------------------------------------------------*/

bool
LoadPatchSet( const CORE::CString& filePath                 ,
              PATCHER::CPatchSetParser::TPatchSet& patchSet )
{GUCEF_TRACE;

    CORE::CDataNode patchSetTree;
    if ( LoadPatchSet( filePath     ,
                       patchSetTree ) )
    {
        GUCEF_LOG( CORE::LOGLEVEL_NORMAL, "Successfully loaded the raw patch set data from file, parsing data into strongly typed data structures" );
        
        PATCHER::CPatchSetParser parser;
        return parser.ParsePatchSet( patchSetTree ,
                                     patchSet     );
    }
    return false;
}

/*-------------------------------------------------------------------------//
//                                                                         //
//      NAMESPACE                                                          //
//                                                                         //
//-------------------------------------------------------------------------*/

}; /* namespace ARCHIVEDIFF */

/*-------------------------------------------------------------------------*/
