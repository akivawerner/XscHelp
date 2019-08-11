/*------------------------------------------------------------------------------
--  Help ToolKit for Motif
-- 
--  (c) Copyright 1997-1999, 2019 Robert S. Werner (a.k.a. Akiva Werner)  
--  ALL RIGHTS RESERVED
--
--  License: The MIT License (MIT)
------------------------------------------------------------------------------*/

#ifndef __XscTipP_h
#define __XscTipP_h


#include <X11/Intrinsic.h>
#include <Xm/Xm.h>

#include <Xsc/Tip.h>

#include "DisplayP.h"
#include "ObjectP.h"
#include "ScreenP.h"
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
XscTip _XscTipCreate( XscObject );
void   _XscTipDestroy( XscTip );

XscObject _XscTipGetObject( XscTip );

void _XscTipLoadResources( XscTip, XscText );

XscText _XscTipGetText( XscTip tip );

void _XscTipActivate( XscTip );
void _XscTipUpdate( XscTip );

void _XscTipPopdown( XscTip );
void _XscTipPopup( XscTip );

void _XscTipRender( XscTip, Widget, GC );

void _XscTipSelectName( XscTip );


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif
