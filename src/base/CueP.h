/*------------------------------------------------------------------------------
--  Help ToolKit for Motif
-- 
--  (c) Copyright 1997-1999, 2019 Robert S. Werner (a.k.a. Akiva Werner)  
--  ALL RIGHTS RESERVED
--
--  License: The MIT License (MIT)
------------------------------------------------------------------------------*/

#ifndef __XscCueP_h
#define __XscCueP_h


#include <X11/Intrinsic.h>
#include <Xm/Xm.h>

#include <Xsc/Cue.h>

#include "ObjectP.h"
#include "TextP.h"

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
XscCue _XscCueCreate( XscObject );
void   _XscCueDestroy( XscCue );

XscObject _XscCueGetObject( XscCue );

void _XscCueLoadResources( XscCue, XscText );

XscText _XscCueGetText( XscCue tip );

void _XscCueActivate( XscCue );
void _XscCueUpdate( XscCue );

void _XscCuePopdown( XscCue );
void _XscCuePopup( XscCue );

void _XscCueRender( XscCue, Widget, GC );

Boolean _XscCueHasValidTopic( XscCue );


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif
