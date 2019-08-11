/*------------------------------------------------------------------------------
--  Help ToolKit for Motif
-- 
--  (c) Copyright 1997-1999, 2019 Robert S. Werner (a.k.a. Akiva Werner)  
--  ALL RIGHTS RESERVED
--
--  License: The MIT License (MIT)
------------------------------------------------------------------------------*/

#ifndef __XscShellP_h
#define __XscShellP_h


#include <X11/Intrinsic.h>
#include <Xm/Xm.h>


/*==============================================================================
                                   Handle
==============================================================================*/
typedef struct _XscShellRec *XscShell;


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
void _XscShellInitialize( XContext );

XscShell _XscShellCreate( Widget );
XscShell _XscShellDeriveFromWidget( Widget );

void _XscShellInstallHint( XscShell, Widget );

Pixel  _XscShellGetBackground( XscShell );
Widget _XscShellGetHintWidget( XscShell );

void _XscShellSetBackground( XscShell, Pixel );

void _XscShellEraseHint  ( XscShell );
void _XscShellDisplayHint( XscShell );
void _XscShellUpdateHint ( XscShell );


Boolean _XscShellIsCueDisplayable ( XscShell );
Boolean _XscShellIsHintDisplayable( XscShell );
Boolean _XscShellIsTipDisplayable ( XscShell );

Boolean _XscShellIsCueEnabled ( XscShell );
Boolean _XscShellIsHintEnabled( XscShell );
Boolean _XscShellIsTipEnabled ( XscShell );

void _XscShellSetCueEnabled ( XscShell, Boolean );
void _XscShellSetHintEnabled( XscShell, Boolean );
void _XscShellSetTipEnabled ( XscShell, Boolean );


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif
