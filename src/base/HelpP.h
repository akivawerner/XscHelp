/*------------------------------------------------------------------------------
--  Help ToolKit for Motif
-- 
--  (c) Copyright 1997-1999, 2019 Robert S. Werner (a.k.a. Akiva Werner)  
--  ALL RIGHTS RESERVED
--
--  License: The MIT License (MIT)
------------------------------------------------------------------------------*/

#ifndef __XscHelpP_h
#define __XscHelpP_h


#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
                                  Prototypes
==============================================================================*/
Widget _XscHelpGetGadgetChild( Widget, int, int );

void _XscHelpInstallOnWidget( Widget );

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif
