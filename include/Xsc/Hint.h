/*------------------------------------------------------------------------------
--  Help ToolKit for Motif
-- 
--  (c) Copyright 1997-1999, 2019 Robert S. Werner (a.k.a. Akiva Werner)  
--  ALL RIGHTS RESERVED
--
--  License: The MIT License (MIT)
------------------------------------------------------------------------------*/

#ifndef __XscHint_h
#define __XscHint_h


#include <X11/Intrinsic.h>
#include <Xm/Xm.h>


#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
                                    Macros
==============================================================================*/


/*==============================================================================
                               Data types
==============================================================================*/
typedef struct _XscHintRec *XscHint;


/*==============================================================================
                                  Prototypes
==============================================================================*/
XscHint XscHintDeriveFromWidget( Widget );

Boolean XscHintHasValidTopic( XscHint );

unsigned char XscHintGetAlignment        ( XscHint );
Boolean       XscHintGetAutoDbReload     ( XscHint );
Pixel         XscHintGetBackground       ( XscHint );
Boolean       XscHintGetCompound         ( XscHint );
Boolean       XscHintGetEnabled          ( XscHint );
XmFontList    XscHintGetFontList         ( XscHint );
String        XscHintGetFontListTag      ( XscHint );
Pixel         XscHintGetForeground       ( XscHint );
Boolean       XscHintGetInheritBackground( XscHint );
Dimension     XscHintGetMarginLeft       ( XscHint );
Dimension     XscHintGetMarginRight      ( XscHint );
Dimension     XscHintGetMarginWidth      ( XscHint );
Boolean       XscHintGetMotifColorModel  ( XscHint );
unsigned char XscHintGetStringConverter  ( XscHint );
unsigned char XscHintGetStringDirection  ( XscHint );
String        XscHintGetTopic            ( XscHint );

void XscHintSetAlignment        ( XscHint, unsigned char, Boolean );
void XscHintSetAutoDbReload     ( XscHint, Boolean       );
void XscHintSetBackground       ( XscHint, Pixel         );
void XscHintSetCompound         ( XscHint, Boolean       );
void XscHintSetEnabled          ( XscHint, Boolean       );
void XscHintSetFontList         ( XscHint, XmFontList   , Boolean );
void XscHintSetFontListTag      ( XscHint, String       , Boolean );
void XscHintSetForeground       ( XscHint, Pixel         );
void XscHintSetInheritBackground( XscHint, Boolean       );
void XscHintSetMarginLeft       ( XscHint, Dimension     );
void XscHintSetMarginRight      ( XscHint, Dimension     );
void XscHintSetMarginWidth      ( XscHint, Dimension     );
void XscHintSetMotifColorModel  ( XscHint, Boolean       );
void XscHintSetStringConverter  ( XscHint, unsigned char, Boolean );
void XscHintSetStringDirection  ( XscHint, unsigned char, Boolean );
void XscHintSetTopic            ( XscHint, String        );



#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif
