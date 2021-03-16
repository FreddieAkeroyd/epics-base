/*************************************************************************\
* Copyright (c) 2021 Facility for Rare Isotope Beams
* EPICS BASE is distributed subject to a Software License Agreement found
* in file LICENSE that is included with this distribution.
\*************************************************************************/

#ifndef osdTimerh
#define osdTimerh

/* Windows supports high-precision timers (in contrast to quantized timers
 * which only support sleeping for a multiple of the quantum). */
#if defined(_MSC_VER) && !defined(_MINGW)
#define HAS_HIGH_PREC_TIMERS
#endif

#endif /* osdTimerh */
