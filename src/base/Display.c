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

#include <Xm/Display.h>

#include <Xsc/Help.h>

#include "CueP.h"
#include "DisplayP.h"
#include "ObjectP.h"
#include "ScreenP.h"
#include "TipP.h"

/*==============================================================================
                               Private data types
==============================================================================*/
/*---------------------------------------------------------------------
-- This structure defines the attributes associated with each display
---------------------------------------------------------------------*/
typedef struct _XscDisplayRec
{
   Display*      xDisplay;
   XtAppContext  appContext;
   XscCue        cueActive;
   XscObject     hintActive;       /* What hint/tip object is active? */
   XscObject     tipActive;
   XscObject     tipSelectedName;
   XtIntervalId  tipTimerCancelRestore;
   XtIntervalId  tipTimerPopup;
   XtIntervalId  tipTimerPopdown;
   XtIntervalId  tipTimerSelectName;
   int           tipActiveGroupId;
   int           tipRestoreGroupId;
   Boolean       tipRestorePrimed;
}
XscDisplayRec;

/*==============================================================================
                            Private prototypes
==============================================================================*/
static void _destroyDisplayCB( Widget, XtPointer, XtPointer );

static void _cancelRestoreTO( XtPointer, XtIntervalId* );
static void _popdownTipTO   ( XtPointer, XtIntervalId* );
static void _popupTipTO     ( XtPointer, XtIntervalId* );
static void _selectNameTipTO( XtPointer, XtIntervalId* );


/*==============================================================================
                           Static global variables
==============================================================================*/
/*-----------------------------------------------------------------------
-- The Help ToolKit data structures are managed by the X Context Manager.  
-- These are the context identifiers used to look up a data structure.
-----------------------------------------------------------------------*/
static XContext _displayContextId;



/*==============================================================================
                            Protected functions
==============================================================================*/
/*-------------------------------------------------
-- This function is used to initialize the module
-------------------------------------------------*/
void _XscDisplayInitialize( XContext context )
{
   if (!_displayContextId)
   {
      if (context)
      {
         _displayContextId = context;
      }
      else
      {
         _displayContextId = XUniqueContext();
      }
   }
}


/*------------------------------------------------------------------
-- This function creates and bind an Htk shell object to the shell
-- associated with the specified widget
------------------------------------------------------------------*/
XscDisplay _XscDisplayCreate( Widget obj )
{
   XscDisplay self;
   
   /*-------------------------------------
   -- Make a Help ToolKit display record
   -------------------------------------*/
   self = XtNew( XscDisplayRec );

   /*----------------------------
   -- Initialize data structure
   ----------------------------*/
   self->xDisplay              = XtDisplayOfObject( obj );
   self->appContext            = XtWidgetToApplicationContext( obj );
   self->hintActive            = NULL;
   self->tipActive             = NULL;
   self->tipSelectedName       = NULL;
   self->cueActive             = NULL;
   self->tipTimerCancelRestore = (XtIntervalId) NULL;
   self->tipTimerPopup         = (XtIntervalId) NULL;
   self->tipTimerPopdown       = (XtIntervalId) NULL;
   self->tipTimerSelectName    = (XtIntervalId) NULL;
   self->tipActiveGroupId      = XmXSC_TIP_GROUP_NULL;
   self->tipRestoreGroupId     = self->tipActiveGroupId;
   self->tipRestorePrimed      = False;

   /*-----------------------------------------------------
   -- Watch for when the display is no longer being used
   -----------------------------------------------------*/
   XtAddCallback( 
      XmGetXmDisplay( self->xDisplay ), 
      XmNdestroyCallback, 
      _destroyDisplayCB, 
      NULL );

   /*----------------------
   -- Save data structure
   ----------------------*/
   XSaveContext( 
      self->xDisplay, 
      (XID) self->xDisplay, 
      _displayContextId, 
      (XPointer) self );
      
   return self;
}

/*------------------------------------------------------------------------------
-- This function retrieves the display structure associated with an object
-- from the context manager
------------------------------------------------------------------------------*/
XscDisplay _XscDisplayDeriveFromWidget( Widget obj )
{
   XscDisplay self;
   Display*   x_display;
   XPointer   data;
   int        not_found;
   
   x_display = XtDisplayOfObject( obj );
   not_found = XFindContext( x_display,(XID)x_display,_displayContextId,&data );

   if (not_found)
   {
      self = NULL;
   }
   else
   {
      self = (XscDisplay) data;
   }
   return self;
}



/*--------------------------------------------------------------------------
-- This function indicates if the specified id matches the active group id
--------------------------------------------------------------------------*/
Boolean _XscDisplayActiveTipGroupIdEquals( XscDisplay self, int id )
{
  return self->tipActiveGroupId == id;
}


/*-----------------------------------------------
-- This function cancel the group cancel timer
-----------------------------------------------*/
void _XscDisplayCancelTimerTipPopdown( XscDisplay self )
{
   if (self->tipTimerPopdown)
   {
      XtRemoveTimeOut( self->tipTimerPopdown );
      self->tipTimerPopdown = (XtIntervalId) NULL;
   }
}

/*-----------------------------------------------
-- This function cancel the group cancel timer
-----------------------------------------------*/
void _XscDisplayCancelTimerTipPopup( XscDisplay self )
{
   if (self->tipTimerPopup)
   {
      XtRemoveTimeOut( self->tipTimerPopup );
      self->tipTimerPopup = (XtIntervalId) NULL;
   }
}


void _XscDisplayCancelTimerSelectName( XscDisplay self )
{
   if (self->tipTimerSelectName)
   {
      XtRemoveTimeOut( self->tipTimerSelectName );
      self->tipTimerSelectName = (XtIntervalId) NULL;
   }
}


/*--------------------------------------------------------------
-- If the group id is different than the recorded id, then
-- cancel the group effect.
--------------------------------------------------------------*/
void _XscDisplayCheckForTipGroupIdMatch( XscDisplay self, int id )
{
   if (self->tipRestorePrimed)
   {
      if (self->tipRestoreGroupId == id)
      {
         self->tipActiveGroupId = id;
         self->tipRestorePrimed = False;

         if (self->tipTimerCancelRestore)
         {
            XtRemoveTimeOut( self->tipTimerCancelRestore );
            self->tipTimerCancelRestore = (XtIntervalId) NULL;
         }
      }
   }
   else if (self->tipActiveGroupId != id)
   {
      self->tipActiveGroupId = XmXSC_TIP_GROUP_NULL;
   }
}

/*-------------------------------------------------
-- This function retrieves the active cue object
-------------------------------------------------*/
XscCue _XscDisplayGetActiveCue( XscDisplay self )
{
   return self->cueActive;
}

/*-------------------------------------------------
-- This function retrieves the active hint object
-------------------------------------------------*/
XscObject _XscDisplayGetActiveHint( XscDisplay self )
{
   return self->hintActive;
}

/*-------------------------------------------------
-- This function retrieves the active tip object
-------------------------------------------------*/
XscObject _XscDisplayGetActiveTip( XscDisplay self )
{
   return self->tipActive;
}

XscObject _XscDisplayGetSelectedTip( XscDisplay self )
{
   return self->tipSelectedName;
}

/*-----------------------------------------------------------------------
-- This function is called to notify the display that the Tip is lost
-- (due to being touched perhaps) and may need to be restored.
-----------------------------------------------------------------------*/
void _XscDisplayPrimeTipRestore( XscDisplay self )
{
   self->tipRestorePrimed = True;

   self->tipTimerCancelRestore = XtAppAddTimeOut(
      self->appContext,
      100,
      _cancelRestoreTO,
      (XtPointer) self );
}

/*------------------------------------
-- This function sets the active Cue
-------------------------------------*/
void _XscDisplaySetActiveCue( XscDisplay self, XscCue cue )
{
   if (self->cueActive)
   {
      /* Lose it */
   }
   self->cueActive = cue;
}


/*------------------------------------
-- This function sets the active Hint
-------------------------------------*/
void _XscDisplaySetActiveHint( XscDisplay self, XscObject object )
{
   if (self->hintActive)
   {
      /* Lose it */
   }
   self->hintActive = object;
}


/*------------------------------------
-- This function sets the active Hint
-------------------------------------*/
void _XscDisplaySetActiveTip( XscDisplay self, XscObject object )
{
   if (self->tipActive)
   {
      /* Lose it */
   }
   self->tipActive = object;
}


void _XscDisplaySetTipActiveGroupId( XscDisplay self, int id )
{
   self->tipRestoreGroupId = self->tipActiveGroupId = id;
}



void _XscDisplayStartTimerTipPopdown( XscDisplay self, XscObject object )
{
   if (self->tipTimerPopdown)
   {
      XtRemoveTimeOut( self->tipTimerPopdown );
   }

   self->tipTimerPopdown = XtAppAddTimeOut(
      self->appContext,
      XscTipGetPopdownInterval( _XscObjectGetTip( object ) ),
      _popdownTipTO,
      (XtPointer) object );
}


void _XscDisplayStartTimerTipPopup( XscDisplay self, XscObject object )
{
   /*------------------------------------------------------
   -- The first entered object has a different tip group,
   -- so clear out the active tip group value.
   ------------------------------------------------------*/
   self->tipActiveGroupId = XmXSC_TIP_GROUP_NULL;

   /*---------------------------------------------------------
   -- Start a new timer to see if the tip should be rendered
   -- for this object
   ---------------------------------------------------------*/
   if (self->tipTimerPopup)
   {
      XtRemoveTimeOut( self->tipTimerPopup );
   }

   self->tipTimerPopup = XtAppAddTimeOut(
      self->appContext,
      XscTipGetPopupInterval( _XscObjectGetTip( object ) ),
      _popupTipTO,
      (XtPointer) object );
}

void _XscDisplayStartTimerSelectName( XscDisplay self, XscObject object )
{
   unsigned int _interval;
   
   if (self->tipTimerSelectName)
   {
      XtRemoveTimeOut( self->tipTimerSelectName );
   }

   _interval = XscTipGetSelectNameInterval( _XscObjectGetTip( object ) );
   
   if (_interval != (unsigned long) -1)
   {
      self->tipTimerSelectName = XtAppAddTimeOut(
	 self->appContext,
	 _interval,
	 _selectNameTipTO,
	 (XtPointer) object );
   }
}



/*==============================================================================
                               Private functions
==============================================================================*/

/*------------------------------------------------------------------------------
-- This callback is called to clean-up the display structure when a 
-- display connection is closed
------------------------------------------------------------------------------*/
static void _destroyDisplayCB( Widget w, XtPointer cd, XtPointer cbd )
{
   XscDisplay display = _XscDisplayDeriveFromWidget( w );
   
   if (display)
   {
      /*--------------------------------------------------------------
      -- Remove the structure from the context manager and delete it
      --------------------------------------------------------------*/
      XDeleteContext( 
         display->xDisplay, (XID)display->xDisplay, _displayContextId );
         
      XtFree( (char*) display );
   }
}


/*------------------------------------------------------------------------------
-- This timer is called when the fast map interval for tips has expired
------------------------------------------------------------------------------*/
static void _cancelRestoreTO( XtPointer cd, XtIntervalId* not_used )
{
   XscDisplay self = (XscDisplay) cd;
   
   self->tipTimerCancelRestore = (XtIntervalId) NULL;

   self->tipRestoreGroupId = self->tipActiveGroupId = XmXSC_TIP_GROUP_NULL;
   self->tipRestorePrimed  = False;
}


/*------------------------------------------------------------------------------
-- This timer is called when it is time for a tip to be automatically 
-- popped-down
------------------------------------------------------------------------------*/
static void _popdownTipTO( XtPointer cd, XtIntervalId* not_used )
{
   XscObject    xsc_object    = (XscObject) cd;
   Widget       object_widget = _XscObjectGetWidget( xsc_object );
   XscDisplay   self          = _XscDisplayDeriveFromWidget( object_widget );

   assert( self );
   
   self->tipTimerPopdown = (XtIntervalId) NULL;
   
   self->tipRestoreGroupId = self->tipActiveGroupId = XmXSC_TIP_GROUP_NULL;
   _XscTipPopdown( _XscObjectGetTip( xsc_object ) );
}


/*------------------------------------------------------------------------------
-- This timer is called when it is time for a tip to be automatically 
-- popped-up
------------------------------------------------------------------------------*/
static void _popupTipTO( XtPointer cd, XtIntervalId* not_used )
{
   XscObject  object        = (XscObject) cd;
   Widget     object_widget = _XscObjectGetWidget( object );
   XscDisplay self          = _XscDisplayDeriveFromWidget( object_widget );
   
   assert( self );
   
   self->tipTimerPopup = (XtIntervalId) NULL;

   _XscTipPopup( _XscObjectGetTip( object ) );
}

static void _selectNameTipTO( XtPointer cd, XtIntervalId* not_used )
{
   XscObject    xsc_object    = (XscObject) cd;
   Widget       object_widget = _XscObjectGetWidget( xsc_object );
   XscDisplay   self          = _XscDisplayDeriveFromWidget( object_widget );

   assert( self );
   
   self->tipTimerSelectName = (XtIntervalId) NULL;
   
   self->tipSelectedName = self->tipActive;
   
   _XscTipSelectName( _XscObjectGetTip( xsc_object ) );
}


