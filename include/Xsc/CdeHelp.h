/*-----------------------------------------------------------------------------
--  Help ToolKit for Motif/CDE
-- 
--  (c) Copyright 1997-1999, 2019 Robert S. Werner (a.k.a. Akiva Werner)  
--  ALL RIGHTS RESERVED
--
--  License: The MIT License (MIT)
-----------------------------------------------------------------------------*/

#ifndef __XscCdeHelp_h
#define __XscCdeHelp_h

#include <Xsc/Help.h>

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
                                    Macros
==============================================================================*/

/*---------
-- Names
---------*/
#ifndef XmNxscCdeHelpColumns
#define XmNxscCdeHelpColumns "xscCdeHelpColumns"
#endif

#ifndef XmNxscCdeHelpDialogTitle
#define XmNxscCdeHelpDialogTitle "xscCdeHelpDialogTitle"
#endif

#ifndef XmNxscCdeHelpRows
#define XmNxscCdeHelpRows "xscCdeHelpRows"
#endif

#ifndef XmNxscCdeHelpTopic
#define XmNxscCdeHelpTopic "xscCdeHelpTopic"
#endif

#ifndef XmNxscCdeHelpTopicTitle
#define XmNxscCdeHelpTopicTitle "xscCdeHelpTopicTitle"
#endif

#ifndef XmNxscCdeHelpType
#define XmNxscCdeHelpType "xscCdeHelpType"
#endif

#ifndef XmNxscCdeHelpVolume
#define XmNxscCdeHelpVolume "xscCdeHelpVolume"
#endif

#ifndef XmNxscCdeHelpWidgetType
#define XmNxscCdeHelpWidgetType "xscCdeHelpWidgetType"
#endif



/*----------
-- Classes
----------*/
#ifndef XmCXscCdeHelpColumns
#define XmCXscCdeHelpColumns "XscCdeHelpColumns"
#endif

#ifndef XmCXscCdeHelpDialogTitle
#define XmCXscCdeHelpDialogTitle "XscCdeHelpDialogTitle"
#endif

#ifndef XmCXscCdeHelpRows
#define XmCXscCdeHelpRows "XscCdeHelpRows"
#endif

#ifndef XmCXscCdeHelpTopic
#define XmCXscCdeHelpTopic "XscCdeHelpTopic"
#endif

#ifndef XmCXscCdeHelpTopicTitle
#define XmCXscCdeHelpTopicTitle "XscCdeHelpTopicTitle"
#endif

#ifndef XmCXscCdeHelpType
#define XmCXscCdeHelpType "XscCdeHelpType"
#endif

#ifndef XmCXscCdeHelpVolume
#define XmCXscCdeHelpVolume "XscCdeHelpVolume"
#endif

#ifndef XmCXscCdeHelpWidgetType
#define XmCXscCdeHelpWidgetType "XscCdeHelpWidgetType"
#endif



/*-------------
-- Rep Types
-------------*/
#ifndef XmRXscCdeHelpWidgetType
#define XmRXscCdeHelpWidgetType "XscCdeHelpWidgetType"
#endif


/*==============================================================================
                                  Data types
==============================================================================*/
enum 
{
   XmXSC_CDE_GENERAL_HELP_WIDGET,
   XmXSC_CDE_QUICK_HELP_WIDGET
};


/*==============================================================================
                                  Prototypes
==============================================================================*/
void XscCdeHelpInstall( Widget, String );


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif
