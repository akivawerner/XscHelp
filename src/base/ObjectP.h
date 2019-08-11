/*------------------------------------------------------------------------------
--  Help ToolKit for Motif
-- 
--  (c) Copyright 1997-2000, 2019 Robert S. Werner (a.k.a. Akiva Werner)  
--  ALL RIGHTS RESERVED
--
--  License: The MIT License (MIT)
------------------------------------------------------------------------------*/

#ifndef __XscObjectP_h
#define __XscObjectP_h


#include <X11/Intrinsic.h>
#include <X11/Xresource.h>
#include <Xm/Xm.h>


/*==============================================================================
                                   Handle
==============================================================================*/
typedef struct _XscObjectRec *XscObject;


/*==============================================================================
                              Private Includes
==============================================================================*/
#include <Xsc/Cue.h>
#include <Xsc/Hint.h>
#include <Xsc/Tip.h>


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
Boolean XscHelpGetGadgetProcessing( Widget );       /* May be removed from ---*/
void XscHelpSetGadgetProcessing( Widget, Boolean ); /* baseline --------------*/
/*----------------------------------------------------------------------------*/

void _XscObjectInitialize( XContext );

XscObject _XscObjectCreate( Widget );
XscObject _XscObjectDeriveFromWidget( Widget );

void _XscObjectGadgetFocusIn ( XscObject );
void _XscObjectGadgetFocusOut( XscObject );

XscCue  _XscObjectGetCue       ( XscObject );
Font    _XscObjectGetFont      ( XscObject );
XscHint _XscObjectGetHint      ( XscObject );
XscTip  _XscObjectGetTip       ( XscObject );
Widget  _XscObjectGetWidget    ( XscObject );
int     _XscObjectGetTipGroupId( XscObject );

void _XscObjectSetTipGroupId( XscObject, int );

Boolean _XscObjectHasValidCue ( XscObject );
Boolean _XscObjectHasValidHint( XscObject );
Boolean _XscObjectHasValidTip ( XscObject );

void _XscObjectRenderTip( XscObject, Widget, GC );


Boolean XscCvtStringToTipGroupId( 
   Display*, XrmValue*, Cardinal*, XrmValue*, XrmValue*, XtPointer* );


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif
