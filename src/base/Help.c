/*------------------------------------------------------------------------------
--  Help ToolKit for Motif
-- 
--  (c) Copyright 1997-2000, 2019 Robert S. Werner (a.k.a. Akiva Werner)  
--  ALL RIGHTS RESERVED
--
--  License: The MIT License (MIT)
------------------------------------------------------------------------------*/

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <X11/IntrinsicP.h>
#include <Xm/Xm.h>
#include <Xm/RepType.h>

#include <Xsc/Help.h>
#include <Xsc/StrDefs.h>

#include "ContextP.h"
#include "CueP.h"
#include "DisplayP.h"
#include "HooksP.h"
#include "ObjectP.h"
#include "ScreenP.h"
#include "ShellP.h"
#include "TipP.h"


/*==============================================================================
                                    Macros
==============================================================================*/
#define XscHelpDefaultXOffset        0
#define XscHelpDefaultYOffset        15

/*==============================================================================
                               Private data types
==============================================================================*/
/*-----------------------------------------------------------------------
-- These enumerated types, when added to a base offset, are used as
-- the reason codes for the Help ToolKit specific callbacks.  The reason
-- codes are accessed via functions in case the offsets need to change
-- due to conflicts with other libraries.  The offset can be changed
-- prior to installing the library by setting the _XscHelpCROffset
-- global variable.  This global variable is referenced only during the 
-- install, so changing it later does not impact the values of the 
-- callback reason codes.
-----------------------------------------------------------------------*/
enum
{
   XscHelpCR_CONTEXT_GRAB_SELECT,
   XscHelpCR_CONTEXT_CALLBACK
};


/*==============================================================================
                            Private prototypes
==============================================================================*/
static void _createDisplayAndScreenRecords( Widget );


/*==============================================================================
                              Global variables
==============================================================================*/

/*--------------------------------------------------------------------------
-- This global is used to adjust the base value of the Help ToolKit callback
-- reason codes.  If changed, it must be changed prior to installing the 
-- library to have an effect.
--------------------------------------------------------------------------*/
int _XscCROffset = 6060;

/*==============================================================================
                           Static global variables
==============================================================================*/
/*--------------------------------------------------------------------------
-- This value is used to adjust the base value of the Help ToolKit callback
-- reason codes.
--------------------------------------------------------------------------*/
static int _CROffset = 6060;

/*--------------------------------------------------------------------
-- These values specify whether or not each type of help is globally
-- enabled or disabled
--------------------------------------------------------------------*/
static Boolean  _cuesEnabled = True;
static Boolean _hintsEnabled = True;
static Boolean  _tipsEnabled = True;

/*==============================================================================
                               Private functions
==============================================================================*/

/*------------------------------------------------------------------------------
-- This function creates the display/screen structures (if necessary)
-- associated with a given widget/gadget
------------------------------------------------------------------------------*/
static void _createDisplayAndScreenRecords( Widget obj )
{
   XscDisplay htk_display;
   XscScreen  htk_screen;

   /*------------------------------------------
   -- Determine if a display structure exists
   ------------------------------------------*/
   htk_display = _XscDisplayDeriveFromWidget( obj );
   if (!htk_display)
   {
      htk_display = _XscDisplayCreate( obj );
   }

   /*----------------------------------------
   -- Determine if a screen structure exist
   ----------------------------------------*/
   htk_screen  = _XscScreenDeriveFromWidget( obj );
   if (!htk_screen)
   {
      htk_screen = _XscScreenCreate( obj );
   }
}


/*==============================================================================
                            Protected functions
==============================================================================*/

/*------------------------------------------------------------------------------
-- This method attaches the Help ToolKit to a widget or gadget
------------------------------------------------------------------------------*/
void _XscHelpInstallOnWidget( Widget obj )
{
   assert( obj );

   /*-----------------------------------------------------------
   -- Do not install the library on the tip widgets themselves
   -----------------------------------------------------------*/
   if (strncmp( XtName( obj ), "xsc _ ", sizeof( "xsc _ " )-1 ) == 0) return;
   
   /*------------------------------------------------------------------
   -- All Motif-based widgets have a context sensitive help callback.
   -- A callback is usually only added when context-sensitive help
   -- is available; when help on a widget is requested, Motif then
   -- calls the widget (or it closest ancestor) that has a help
   -- callback defined.  However, there is no way for Motif to know
   -- if there is Help ToolKit context sensitive help available, so
   -- a callback is added to every widget and Help ToolKit does the
   -- ancestor searching.
   ------------------------------------------------------------------*/
   if (XtHasCallbacks( obj, XmNhelpCallback ) != XtCallbackNoList)
   {
      XtAddCallback( obj, XmNhelpCallback, _XscHelpContextHelpCB, NULL );
   }
    
   /*-----------------------------------------------------------------------
   -- Shell widgets cannot have tips, hints, etc.  However, this is a good
   -- place to create the display/screen data structures (if not already
   -- created.
   -----------------------------------------------------------------------*/
   if (XtIsShell( obj ))
   {
      /*--------------------------------------------------------------
      -- If there is no screen structure, there may not be a display
      -- structure as well.
      --------------------------------------------------------------*/
      XscScreen htk_screen = _XscScreenDeriveFromWidget( obj );
      
      if (!htk_screen)
      {
         _createDisplayAndScreenRecords( obj );
      }
      
      /*-----------------------------------------------------------------
      -- Make sure a shell record is created for this shell.  This used
      -- to only be needed if Hints were used.  It is now _always_
      -- needed because Tips and Cues can be disabled on a per
      -- shell basis
      -----------------------------------------------------------------*/
      _XscShellCreate( obj );
   }
   else
   {
      /*-------------------------------------------------------
      -- Make sure this widget does not already have a record
      -------------------------------------------------------*/
      XscObject htk_object = _XscObjectDeriveFromWidget( obj );

      if (!htk_object)
      {
      	 Widget _parent = XtParent( obj );
	 XscObject _parentObject = _XscObjectDeriveFromWidget( _parent );
	 
	 /*---------------------------------------------------------
	 -- If the parent of this widget has already been seen by
	 -- the toolkit, then it is safe to create this one too.
	 -- If the parent has not yet been seen, then don't create
	 -- this kid yet.  Wait until the parent's creation is
	 -- finished and then do the kids.  This avoids some nasty
	 -- problems of inheritance, because otherwise the kid
	 -- has no htk records from which to inherit.
	 ---------------------------------------------------------*/
	 if (_parentObject || XtIsShell( _parent ))
	 {
	    _XscObjectCreate( obj );
	    
	    /*----------------------------------------------------------
	    -- If it is a composite, there may be some children that
	    -- were "created" -- but not registered -- earlier
	    ----------------------------------------------------------*/
	    if (XtIsComposite( obj )) 
	    {
	       CompositeRec* _cr = (CompositeRec*) obj;
	       if (((int) _cr->composite.num_children) > 0)
	       {
		  int i;
		  for (i = 0; i < _cr->composite.num_children; i++)
		  {
	             Widget _kid = _cr->composite.children[ i ];
		     _XscObjectCreate( _kid );
		  }
	       }
	    }
	 }
      }
   }
}


/*------------------------------------------------------------------------------
-- This function is used to determine if the pointer is on top of a gadget
------------------------------------------------------------------------------*/
Widget _XscHelpGetGadgetChild( Widget obj, int x, int y )
{
   Widget gadget = NULL;
   
   /*----------------------------------------------
   -- Gadgets can only be children of a Composite
   ----------------------------------------------*/
   if (XtIsComposite( obj ))
   {
      CompositeWidget mgr = (CompositeWidget) obj;
      int             i;

      /*-------------------------------------------------------------------
      -- Search all the children....  Some may think this is a poor
      -- implementation since I use the widget internals directly.
      -- Well, my view is that this is far more efficient (there could be
      -- a lot of gadgets) and, lets face it, although private, this
      -- internal widget structure is not going to change.
      -------------------------------------------------------------------*/
      for (i = mgr->composite.num_children - 1; i >= 0; i--)
      {
	 Widget child = mgr->composite.children[ i ];

         /*--------------------
         -- For each child...
         --------------------*/
	 if (child)
	 {
	    /*---------------------------------------
	    -- Make sure it is not a widget, and...
	    ---------------------------------------*/
            if (!XtIsWidget( child ))
            {
               /*---------------------------------------------------
               -- Make sure it is a rect object; must be a gadget!
               ---------------------------------------------------*/
               if (XtIsRectObj( child ))
               {
                  /*-------------------------------------------
                  -- Make sure it is managed (or else it will 
                  -- not be displayed)
                  -------------------------------------------*/
        	  if (XtIsManaged( child ))
        	  {
        	     /*----------------------------------------------
        	     -- See if the (x,y) values are within the area 
        	     -- defined by the gadget
        	     ----------------------------------------------*/
                     if ((x >= child->core.x                     ) && 
                	 (x <  child->core.x + child->core.width ) &&
                	 (y >= child->core.y                     ) &&
                	 (y <  child->core.y + child->core.height))
                     {
                	gadget = child;
                	break;
                     }
        	  }
               }
            }
	 }	 
      }
   }
   return gadget;
}


/*==============================================================================
                             Public functions
==============================================================================*/
/*------------------------------------------------------------------------------
-- The following functions are used to simulate enumerated callback
-- reason codes.  
------------------------------------------------------------------------------*/
int _XscHelpCR_CONTEXT_GRAB_SELECT( void ) 
{
   return XscHelpCR_CONTEXT_GRAB_SELECT + _CROffset; 
}

int _XscHelpCR_CONTEXT_CALLBACK( void ) 
{
   return XscHelpCR_CONTEXT_CALLBACK + _CROffset; 
}


 
/*------------------------------------------------------------------------------
-- This function is used to activate hint processing for the descendants
-- of a shell.
------------------------------------------------------------------------------*/
void XscHelpHintInstall( Widget w )
{
   /*----------------------------------------------------------
   -- The hint display widget must not be a gadget or a shell
   ----------------------------------------------------------*/
   if (XtIsWidget( w ))
   {
      if (!XtIsShell( w ))
      {
      	 /*---------------------------------------------------
         -- Get the shell record associated with this widget
         ---------------------------------------------------*/
         XscShell htk_shell = _XscShellDeriveFromWidget( w );
         if (!htk_shell)
         {
            /*-------------------------------------------------------
            -- If the record does not exist, then make one!  
            -------------------------------------------------------*/
            htk_shell = _XscShellCreate( w );
         }
         
         /*--------------------------
         -- Install the hint widget
         --------------------------*/
         _XscShellInstallHint( htk_shell, w );
      }
   }
}


/*------------------------------------------------------------------------------
-- This function installs the Help ToolKit on a specific widget.
-- It is typically used to infect the session/application shells that
-- have no parents
------------------------------------------------------------------------------*/
void XscHelpInstall( Widget shell )
{
   static Boolean initialized = False;
   
   /*-------------------------------------------------------------------
   -- Make sure the Help ToolKit Composite extension record identifier
   -- is properly initalized
   -------------------------------------------------------------------*/
   if (initialized == False)
   {
      static String cue_position_name[] = 
      {
         "xsc_cue_position_pointer", 
         "xsc_cue_position_top_left", 
         "xsc_cue_position_top_right", 
         "xsc_cue_position_bottom_left",
         "xsc_cue_position_bottom_right",
         "xsc_cue_position_top_beginning", 
         "xsc_cue_position_top_end", 
         "xsc_cue_position_bottom_beginning",
         "xsc_cue_position_bottom_end"
      };
      static unsigned char cue_position_value[] = 
      {   
         (unsigned char) XmXSC_CUE_POSITION_SHELL,
         (unsigned char) XmXSC_CUE_POSITION_TOP_LEFT,
         (unsigned char) XmXSC_CUE_POSITION_TOP_RIGHT,
         (unsigned char) XmXSC_CUE_POSITION_BOTTOM_LEFT,
         (unsigned char) XmXSC_CUE_POSITION_BOTTOM_RIGHT,
         (unsigned char) XmXSC_CUE_POSITION_TOP_BEGINNING,
         (unsigned char) XmXSC_CUE_POSITION_TOP_END,
         (unsigned char) XmXSC_CUE_POSITION_BOTTOM_BEGINNING,
         (unsigned char) XmXSC_CUE_POSITION_BOTTOM_END
      };
      
      static String tip_position_name[] = 
      {
         "xsc_tip_position_pointer", 
         "xsc_tip_position_top_left", 
         "xsc_tip_position_top_right", 
         "xsc_tip_position_bottom_left",
         "xsc_tip_position_bottom_right",
         "xsc_tip_position_top_beginning", 
         "xsc_tip_position_top_end", 
         "xsc_tip_position_bottom_beginning",
         "xsc_tip_position_bottom_end"
      };
      static unsigned char tip_position_value[] = 
      {   
         (unsigned char) XmXSC_TIP_POSITION_POINTER,
         (unsigned char) XmXSC_TIP_POSITION_TOP_LEFT,
         (unsigned char) XmXSC_TIP_POSITION_TOP_RIGHT,
         (unsigned char) XmXSC_TIP_POSITION_BOTTOM_LEFT,
         (unsigned char) XmXSC_TIP_POSITION_BOTTOM_RIGHT,
         (unsigned char) XmXSC_TIP_POSITION_TOP_BEGINNING,
         (unsigned char) XmXSC_TIP_POSITION_TOP_END,
         (unsigned char) XmXSC_TIP_POSITION_BOTTOM_BEGINNING,
         (unsigned char) XmXSC_TIP_POSITION_BOTTOM_END
      };
      
      static String string_conversion_name[] = 
      {
         "xsc_string_converter_standard", 
         "xsc_string_converter_font_tag", 
         "xsc_string_converter_segmented"
      };
      static unsigned char string_conversion_value[] = 
      {   
         (unsigned char) XmXSC_STRING_CONVERTER_STANDARD,
         (unsigned char) XmXSC_STRING_CONVERTER_FONT_TAG,
         (unsigned char) XmXSC_STRING_CONVERTER_SEGMENTED 
      };
      
      static String tip_group_name[] = 
      {
         "xsc_tip_group_null", 
         "xsc_tip_group_self", 
         "xsc_tip_group_parent"
      };
      static unsigned char tip_group_value[] = 
      {   
         (unsigned char) XmXSC_TIP_GROUP_NULL,
         (unsigned char) XmXSC_TIP_GROUP_SELF,
         (unsigned char) XmXSC_TIP_GROUP_PARENT 
      };
      
      static String show_name_name[] = 
      {
         "xsc_show_name_none", 
         "xsc_show_name_self", 
         "xsc_show_name_shell", 
         "xsc_show_name_all"
      };
      static unsigned char show_name_value[] = 
      {   
         (unsigned char) XmXSC_SHOW_NAME_NONE,
         (unsigned char) XmXSC_SHOW_NAME_SELF,
         (unsigned char) XmXSC_SHOW_NAME_SHELL,
         (unsigned char) XmXSC_SHOW_NAME_ALL
      };
      
      initialized = True;
      _CROffset = _XscCROffset;
      
      {
         XContext context = XUniqueContext();
         _XscDisplayInitialize( context );
         _XscScreenInitialize ( context );
         _XscShellInitialize  ( context );
         
         _XscObjectInitialize( XUniqueContext() );
      }
      
      if (XmRepTypeGetId( XmRXscCuePosition ) == XmREP_TYPE_INVALID)
      {
         XmRepTypeRegister( 
            XmRXscCuePosition,
            cue_position_name,
            cue_position_value,
            XtNumber( cue_position_value ) );
      }
      else
      {
         char* param = XmRXscCuePosition;
         Cardinal num_params = 1;

         XtErrorMsg(
            "repTypRegister",
            "alreadyRegistered",
            "XscHelp",
            "The enumeration \"%s\" defined by the XscHelp library "
               "was already registered.",
            &param, &num_params );
      }

      if (XmRepTypeGetId( XmRXscTipPosition ) == XmREP_TYPE_INVALID)
      {
         XmRepTypeRegister( 
            XmRXscTipPosition,
            tip_position_name,
            tip_position_value,
            XtNumber( tip_position_value ) );
      }
      else
      {
         char* param = XmRXscTipPosition;
         Cardinal num_params = 1;

         XtErrorMsg(
            "repTypRegister",
            "alreadyRegistered",
            "XscHelp",
            "The enumeration \"%s\" defined by the XscHelp library "
               "was already registered.",
            &param, &num_params );
      }

      if (XmRepTypeGetId( XmRXscStringConverter ) == XmREP_TYPE_INVALID)
      {
         XmRepTypeRegister( 
            XmRXscStringConverter,
            string_conversion_name,
            string_conversion_value,
            XtNumber( string_conversion_value ) );
      }
      else
      {
         char* param = XmRXscStringConverter;
         Cardinal num_params = 1;

         XtErrorMsg(
            "repTypRegister",
            "alreadyRegistered",
            "XscHelp",
            "The enumeration \"%s\" defined by the XscHelp library "
               "was already registered.",
            &param, &num_params );
      }
      
      if (XmRepTypeGetId( XmRXscTipGroup ) == XmREP_TYPE_INVALID)
      {
         XmRepTypeRegister( 
            XmRXscTipGroup,
            tip_group_name,
            tip_group_value,
            XtNumber( tip_group_value ) );
      }
      else
      {
         char* param = XmRXscTipGroup;
         Cardinal num_params = 1;

         XtErrorMsg(
            "repTypRegister",
            "alreadyRegistered",
            "XscHelp",
            "The enumeration \"%s\" defined by the XscHelp library "
               "was already registered.",
            &param, &num_params );
      }
      
      if (XmRepTypeGetId( XmRXscShowName ) == XmREP_TYPE_INVALID)
      {
         XmRepTypeRegister( 
            XmRXscShowName,
            show_name_name,
            show_name_value,
            XtNumber( show_name_value ) );
      }
      else
      {
         char* param = XmRXscShowName;
         Cardinal num_params = 1;

         XtErrorMsg(
            "repTypRegister",
            "alreadyRegistered",
            "XscHelp",
            "The enumeration \"%s\" defined by the XscHelp library "
               "was already registered.",
            &param, &num_params );
      }
   }
   
   if (shell)
   {
      /*--------------------------------------------------
      -- Do not install a display that is already known!
      --------------------------------------------------*/
      XscDisplay htk_display = _XscDisplayDeriveFromWidget( shell );
      if (!htk_display)
      {
      	 /*---------------------------------------------------------
      	 -- This is technically a bug since it assumes that this
	 -- is the only application context that will be used. :-(
	 ---------------------------------------------------------*/
      	 XtAppSetTypeConverter(
	    XtWidgetToApplicationContext( shell ),
      	    XtRString,
	    XmRXscTipGroupId, 
	    XscCvtStringToTipGroupId,
      	    NULL,
	    (Cardinal) 0,
	    XtCacheAll, 
	    NULL );

      
         _XscHooksInstall( shell );
	 _XscHelpInstallOnWidget( shell );
      }
   }
}

/*------------------------------------------------------------------------------
-- This function loads "global" topics into the resource database.  There is
-- a very simple parsing rule
------------------------------------------------------------------------------*/
int XscHelpLoadTopics( Display* theDisplay, const char* theFilename )
{
   int _result = 0;
   
   FILE* _file = fopen( theFilename, "r" );
   
   if (_file)
   {
      char    _specifier [  256 ];
      char    _lineBuffer[ 1024 ];
      char*   _ptr;
      char*   _topicBuffer     = NULL;
      int     _topicBufferSize = 0;
      int     _topicLen        = 0;
      char*   _topicEnd        = NULL;
      
      XrmDatabase _db = XrmGetDatabase( theDisplay );
      
      Boolean _inTopic = False;
      
      _topicBufferSize = sizeof( _lineBuffer );
      _topicBuffer = XtMalloc( _topicBufferSize );
      _topicLen    = 0;
      _topicEnd    = _topicBuffer;
      
      *_topicBuffer = '\0';
      
      while (!feof( _file ))
      {
      	 _ptr = fgets( _lineBuffer, sizeof( _lineBuffer ), _file );
	 if (_ptr)
	 {
	    if (strncmp( _ptr, ".TEXT[", 6 ) == 0)
	    {
	       char* _walk;
	       char* _end;
	       
	       _ptr += 6;
	       _walk = _ptr;
	       
	       while (*_walk)
	       {
	          if (!isspace( *_walk )) break;
		  
		  _ptr++;
		  _walk++;
	       }
	       
	       _end = NULL;
	       while (*_walk)
	       {
	          if (isspace( *_walk ))
		  {
		     _end = _walk;
		  }
		  else if (*_walk == ']')
		  {
		     if (_end == NULL) _end = _walk;
		     break;
		  }
		  else
		  {
		     _end = NULL;
		  }
		  _walk++;
	       }
	       *_end = '\0';
	       
	       if (_inTopic)
	       {
		  if (_topicLen)
		  {
	             _topicEnd--;
		     if (*_topicEnd == '\n') *_topicEnd = '\0';
		  }
	       
		  XrmPutStringResource(
	             &_db, _specifier, _topicBuffer );
	       
		  *_topicBuffer = '\0';
		  _topicEnd = _topicBuffer;
		  _topicLen = 0;
	       }
	       sprintf( _specifier, "_xscHelp.topic.%s", _ptr );
	       _inTopic = True;
	    }
	    else if (strncmp( _ptr, ".END", 4 ) == 0)
	    {
	       if (_topicLen)
	       {
	          _topicEnd--;
		  if (*_topicEnd == '\n') *_topicEnd = '\0';
	       }
	       
	       XrmPutStringResource(
	          &_db, _specifier, _topicBuffer );
		  
	       _inTopic = False;
	       *_topicBuffer = '\0';
	       _topicEnd = _topicBuffer;
	       _topicLen = 0;
	    }
	    else
	    {
	       if (_inTopic)
	       {
		  int _lineLen = strlen( _ptr );
		  if (_topicLen + _lineLen >= _topicBufferSize)
		  {
		     _topicBufferSize = _topicLen + _lineLen + 1024;
		     
		     _topicBuffer = XtRealloc( _topicBuffer, _topicBufferSize );
		     _topicEnd = _topicBuffer + _topicLen;
		  }
		  strcpy( _topicEnd, _ptr );
		  _topicEnd += _lineLen;
		  _topicLen += _lineLen;
	       }
	    }
	 }
      }
      XtFree( _topicBuffer );
      fclose( _file );
   }
   else
   {
      _result = errno;
   }
   return _result;
}


Boolean XscHelpAreCuesDisplayable( Widget aWidget )
{
   if (_cuesEnabled)
   {
      XscShell _shell = _XscShellDeriveFromWidget( aWidget );
      return _XscShellIsCueEnabled( _shell );
   }
   return False;
}
Boolean XscHelpAreHintsDisplayable( Widget aWidget )
{
   if (_hintsEnabled)
   {
      XscShell _shell = _XscShellDeriveFromWidget( aWidget );
      return _XscShellIsHintEnabled( _shell );
   }
   return False;
}
Boolean XscHelpAreTipsDisplayable( Widget aWidget )
{
   if (_tipsEnabled)
   {
      XscShell _shell = _XscShellDeriveFromWidget( aWidget );
      return _XscShellIsTipEnabled( _shell );
   }
   return False;
}

Boolean XscHelpAreCuesEnabledOnShell( Widget aWidget )
{
   XscShell _shell = _XscShellDeriveFromWidget( aWidget );
   return _XscShellIsCueEnabled( _shell );
}
Boolean XscHelpAreHintsEnabledOnShell( Widget aWidget )
{
   XscShell _shell = _XscShellDeriveFromWidget( aWidget );
   return _XscShellIsHintEnabled( _shell );
}
Boolean XscHelpAreTipsEnabledOnShell( Widget aWidget )
{
   XscShell _shell = _XscShellDeriveFromWidget( aWidget );
   return _XscShellIsTipEnabled( _shell );
}

Boolean XscHelpAreCuesEnabledGlobally () { return _cuesEnabled;  }
Boolean XscHelpAreHintsEnabledGlobally() { return _hintsEnabled; }
Boolean XscHelpAreTipsEnabledGlobally () { return _tipsEnabled;  }

void XscHelpSetCuesEnabledOnShell( Widget aWidget, Boolean aFlag )
{
   XscShell _shell = _XscShellDeriveFromWidget( aWidget );
   _XscShellSetCueEnabled( _shell, aFlag );
   
   if (aFlag == False)
   {
      XscDisplay _display = _XscDisplayDeriveFromWidget( aWidget );
      XscCue _cue = _XscDisplayGetActiveCue( _display );
      if (_cue)
      {
      	 _XscCuePopdown( _cue );
      }
   }
}
void XscHelpSetHintsEnabledOnShell( Widget aWidget, Boolean aFlag )
{
   XscShell _shell = _XscShellDeriveFromWidget( aWidget );
   _XscShellSetHintEnabled( _shell, aFlag );
   
   if (aFlag == False)
   {
      XscDisplay _display = _XscDisplayDeriveFromWidget( aWidget );
      XscShell _shell = _XscShellDeriveFromWidget( aWidget );
      if (_shell)
      {      
      	 _XscShellEraseHint( _shell );
      }
      _XscDisplaySetActiveHint( _display, NULL );
   }
}
void XscHelpSetTipsEnabledOnShell( Widget aWidget, Boolean aFlag )
{
   XscShell _shell = _XscShellDeriveFromWidget( aWidget );
   _XscShellSetTipEnabled( _shell, aFlag );
   
   if (aFlag == False)
   {
      XscDisplay _display = _XscDisplayDeriveFromWidget( aWidget );
      XscObject _obj = _XscDisplayGetActiveTip( _display );
      if (_obj)
      {
	 XscTip _tip = _XscObjectGetTip( _obj );
	 if (_tip)
	 {
      	    _XscTipPopdown( _tip );
	 }
      }
   }
}

void XscHelpSetCuesEnabledGlobally( Boolean aFlag ) 
{  
   _cuesEnabled = aFlag; 
}
void XscHelpSetHintsEnabledGlobally( Boolean aFlag ) 
{ 
   _hintsEnabled = aFlag; 
}
void XscHelpSetTipsEnabledGlobally( Boolean aFlag ) 
{  
   _tipsEnabled = aFlag; 
}
