/*------------------------------------------------------------------------------
--  Help ToolKit for Motif
-- 
--  (c) Copyright 1997-1999, 2019 Robert S. Werner (a.k.a. Akiva Werner)  
--  ALL RIGHTS RESERVED
--
--  License: The MIT License (MIT)
------------------------------------------------------------------------------*/

#ifndef __XscTip_h
#define __XscTip_h


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
typedef struct _XscTipRec *XscTip;


enum 
{
   XmXSC_TIP_POSITION_POINTER,
   XmXSC_TIP_POSITION_TOP_LEFT,
   XmXSC_TIP_POSITION_TOP_RIGHT,
   XmXSC_TIP_POSITION_BOTTOM_LEFT,
   XmXSC_TIP_POSITION_BOTTOM_RIGHT,
   XmXSC_TIP_POSITION_TOP_BEGINNING,
   XmXSC_TIP_POSITION_TOP_END,
   XmXSC_TIP_POSITION_BOTTOM_BEGINNING,
   XmXSC_TIP_POSITION_BOTTOM_END
};

enum
{
   XmXSC_TIP_GROUP_SELF   = -1,
   XmXSC_TIP_GROUP_PARENT = -2,
   XmXSC_TIP_GROUP_NULL   = -3
};


/*==============================================================================
                                  Prototypes
==============================================================================*/
XscTip XscTipDeriveFromWidget( Widget );

Boolean XscTipHasValidTopic( XscTip );

unsigned char XscTipGetAlignment          ( XscTip );
Boolean       XscTipGetAutoDbReload       ( XscTip );
Pixel         XscTipGetBackground         ( XscTip );
Pixel         XscTipGetBorderColor        ( XscTip );
Dimension     XscTipGetBorderWidth        ( XscTip );
Pixel         XscTipGetColorBase          ( XscTip );
Boolean       XscTipGetCompound           ( XscTip );
Boolean       XscTipGetEnabled            ( XscTip );
XmFontList    XscTipGetFontList           ( XscTip );
String        XscTipGetFontListTag        ( XscTip );
Pixel         XscTipGetForeground         ( XscTip );
int           XscTipGetGroupId            ( XscTip );
Dimension     XscTipGetMarginBottom       ( XscTip );
Dimension     XscTipGetMarginHeight       ( XscTip );
Dimension     XscTipGetMarginLeft         ( XscTip );
Dimension     XscTipGetMarginRight        ( XscTip );
Dimension     XscTipGetMarginTop          ( XscTip );
Dimension     XscTipGetMarginWidth        ( XscTip );
Boolean       XscTipGetMotifColorModel    ( XscTip );
unsigned long XscTipGetPopdownInterval    ( XscTip );
unsigned long XscTipGetPopupInterval      ( XscTip );
unsigned char XscTipGetPosition           ( XscTip );
unsigned long XscTipGetSelectNameInterval ( XscTip );
Dimension     XscTipGetShadowThickness    ( XscTip );
unsigned char XscTipGetShadowType         ( XscTip );
unsigned char XscTipGetStringConverter    ( XscTip );
unsigned char XscTipGetStringDirection    ( XscTip );
String        XscTipGetTopic              ( XscTip );
int           XscTipGetXOffset            ( XscTip );
int           XscTipGetYOffset            ( XscTip );

void XscTipSetAlignment          ( XscTip, unsigned char, Boolean u );
void XscTipSetAutoDbReload       ( XscTip, Boolean       );
void XscTipSetBackground         ( XscTip, Pixel         );
void XscTipSetBorderColor        ( XscTip, Pixel         );
void XscTipSetBorderWidth        ( XscTip, Dimension     );
void XscTipSetColorBase          ( XscTip, Pixel         );
void XscTipSetCompound           ( XscTip, Boolean       );
void XscTipSetEnabled            ( XscTip, Boolean       );
void XscTipSetFontList           ( XscTip, XmFontList   , Boolean u );
void XscTipSetFontListTag        ( XscTip, String       , Boolean u );
void XscTipSetForeground         ( XscTip, Pixel         );
void XscTipSetGroupId            ( XscTip, int           );
void XscTipSetMarginBottom       ( XscTip, Dimension     );
void XscTipSetMarginHeight       ( XscTip, Dimension     );
void XscTipSetMarginLeft         ( XscTip, Dimension     );
void XscTipSetMarginRight        ( XscTip, Dimension     );
void XscTipSetMarginTop          ( XscTip, Dimension     );
void XscTipSetMarginWidth        ( XscTip, Dimension     );
void XscTipSetMotifColorModel    ( XscTip, Boolean       );
void XscTipSetPopdownInterval    ( XscTip, unsigned long );
void XscTipSetPopupInterval      ( XscTip, unsigned long );
void XscTipSetPosition           ( XscTip, unsigned char );
void XscTipSetShadowThickness    ( XscTip, Dimension     );
void XscTipSetShadowType         ( XscTip, unsigned char );
void XscTipSetStringConverter    ( XscTip, unsigned char, Boolean u );
void XscTipSetStringDirection    ( XscTip, unsigned char, Boolean u );
void XscTipSetTopic              ( XscTip, String        );
void XscTipSetXOffset            ( XscTip, int           );
void XscTipSetYOffset            ( XscTip, int           );


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif
