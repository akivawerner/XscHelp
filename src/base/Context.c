/*------------------------------------------------------------------------------
--  Help ToolKit for Motif
-- 
--  (c) Copyright 1997-1999, 2019 Robert S. Werner (a.k.a. Akiva Werner)  
--  ALL RIGHTS RESERVED
--
--  License: The MIT License (MIT)
------------------------------------------------------------------------------*/


#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <X11/Intrinsic.h>
#include <X11/keysym.h>

#include <Xm/Xm.h>

#include <Xsc/Help.h>

#include "ContextP.h"
#include "HelpP.h"

/*==============================================================================
                            Private prototypes
==============================================================================*/
static void        _helpOnWidget( Widget, XtPointer, int );

/*==============================================================================
                           Static global variables
==============================================================================*/
/*----------------------------------------------------------
-- Data associated with the context-sensitive help look-up
----------------------------------------------------------*/
static int            _helpBufferSize;
static XtResourceList _helpResources;
static Cardinal       _numHelpResources;
static int            _helpCheckOffset;
static XtCallbackProc _contextHelpProc;
static XtPointer      _contextHelpClientData;

/*==============================================================================
                               Private functions
==============================================================================*/

/*------------------------------------------------------------------------------
-- This function finds the best context-sensitive help associated with
-- a widget and calls the Help ToolKit context-help callback function.
------------------------------------------------------------------------------*/
static void _helpOnWidget( Widget w, XtPointer cbd, int reason )
{
   /*--------------------------------------------------------------------
   -- Don't bother doing anything if these basic, required context-help 
   -- records are not defined
   --------------------------------------------------------------------*/
   if (_helpBufferSize && _numHelpResources && _contextHelpProc)
   {
      /*--------------------------------------------------------------
      -- Create a buffer to store the data; depth starts at 0 and is 
      -- incremented each time a parent is examined for help.
      --------------------------------------------------------------*/
      char*   data       = XtMalloc( _helpBufferSize );
      int     depth      = 0;
      Boolean data_found = False;
      
      while (w)
      {
         XtPointer help_check;
         
         /*------------------------------------------
         -- Try to find the context-help attributes
         ------------------------------------------*/
         XtGetApplicationResources( 
            w, 
            (XtPointer) data, 
            _helpResources,      _numHelpResources,
            NULL,    	      	 (Cardinal) 0 );
         
         /*------------------------------------------------------------
         -- Verify that the guaranteed pointer is, in fact, defined.
         -- If the pointer at this offset is equal to NULL, then, by
         -- definition, there is no context-sensitive help defined at
         -- this level and the search continues with the parent.
         ------------------------------------------------------------*/
         help_check = (XtPointer)(data + _helpCheckOffset);
         if (*((char**)help_check) == NULL)
         {
            w = XtParent( w );
            depth++;
         }
         else
         {
            data_found = True;
            break;
         }
      }
      
      /*------------------------------------------
      -- Prepare and call the callback function!
      ------------------------------------------*/
      if (w)
      {
         XscHelpContextCallbackStruct cb_data;
         
         cb_data.reason = reason;
         
         if (cbd)
         {
            XmAnyCallbackStruct* all_cb_data = (XmAnyCallbackStruct*) cbd;
            
            cb_data.event = all_cb_data->event;
         }
         else
         {
            cb_data.event = NULL;
         }
         cb_data.depth = depth;
         
         if (data_found)
         {
            cb_data.data = data;
         }
         else
         {
            cb_data.data = NULL;
         }
         
         _contextHelpProc( w, _contextHelpClientData, (XtPointer) &cb_data );
      }
      
      XtFree( data );
   }
}

/*==============================================================================
                            Protected functions
==============================================================================*/

/*------------------------------------------------------------------------------
-- This callback function is called as a result if the standard Motif 
-- help callback found on all Motif widgets.
------------------------------------------------------------------------------*/
void _XscHelpContextHelpCB( Widget w, XtPointer cd, XtPointer cbd )
{
   _helpOnWidget( w, cbd, XmCR_XSC_HELP_CONTEXT_CALLBACK );
}

/*==============================================================================
                             Public functions
==============================================================================*/

/*------------------------------------------------------------------------------
-- This function saves all the attributes needed to perform context-sensitive
-- help lookups.
------------------------------------------------------------------------------*/
void XscHelpContextInstall(
   XtResourceList resource_list,
   Cardinal       num_resources,
   int            size_of_help_struct,
   XtCallbackProc context_help_proc,
   XtPointer      client_data )
{
   _helpBufferSize        = size_of_help_struct;
   _helpResources         = resource_list;
   _numHelpResources      = num_resources;
   _contextHelpProc       = context_help_proc;
   _contextHelpClientData = client_data;
   
   if (num_resources)
   {
      _helpCheckOffset = resource_list[ 0 ].resource_offset;
   }
}

/*------------------------------------------------------------------------------
-- This function allows an end-user to request context-sensitive help on a
-- specific screen object.
------------------------------------------------------------------------------*/
void XscHelpContextPickAndActivate( 
   Widget  widget,
   Cursor  cursor, 
   Boolean confine_to )
{
   XtAppContext app_context;
   Time         event_time;
   Widget       choosen_widget = NULL;
   
   event_time = XtLastTimestampProcessed( XtDisplayOfObject( widget ) );
   
   /*------------------------------------
   -- Grab the keyboard and the pointer
   ------------------------------------*/
   XtGrabPointer( 
      widget, 
      False, 
      ButtonPressMask,
      GrabModeAsync,
      GrabModeAsync,
      confine_to ? XtWindow( widget ) : None,
      cursor,
      event_time );
      
   XtGrabKeyboard( 
      widget, 
      False, 
      GrabModeAsync,
      GrabModeAsync,
      event_time );
   
   app_context = XtWidgetToApplicationContext( widget );
   
   while (True)
   {
      XEvent event;
      
      /*---------------------
      -- Get the next event
      ---------------------*/
      XtAppNextEvent( app_context, &event );

      /*---------------------------------------------
      -- Watch for button press and key press events
      ----------------------------------------------*/
      if (event.type == ButtonPress || event.type == ButtonRelease)
      {
      	 /*------------------------------------------------
         -- Determine if the button press was on a gadget
         ------------------------------------------------*/
         choosen_widget = 
            _XscHelpGetGadgetChild( widget, event.xbutton.x, event.xbutton.y );
         
         if (!choosen_widget)
         {
            choosen_widget = widget;
         }

         event_time = event.xbutton.time;         
         break;
      }
      else if (event.type == KeyPress || event.type == KeyRelease)
      {
         KeySym    key_sym;

      	 /*----------------------------------------------------
      	 -- If the key pressed was the Ecape key, then do not
      	 -- cancel the request without assigning a widget
      	 ----------------------------------------------------*/
         key_sym = XLookupKeysym( &event.xkey, 0 );
         if (key_sym != XK_Escape)
         {
            /*---------------------------------------------
            -- Determine if the key press was on a gadget
            ---------------------------------------------*/
            choosen_widget = 
               _XscHelpGetGadgetChild( widget,event.xbutton.x,event.xbutton.y );

            if (!choosen_widget)
            {
               choosen_widget = widget;
            }
         }
         event_time = event.xkey.time;         
         break;
      }
      else
      {
         XtDispatchEvent( &event );
      }
   }
   XtUngrabPointer ( widget, event_time );
   XtUngrabKeyboard( widget, event_time );
   
   /*-------------------------------------------------
   -- Request help on the specified widget or gadget
   -------------------------------------------------*/
   if (choosen_widget)
   {
      _helpOnWidget( choosen_widget, NULL, XmCR_XSC_HELP_CONTEXT_GRAB_SELECT );
   }
}

