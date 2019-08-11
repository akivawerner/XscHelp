/*------------------------------------------------------------------------------
--  Help ToolKit for Motif
-- 
--  (c) Copyright 1997-1999, 2019 Robert S. Werner (a.k.a. Akiva Werner)  
--  ALL RIGHTS RESERVED
--
--  License: The MIT License (MIT)
------------------------------------------------------------------------------*/

#ifndef __XscCue_h
#define __XscCue_h


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
typedef struct _XscCueRec *XscCue;


enum 
{
   XmXSC_CUE_POSITION_SHELL,
   XmXSC_CUE_POSITION_TOP_LEFT,
   XmXSC_CUE_POSITION_TOP_RIGHT,
   XmXSC_CUE_POSITION_BOTTOM_LEFT,
   XmXSC_CUE_POSITION_BOTTOM_RIGHT,
   XmXSC_CUE_POSITION_TOP_BEGINNING,
   XmXSC_CUE_POSITION_TOP_END,
   XmXSC_CUE_POSITION_BOTTOM_BEGINNING,
   XmXSC_CUE_POSITION_BOTTOM_END
};


/*==============================================================================
                                  Prototypes
==============================================================================*/
XscCue XscCueDeriveFromWidget( Widget );

Boolean XscCueHasValidTopic( XscCue );

unsigned char XscCueGetAlignment      ( XscCue );
Boolean       XscCueGetAutoDbReload   ( XscCue );
Pixel         XscCueGetBackground     ( XscCue );
Pixel         XscCueGetBorderColor    ( XscCue );
Dimension     XscCueGetBorderWidth    ( XscCue );
Pixel         XscCueGetColorBase      ( XscCue );
Boolean       XscCueGetEnabled        ( XscCue );
XmFontList    XscCueGetFontList       ( XscCue );
String        XscCueGetFontListTag    ( XscCue );
Pixel         XscCueGetForeground     ( XscCue );
Dimension     XscCueGetMarginBottom   ( XscCue );
Dimension     XscCueGetMarginHeight   ( XscCue );
Dimension     XscCueGetMarginLeft     ( XscCue );
Dimension     XscCueGetMarginRight    ( XscCue );
Dimension     XscCueGetMarginTop      ( XscCue );
Dimension     XscCueGetMarginWidth    ( XscCue );
Boolean       XscCueGetMotifColorModel( XscCue );
unsigned char XscCueGetPosition       ( XscCue );
Dimension     XscCueGetShadowThickness( XscCue );
unsigned char XscCueGetShadowType     ( XscCue );
unsigned char XscCueGetStringConverter( XscCue );
unsigned char XscCueGetStringDirection( XscCue );
String        XscCueGetTopic          ( XscCue );
int           XscCueGetXOffset        ( XscCue );
int           XscCueGetYOffset        ( XscCue );

void XscCueSetAlignment      ( XscCue, unsigned char, Boolean );
void XscCueSetAutoDbReload   ( XscCue, Boolean       );
void XscCueSetBackground     ( XscCue, Pixel         );
void XscCueSetBorderColor    ( XscCue, Pixel         );
void XscCueSetBorderWidth    ( XscCue, Dimension     );
void XscCueSetColorBase      ( XscCue, Pixel         );
void XscCueSetEnabled        ( XscCue, Boolean       );
void XscCueSetFontList       ( XscCue, XmFontList   , Boolean );
void XscCueSetFontListTag    ( XscCue, String       , Boolean );
void XscCueSetForeground     ( XscCue, Pixel         );
void XscCueSetMarginBottom   ( XscCue, Dimension     );
void XscCueSetMarginHeight   ( XscCue, Dimension     );
void XscCueSetMarginLeft     ( XscCue, Dimension     );
void XscCueSetMarginRight    ( XscCue, Dimension     );
void XscCueSetMarginTop      ( XscCue, Dimension     );
void XscCueSetMarginWidth    ( XscCue, Dimension     );
void XscCueSetMotifColorModel( XscCue, Boolean       );
void XscCueSetPosition       ( XscCue, unsigned char );
void XscCueSetShadowThickness( XscCue, Dimension     );
void XscCueSetShadowType     ( XscCue, unsigned char );
void XscCueSetStringConverter( XscCue, unsigned char, Boolean );
void XscCueSetStringDirection( XscCue, unsigned char, Boolean );
void XscCueSetTopic          ( XscCue, String        );
void XscCueSetXOffset        ( XscCue, int           );
void XscCueSetYOffset        ( XscCue, int           );



#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif
