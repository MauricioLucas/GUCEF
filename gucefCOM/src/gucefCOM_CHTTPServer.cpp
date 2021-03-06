/*
 *  gucefCOM: GUCEF module providing communication
 *  implementations for standardized protocols.
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

#ifndef GUCEF_CORE_CLOGMANAGER_H
#include "CLogManager.h"
#define GUCEF_CORE_CLOGMANAGER_H
#endif /* GUCEF_CORE_CLOGMANAGER_H ? */

#ifndef GUCEF_COM_CDEFAULTHTTPSERVERROUTERCONTROLLER_H
#include "gucefCOM_CDefaultHTTPServerRouterController.h"
#define GUCEF_COM_CDEFAULTHTTPSERVERROUTERCONTROLLER_H
#endif /* GUCEF_COM_CDEFAULTHTTPSERVERROUTERCONTROLLER_H ? */

#include "gucefCOM_CHTTPServer.h"

/*-------------------------------------------------------------------------//
//                                                                         //
//      NAMESPACE                                                          //
//                                                                         //
//-------------------------------------------------------------------------*/

namespace GUCEF {
namespace COM {

/*-------------------------------------------------------------------------//
//                                                                         //
//      CLASSES                                                            //
//                                                                         //
//-------------------------------------------------------------------------*/

CHTTPServer::CHTTPServer( CIHTTPServerRouterController* routerController /* = NULL */ )
    : CObserver()                ,
      m_tcpServerSocket( false ) ,
      m_routerController( NULL ) ,
      m_lastRequestUri()
{GUCEF_TRACE;

    if ( NULL == routerController )
    {
        m_routerController = new CDefaultHTTPServerRouterController( *this );
    }
    else
    {
        m_routerController = routerController;
    }

    m_tcpServerSocket.Subscribe( this );
}

/*-------------------------------------------------------------------------*/

CHTTPServer::CHTTPServer( CORE::CPulseGenerator& pulsGenerator                        ,
                          CIHTTPServerRouterController* routerController /* = NULL */ )
    : CObserver()                               ,
      m_tcpServerSocket( pulsGenerator, false ) ,
      m_routerController( NULL )                ,
      m_lastRequestUri()
{GUCEF_TRACE;

    if ( NULL == routerController )
    {
        m_routerController = new CDefaultHTTPServerRouterController( *this );
    }
    else
    {
        m_routerController = routerController;
    }

    m_tcpServerSocket.Subscribe( this );
}

/*-------------------------------------------------------------------------*/

CHTTPServer::~CHTTPServer()
{GUCEF_TRACE;

}

/*-------------------------------------------------------------------------*/

bool
CHTTPServer::MatchResourceVersion( const CString& resourceVersion  ,
                                   const TStringVector& searchList )
{GUCEF_TRACE;

    if ( searchList.size() > 0 && resourceVersion.Length() > 0 )
    {
        TStringVector::const_iterator i = searchList.begin();
        while( i != searchList.end() )
        {
            if ( resourceVersion.Equals( *i, false ) )
            {
                return true;
            }
            ++i;
        }
    }
    return false;
}

/*-------------------------------------------------------------------------*/

CHTTPServer::THttpReturnData*
CHTTPServer::OnRead( const THttpRequestData& request )
{GUCEF_TRACE;

    return PerformReadOperation( request, true );
}

/*-------------------------------------------------------------------------*/

CHTTPServer::THttpReturnData*
CHTTPServer::OnReadMetaData( const THttpRequestData& request )
{GUCEF_TRACE;

    return PerformReadOperation( request, false );
}

/*-------------------------------------------------------------------------*/

CHTTPServer::THttpReturnData*
CHTTPServer::PerformReadOperation( const THttpRequestData& request ,
                                   bool contentRequested           )
{GUCEF_TRACE;

    THttpReturnData* returnData = new THttpReturnData;

    if ( NULL == m_routerController )
    {
        returnData->statusCode = 500; // Improperly configured
        return returnData;
    }

    try
    {
        const CString& resourceURI = request.requestUri;
        m_lastRequestUri = resourceURI;

        CIHTTPServerRouter* resourceRouter = m_routerController->GetHandler( resourceURI );
        if ( NULL == resourceRouter )
        {
            // No handler is mounted to handle this Uri
            returnData->statusCode = 404;
            return returnData;
        }

        // Obtain the relative Uri for the given handler
        CString uriAfterBaseAddress = m_routerController->GetUriAfterTheBaseAddress( *resourceRouter, resourceURI );

        // Obtain access to the resource based of the relative Uri
        CIHTTPServerRouter::THTTPServerResourcePtr resource = resourceRouter->ResolveUriToResource( uriAfterBaseAddress );
        if ( 0 == resource )
        {
            // Not found
            returnData->statusCode = 404;
            return returnData;
        }

        returnData->contentType = resource->GetBestMatchedSerializationRepresentation( request.resourceRepresentations );

        // Did we find a supported type match?
        if ( returnData->contentType.Length() > 0 )
        {
            // Unsupported media type
            returnData->statusCode = 415;
            returnData->acceptedTypes = resource->GetSupportedSerializationRepresentations();
            return returnData;
        }

        // Check if we need to perform a version check for efficiency purposes
        // If a specific version was requested we should only send the resource if the resource has
        // been altered thus saving bandwidth
        if ( MatchResourceVersion( resource->GetResourceVersion(), request.resourceVersions ) )
        {
            // The resource has not been changed thus no need to send the resource to the client
            returnData->statusCode = 304;
            returnData->eTag = resource->GetResourceVersion();
            return returnData;
        }

        if ( contentRequested )
        {
            // Set the serialized resource as the return data content
            if ( !resource->Serialize( returnData->content, returnData->contentType ) )
            {
                // Something went wrong in the handler while serializing the resource
                returnData->statusCode = 415;
                returnData->acceptedTypes = resource->GetSupportedSerializationRepresentations();
                return returnData;
            }
        }

        returnData->eTag = resource->GetResourceVersion();
        returnData->lastModified = resource->GetLastModifiedTime();
        returnData->cacheability = resource->GetCacheability();

        // Inform the client of the actual location of the resource if the request Uri was
        // actually an alias
        CString actualResourceUri = m_routerController->MakeUriAbsolute( *resourceRouter, resourceURI, resource->GetURL() );
        if ( resourceURI != actualResourceUri )
        {         //@TODO: should the status code be 3xx instead of 200??
            returnData->location = actualResourceUri;
        }

        // Set the StatusCode to OK
        returnData->statusCode = 200;
        return returnData;
    }
    catch ( std::exception& )
    {
        // Unhandled exception during execution
        returnData->statusCode = 500;
        return returnData;
    }
}

/*-------------------------------------------------------------------------*/

CHTTPServer::THttpReturnData*
CHTTPServer::OnUpdate( const THttpRequestData& request )
{GUCEF_TRACE;

    THttpReturnData* returnData = new THttpReturnData;

    if ( NULL == m_routerController )
    {
        // No system has been set for providing handlers
        returnData->statusCode = 500;
        return returnData;
    }
    try
    {
        const CString& resourceURI = request.requestUri;
        m_lastRequestUri = resourceURI;

        CIHTTPServerRouter* resourceRouter = m_routerController->GetHandler( resourceURI );
        if ( NULL == resourceRouter )
        {
            // No system has been set for providing handlers
            returnData->statusCode = 500;
            return returnData;
        }

        // Check if the client send the content for the update
        if ( request.content.GetDataSize() > 0 )
        {
            returnData->statusCode = 400; // Bad request
            return returnData;
        }

        CString remainderUri = m_routerController->GetUriAfterTheBaseAddress( *resourceRouter, resourceURI );
        CIHTTPServerRouter::THTTPServerResourcePtr resource = resourceRouter->ResolveUriToResource( remainderUri );

        if ( 0 == resource )
        {
            // Not found
            returnData->statusCode = 404;
            return returnData;
        }

        CString inputRepresentation;
        if ( request.resourceRepresentations.size() > 0 )
        {
            inputRepresentation = request.resourceRepresentations.front();
            bool isSupportedMimeType = false;

            const TStringVector& supportedRepresentations = resource->GetSupportedDeserializationRepresentations();
            TStringVector::const_iterator i = supportedRepresentations.begin();
            while ( i != supportedRepresentations.end() )
            {
                if ( (*i).Equals( inputRepresentation, false ) )
                {
                    isSupportedMimeType = true;
                    break;
                }
                ++i;
            }

            if ( !isSupportedMimeType )
            {
                returnData->statusCode = 415;
                returnData->acceptedTypes = resource->GetSupportedDeserializationRepresentations();
                return returnData;
            }
        }

        // Check if we need to perform a version (concurrency) check
        if ( request.resourceVersions.size() > 0 )
        {
            if ( !MatchResourceVersion( resource->GetResourceVersion(), request.resourceVersions ) )
            {
                // The resource has been changed while the client was making its changes,..
                // We have a race condition and should abort the update
                returnData->statusCode = 412; // Pre condition failed
                return returnData;
            }
        }

        // Assign to the entry
        CIHTTPServerResource::TDeserializeState deserializeState = resource->Deserialize( request.content, inputRepresentation );
        if ( CIHTTPServerResource::DESERIALIZESTATE_CORRUPTEDINPUT == deserializeState  )
        {
            // There was a problem in the backend deserializing the content in the negotiated format
            returnData->statusCode = 400; // <- corrupt data,.. bad request
            returnData->acceptedTypes = resource->GetSupportedDeserializationRepresentations();
            return returnData;
        }
        else
        if ( CIHTTPServerResource::DESERIALIZESTATE_UNABLETOUPDATE == deserializeState  )
        {
            // There was a problem in the backend actually updating the resource with the deserialized content
            returnData->statusCode = 304; // <- resource not modified
        }
        else
        {
            // Update to the existing entry was successfull
            returnData->statusCode = 200;
        }

        returnData->eTag = resource->GetResourceVersion();
        returnData->lastModified = resource->GetLastModifiedTime();

        // Make sure we only send back absolute Uri's for resource locations
        returnData->location = m_routerController->MakeUriAbsolute( *resourceRouter, resourceURI, resource->GetURL() );

        // Send the serverside representation back to the client in the representation of the update
        resource->Serialize( returnData->content, inputRepresentation );
        returnData->contentType = inputRepresentation;

        return returnData;
    }
    catch ( std::exception& )
    {
        // Unhandled exception during execution
        returnData->statusCode = 500;
        return returnData;
    }
}

/*-------------------------------------------------------------------------*/

CHTTPServer::THttpReturnData*
CHTTPServer::OnCreate( const THttpRequestData& request )
{GUCEF_TRACE;

    THttpReturnData* returnData = new THttpReturnData;

    if ( request.content.GetDataSize() == 0 )
    {
        //Either the client request could not be parsed or the client has not provided any data
        //to create. So throw bad request.
        returnData->statusCode = 400;
        return returnData;
    }

    if ( NULL == m_routerController )
    {
        returnData->statusCode = 500; // Improperly configured
        return returnData;
    }

    try
    {
        const CString& resourceURI = request.requestUri;
        m_lastRequestUri = resourceURI;

        CIHTTPServerRouter* resourceRouter = m_routerController->GetHandler( resourceURI );
        if ( NULL == resourceRouter )
        {
            // No system has been set for providing handlers
            // There is nothing the client can do here.
            returnData->statusCode = 500;
            return returnData;
        }

        // Get relative collection URI and subsequently the collection itself
        CString containerUri = m_routerController->GetUriAfterTheBaseAddress( *resourceRouter, resourceURI );
        CIHTTPServerRouter::THTTPServerResourcePtr containerResource = resourceRouter->ResolveUriToResource( containerUri );

        if ( 0 == containerResource )
        {
            // Unable to locate the collection in which to place the entry
            returnData->statusCode = 404;
            return returnData;
        }

        // create resource in preperation for deserialization
        CIHTTPServerRouter::THTTPServerResourcePtr resource;
        TStringVector supportedRepresentations;
        CString createRepresentation = request.resourceRepresentations.front();

        CIHTTPServerResource::TCreateState createState = containerResource->CreateResource( request.transactionID    ,
                                                                                            request.content          ,
                                                                                            createRepresentation     ,
                                                                                            resource                 ,
                                                                                            supportedRepresentations );

        if ( ( CIHTTPServerResource::CREATESTATE_FAILED == createState ) || ( 0 == resource ) )
        {
            // Failed to finalize (thus persist) the data
            returnData->statusCode = 500;
            return returnData;
        }
        // See if the handler was able to already provide access to the placeholder as a resource
        if ( CIHTTPServerResource::CREATESTATE_CONFLICTING == createState )
        {
            returnData->statusCode = 409; // Conflict: already exists and it is not the same item
            return returnData;
        }
        if ( CIHTTPServerResource::CREATESTATE_UNSUPPORTEDREPRESENTATION == createState )
        {
            returnData->statusCode = 415;
            returnData->acceptedTypes = supportedRepresentations;
            return returnData;
        }

        if ( CIHTTPServerResource::CREATESTATE_DESERIALIZATIONFAILED == createState )
        {
            // tell client about the error,.. cannot create because of corrupted stream
            returnData->statusCode = 400;
            returnData->acceptedTypes = supportedRepresentations;
            return returnData;
        }

        if ( CIHTTPServerResource::CREATESTATE_CREATED == createState )
        {
            returnData->location = resource->GetURL();

            // To comply with protocols like ATOM-pub we need to stream out whatever content is already present
            // as the new resource as part of our reply regardless of whether content was send to the server

            // Perform the serialization
            resource->Serialize( returnData->content, createRepresentation );

            // Make sure the client gets an absolute path to the resource
            // which may be a placeholder
            returnData->location = m_routerController->MakeUriAbsolute( *resourceRouter, resourceURI, returnData->location );

            // tell client what representation the resource is stored as plus its version
            returnData->contentType = createRepresentation;
            returnData->eTag = resource->GetResourceVersion();
            returnData->lastModified = resource->GetLastModifiedTime();

            // Tell the client we succeeded in creating a new resource
            returnData->statusCode = 201;
            return returnData;
        }

        // We should not be able to get here,.. bad create state
        returnData->statusCode = 500;
        return returnData;

    }
    catch ( std::exception& )
    {
        // Unhandled exception during execution
        returnData->statusCode = 500;
        return returnData;
    }
}

/*-------------------------------------------------------------------------*/

CHTTPServer::THttpReturnData*
CHTTPServer::OnDelete( const THttpRequestData& request )
{GUCEF_TRACE;

    THttpReturnData* returnData = new THttpReturnData;

    if ( NULL == m_routerController )
    {
        returnData->statusCode = 500; // Improperly configured
        return returnData;
    }

    try
    {
        const CString& resourceURI = request.requestUri;
        m_lastRequestUri = resourceURI;

        CIHTTPServerRouter* resourceRouter = m_routerController->GetHandler( resourceURI );
        if ( NULL == resourceRouter )
        {
            // No system has been set for providing handlers
            returnData->statusCode = 404;
            return returnData;
        }

        CString remainderUri = m_routerController->GetUriAfterTheBaseAddress( *resourceRouter, resourceURI );
        CIHTTPServerRouter::THTTPServerResourcePtr resource = resourceRouter->ResolveUriToResource( remainderUri );

        if ( 0 == resource )
        {
            // Not fount
            returnData->statusCode = 404;
            return returnData;
        }

        if ( request.resourceVersions.size() > 0 )
        {
            // Check if we need to perform a version (concurrency) check
            // Be determine this by checking whether client entered a specific resource version to delete
            if ( !MatchResourceVersion( resource->GetResourceVersion(), request.resourceVersions ) )
            {
                // The resource it of a different version then what the client expected,..
                // We will abort the delete
                returnData->statusCode = 412;
                returnData->eTag = resource->GetResourceVersion();
                return returnData;
            }
        }

        if ( resource->DeleteResource() )
        {
            // tell client the delete succeeded
            returnData->statusCode = 200;
            return returnData;
        }
        else
        {
            returnData->statusCode = 405; //
            returnData->allowedMethods.push_back( "GET" ); // Hmm, we're not quite sure are we...
            returnData->allowedMethods.push_back( "POST" );
            return returnData;
        }
    }
    catch ( std::exception& )
    {
        // Unhandled exception during execution
        returnData->statusCode = 500;
        return returnData;
    }
}

/*-------------------------------------------------------------------------*/

CString
CHTTPServer::GetLastRequestUri( void ) const
{GUCEF_TRACE;

    return m_lastRequestUri;
}

/*-------------------------------------------------------------------------*/

bool
CHTTPServer::ListenOnPort( const UInt16 port )
{GUCEF_TRACE;

    return m_tcpServerSocket.ListenOnPort( port );
}

/*-------------------------------------------------------------------------*/

void
CHTTPServer::Close( void )
{GUCEF_TRACE;

    m_tcpServerSocket.Close();
}

/*-------------------------------------------------------------------------*/

UInt16
CHTTPServer::GetPort( void ) const
{GUCEF_TRACE;

    return m_tcpServerSocket.GetPort();
}

/*-------------------------------------------------------------------------*/

void
CHTTPServer::OnNotify( CORE::CNotifier* notifier                 ,
                       const CORE::CEvent& eventid               ,
                       CORE::CICloneable* eventdata /* = NULL */ )
{GUCEF_TRACE;

    if ( COMCORE::CTCPServerSocket::ClientConnectedEvent == eventid )
    {
        const COMCORE::CTCPServerSocket::TClientConnectedEventData* eData = static_cast< COMCORE::CTCPServerSocket::TClientConnectedEventData* >( eventdata );
        const COMCORE::CTCPServerSocket::TConnectionInfo& storage = eData->GetData();

        GUCEF_DEBUG_LOG( CORE::LOGLEVEL_NORMAL, "CHTTPServer(" + CORE::PointerToString( this ) + "): Client connected" );

        // Subscribe to the connection
        storage.connection->Subscribe( this );
    }
    else
    if ( COMCORE::CTCPServerConnection::DataRecievedEvent == eventid )
    {
        const COMCORE::CTCPServerConnection::TDataRecievedEventData* eData = static_cast< COMCORE::CTCPServerConnection::TDataRecievedEventData* >( eventdata );
        const CORE::CDynamicBuffer& receivedData = eData->GetData();
        COMCORE::CTCPServerConnection* connection = static_cast< COMCORE::CTCPServerConnection* >( notifier );

        GUCEF_DEBUG_LOG( CORE::LOGLEVEL_NORMAL, "CHTTPServer(" + CORE::PointerToString( this ) + "): " + CORE::UInt32ToString( receivedData.GetDataSize() ) + " Bytes received from client " + connection->GetRemoteHostName() );

        // Process the request
        CORE::CDynamicBuffer responseBuffer;
        ProcessReceivedData( receivedData, responseBuffer );

        // Send the reponse back to the client
        connection->Send( responseBuffer.GetConstBufferPtr(), responseBuffer.GetDataSize() );
        connection->Close();
    }
}

/*-------------------------------------------------------------------------*/

void
CHTTPServer::ExtractCommaSeparatedValues( const CString& stringToExtractFrom ,
                                          TStringVector& list                ) const
{GUCEF_TRACE;

    // Create a list and make sure we trim each element
    TStringVector elements = stringToExtractFrom.ParseElements( ',' );
    TStringVector::iterator i = elements.begin();
    while ( i != elements.end() )
    {
        list.push_back( (*i).RemoveChar( ' ' ) );
        ++i;
    }
}

/*-------------------------------------------------------------------------*/

void
CHTTPServer::ProcessReceivedData( const CORE::CDynamicBuffer& inputBuffer ,
                                  CORE::CDynamicBuffer& outputBuffer      )
{GUCEF_TRACE;

    try
    {
        THttpReturnData* returnData = NULL;
        THttpRequestData* requestData = ParseRequest( inputBuffer );

        GUCEF_DEBUG_LOG( CORE::LOGLEVEL_NORMAL, "CHTTPServer(" + CORE::PointerToString( this ) + "): About to process request of type: " + requestData->requestType );

        if ( NULL != requestData )
        {
            if ( requestData->requestType == "GET" )
            {
                returnData = OnRead( *requestData );
            }
            else
            if ( requestData->requestType == "HEAD" )
            {
                returnData = OnReadMetaData( *requestData );
            }
            else
            if ( requestData->requestType == "POST" )
            {
                returnData = OnCreate( *requestData );
            }
            else
            if ( requestData->requestType == "PUT" )
            {
                returnData = OnUpdate( *requestData );
            }
            else
            if ( requestData->requestType == "DELETE" )
            {
                returnData = OnDelete( *requestData );
            }
        }

        if ( NULL != returnData )
        {
            ParseResponse( *returnData  ,
                           outputBuffer );
        }
    }
    catch ( std::exception& )
    {
    }
}

/*-------------------------------------------------------------------------*/

UInt32
CHTTPServer::ParseHeaderFields( const char* bufferPtr       ,
                                const UInt32 bufferSize     ,
                                TStringVector& headerFields ) const
{GUCEF_TRACE;

    try
    {
        UInt32 startIndex = 0;
        UInt32 headerSize = 0;

        // According to the RFC lines are separated using '\r\n'
        for ( UInt32 i=0; i<bufferSize; ++i )
        {
            // Check for the end of line delimiter, cariage return first
            if ( bufferPtr[ i ] == '\r' )
            {
                if ( i+1 < bufferSize )
                {
                    // Check for line feed
                    if ( bufferPtr[ i+1 ] == '\n' )
                    {
                        ++i;
                        if ( i+1 < bufferSize )
                        {
                            // Add the segment to our list
                            headerFields.push_back( CString( bufferPtr+startIndex, i-1-startIndex ) );
                            startIndex = i+1;

                            // Check for empty line which is the end of header delimiter
                            // We start with cariage return
                            if ( bufferPtr[ i+1 ] == '\r' )
                            {
                                ++i;
                                if ( i+1 < bufferSize )
                                {
                                    // Check for line feed
                                    if ( bufferPtr[ i+1 ] == '\n' )
                                    {
                                        // Proper end of header delimiter found, we can stop
                                        headerSize = i+2;
                                        break;
                                    }

                                    // If we get here:
                                    // Some HTTP1.0 implementations add an extra '/r' after a '/r/n' so we have to be robust
                                    // and tolerate this condition
                                }
                                else
                                {
                                    // Not a well formatted HTTP header
                                    headerSize = i;
                                    break;
                                }
                            }
                        }
                        else
                        {
                            // Not a well formatted HTTP header
                            headerSize = i;
                            break;
                        }
                    }
                }
                else
                {
                    // Not a well formatted HTTP header
                    headerSize = i;
                    break;
                }
            }
        }
        return headerSize;
    }
    catch ( std::exception& )
    {
        return 0;
    }
}

/*-------------------------------------------------------------------------*/

CHTTPServer::THttpRequestData*
CHTTPServer::ParseRequest( const CORE::CDynamicBuffer& inputBuffer )
{GUCEF_TRACE;

    try
    {
        if ( inputBuffer.GetDataSize() == 0 )
        {
            // Invalid input
            return NULL;
        }

        // Parse all the HTTP Header fields out of the buffer
        TStringVector headerFields;
        UInt32 headerSize = ParseHeaderFields( static_cast< const char* >( inputBuffer.GetConstBufferPtr() ) ,
                                               inputBuffer.GetDataSize()                                     ,
                                               headerFields                                                  );

        // Sanity check on the parsed result
        if ( headerSize == 0 || headerFields.size() == 0 )
        {
            return NULL;
        }

        GUCEF_DEBUG_LOG( CORE::LOGLEVEL_NORMAL, "CHTTPServer(" + CORE::PointerToString( this ) + "): Finished parsing request header" );

        THttpRequestData* request = new THttpRequestData;

        CString temp = headerFields.front().CompactRepeatingChar( ' ' );
        headerFields.erase( headerFields.begin() );

        // Parse the request type from the first line
        request->requestType = temp.SubstrToChar( ' ', true );
        temp = temp.CutChars( request->requestType.Length()+1, true );

        // Parse the request URI
        request->requestUri = temp.SubstrToChar( ' ', true );

        // Parse all the subsequent HTTP header fields
        TStringVector::iterator i = headerFields.begin();
        while ( i != headerFields.end() )
        {
            // Parse the header name and header value out of the header field
            CString& headerField = (*i);
            CString headerName = headerField.SubstrToChar( ':', true );
            CString headerValue( headerField.C_String() + headerName.Length(), headerField.Length() - headerName.Length() );

            // Remove additional spaces and lowercase the headername for easy comparison
            headerName = headerName.RemoveChar( ' ' ).Lowercase();
            headerValue = headerValue.RemoveChar( ' ' );

            //  Remove the ':' prefix
            headerValue = headerValue.CutChars( 1, true );

            // Now that we have formatted the header name + value we can use them
            if ( headerName == "accept" )
            {
                ExtractCommaSeparatedValues( headerValue                      ,
                                             request->resourceRepresentations );
            }
            else
            if ( headerName == "content-type" )
            {
                ExtractCommaSeparatedValues( headerValue               ,
                                             request->resourceVersions );
            }
            else
            if ( headerName == "if-match" )
            {
                ExtractCommaSeparatedValues( headerValue               ,
                                             request->resourceVersions );
            }
            else
            if ( headerName == "cookie" )
            {
                request->transactionID = headerValue;
            }
            else
            if ( headerName == "host" )
            {
                request->requestHost = headerValue;
            }
            ++i;
        }

        // Set the content as a sub-segment of our data buffer
        if ( inputBuffer.GetDataSize() - headerSize > 0 )
        {
            request->content.LinkTo( inputBuffer.GetConstBufferPtr( headerSize ) ,
                                     inputBuffer.GetDataSize() - headerSize      );
        }

        return request;
    }
    catch ( std::exception& )
    {
        return NULL;
    }
}

/*-------------------------------------------------------------------------*/

void
CHTTPServer::ParseResponse( const THttpReturnData& returnData  ,
                            CORE::CDynamicBuffer& outputBuffer )
{GUCEF_TRACE;

    CString response = "HTTP/1.1 " + CORE::Int32ToString( returnData.statusCode ) + "\r\nServer: gucefCOM\r\nConnection: close\r\n";
    if ( !returnData.location.IsNULLOrEmpty() )
    {
        response += "Content-Location: " + returnData.location + "\r\n";
    }
    if ( !returnData.eTag.IsNULLOrEmpty() )
    {
        response += "ETag: " + returnData.eTag + "\r\n";
    }
    if ( !returnData.cacheability.IsNULLOrEmpty() )
    {
        response += "Cache-Control: " + returnData.cacheability + "\r\n";
    }
    if ( !returnData.lastModified.IsNULLOrEmpty() )
    {
        response += "Last-Modified: " + returnData.lastModified + "\r\n";
    }
    if ( returnData.content.GetDataSize() > 0 )
    {
        response += "Content-Length: " + CORE::UInt32ToString( returnData.content.GetDataSize() ) + "\r\n";
        response += "Content-Type: " + returnData.contentType + "\r\n";
    }

    // You cannot send back accept types to the client using a standard HTTP header since
    // it is a HTTP request only header. We don't want the client to aimlessly retry different
    // representations either on PUT/POST so we do want to inform the client of the accepted types.
    // The client will need to have support for this operation to take advantage of it since it is a custom HTTP header
    if ( returnData.statusCode == 415 )
    {
        response += "Accept: ";
        TStringVector::const_iterator i = returnData.acceptedTypes.begin();
        while ( i != returnData.acceptedTypes.end() )
        {
            response += (*i) + ',';
        }
        response += "\r\n";
    }

    // If the operation was and a subset of allowed operations where specified
    // we will send those to the client.
    if ( returnData.statusCode == 405 && returnData.allowedMethods.size() > 0 )
    {
        response += "Allow: ";
        TStringVector::const_iterator i = returnData.allowedMethods.begin();
        while ( i != returnData.allowedMethods.end() )
        {
            response += (*i) + ',';
        }
        response += "\r\n";
    }

    // Add the end of HTTP header delimiter
    response += "\r\n";

    GUCEF_DEBUG_LOG( CORE::LOGLEVEL_NORMAL, "CHTTPServer(" + CORE::PointerToString( this ) + "): Finished building response header: " + response );

    // Copy the HTTP header into the buffer
    outputBuffer.CopyFrom( response.Length(), response.C_String() );

    // Copy the HTTP message body into the buffer
    if ( returnData.content.GetDataSize() > 0 )
    {
        outputBuffer.CopyFrom( response.Length(), returnData.content.GetDataSize(), returnData.content.GetConstBufferPtr() );
    }
}

/*-------------------------------------------------------------------------//
//                                                                         //
//      NAMESPACE                                                          //
//                                                                         //
//-------------------------------------------------------------------------*/

} /* namespace COM */
} /* namespace GUCEF */

/*-------------------------------------------------------------------------*/
