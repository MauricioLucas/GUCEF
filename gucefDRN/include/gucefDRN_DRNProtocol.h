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

#ifndef GUCEF_DRN_DRNPROTOCOL_H
#define GUCEF_DRN_DRNPROTOCOL_H

/*-------------------------------------------------------------------------//
//                                                                         //
//      CONSTANTS                                                          //
//                                                                         //
//-------------------------------------------------------------------------*/

#define DRN_PROTOCOL_MAYOR_VERION               0
#define DRN_PROTOCOL_MINOR_VERION               1
#define DRN_PROTOCOL_PATCH_VERION               0

#define DRN_TRANSMISSION_START                  7
#define DRN_TRANSMISSION_END                    8

/* The controller system is on the drawing board
#define DRN_CONTROLLERCOMM_NETWORK_INFO                     17
#define DRN_CONTROLLERCOMM_CONNECT_TO_PEER_COMMAND          21
#define DRN_CONTROLLERCOMM_CONNECT_TO_CONTROLLER_COMMAND    22
*/

#define DRN_PEERCOMM_GREETING                       100
#define DRN_PEERCOMM_SERVICE                        101
#define DRN_PEERCOMM_INCOMPATIBLE_LINK              102
#define DRN_PEERCOMM_AUTHENTICATION                 103
#define DRN_PEERCOMM_AUTHENTICATION_REQUIRED        104
#define DRN_PEERCOMM_AUTHENTICATION_FAILED          105
#define DRN_PEERCOMM_AUTHENTICATION_SUCCESS         106
#define DRN_PEERCOMM_LINK_OPERATIONAL               107
#define DRN_PEERCOMM_DATAGROUP_ITEM_ADDED           108
#define DRN_PEERCOMM_DATAGROUP_ITEM_UPDATE          109
#define DRN_PEERCOMM_DATAGROUP_ITEM_REMOVED         110
#define DRN_PEERCOMM_STREAM_DATA                    111
#define DRN_PEERCOMM_NOT_ALLOWED                    112
#define DRN_PEERCOMM_PEERLIST_REQUEST               113
#define DRN_PEERCOMM_PEERLIST                       114
#define DRN_PEERCOMM_STREAMLIST_REQUEST             115
#define DRN_PEERCOMM_STREAMLIST                     116
#define DRN_PEERCOMM_DATAGROUPLIST_REQUEST          117
#define DRN_PEERCOMM_DATAGROUPLIST                  118
#define DRN_PEERCOMM_SUBSCRIBE_TO_DATAGROUP_REQUEST 119
#define DRN_PEERCOMM_SUBSCRIBE_TO_STREAM_REQUEST    120
#define DRN_PEERCOMM_SUBSCRIBED_TO_DATAGROUP        121
#define DRN_PEERCOMM_SUBSCRIBED_TO_DATASTREAM       122
#define DRN_PEERCOMM_CONNECTBACKINFO                123

/*-------------------------------------------------------------------------*/

#endif /* GUCEF_DRN_DRNPROTOCOL_H ? */

/*-------------------------------------------------------------------------//
//                                                                         //
//      Info & Changes                                                     //
//                                                                         //
//-------------------------------------------------------------------------//

- 23-07-2005 :
        - initial version

-----------------------------------------------------------------------------*/