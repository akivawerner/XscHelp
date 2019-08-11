/*------------------------------------------------------------------------------
--  Help ToolKit for Motif
-- 
--  (c) Copyright 1997-1999, 2019 Robert S. Werner (a.k.a. Akiva Werner)  
--  ALL RIGHTS RESERVED
--
--  License: The MIT License (MIT)
------------------------------------------------------------------------------*/

#ifndef __XscHintP_h
#define __XscHintP_h


#include <X11/Intrinsic.h>
#include <Xm/Xm.h>

#include <Xsc/Hint.h>

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
XscHint _XscHintCreate( XscObject );
void    _XscHintDestroy( XscHint );

XscObject _XscHintGetObject( XscHint );
XscText   _XscHintGetText  ( XscHint );

void _XscHintLoadResources( XscHint, XscText );

void _XscHintActivate( XscHint );
void _XscHintRender( XscHint, Widget, GC, Pixel );


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif
