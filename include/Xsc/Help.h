/*------------------------------------------------------------------------------ 
--  Help ToolKit for Motif
-- 
--  (c) Copyright 1997-2000, 2019 Robert S. Werner (a.k.a. Akiva Werner)  
--  ALL RIGHTS RESERVED
--
--  License: The MIT License (MIT)
------------------------------------------------------------------------------*/

#ifndef __XscHelp_h
#define __XscHelp_h

#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <Xm/XmStrDefs.h>

#ifdef __cplusplus
extern "C" {
#endif


#define XscHelpVERSION	    1
#define XscHelpREVISION	    1
#define XscHelpUPDATE_LEVEL 2
#define XscHelpREGISTERED   1
#define XscHelpVersion (XscHelpVERSION * 1000 + XscHelpREVISION)
#define XscHelpVERSION_STRING "@(#)XscHelp Version 1.1.2"


/*==============================================================================
                                    Macros
==============================================================================*/


/*==============================================================================
                                  Data types
==============================================================================*/

enum
{
   XmXSC_STRING_CONVERTER_STANDARD,
   XmXSC_STRING_CONVERTER_FONT_TAG,
   XmXSC_STRING_CONVERTER_SEGMENTED 
};

enum
{
   XmXSC_SHOW_NAME_NONE,
   XmXSC_SHOW_NAME_SELF,
   XmXSC_SHOW_NAME_SHELL,
   XmXSC_SHOW_NAME_ALL
};

/*------------------------------------------------------------------------
-- The following are psuedo enumerated values used to specify specific
-- callback reasons used by this toolkit.  They should be enumerated 
-- values but are implemented as functions instead so that value conflicts 
-- with other libraries can be avoided.  The values can be adjusted by
-- setting the global variable _XscCROffset *prior* to installing
-- the Help ToolKit.  This should only be done if the default
-- callback reason codes conflict with another library's codes.
------------------------------------------------------------------------*/
#define XmCR_XSC_HELP_CONTEXT_GRAB_SELECT (_XscHelpCR_CONTEXT_GRAB_SELECT())
#define XmCR_XSC_HELP_CONTEXT_CALLBACK    (_XscHelpCR_CONTEXT_CALLBACK())

extern int _XscHelpCR_CONTEXT_GRAB_SELECT( void );
extern int _XscHelpCR_CONTEXT_CALLBACK( void );


typedef struct
{
   int       reason;
   XEvent*   event;
   int       depth;
   XtPointer data;
}
XscHelpContextCallbackStruct;


/*==============================================================================
                               External Variables
==============================================================================*/

/*--------------------------------------------------------------------------
-- This global is used to adjust the base value of the Help ToolKit callback
-- reason codes.  If changed, it must be changed prior to installing the '
-- library to have an effect.
--------------------------------------------------------------------------*/
extern int _XscCROffset;


/*==============================================================================
                                  Prototypes
==============================================================================*/
Boolean XscHelpCueExists( Widget );
void    XscHelpCueUpdate( Widget );

void XscHelpContextInstall( XtResourceList, Cardinal, int, XtCallbackProc, 
                            XtPointer );

void XscHelpContextPickAndActivate( Widget, Cursor, Boolean );

void XscHelpDbReload( Widget );

Boolean XscHelpHintExists ( Widget );
void    XscHelpHintInstall( Widget );
void    XscHelpHintUpdate ( Widget );

void XscHelpInstall( Widget );

int XscHelpLoadTopics( Display*, const char* );

void XscHelpSetCueTopic        ( Widget, String );
void XscHelpSetCueTopicDetails ( Widget, String, XmFontList, String, 
                                 unsigned char, unsigned char, unsigned char );
void XscHelpSetHintTopic       ( Widget, String );
void XscHelpSetHintTopicDetails( Widget, String, XmFontList, String, 
                                 unsigned char, unsigned char, unsigned char );
void XscHelpSetTipTopic        ( Widget, String );
void XscHelpSetTipTopicDetails ( Widget, String, XmFontList, String, 
                                 unsigned char, unsigned char, unsigned char );

Boolean XscHelpTipExists( Widget );
void    XscHelpTipUpdate( Widget );

void XscHelpUpdate( Widget );



Boolean XscHelpAreCuesDisplayable ( Widget );
Boolean XscHelpAreHintsDisplayable( Widget );
Boolean XscHelpAreTipsDisplayable ( Widget );

Boolean XscHelpAreCuesEnabledOnShell ( Widget );
Boolean XscHelpAreHintsEnabledOnShell( Widget );
Boolean XscHelpAreTipsEnabledOnShell ( Widget );

Boolean XscHelpAreCuesEnabledGlobally ();
Boolean XscHelpAreHintsEnabledGlobally();
Boolean XscHelpAreTipsEnabledGlobally ();

void XscHelpSetCuesEnabledOnShell ( Widget, Boolean );
void XscHelpSetHintsEnabledOnShell( Widget, Boolean );
void XscHelpSetTipsEnabledOnShell ( Widget, Boolean );

void XscHelpSetCuesEnabledGlobally ( Boolean );
void XscHelpSetHintsEnabledGlobally( Boolean );
void XscHelpSetTipsEnabledGlobally ( Boolean );

Boolean XscHelpIsDynamicTipGroupIdDefaultActive();
void XscHelpSetDynamicTipGroupDefault( Boolean );


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif
