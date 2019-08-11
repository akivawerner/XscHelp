/*------------------------------------------------------------------------------
--  Help ToolKit for Motif
-- 
--  (c) Copyright 1997-1999, 2019 Robert S. Werner (a.k.a. Akiva Werner)  
--  ALL RIGHTS RESERVED
--
--  License: The MIT License (MIT)
------------------------------------------------------------------------------*/

#ifndef __XscTextP_h
#define __XscTextP_h


#include <X11/Intrinsic.h>
#include <Xm/Xm.h>


/*==============================================================================
                                   Handle
==============================================================================*/
typedef struct _XscTextRec *XscText;


/*==============================================================================
                              Private Includes
==============================================================================*/


#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
                                    Macros
==============================================================================*/
#define XscHelpDefaultFont "-adobe-*-medium-r-normal-*-*-120-*-*-p-*-iso8859-1"


/*==============================================================================
                               Data types
==============================================================================*/


/*==============================================================================
                                  Prototypes
==============================================================================*/
XscText _XscTextCreate();

void _XscTextDestroy( XscText );

void _XscTextConvertSegmented    ( XscText, Widget );
void _XscTextConvertFontTag      ( XscText, Widget );
void _XscTextConvertStandard     ( XscText, Widget );
void _XscTextCreateCompoundString( XscText, Widget );

unsigned char _XscTextGetAlignment  ( XscText );
unsigned char _XscTextGetConverter  ( XscText );
unsigned char _XscTextGetDirection  ( XscText );
XmFontList    _XscTextGetFontList   ( XscText );
const char*   _XscTextGetFontListTag( XscText );
Dimension     _XscTextGetHeight     ( XscText );
const char*   _XscTextGetTextName   ( XscText );
const char*   _XscTextGetTopic      ( XscText );
Dimension     _XscTextGetWidth      ( XscText );

void _XscTextSetAlignment  ( XscText, unsigned char );
void _XscTextSetConverter  ( XscText, unsigned char );
void _XscTextSetDirection  ( XscText, unsigned char );
void _XscTextSetFontList   ( XscText, XmFontList    );
void _XscTextSetFontListTag( XscText, String        );
void _XscTextSetTopic      ( XscText, String        );


void _XscTextLoadResources(
   XscText, Widget, const char*, XtResource*, Cardinal );
   
void _XscTextLoadCueResources ( XscText, Widget, const char* );
void _XscTextLoadHintResources( XscText, Widget, const char* );
void _XscTextLoadTipResources ( XscText, Widget, const char* );

void _XscTextStringDraw( 
   XscText, 
   Display*, 
   Window, 
   GC, 
   Position, 
   Position, 
   Dimension, 
   XRectangle*,
   Pixel,
   Pixel,
   Boolean );
   
Boolean _XscTextStringExists( XscText );
Boolean _XscTextHasWidgetName( XscText );


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif
