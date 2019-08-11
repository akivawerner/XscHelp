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

#include <Xsc/Help.h>

#include "CueP.h"
#include "DisplayP.h"
#include "HintP.h"
#include "ScreenP.h"
#include "ShellP.h"

/*==============================================================================
                               Private data types
==============================================================================*/
/*----------------------------------------------------------------------
-- This structure defines the attributes associated with each WM shell
----------------------------------------------------------------------*/
typedef struct _XscShellRec
{
   Widget       shell;
   Widget       hintWidget;      /* Reference to hint display widget */
   Pixel        hintBackground;  /* Default background for the hint widget */
   XtIntervalId hintUnmapTimer;  /* How long with no hints before unmapped? */
   Boolean      hintIsMapped;    /* Is the hint widget mapped? */
   Boolean      inFocus;
   Boolean      tipsEnabled;
   Boolean      cuesEnabled;
   Boolean      hintsEnabled;
}
XscShellRec;

/*==============================================================================
                            Private prototypes
==============================================================================*/
static void _destroyShellCB( Widget, XtPointer, XtPointer );

static void _hintUnmapTO( XtPointer, XtIntervalId* );

static void _renderHintEH( Widget, XtPointer, XEvent*, Boolean* );
static void _trackShellEH( Widget, XtPointer, XEvent*, Boolean* );


/*==============================================================================
                           Static global variables
==============================================================================*/
/*-----------------------------------------------------------------------
-- The Help ToolKit data structures are managed by the X Context Manager.  
-- These are the context identifiers used to llok up a data structure.
-----------------------------------------------------------------------*/
static XContext _shellContextId;

/*==============================================================================
                               Protected functions
==============================================================================*/
/*-------------------------------------------------
-- This function is used to initialize the module
-------------------------------------------------*/
void _XscShellInitialize( XContext context )
{
   if (!_shellContextId)
   {
      if (context)
      {
         _shellContextId = context;
      }
      else
      {
         _shellContextId = XUniqueContext();
      }
   }
}


/*------------------------------------------------------------------
-- This function creates and bind an Htk shell object to the shell
-- associated with the specified widget
------------------------------------------------------------------*/
XscShell _XscShellCreate( Widget obj )
{
   XscShell self;
   Widget   shell_widget = obj;

   /*-------------------------------------------------------
   -- The nearest shell ancestor of the specified widget
   -- is needed
   -------------------------------------------------------*/
   while (!XtIsWMShell( shell_widget ))
   {
      shell_widget = XtParent( shell_widget );
   }

   /*---------------------------------------------------------------
   -- Create and save the data structure associated with the shell
   ---------------------------------------------------------------*/
   self = XtNew( XscShellRec );
   self->shell           = shell_widget;
   self->inFocus         = False;
   self->hintBackground  = 0;
   self->hintUnmapTimer  = (XtIntervalId) NULL;
   self->hintIsMapped    = False;
   self->hintWidget      = NULL;
   self->tipsEnabled     = True;
   self->hintsEnabled    = True;
   self->cuesEnabled     = True;

   /*-------------------------------------------------------
   -- Add a callback to clean-up if the shell is destroyed
   -------------------------------------------------------*/
   XtAddCallback( shell_widget, XmNdestroyCallback, _destroyShellCB, NULL );
   
   XtAddEventHandler( 
      shell_widget, 
      FocusChangeMask | LeaveWindowMask | StructureNotifyMask, 
      False, 
      _trackShellEH, 
      self );
      
   /*----------------------
   -- Save data structure
   ----------------------*/
   XSaveContext( 
      XtDisplayOfObject( obj ), 
      (XID) shell_widget, 
      _shellContextId, 
      (XPointer) self );

   return self;
}


/*------------------------------------------------------------------------------
-- This function retrieves the shell structure associated with an object
-- from the context manager
------------------------------------------------------------------------------*/
XscShell _XscShellDeriveFromWidget( Widget obj )
{
   XscShell self;
   Display* x_display;
   XPointer data;
   int      not_found;
   Widget   shell_widget;
   
   shell_widget = obj;
   while (!XtIsWMShell( shell_widget ))
   {
      shell_widget = XtParent( shell_widget );
   }
   
   x_display = XtDisplayOfObject( obj );
   
   not_found = XFindContext( 
      x_display, (XID)shell_widget, _shellContextId, &data );

   if (not_found)
   {
      self = NULL;
   }
   else
   {
      self = (XscShell) data;
   }
   return self;
}


/*--------------------------------------------------
-- This function sets up the shell to process Hints
---------------------------------------------------*/
void _XscShellInstallHint( XscShell self, Widget w )
{
   if (self->hintWidget == w) return;
   
   if (self->hintWidget)
   {
      /*-------------------------------------------------------------
      -- Remove the handler for the old hint widget 
      -------------------------------------------------------------*/
      XtRemoveEventHandler( 
         self->hintWidget, 
         ExposureMask, 
         False, 
         _renderHintEH, 
         self );

      if (XtIsRealized( self->hintWidget ))
      {
         XtUnmapWidget( self->hintWidget );
      }
   }
   
   /*--------------------------------------------
   -- Install the specified hint display widget
   --------------------------------------------*/
   self->hintWidget = w;

   XtAddEventHandler( w, ExposureMask, False, _renderHintEH, self );
   XtSetMappedWhenManaged( w, False );
   if (XtIsRealized( w ))
   {
      XtUnmapWidget( w );
   }
}


Pixel  _XscShellGetBackground( XscShell self )
{
   return self->hintBackground;
}


Widget _XscShellGetHintWidget( XscShell self )
{
   return self->hintWidget;
}


void _XscShellSetBackground( XscShell self, Pixel background )
{
   self->hintBackground = background;
}


void _XscShellEraseHint( XscShell self )
{
   /*------------------------------------------------------------
   -- Set the timer to unmap the hint.  The hint could be
   -- unmapped right away; however, this would lead to flashing
   -- and blinking if the pointer was quickly moved from object
   -- to object (e.g., in a menu).  By delaying the unmap,
   -- it can sometimes be avoided all together.
   ------------------------------------------------------------*/
   if (self->hintWidget)
   {
      self->hintUnmapTimer = XtAppAddTimeOut(
         XtWidgetToApplicationContext( self->hintWidget ),
         100,
         _hintUnmapTO,
         (XtPointer) self );
   }
}


void _XscShellDisplayHint( XscShell self )
{
   if (self->hintUnmapTimer)
   {
      XtRemoveTimeOut( self->hintUnmapTimer );
      self->hintUnmapTimer = (XtIntervalId) NULL;

      if (XtIsRealized( self->hintWidget ))
      {
         XClearArea( 
            XtDisplay( self->hintWidget ),
            XtWindow(  self->hintWidget ),
            0, 0, 0, 0,
            True );
         XFlush( XtDisplay( self->hintWidget ) );
      }
   }
   else
   {
      /*-----------------------------------
      -- Map the widget if it is unmapped
      -----------------------------------*/
      XtMapWidget( self->hintWidget );
      self->hintIsMapped = True;
   }
}


void _XscShellUpdateHint( XscShell self )
{
      /*------------------------------------------------------
      -- Determine what the hint's backgrond color should be
      ------------------------------------------------------*/
      Pixel      background;
      Widget     hint_widget = _XscShellGetHintWidget( self );
      XscDisplay display     = _XscDisplayDeriveFromWidget( hint_widget );
      XscObject  object      = _XscDisplayGetActiveHint( display );
      XscHint    hint        = _XscObjectGetHint( object );


      if (XscHintGetInheritBackground( hint ))
      {
         XtVaGetValues( 
            XtParent( hint_widget ),
            XmNbackground,     &background,
            NULL );
      }
      else
      {
         background = XscHintGetBackground( hint );
      }

      /*-------------------------------------------------------
      -- Store this with the shell so that it can be accessed
      -- when the hint is actually rendered.  It is needed
      -- in some cases to compute the forground color.
      -------------------------------------------------------*/
      _XscShellSetBackground( self, background );

      /*--------------------------------------------------------
      -- Set the background color now to avoid flashing if the
      -- hint display widget is currently unmapped
      --------------------------------------------------------*/
      XtVaSetValues(
         hint_widget,
         XmNbackground,        background,
         NULL );

      if (XtIsRealized( hint_widget ))
      {
         XClearArea( 
            XtDisplay( hint_widget ),
            XtWindow(  hint_widget ),
            0, 0, 0, 0,
            True );
         XFlush( XtDisplay( hint_widget ) );
      }
}

Boolean _XscShellIsCueDisplayable( XscShell self )
{
   return self->cuesEnabled && XscHelpAreCuesEnabledGlobally();
}
Boolean _XscShellIsHintDisplayable( XscShell self )
{
   return self->hintsEnabled && XscHelpAreHintsEnabledGlobally();
}
Boolean _XscShellIsTipDisplayable( XscShell self )
{
   return self->tipsEnabled && XscHelpAreTipsEnabledGlobally();
}

Boolean _XscShellIsCueEnabled( XscShell self )
{
   return self->cuesEnabled;
}
Boolean _XscShellIsHintEnabled( XscShell self )
{
   return self->hintsEnabled;
}
Boolean _XscShellIsTipEnabled( XscShell self )
{
   return self->tipsEnabled;
}

void _XscShellSetCueEnabled( XscShell self, Boolean flag )
{
   self->cuesEnabled = flag;
}
void _XscShellSetHintEnabled( XscShell self, Boolean flag )
{
   self->hintsEnabled = flag;
}
void _XscShellSetTipEnabled( XscShell self, Boolean flag )
{
   self->tipsEnabled = flag;
}


/*==============================================================================
                               Private functions
==============================================================================*/

/*------------------------------------------------------------------------------
-- This callback is called to clean-up the shell structure when a given
-- shell is destroyed
------------------------------------------------------------------------------*/
static void _destroyShellCB( Widget shell, XtPointer cd, XtPointer cbd )
{
   XscShell self = _XscShellDeriveFromWidget( shell );
   
   assert( XtIsShell( shell ) );
   
   if (self)
   {
      /*--------------------------------------------------------------
      -- Remove the structure from the context manager and delete it
      --------------------------------------------------------------*/
      XDeleteContext( XtDisplay( shell ), (XID)shell, _shellContextId );
         
      XtFree( (char*) self );
   }
   
   XtRemoveEventHandler( 
      shell, 
      FocusChangeMask | LeaveWindowMask | StructureNotifyMask, 
      False, 
      _trackShellEH, 
      self );
}


/*------------------------------------------------------------------------------
-- This timer is called when it is time to unmap the hint display widget
------------------------------------------------------------------------------*/
static void _hintUnmapTO( XtPointer cd, XtIntervalId* not_used )
{
   XscShell self = (XscShell) cd;
   
   self->hintUnmapTimer = (XtIntervalId) NULL;
   
   XtUnmapWidget( self->hintWidget );
   self->hintIsMapped = False;
}


/*------------------------------------------------------------------------------
-- This event handler is called when the hint needs to be rendered
------------------------------------------------------------------------------*/
static void _renderHintEH( 
   Widget     hint_widget, 
   XtPointer  shell, 
   XEvent*    event, 
   Boolean*   not_used2 )
{
   XscShell     self   = (XscShell) shell;
   XscDisplay   display = _XscDisplayDeriveFromWidget( hint_widget );
   XscObject object;
   
   assert( display );
   
   object = _XscDisplayGetActiveHint( display );
   if ((event->type == Expose) && (object))
   {
      XscScreen screen = _XscScreenDeriveFromWidget( hint_widget );
      GC        gc     = _XscScreenGetGC( screen );
      XscHint   hint   = _XscObjectGetHint( object );
      
      /*--------------------------
      -- This keeps the GC happy
      --------------------------*/
      XSetFont( XtDisplay( hint_widget ), gc, _XscObjectGetFont( object ) );
      
      _XscHintRender( hint, hint_widget, gc, self->hintBackground );
   }
}


static void _trackShellEH( 
   Widget     shell, 
   XtPointer  _xsc_shell, 
   XEvent*    event, 
   Boolean*   not_used2 )
{
   XscShell xsc_shell = (XscShell) _xsc_shell;
   
   switch( event->type )
   {
   case FocusIn:
      xsc_shell->inFocus = True;
      break;
      
   case FocusOut:
      xsc_shell->inFocus = False;
      break;
      
   case ConfigureNotify:
      if (xsc_shell->inFocus)
      {
         XscDisplay xsc_display = _XscDisplayDeriveFromWidget( shell );
         XscCue     xsc_cue     = _XscDisplayGetActiveCue( xsc_display );

         if (xsc_cue)
         {
            _XscCuePopup( xsc_cue );
            
         }
      }
      break;
   
   case LeaveNotify:
      {
         XscDisplay xsc_display = _XscDisplayDeriveFromWidget( shell );
         _XscDisplayPrimeTipRestore( xsc_display );
      }
      break;
   }
}
