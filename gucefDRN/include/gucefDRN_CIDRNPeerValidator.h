/*
 *  gucefDRN: GUCEF module providing RAD networking trough data replication
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

#ifndef GUCEF_DRN_CIDRNPEERVALIDATOR_H
#define GUCEF_DRN_CIDRNPEERVALIDATOR_H

/*-------------------------------------------------------------------------//
//                                                                         //
//      INCLUDES                                                           //
//                                                                         //
//-------------------------------------------------------------------------*/ 

#ifndef GUCEF_COMCORE_CSOCKET_H
#include "CSocket.h"
#define GUCEF_COMCORE_CSOCKET_H
#endif /* GUCEF_COMCORE_CSOCKET_H ? */

#ifndef GUCEF_CORE_ESTRUCTS_H
#include "EStructs.h"
#define GUCEF_CORE_ESTRUCTS_H
#endif /* GUCEF_CORE_ESTRUCTS_H ? */

#ifndef GUCEF_DRN_CDRNNODE_H
#include "gucefDRN_CDRNNode.h"
#define GUCEF_DRN_CDRNNODE_H
#endif /* GUCEF_DRN_CDRNNODE_H ? */

/*-------------------------------------------------------------------------//
//                                                                         //
//      NAMESPACE                                                          //
//                                                                         //
//-------------------------------------------------------------------------*/

namespace GUCEF {
namespace DRN {

/*-------------------------------------------------------------------------//
//                                                                         //
//      CLASSES                                                            //
//                                                                         //
//-------------------------------------------------------------------------*/

class GUCEF_DRN_EXPORT_CPP CIDRNPeerValidator
{
    public:
    
    typedef COMCORE::CHostAddress CHostAddress;
    
    CIDRNPeerValidator( void );
    
    CIDRNPeerValidator( const CIDRNPeerValidator& src );
    
    virtual ~CIDRNPeerValidator();
    
    CIDRNPeerValidator& operator=( const CIDRNPeerValidator& src );
    
    virtual bool IsPeerAddressValid( const CHostAddress& hostAddress ) const = 0;

    virtual bool IsPeerLoginRequired( const CDRNPeerLink& peerLink ) const = 0;
    
    virtual bool IsPeerLoginValid( const CDRNPeerLink& peerLink     ,
                                   const CORE::CString& accountName ,
                                   const CORE::CString& password    ) const = 0;

    virtual bool IsPeerDataStreamSubscriptionAllowed( const CDRNPeerLink& peerLink        ,
                                                      const CORE::CString& dataStreamName ) const = 0;

    virtual bool IsPeerDataGroupSubscriptionAllowed( const CDRNPeerLink& peerLink       ,
                                                      const CORE::CString& dataGroupName ) const = 0;

    virtual bool IsPeerServiceCompatible( const CDRNPeerLink& peerLink         ,
                                          const CORE::CString& peerServiceName ) const = 0;
                                                      
    virtual CORE::CString GetServiceName( const CDRNPeerLink& peerLink ) const = 0;
                                                                                                 
    virtual bool GetLoginForPeer( const CDRNPeerLink& peerLink  ,
                                  CORE::CString& ourAccountName ,
                                  CORE::CString& ourPassword    ) = 0;    
};

/*-------------------------------------------------------------------------//
//                                                                         //
//      NAMESPACE                                                          //
//                                                                         //
//-------------------------------------------------------------------------*/

}; /* namespace DRN */
}; /* namespace GUCEF */

/*-------------------------------------------------------------------------*/

#endif /* GUCEF_DRN_CIDRNPEERVALIDATOR_H ? */

/*-------------------------------------------------------------------------//
//                                                                         //
//      Info & Changes                                                     //
//                                                                         //
//-------------------------------------------------------------------------//

- 02-03-2007 :
        - Dinand: re-added this header

-----------------------------------------------------------------------------*/
