/*
 *  gucefCORE: GUCEF module providing O/S abstraction and generic solutions
 *  Copyright (C) 2002 - 2007.  Dinand Vanvelzen
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

#ifndef GUCEF_CORE_DYNMEMORYMANAGERLOADER_H
#define GUCEF_CORE_DYNMEMORYMANAGERLOADER_H

/*-------------------------------------------------------------------------//
//                                                                         //
//      INCLUDES                                                           //
//                                                                         //
//-------------------------------------------------------------------------*/

#include "gucef_dynnewoff.h"      /* Make sure that the new/delete are not declared to avoid circular definitions. */

#include <windows.h>
#include <malloc.h>

/*-------------------------------------------------------------------------//
//                                                                         //
//      CONSTANTS                                                          //
//                                                                         //
//-------------------------------------------------------------------------*/

/*
 *      Posible allocation/deallocation types.
 *
 *      Declared as characters to minimize memory footprint,
 *      char = 1 byte
 *      enum types = int = 32 bits = 8 bytes on standard machines
 */
#undef MM_UNKNOWN
#undef MM_NEW
#undef MM_NEW_ARRAY
#undef MM_MALLOC
#undef MM_CALLOC
#undef MM_REALLOC
#undef MM_DELETE
#undef MM_DELETE_ARRAY
#undef MM_FREE
#define MM_UNKNOWN        0
#define MM_NEW            1
#define MM_NEW_ARRAY      2
#define MM_MALLOC         3
#define MM_CALLOC         4
#define MM_REALLOC        5
#define MM_DELETE         6
#define MM_DELETE_ARRAY   7
#define MM_FREE           8

/*-------------------------------------------------------------------------//
//                                                                         //
//      TYPES                                                              //
//                                                                         //
//-------------------------------------------------------------------------*/

typedef unsigned __int32 UInt32;


/*
 *      User API, these functions can be used to set parameters within the Memory
 *      Manager to control the type and extent of the memory tests that are performed.
 *      Note that it is not necessary to call any of these methods, you will get the default
 *      Memory Manager automatically.
 */

typedef UInt32 ( *TFP_MEMMAN_Initialize )( void );
typedef void ( *TFP_MEMMAN_Shutdown )( void );
typedef void ( *TFP_MEMMAN_DumpLogReport )( void );
typedef void ( *TFP_MEMMAN_DumpMemoryAllocations )( void );
typedef void ( *TFP_MEMMAN_SetLogFile )( const char *file );
typedef void ( *TFP_MEMMAN_SetExhaustiveTesting )( UInt32 test );
typedef void ( *TFP_MEMMAN_SetLogAlways )( UInt32 log );
typedef void ( *TFP_MEMMAN_SetPaddingSize )( UInt32 clean );
typedef void ( *TFP_MEMMAN_CleanLogFile )( UInt32 test );
typedef void ( *TFP_MEMMAN_BreakOnAllocation )( int alloccount );
typedef void ( *TFP_MEMMAN_BreakOnDeallocation )( void *address );
typedef void ( *TFP_MEMMAN_BreakOnReallocation )( void *address );
typedef void ( *TFP_MEMMAN_Validate )( const void* address, UInt32 blocksize, const char *file, int line );
typedef void ( *TFP_MEMMAN_ValidateChunk )( const void* address, const void* chunk, UInt32 blocksize, const char *file, int line );

/*-------------------------------------------------------------------------*/

/*
 *  Memory tracking functions which are invoked by the memory allocation overrides
 */

typedef void* ( *TFP_MEMMAN_AllocateMemory )( const char *file, int line, size_t size, char type, void *address );
typedef void ( *TFP_MEMMAN_DeAllocateMemory )( void *address, char type );
typedef void ( *TFP_MEMMAN_SetOwner )( const char *file, int line );

/*-------------------------------------------------------------------------//
//                                                                         //
//      GLOBAL VARS                                                        //
//                                                                         //
//-------------------------------------------------------------------------*/

static TFP_MEMMAN_Initialize fp_MEMMAN_Initialize = 0;
static TFP_MEMMAN_Shutdown fp_MEMMAN_Shutdown = 0;
static TFP_MEMMAN_DumpLogReport fp_MEMMAN_DumpLogReport = 0;
static TFP_MEMMAN_DumpMemoryAllocations fp_MEMMAN_DumpMemoryAllocations = 0;
static TFP_MEMMAN_SetLogFile fp_MEMMAN_SetLogFile = 0;
static TFP_MEMMAN_SetExhaustiveTesting fp_MEMMAN_SetExhaustiveTesting = 0;
static TFP_MEMMAN_SetLogAlways fp_MEMMAN_SetLogAlways = 0;
static TFP_MEMMAN_SetPaddingSize fp_MEMMAN_SetPaddingSize = 0;
static TFP_MEMMAN_CleanLogFile fp_MEMMAN_CleanLogFile = 0;
static TFP_MEMMAN_BreakOnAllocation fp_MEMMAN_BreakOnAllocation = 0;
static TFP_MEMMAN_BreakOnDeallocation fp_MEMMAN_BreakOnDeallocation = 0;
static TFP_MEMMAN_BreakOnReallocation fp_MEMMAN_BreakOnReallocation = 0;
static TFP_MEMMAN_Validate fp_MEMMAN_Validate = 0;
static TFP_MEMMAN_ValidateChunk fp_MEMMAN_ValidateChunk = 0;

/*-------------------------------------------------------------------------*/

static TFP_MEMMAN_AllocateMemory fp_MEMMAN_AllocateMemory = 0;
static TFP_MEMMAN_DeAllocateMemory fp_MEMMAN_DeAllocateMemory = 0;
static TFP_MEMMAN_SetOwner fp_MEMMAN_SetOwner = 0;

/*-------------------------------------------------------------------------*/

static void* g_memoryManagerModulePtr = 0; 
static void* g_dynLoadMutex = CreateMutex( NULL, FALSE, NULL );

/*-------------------------------------------------------------------------//
//                                                                         //
//      UTILITIES                                                          //
//                                                                         //
//-------------------------------------------------------------------------*/

static int 
LazyLoadMemoryManager( void )
{
    /* check to make sure we havent already loaded the library */
    if ( 0 != g_memoryManagerModulePtr ) return 1;    
    if ( WAIT_FAILED == WaitForSingleObject( (HANDLE) g_memoryManagerModulePtr, INFINITE ) ) return 0;
    if ( 0 != g_memoryManagerModulePtr ) return 1;

    /* check to see if the module is already loaded elsewhere in the process */
    g_memoryManagerModulePtr = (void*) GetModuleHandleA( "MemoryLeakFinder.dll" );
    if ( 0 == g_memoryManagerModulePtr )
    {
        /* load the library */
        g_memoryManagerModulePtr = (void*) LoadLibrary( "MemoryLeakFinder.dll" );
        if ( 0 == g_memoryManagerModulePtr )
        {
            ReleaseMutex( (HANDLE) g_dynLoadMutex );
            return 0;
        }
    }

    /* load the function pointers */
    fp_MEMMAN_Initialize = (TFP_MEMMAN_Initialize) GetProcAddress( (HMODULE) g_memoryManagerModulePtr, "MEMMAN_Initialize" );
    fp_MEMMAN_Shutdown = (TFP_MEMMAN_Shutdown) GetProcAddress( (HMODULE) g_memoryManagerModulePtr, "MEMMAN_Shutdown" );
    fp_MEMMAN_DumpLogReport = (TFP_MEMMAN_DumpLogReport) GetProcAddress( (HMODULE) g_memoryManagerModulePtr, "MEMMAN_DumpLogReport" );
    fp_MEMMAN_DumpMemoryAllocations = (TFP_MEMMAN_DumpMemoryAllocations) GetProcAddress( (HMODULE) g_memoryManagerModulePtr, "MEMMAN_DumpMemoryAllocations" );
    fp_MEMMAN_SetLogFile = (TFP_MEMMAN_SetLogFile) GetProcAddress( (HMODULE) g_memoryManagerModulePtr, "MEMMAN_SetLogFile" );
    fp_MEMMAN_SetExhaustiveTesting = (TFP_MEMMAN_SetExhaustiveTesting) GetProcAddress( (HMODULE) g_memoryManagerModulePtr, "MEMMAN_SetExhaustiveTesting" );
    fp_MEMMAN_SetLogAlways = (TFP_MEMMAN_SetLogAlways) GetProcAddress( (HMODULE) g_memoryManagerModulePtr, "MEMMAN_SetLogAlways" );
    fp_MEMMAN_SetPaddingSize = (TFP_MEMMAN_SetPaddingSize) GetProcAddress( (HMODULE) g_memoryManagerModulePtr, "MEMMAN_SetPaddingSize" );
    fp_MEMMAN_CleanLogFile = (TFP_MEMMAN_CleanLogFile) GetProcAddress( (HMODULE) g_memoryManagerModulePtr, "MEMMAN_CleanLogFile" );
    fp_MEMMAN_BreakOnAllocation = (TFP_MEMMAN_BreakOnAllocation) GetProcAddress( (HMODULE) g_memoryManagerModulePtr, "MEMMAN_BreakOnAllocation" );
    fp_MEMMAN_BreakOnDeallocation = (TFP_MEMMAN_BreakOnDeallocation) GetProcAddress( (HMODULE) g_memoryManagerModulePtr, "MEMMAN_BreakOnDeallocation" );
    fp_MEMMAN_BreakOnReallocation = (TFP_MEMMAN_BreakOnReallocation) GetProcAddress( (HMODULE) g_memoryManagerModulePtr, "MEMMAN_BreakOnReallocation" );
    fp_MEMMAN_Validate = (TFP_MEMMAN_Validate) GetProcAddress( (HMODULE) g_memoryManagerModulePtr, "MEMMAN_Validate" );
    fp_MEMMAN_ValidateChunk = (TFP_MEMMAN_ValidateChunk) GetProcAddress( (HMODULE) g_memoryManagerModulePtr, "MEMMAN_ValidateChunk" );
    fp_MEMMAN_AllocateMemory = (TFP_MEMMAN_AllocateMemory) GetProcAddress( (HMODULE) g_memoryManagerModulePtr, "MEMMAN_AllocateMemory" );
    fp_MEMMAN_DeAllocateMemory = (TFP_MEMMAN_DeAllocateMemory) GetProcAddress( (HMODULE) g_memoryManagerModulePtr, "MEMMAN_DeAllocateMemory" );
    fp_MEMMAN_SetOwner = (TFP_MEMMAN_SetOwner) GetProcAddress( (HMODULE) g_memoryManagerModulePtr, "MEMMAN_SetOwner" );

    ReleaseMutex( (HANDLE) g_dynLoadMutex );

    return 1;
}

/*-------------------------------------------------------------------------*/

#endif /* GUCEF_CORE_DYNMEMORYMANAGERLOADER_H ? */