/*------------------------------------------------------------------------------
--  Help ToolKit for Motif
-- 
--  (c) Copyright 1997-1999, 2019 Robert S. Werner (a.k.a. Akiva Werner)  
--  ALL RIGHTS RESERVED
--
--  License: The MIT License (MIT)
------------------------------------------------------------------------------*/

#ifndef __XscScreenP_h
#define __XscScreenP_h


#include <X11/Intrinsic.h>
#include <Xm/Xm.h>


/*==============================================================================
                                   Handle
==============================================================================*/
typedef struct _XscScreenRec *XscScreen;


/*==============================================================================
                              Private Includes
==============================================================================*/

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
void _XscScreenInitialize( XContext );

XscScreen _XscScreenCreate( Widget );
XscScreen _XscScreenDeriveFromWidget( Widget );

Widget _XscScreenGetCueShell( XscScreen );
Widget _XscScreenGetCueFrame( XscScreen );
Widget _XscScreenGetCueLabel( XscScreen );
GC     _XscScreenGetGC      ( XscScreen );
Widget _XscScreenGetTipShell( XscScreen );
Widget _XscScreenGetTipFrame( XscScreen );
Widget _XscScreenGetTipLabel( XscScreen );

void _XscScreenPopdownCue( XscScreen );
void _XscScreenPopupCue  ( XscScreen );

void _XscScreenPopdownTip( XscScreen );
void _XscScreenPopupTip  ( XscScreen );

void _XscScreenRedrawCue( XscScreen );
void _XscScreenRedrawTip( XscScreen );


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif
