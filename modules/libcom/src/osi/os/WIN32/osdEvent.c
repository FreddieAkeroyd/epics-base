/*************************************************************************\
* Copyright (c) 2011 UChicago Argonne LLC, as Operator of Argonne
*     National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
*     Operator of Los Alamos National Laboratory.
* SPDX-License-Identifier: EPICS
* EPICS BASE is distributed subject to a Software License Agreement found
* in file LICENSE that is included with this distribution.
\*************************************************************************/
/* osdEvent.c */
/*
 *      WIN32 version
 *
 *      Author  Jeffrey O. Hill
 *              johill@lanl.gov
 *              505 665 1831
 *
 */

#include <limits.h>

#define VC_EXTRALEAN
#define STRICT
#include <windows.h>

#include "libComAPI.h"
#include "epicsEvent.h"

#include "osdThreadPvt.h"

typedef struct epicsEventOSD {
    HANDLE handle;
} epicsEventOSD;

/*
 * epicsEventCreate ()
 */
LIBCOM_API epicsEventId epicsEventCreate (
    epicsEventInitialState initialState )
{
    epicsEventOSD *pSem;

    pSem = malloc ( sizeof ( *pSem ) );
    if ( pSem ) {
        pSem->handle =  CreateEvent ( NULL, FALSE, initialState?TRUE:FALSE, NULL );
        if ( pSem->handle == 0 ) {
            free ( pSem );
            pSem = 0;
        }
    }

    return pSem;
}

/*
 * epicsEventDestroy ()
 */
LIBCOM_API void epicsEventDestroy ( epicsEventId pSem ) 
{
    CloseHandle ( pSem->handle );
    free ( pSem );
}

/*
 * epicsEventTrigger ()
 */
LIBCOM_API epicsEventStatus epicsEventTrigger ( epicsEventId pSem ) 
{
    BOOL status;
    status = SetEvent ( pSem->handle );
    return status ? epicsEventOK : epicsEventError;
}

/*
 * epicsEventWait ()
 */
LIBCOM_API epicsEventStatus epicsEventWait ( epicsEventId pSem ) 
{
    DWORD status;
    status = WaitForSingleObject (pSem->handle, INFINITE);
    if ( status == WAIT_OBJECT_0 ) {
        return epicsEventOK;
    }
    else {
        return epicsEventError;
    }
}

/*
 * epicsEventWaitWithTimeout ()
 */
LIBCOM_API epicsEventStatus epicsEventWaitWithTimeout (
    epicsEventId pSem, double timeOut )
{
    static const unsigned nSec100PerSec = 10000000u;
    HANDLE handles[2];
    DWORD status;
    LARGE_INTEGER tmo;
    HANDLE timer;
    LONGLONG nSec100;

    if ( timeOut <= 0.0 ) {
        tmo.QuadPart = 0u;
    }
    else {
        nSec100 = (LONGLONG)(timeOut * nSec100PerSec + 0.999999);
        if (nSec100 % 10000 != 0) {
            nSec100 = 10000 * (nSec100 / 10000 + 1); /* round up to next ms */
        }
        tmo.QuadPart = -nSec100;
    }

    if (tmo.QuadPart < 0) {
        timer = osdThreadGetTimer();
        if (!SetWaitableTimer(timer, &tmo, 0, NULL, NULL, 0)) {
            return epicsEventError;
        }
        handles[0] = pSem->handle;
        handles[1] = timer;
        status = WaitForMultipleObjects (2, handles, FALSE, INFINITE);
    }
    else {
        status = WaitForSingleObject(pSem->handle, 0);
    }
    if ( status == WAIT_OBJECT_0 ) {
        return epicsEventOK;
    }
    else if ( status == WAIT_OBJECT_0 + 1 || status == WAIT_TIMEOUT ) {
        /* WaitForMultipleObjects will trigger WAIT_OBJECT_0 + 1,
           WaitForSingleObject will trigger WAIT_TIMEOUT */
        return epicsEventWaitTimeout;
    }
    else {
        return epicsEventError;
    }
}

/*
 * epicsEventTryWait ()
 */
LIBCOM_API epicsEventStatus epicsEventTryWait ( epicsEventId pSem ) 
{
    DWORD status;

    status = WaitForSingleObject ( pSem->handle, 0 );
    if ( status == WAIT_OBJECT_0 ) {
        return epicsEventOK;
    }
    else if ( status == WAIT_TIMEOUT ) {
        return epicsEventWaitTimeout;
    }
    else {
        return epicsEventError;
    }
}

/*
 * epicsEventShow ()
 */
LIBCOM_API void epicsEventShow ( epicsEventId id, unsigned level ) 
{
}
