/*------------------------------------------------------------------------------
--  Help ToolKit for Motif
-- 
--  (c) Copyright 1997-1999, 2019 Robert S. Werner (a.k.a. Akiva Werner)  
--  ALL RIGHTS RESERVED
--
--  License: The MIT License (MIT)
------------------------------------------------------------------------------*/

#ifndef __XscDisplayP_h
#define __XscDisplayP_h


#include <X11/Intrinsic.h>
#include <Xm/Xm.h>

#include <Xsc/Cue.h>

/*==============================================================================
                                   Handle
==============================================================================*/
typedef struct _XscDisplayRec *XscDisplay;


/*==============================================================================
                              Private Includes
==============================================================================*/
#include "ObjectP.h"

#ifdef __cplusplus
extern "C" {
#endif


/*==============================================================================
                                    Macros
==============================================================================*/


/*==============================================================================
                               Data types
==============================================================================*/


/*==============================================================================
                                  Prototypes
==============================================================================*/
void  _XscDisplayInitialize( XContext );

XscDisplay _XscDisplayCreate( Widget );
XscDisplay _XscDisplayDeriveFromWidget( Widget );

Boolean _XscDisplayActiveTipGroupIdEquals ( XscDisplay, int );
void    _XscDisplayCheckForTipGroupIdMatch( XscDisplay, int );

XscCue    _XscDisplayGetActiveCue  ( XscDisplay );
XscObject _XscDisplayGetActiveHint ( XscDisplay );
XscObject _XscDisplayGetActiveTip  ( XscDisplay );
XscObject _XscDisplayGetSelectedTip( XscDisplay );

void _XscDisplayPrimeTipRestore( XscDisplay );

void _XscDisplaySetActiveCue ( XscDisplay, XscCue    );
void _XscDisplaySetActiveHint( XscDisplay, XscObject );
void _XscDisplaySetActiveTip ( XscDisplay, XscObject );
void _XscDisplaySetTipActiveGroupId( XscDisplay, int );

void _XscDisplayCancelTimerSelectName( XscDisplay );
void _XscDisplayCancelTimerTipPopdown( XscDisplay );
void _XscDisplayCancelTimerTipPopup  ( XscDisplay );

void _XscDisplayStartTimerSelectName( XscDisplay, XscObject );
void _XscDisplayStartTimerTipPopdown( XscDisplay, XscObject );
void _XscDisplayStartTimerTipPopup  ( XscDisplay, XscObject );


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif
