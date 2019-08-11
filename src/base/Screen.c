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

#include <Xm/Xm.h>
#include <Xm/Frame.h>
#include <Xm/Label.h>
#include <Xm/Screen.h>

#include "CueP.h"
#include "DisplayP.h"
#include "ObjectP.h"
#include "ScreenP.h"
#include "TipP.h"

/*==============================================================================
                               Private data types
==============================================================================*/
/*--------------------------------------------------------------------
-- This structure defines the attributes associated with each screen
--------------------------------------------------------------------*/
typedef struct _XscScreenRec
{
   Screen* xScreen;
   Widget  cueShell;
   Widget  cueFrame;
   Widget  cueLabel;
   Widget  tipShell;
   Widget  tipFrame;
   Widget  tipLabel;
   GC      gc;
}
XscScreenRec;

/*==============================================================================
                            Private prototypes
==============================================================================*/
static void _destroyScreenCB( Widget, XtPointer, XtPointer );

static void _renderCueEH( Widget, XtPointer, XEvent*, Boolean* );
static void _renderTipEH( Widget, XtPointer, XEvent*, Boolean* );


/*==============================================================================
                           Static global variables
==============================================================================*/
/*-----------------------------------------------------------------------
-- The Help ToolKit data structures are managed by the X Context Manager.  
-- These are the context identifiers used to lok up a data structure.
-----------------------------------------------------------------------*/
static XContext _screenContextId;


/*==============================================================================
                            Private functions
==============================================================================*/


/*==============================================================================
                            Protected functions
==============================================================================*/
/*-------------------------------------------------
-- This function is used to initialize the module
-------------------------------------------------*/
void _XscScreenInitialize( XContext context )
{
   if (!_screenContextId)
   {
      if (context)
      {
         _screenContextId = context;
      }
      else
      {
         _screenContextId = XUniqueContext();
      }
   }
}



XscScreen _XscScreenCreate( Widget obj )
{
   XscScreen self;
   Display*  x_display = XtDisplayOfObject( obj );
   Screen*   x_screen  = XtScreenOfObject ( obj );
   
   /*------------------------------------
   -- Make a Help ToolKit screen record
   ------------------------------------*/
   self = XtNew( XscScreenRec );

   /*----------------------------
   -- Initialize data structure
   ----------------------------*/
   self->xScreen = x_screen;

   /*-------------------------------------------------------------
   -- Create the GC that will be used for drawing on this screen
   -------------------------------------------------------------*/
   self->gc = XCreateGC( x_display, RootWindowOfScreen( x_screen ), 0, NULL );

   /*--------------------------------------------------------
   -- Create the widgets needed for the cues on this screen
   --------------------------------------------------------*/
   self->cueShell = XtAppCreateShell(
      "xsc _ Cue Shell",
      "Xsc _ Cue Shell",
      overrideShellWidgetClass,
      x_display,
      NULL, (Cardinal) 0 );
   XtVaSetValues( self->cueShell, XtNallowShellResize, TRUE, NULL );

   self->cueFrame = XtVaCreateManagedWidget(
      "xsc _ Cue Frame",
      xmFrameWidgetClass,
      self->cueShell,
      NULL );

   /*---------------------------------------------------
   -- Core is used as a very light-weight drawing area
   ---------------------------------------------------*/
   self->cueLabel = XtVaCreateManagedWidget(
      "xsc _ Cue Label",
      coreWidgetClass,
      self->cueFrame,
      XtNborderWidth,        0,
      NULL );

   /*----------------------------------------------------------------
   -- The library needs to know when the tip is exposed in order to
   -- render the text
   ----------------------------------------------------------------*/
   XtAddEventHandler( 
      self->cueLabel, 
      ExposureMask, 
      False, 
      _renderCueEH,
      NULL );
      
   /*--------------------------------------------------------
   -- Create the widgets needed for the tips on this screen
   --------------------------------------------------------*/
   self->tipShell = XtAppCreateShell(
      "xsc _ Tip Shell",
      "Xsc _ Tip Shell",
      overrideShellWidgetClass,
      x_display,
      NULL, (Cardinal) 0 );
   XtVaSetValues( self->tipShell, XtNallowShellResize, TRUE, NULL );

   self->tipFrame = XtVaCreateManagedWidget(
      "xsc _ Tip Frame",
      xmFrameWidgetClass,
      self->tipShell,
      NULL );

   /*---------------------------------------------------
   -- Core is used as a very light-weight drawing area
   ---------------------------------------------------*/
   self->tipLabel = XtVaCreateManagedWidget(
      "xsc _ Tip Label",
      coreWidgetClass,
      self->tipFrame,
      XtNborderWidth,        0,
      NULL );

   /*----------------------------------------------------------------
   -- The library needs to know when the tip is exposed in order to
   -- render the text
   ----------------------------------------------------------------*/
   XtAddEventHandler( 
      self->tipLabel, 
      ExposureMask, 
      False, 
      _renderTipEH,
      NULL );
      
   /*----------------------------------------------------
   -- Watch for when the screen is no longer being used
   ----------------------------------------------------*/
   XtAddCallback( 
      XmGetXmScreen( x_screen ), XmNdestroyCallback, _destroyScreenCB, NULL );

  /*----------------------
   -- Save data structure
   ----------------------*/
   XSaveContext( 
      x_display, (XID)x_screen, _screenContextId, (XPointer) self );   

   return self;
}

/*------------------------------------------------------------------------------
-- This function retrieves the screen structure associated with an object
-- from the context manager
------------------------------------------------------------------------------*/
XscScreen _XscScreenDeriveFromWidget( Widget obj )
{
   XscScreen self;
   Display*  x_display;
   Screen*   x_screen;
   XPointer  data;
   int       not_found;
   
   x_display = XtDisplayOfObject( obj );
   x_screen  = XtScreenOfObject ( obj );
   
   not_found = XFindContext( x_display,(XID)x_screen,_screenContextId,&data );

   if (not_found)
   {
      self = NULL;
   }
   else
   {
      self = (XscScreen) data;
   }
   return self;
}


Widget _XscScreenGetCueShell( XscScreen self ) { return self->cueShell; }
Widget _XscScreenGetCueFrame( XscScreen self ) { return self->cueFrame; }
Widget _XscScreenGetCueLabel( XscScreen self ) { return self->cueLabel; }
GC     _XscScreenGetGC      ( XscScreen self ) { return self->gc;       }
Widget _XscScreenGetTipShell( XscScreen self ) { return self->tipShell; }
Widget _XscScreenGetTipFrame( XscScreen self ) { return self->tipFrame; }
Widget _XscScreenGetTipLabel( XscScreen self ) { return self->tipLabel; }


void _XscScreenPopdownCue( XscScreen self )
{
   XtPopdown( self->cueShell );
}

void _XscScreenPopupCue( XscScreen self )
{
   XtPopup( self->cueShell, XtGrabNone );
}


void _XscScreenPopdownTip( XscScreen self )
{
   XtPopdown( self->tipShell );
}

void _XscScreenPopupTip( XscScreen self )
{
   XtPopup( self->tipShell, XtGrabNone );
}


void _XscScreenRedrawCue( XscScreen self )
{
   if (XtIsRealized( self->cueShell ))
   {
      Display* x_display = XtDisplay( self->cueShell );

      XClearArea( x_display, XtWindow( self->cueShell ), 0, 0, 0, 0, True );
      XClearArea( x_display, XtWindow( self->cueFrame ), 0, 0, 0, 0, True );
      XClearArea( x_display, XtWindow( self->cueLabel ), 0, 0, 0, 0, True );
      
      XFlush( x_display );
   }
}

void _XscScreenRedrawTip( XscScreen self )
{
   if (XtIsRealized( self->tipShell ))
   {
      Display* x_display = XtDisplay( self->tipShell );

      XClearArea( x_display, XtWindow( self->tipShell ), 0, 0, 0, 0, True );
      XClearArea( x_display, XtWindow( self->tipFrame ), 0, 0, 0, 0, True );
      XClearArea( x_display, XtWindow( self->tipLabel ), 0, 0, 0, 0, True );
      
      XFlush( x_display );
   }
}


/*==============================================================================
                               Private functions
==============================================================================*/
/*------------------------------------------------------------------------------
-- This callback is called to clean-up the screen structure when the last
-- widget on that screen is destroyed
------------------------------------------------------------------------------*/
static void _destroyScreenCB( Widget w, XtPointer cd, XtPointer cbd )
{
   XscScreen screen = _XscScreenDeriveFromWidget( w );
   
   if (screen)
   {
      /*----------------------------------
      -- Don't forget to clean up the GC
      ----------------------------------*/
      XFreeGC( XtDisplayOfObject( w ), screen->gc );
   
      /*--------------------------------------------------------------
      -- Remove the structure from the context manager and delete it
      --------------------------------------------------------------*/
      XDeleteContext( 
         XtDisplayOfObject( w ), (XID) screen->xScreen, _screenContextId );
         
      XtFree( (char*) screen );
   }
}


/*------------------------------------------------------------------------------
-- This event handler is called when the cue needs to be rendered
------------------------------------------------------------------------------*/
static void _renderCueEH( 
   Widget     cue_widget, 
   XtPointer  not_used1, 
   XEvent*    event, 
   Boolean*   not_used2 )
{
   XscDisplay display = _XscDisplayDeriveFromWidget( cue_widget );
   XscCue     cue;
      
   assert( display );
   
   cue = _XscDisplayGetActiveCue( display );
   if ((event->type == Expose) && (cue))
   {
      XscScreen screen = _XscScreenDeriveFromWidget( cue_widget );
      
      _XscCueRender( cue, cue_widget, screen->gc );
   }
}


/*------------------------------------------------------------------------------
-- This event handler is called when the tip needs to be rendered
------------------------------------------------------------------------------*/
static void _renderTipEH( 
   Widget     tip_widget, 
   XtPointer  not_used1, 
   XEvent*    event, 
   Boolean*   not_used2 )
{
   XscDisplay   display = _XscDisplayDeriveFromWidget( tip_widget );
   XscObject object;
      
   assert( display );
   
   object = _XscDisplayGetActiveTip( display );
   if ((event->type == Expose) && (object))
   {
      XscScreen screen = _XscScreenDeriveFromWidget( tip_widget );
      
      _XscTipRender( _XscObjectGetTip( object ), tip_widget, screen->gc );
   }
}

