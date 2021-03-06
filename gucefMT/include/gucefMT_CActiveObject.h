/*
 *  gucefMT: GUCEF module providing multithreading solutions
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

#ifndef GUCEF_MT_CACTIVEOBJECT_H
#define GUCEF_MT_CACTIVEOBJECT_H

/*-------------------------------------------------------------------------//
//                                                                         //
//      INCLUDES                                                           //
//                                                                         //
//-------------------------------------------------------------------------*/

#ifndef GUCEF_MT_ETYPES_H
#include "gucefMT_ETypes.h"             /* Simple types used */
#define GUCEF_MT_ETYPES_H
#endif /* GUCEF_MT_ETYPES_H */

#ifndef GUCEF_MT_GUCEFMT_MACROS_H
#include "gucefMT_macros.h"             /* often used gucef macros */
#define GUCEF_MT_GUCEFMT_MACROS_H
#endif /* GUCEF_MT_GUCEFMT_MACROS_H ? */

/*-------------------------------------------------------------------------//
//                                                                         //
//      NAMESPACE                                                          //
//                                                                         //
//-------------------------------------------------------------------------*/

namespace GUCEF {
namespace MT {

/*-------------------------------------------------------------------------//
//                                                                         //
//      TYPES                                                              //
//                                                                         //
//-------------------------------------------------------------------------*/

struct SThreadData;

/*-------------------------------------------------------------------------//
//                                                                         //
//      CLASSES                                                            //
//                                                                         //
//-------------------------------------------------------------------------*/

class GUCEF_MT_PUBLIC_CPP CActiveObject
{
    public:

    CActiveObject( void );

    CActiveObject( const CActiveObject& src );

    virtual ~CActiveObject();

    /**
     *  Flag that indicates whether the thread is active
     */
    bool IsActive( void ) const;

    /**
     *  Flag that indicates whether the thread has been asked to gracefully deactivate
     */
    bool IsDeactivationRequested( void ) const;

    /**
     *  Flag that indicates whether the thread has been asked to pause on the next available thread cycle.
     */
    bool IsPauseRequested( void ) const;

    /**
     *  Activates the object (if it is not active already).
     *  This operation will spawn a thread.
     */
    bool Activate( void* taskdata = NULL               ,
                   const UInt32 cycleDelay = 10        ,
                   const UInt32 minimalCycleDelta = 10 );

    /**
     *  Deactivates the active object
     *  If 'force' is false then the thread will deactivate on the next available thread cycle.
     *  If 'force' is true then the thread will be killed immediatly regardless of whether it is busy processing a cycle.
     *
     *  Use caution when killing a thread because this is likely to cause memory leaks etc. and should not be used for normal operation.
     *  If a thread is killed in this manner OnTaskEnd() will still be called from the calling thread so you can place cleanup code there.
     */
    void Deactivate( bool force, bool callerShouldWait );

    /**
     *  Pauses the active object.
     *  If 'force' is false then the thread will pause on the next available thread cycle.
     *  If 'force' is true then the thread will be paused immediatly regardless of whether it is busy processing a cycle
     */
    void Pause( bool force );

    void Resume( void );

    UInt32 GetThreadID( void ) const;

    protected:

    /**
     *  Startup routine for the task. You should return true if startup succeeded and the task can commence
     *  cycling.
     */
    virtual bool OnTaskStart( void* taskdata ) = 0;
    
    /**
     *  Called after a successfull call to OnTaskStart
     */
    virtual void OnTaskStarted( void* taskdata );

    /**
     *  Perorm all your main task work in this function.
     *  It will be called repeatedly until true is returned indicating that the task has been completed.
     *  Thus for ongoing tasks you can write this function to take care of a single interation of the task.
     */
    virtual bool OnTaskCycle( void* taskdata ) = 0;

    /**
     *  This is where all the cleanup should be done for task data
     *  Note that this member function will be called from within the spawned thread when ending gracefully
     *  but in the case of a forcefull termination of the spawned thread this member function will be called
     *  from the thread that triggered the forcefull termination.
     */
    virtual void OnTaskEnd( void* taskdata ) = 0;

    virtual void OnTaskPausedForcibly( void* taskdata );
    
    virtual void OnTaskResumed( void* taskdata );
    
    virtual void OnTaskEnded( void* taskdata ,
                              bool forced    );

    void* GetTaskData( void ) const;

    private:
    CActiveObject& operator=( const CActiveObject& src );
    static UInt32 GUCEF_CALLSPEC_STD_PREFIX OnActivate( void* thisobject ) GUCEF_CALLSPEC_STD_SUFFIX;

    struct SThreadData* _td;
    void* _taskdata;
    UInt32 _delay;
    Float64 m_minimalCycleDelta;
    bool m_isDeactivationRequested;
    bool _suspend;
    bool _active;
};

/*-------------------------------------------------------------------------//
//                                                                         //
//      NAMESPACE                                                          //
//                                                                         //
//-------------------------------------------------------------------------*/

}; /* namespace MT */
}; /* namespace GUCEF */

/*-------------------------------------------------------------------------*/

#endif /* GUCEF_MT_CACTIVEOBJECT_H ? */

/*-------------------------------------------------------------------------//
//                                                                         //
//      Info & Changes                                                     //
//                                                                         //
//-------------------------------------------------------------------------//

- 07-02-2005 :
       - Initial implementation.

---------------------------------------------------------------------------*/
