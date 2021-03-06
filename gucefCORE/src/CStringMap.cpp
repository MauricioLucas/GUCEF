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

/*-------------------------------------------------------------------------//
//                                                                         //
//      INCLUDES                                                           //
//                                                                         //
//-------------------------------------------------------------------------*/

#ifndef GUCEF_CORE_CDYNAMICARRAY_H
#include "CDynamicArray.h"           /* dynamic array implementation */
#define GUCEF_CORE_CDYNAMICARRAY_H
#endif /* GUCEF_CORE_CDYNAMICARRAY_H ? */

#ifndef GUCEF_CORE_CDVSTRING_H
#include "CDVString.h"               /* string implementation */
#define GUCEF_CORE_CDVSTRING_H
#endif /* GUCEF_CORE_CDVSTRING_H ? */

#ifndef GUCEF_CORE_MACROS_H
#include "gucefCORE_macros.h"         /* often used gucef macros */
#define GUCEF_CORE_MACROS_H
#endif /* GUCEF_CORE_MACROS_H ? */

#include "CStringMap.h"

#ifndef GUCEF_CORE_GUCEF_ESSENTIALS_H
#include "gucef_essentials.h"
#define GUCEF_CORE_GUCEF_ESSENTIALS_H
#endif /* GUCEF_CORE_GUCEF_ESSENTIALS_H ? */  

/*-------------------------------------------------------------------------//
//                                                                         //
//      NAMESPACE                                                          //
//                                                                         //
//-------------------------------------------------------------------------*/

namespace GUCEF {
namespace CORE {

/*-------------------------------------------------------------------------//
//                                                                         //
//      TYPES                                                              //
//                                                                         //
//-------------------------------------------------------------------------*/ 

struct SStringLink
{
        CString typenamestr;
        void* linkdata;
};
typedef struct SStringLink TStringLink;

/*-------------------------------------------------------------------------//
//                                                                         //
//      CLASSES                                                            //
//                                                                         //
//-------------------------------------------------------------------------*/

CStringMap::CStringMap( void )
{
        GUCEF_BEGIN;
        GUCEF_END;
}

/*-------------------------------------------------------------------------*/

CStringMap::CStringMap( const CStringMap& src )
{
        GUCEF_BEGIN;
        _items.SetArraySize( _items.GetLast()+1 );
        TStringLink* srcsl, *sl;
        for ( Int32 i=0; i<=src._items.GetLast(); ++i )
        {
                srcsl = (TStringLink*) src._items[ i ];
                if ( srcsl )
                {
                        sl = new TStringLink;
                        CHECKMEM( sl, sizeof(TStringLink) );
                        sl->typenamestr = srcsl->typenamestr;
                        sl->linkdata = srcsl->linkdata;
                        _items.SetEntry( i, sl );
                }
                else
                {
                        _items.SetEntry( i, NULL );
                }
        }
        GUCEF_END;
}

/*-------------------------------------------------------------------------*/

CStringMap::~CStringMap()
{
        GUCEF_BEGIN;
        DeleteAll();
        GUCEF_END;
}

/*-------------------------------------------------------------------------*/

CStringMap&
CStringMap::operator=( const CStringMap& src )
{
        GUCEF_BEGIN;
        if ( this != &src )
        {
                DeleteAll();
                
                _items.SetArraySize( _items.GetLast()+1 );
                TStringLink* srcsl;
                TStringLink* sl;
                for ( Int32 i=0; i<=src._items.GetLast(); ++i )
                {
                        srcsl = (TStringLink*) src._items[ i ];
                        if ( srcsl )
                        {
                                CHECKMEM( srcsl, sizeof(TStringLink) );
                                sl = new TStringLink;
                                CHECKMEM( sl, sizeof(TStringLink) );
                                sl->typenamestr = srcsl->typenamestr;
                                sl->linkdata = srcsl->linkdata;
                                _items.SetEntry( i, sl );
                        }
                        else
                        {
                                _items.SetEntry( i, NULL );
                        }
                }
        }
        GUCEF_END;                
        return *this;        
}

/*-------------------------------------------------------------------------*/

void
CStringMap::DeleteAll( void )
{
        GUCEF_BEGIN;
        for ( Int32 i=0; i<=_items.GetLast(); ++i )
        {
                delete (TStringLink*) _items[ i ];
                _items.SetEntry( i, NULL );
        }
        GUCEF_END;
}        

/*-------------------------------------------------------------------------*/

/**
 *      Retrieves the pointer for the given name
 *      If the name is not found NULL will be returned.
 *
 *      @param name for the pointer you wish to obtain
 *      @return pointer related to the name given.
 */
void* 
CStringMap::Get( const CString& name ) const
{GUCEF_TRACE;
        return this->operator[]( name );
}

/*-------------------------------------------------------------------------*/

void* 
CStringMap::operator[]( const CString& name ) const
{
        GUCEF_BEGIN;
        TStringLink* sl;
        for ( Int32 i=0; i<=_items.GetLast(); ++i )
        {
                sl = (TStringLink*) _items[ i ];
                if ( sl )
                {
                        CHECKMEM( sl, sizeof(TStringLink) );
                        if ( sl->typenamestr == name )
                        {
                                GUCEF_END;
                                return sl->linkdata;
                        }
                }
        }
        GUCEF_END;
        return NULL;        
}

/*-------------------------------------------------------------------------*/

/**
 *      index operator for array-like access
 *      Retrieves the pointer for the given index
 *
 *      @param index for the pointer you wish to obtain
 *      @return pointer related to the given map entry index.
 */        
void* 
CStringMap::operator[]( const UInt32 index ) const
{GUCEF_TRACE;
        TStringLink* sl = (TStringLink*)_items[ index ];        
        if ( sl )
        {
                CHECKMEM( sl, sizeof(TStringLink) );
                return sl->linkdata;
        }       
        return NULL;
}

/*-------------------------------------------------------------------------*/

/**
 *      index operator for finding the string for the given data
 *
 *      @param data pointer for the string you wish to obtain
 *      @return pointer related to the given map entry index.
 */        
CString 
CStringMap::operator[]( const void* data ) const
{
        GUCEF_BEGIN;
        TStringLink* sl;
        for ( Int32 i=0; i<_items.GetLast(); ++i )
        {
                sl = (TStringLink*) _items[ i ];
                if ( sl )
                {
                        if ( sl->linkdata == data )
                        {
                                GUCEF_END;
                                return sl->typenamestr;
                        }
                }
        }
        CString foobar;
        GUCEF_END;
        return foobar;
}

/*-------------------------------------------------------------------------*/

UInt32 
CStringMap::GetCount( void ) const
{GUCEF_TRACE;
        return _items.GetCount();
}

/*-------------------------------------------------------------------------*/

Int32 
CStringMap::Add( const CString& name ,
                 void* linkeddata    )
{GUCEF_TRACE;
        if ( Get( name ) == NULL )
        {
                TStringLink* sl = new TStringLink;
                sl->linkdata = linkeddata;
                sl->typenamestr = name;
                return (Int32) _items.AddEntry( sl );
        }                  
        return -1;  
}

/*-------------------------------------------------------------------------*/                 
        
void 
CStringMap::Delete( const CString& name )
{GUCEF_TRACE;
        TStringLink* sl;
        for ( Int32 i=0; i<=_items.GetLast(); ++i )
        {
                sl = (TStringLink*) _items[ i ];
                if ( sl )
                {
                        if ( sl->typenamestr == name )
                        {
                                 _items.SetEntry( i, NULL );
                                 delete sl;
                        }
                }
        }
}

/*-------------------------------------------------------------------------*/

CString 
CStringMap::GetKey( const UInt32 index ) const
{GUCEF_TRACE;
        TStringLink* sl = (TStringLink*)_items[ index ];        
        if ( sl )
        {
                return sl->typenamestr;
        }
        
        CString emptystr;
        return emptystr;         
}

/*-------------------------------------------------------------------------*/

CStringList 
CStringMap::GetKeyList( void ) const
{GUCEF_TRACE;
        CStringList list;
        TStringLink* sl( NULL );
        for ( Int32 i=0; i<=_items.GetLast(); ++i )
        {
                sl = (TStringLink*) _items[ i ];
                if ( sl )
                {
                        list.Append( sl->typenamestr );
                }
        }
        return list;        
}

/*-------------------------------------------------------------------------*/

Int32 
CStringMap::GetKeyIndex( const CString& name ) const
{
        TStringLink* sl( NULL );
        for ( Int32 i=0; i<=_items.GetLast(); ++i )
        {
                sl = (TStringLink*) _items[ i ];
                if ( sl )
                {
                        if ( sl->typenamestr == name )
                        {
                                return i;
                        }
                }
        }
        return -1;
}

/*-------------------------------------------------------------------------//
//                                                                         //
//      NAMESPACE                                                          //
//                                                                         //
//-------------------------------------------------------------------------*/

}; /* namespace CORE */
}; /* namespace GUCEF */

/*-------------------------------------------------------------------------*/
