/*
 *  GucefLogServiceLib: Library containing the main logic for the Logging service
 *  Copyright (C) 2002 - 2008.  Dinand Vanvelzen
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

#ifndef GUCEF_LOGSERVICELIB_CLOGSVCSERVERFILELOGGER_H
#define GUCEF_LOGSERVICELIB_CLOGSVCSERVERFILELOGGER_H

/*-------------------------------------------------------------------------//
//                                                                         //
//      INCLUDES                                                           //
//                                                                         //
//-------------------------------------------------------------------------*/

#ifndef GUCEF_LOGSERVICELIB_CILOGSVCSERVERLOGGER_H
#include "GucefLogServiceLib_CILogSvcServerLogger.h"
#define GUCEF_LOGSERVICELIB_CILOGSVCSERVERLOGGER_H
#endif /* GUCEF_LOGSERVICELIB_CILOGSVCSERVERLOGGER_H ? */

/*-------------------------------------------------------------------------//
//                                                                         //
//      NAMESPACE                                                          //
//                                                                         //
//-------------------------------------------------------------------------*/

namespace GUCEF {
namespace LOGSERVICELIB {

/*-------------------------------------------------------------------------//
//                                                                         //
//      CLASSES                                                            //
//                                                                         //
//-------------------------------------------------------------------------*/

class CIOAccess;

/*-------------------------------------------------------------------------*/

class GUCEF_LOGSERVICELIB_EXPORT_CPP CLogSvcServerFileLogger : public CILogSvcServerLogger
{
    public:

    CLogSvcServerFileLogger( void );
    
    CLogSvcServerFileLogger( CORE::CIOAccess& output );
                                            
    virtual ~CLogSvcServerFileLogger();
    
    virtual void Log( const CLogSvcServer::TClientInfo& clientInfo ,
                      const TLogMsgType logMsgType                 ,
                      const CORE::Int32 logLevel                   ,
                      const CORE::CString& logMessage              ,
                      const CORE::UInt32 threadId                  );
                      
    virtual void FlushLog( void );

    void SetOutput( CORE::CIOAccess& output );

    void SetMinimalLogLevel( const CORE::Int32 minimalLogLevel );

    CORE::Int32 GetMinimalLogLevel( void ) const;
    
    void SetWhetherAppNameIsLogged( bool logAppName );
    
    bool GetWhetherAppNameIsLogged( void ) const;

    void SetWhetherProcessNameIsLogged( bool logProcessName );
    
    bool GetWhetherProcessNameIsLogged( void ) const;
    
    void SetWhetherProcessIdIsLogged( bool logProcessId );
    
    bool GetWhetherProcessIdIsLogged( void ) const;
    
    private:
                                         
    CLogSvcServerFileLogger( const CLogSvcServerFileLogger& src );             /** <- not implemented */
    CLogSvcServerFileLogger& operator=( const CLogSvcServerFileLogger& src );  /** <- not implemented */
    
    private:
    
    CORE::CIOAccess* m_output;
    CORE::Int32 m_minimalLogLevel;
    bool m_logAppName;
    bool m_logProcessName;
    bool m_logProcessId;
};

/*-------------------------------------------------------------------------//
//                                                                         //
//      NAMESPACE                                                          //
//                                                                         //
//-------------------------------------------------------------------------*/

}; /* namespace LOGSERVICELIB */
}; /* namespace GUCEF */

/*-------------------------------------------------------------------------*/

#endif /* GUCEF_LOGSERVICELIB_CLOGSVCSERVERFILELOGGER_H ? */

/*-------------------------------------------------------------------------//
//                                                                         //
//      Info & Changes                                                     //
//                                                                         //
//-------------------------------------------------------------------------//

- 04-05-2005 :
        - Dinand: Initial version.

---------------------------------------------------------------------------*/