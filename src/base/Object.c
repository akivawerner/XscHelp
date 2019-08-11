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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <X11/Intrinsic.h>
#include <X11/IntrinsicP.h>

#include <Xsc/Help.h>
#include <Xsc/Hint.h>
#include <Xsc/StrDefs.h>

#include "CueP.h"
#include "DisplayP.h"
#include "HelpP.h"
#include "HintP.h"
#include "ObjectP.h"
#include "ScreenP.h"
#include "ShellP.h"
#include "TextP.h"
#include "TipP.h"

/*==============================================================================
                               Private data types
==============================================================================*/
/*-----------------------------------------------------------------------
-- This structure tracks data associated with a monitored widget/gadget
-----------------------------------------------------------------------*/
typedef struct _XscObjectRec
{
   Widget       object;
   XFontStruct* font;	      	    /* Font to use if no font list is found */
   int          tipGroupId;
   Boolean      hasGroupLeader;
   Cardinal     gadgetChildCount;   /* How many gadget children are there? */
   Boolean      gadgetProcessing;    /* Should gadgets be monitored? */
   XscObject    activeGadget;
   XscTip       tip;
   XscHint      hint;
   XscCue       cue;
   XscCue       cueSubstitute;
}
XscObjectRec;

XscCue  _XscObjectGetCue       ( XscObject self ) { return self->cue;        }
Font    _XscObjectGetFont      ( XscObject self ) { return self->font->fid;  }
XscTip  _XscObjectGetTip       ( XscObject self ) { return self->tip;        }
XscHint _XscObjectGetHint      ( XscObject self ) { return self->hint;       }
Widget  _XscObjectGetWidget    ( XscObject self ) { return self->object;     }
int     _XscObjectGetTipGroupId( XscObject self ) { return self->tipGroupId; }

void _XscObjectSetTipGroupId( XscObject self, int id ) { self->tipGroupId=id; }

Boolean _XscObjectHasValidCue( XscObject self )
{
   return self->cue && XscCueHasValidTopic( self->cue );
}
Boolean _XscObjectHasValidHint( XscObject self )
{
   return self->hint && XscHintHasValidTopic( self->hint );
}
Boolean _XscObjectHasValidTip( XscObject self )
{
   return self->tip && XscTipHasValidTopic( self->tip );
}

/*==============================================================================
                            Private prototypes
==============================================================================*/
static void _destroyObjectCB     ( Widget, XtPointer, XtPointer );
static void _enterObjectCue      ( XscObject, int );
static void _enterObjectHint     ( XscObject, int );
static void _enterObjectTip      ( XscObject, int );
static void _enterOrLeaveObjectEH( Widget, XtPointer, XEvent*, Boolean* );
static void _focusChangeEH       ( Widget, XtPointer, XEvent*, Boolean* );
static void _leaveObject         ( XscObject, Boolean );
static void _leaveObjectCue      ( XscObject, int );
static void _motionInObjectEH    ( Widget, XtPointer, XEvent*, Boolean* );
static void _setupHint           ( XscObject, const char* );
static void _setupCue            ( XscObject, const char* );
static void _setupObject         ( XscObject );
static void _setupTip            ( XscObject, const char* );
static void _updateCue           ( XscObject, XscDisplay );
static void _updateHint          ( XscObject, XscDisplay );
static void _updateTip           ( XscObject, XscDisplay );


/*==============================================================================
                           Static global variables
==============================================================================*/
/*-----------------------------------------------------------------------
-- The Help ToolKit data structures are managed by the X Context Manager.  
-- These are the context identifiers used to llok up a data structure.
-----------------------------------------------------------------------*/
static XContext _objectContextId;

/*---------------------------------------------------------------------------
-- This variable controls if the tip group default is the original 
-- (default is static XmXSC_TIP_GROUP_PARENT) or the new dynamic
-- style (default is XmXSC_TIP_GROUP_SELF unless an ancestor
-- was not defaulted, in which case, the default is XmXSC_TIP_GROUP_PARENT)
---------------------------------------------------------------------------*/
static Boolean __smartGroupDefault = True;

Boolean XscHelpIsDynamicTipGroupIdDefaultActive() {return __smartGroupDefault;}
void XscHelpSetDynamicTipGroupDefault( Boolean b ) { __smartGroupDefault = b; }

/*==============================================================================
                               Protected functions
==============================================================================*/
/*-------------------------------------------------
-- This function is used to initialize the module
-------------------------------------------------*/
void _XscObjectInitialize( XContext context )
{
   if (!_objectContextId)
   {
      if (context)
      {
         _objectContextId = context;
      }
      else
      {
         _objectContextId = XUniqueContext();
      }
   }
}


/*------------------------------------------------------------------------------
-- Every object needs a light-weight record for generic information
------------------------------------------------------------------------------*/
XscObject _XscObjectCreate( Widget obj )
{
   XscObject self;
   Display*  x_display = XtDisplayOfObject( obj );

   /*------------------------------------
   -- Make a Help ToolKit object record
   ------------------------------------*/
   self = XtNew( XscObjectRec );

   /*----------------------
   -- Initialize 
   ----------------------*/
   self->object           = obj;
   self->gadgetChildCount = 0;
   self->gadgetProcessing = True;
   self->activeGadget     = NULL;
   self->cue              = NULL;
   self->cueSubstitute    = NULL;
   self->hint             = NULL;
   self->tip              = NULL;
   self->tipGroupId       = XmXSC_TIP_GROUP_NULL;
   self->hasGroupLeader   = False;

   /*-------------------------------------------------
   -- Load object members from the resource database
   -------------------------------------------------*/
   _setupObject( self );

   /*----------------------------------------------------------------------
   -- Determine if this object has an associated tip and/or hint.  If so,
   -- create and populate the associated data structures.  The new
   -- structures are bound to the object structure via the hint/tip
   -- members.
   ----------------------------------------------------------------------*/
   _setupCue ( self, NULL );
   _setupHint( self, NULL );
   _setupTip ( self, NULL );

   /*------------------------------------------------------
   -- The library needs to track when the pointer moves 
   -- in and out of widgets even if the widget does not
   -- currently have a hint and tip.  There are several
   -- reasons for this, but here's one:  the widget could
   -- be a Composite that has gadgets.
   ------------------------------------------------------*/
   if (XtIsWidget( obj ))
   {
      XtAddEventHandler( 
         obj, 
         EnterWindowMask | LeaveWindowMask,
         False,
         _enterOrLeaveObjectEH,
         (XtPointer) self );
         
       XtAddEventHandler( 
         obj, 
         FocusChangeMask,
         False,
         _focusChangeEH,
         (XtPointer) self );
  }

   XSaveContext( x_display, (XID)obj, _objectContextId, (XPointer)self );

   /*---------------------------------------------------------------------
   -- All non-shell widgets need a destroy callback for resource cleanup
   ---------------------------------------------------------------------*/
   XtAddCallback( obj, XmNdestroyCallback, _destroyObjectCB, NULL );

   /*--------------------------------------------------------------------
   -- Keep track of the number of gadgets maintained by each composite.
   -- This is used to activate gadget managment only when gadgets are
   -- present.
   --
   -- In some cases, a gadget may be "created" before its parent.  This
   -- will happen when gadgets are used to create a single "compound"
   -- widget.  Therefore, the gadget counting works like this:
   --  a)  When a composite is created, its gadget count is set to zero
   --      and all of its gadget children are counted
   --  b)  When a gadget is created, its parent's gadget count is 
   --      incremented iff it has been installed (has a HtkObject record)
   ---------------------------------------------------------------------*/
   if (XtIsComposite( obj ))
   {
      CompositeWidget mgr = (CompositeWidget) obj;

      int i;
      int original_gadget_count = self->gadgetChildCount;

      self->gadgetChildCount = 0;
      for (i = mgr->composite.num_children - 1; i >= 0; i--)
      {
	 Widget child = mgr->composite.children[ i ];

	 if (child)
	 {
            if (!XtIsWidget( child ))
            {
               if (XtIsRectObj( child ))
               {
                  self->gadgetChildCount += 1;
               }
            }
	 }	 
      }

      /*---------------------------------------------------------
      -- If there are gadgets, install a pointer motion handler
      ---------------------------------------------------------*/
      if ((original_gadget_count==0) && (self->gadgetChildCount))
      {
         XtAddEventHandler( 
	    obj, 
	    PointerMotionMask,
	    False,
	    _motionInObjectEH,
	    (XtPointer) self );
      }
   }
   else if (!XtIsWidget( obj ))
   {
      Widget parent = XtParent( obj );
      XscObject obj_parent = _XscObjectDeriveFromWidget( parent );

      if (obj_parent)
      {
         obj_parent->gadgetChildCount += 1;


         /*---------------------------------------------------------
         -- Install a pointer motion handler when the first gadget
         -- is located
         ---------------------------------------------------------*/
         if (obj_parent->gadgetChildCount == 1)
         {
            XtAddEventHandler( 
	       parent, 
	       PointerMotionMask,
	       False,
	       _motionInObjectEH,
	       (XtPointer) obj_parent );
         }
      }
   }
   return self;
}


/*------------------------------------------------------------------------------
-- This function retrieves the object structure associated with an object
-- from the context manager
------------------------------------------------------------------------------*/
XscObject _XscObjectDeriveFromWidget( Widget obj )
{
   Display*  x_display;
   XscObject self;
   XPointer  data;
   int       not_found;
   
   x_display = XtDisplayOfObject( obj );
   
   not_found = XFindContext( x_display, (XID)obj, _objectContextId, &data );

   if (not_found)
   {
      self = NULL;
   }
   else
   {
      self = (XscObject) data;
   }
   return self;
}


void _XscObjectGadgetFocusIn ( XscObject self )
{
    _enterObjectCue( self, NotifyAncestor );
}

void _XscObjectGadgetFocusOut( XscObject self )
{
   _leaveObjectCue( self, NotifyAncestor );
}


/*==============================================================================
                               Private functions
==============================================================================*/


/*------------------------------------------------------------------------------
-- This callback is called to clean-up the structures associated with an
-- object when its being destroyed
------------------------------------------------------------------------------*/
static void _destroyObjectCB( Widget w, XtPointer cd, XtPointer cbd )
{
   XscObject self = _XscObjectDeriveFromWidget( w );
   
   if (self)
   {
      /*--------------------------------------
      -- If it has a tip structure delete it
      --------------------------------------*/
      if (self->cue)
      {
         _XscCueDestroy( self->cue );
      }
      
      /*--------------------------------------
      -- If it has a tip structure delete it
      --------------------------------------*/
      if (self->tip)
      {
         _XscTipDestroy( self->tip );
      }
      
      /*---------------------------------------
      -- If it has a hint structure delete it
      ---------------------------------------*/
      if (self->hint)
      {
         _XscHintDestroy( self->hint );
      }
      
      /*--------------------------------------------------------------
      -- Remove the structure from the context manager and delete it
      --------------------------------------------------------------*/
      XDeleteContext( 
         XtDisplayOfObject( w ), (XID)self->object, _objectContextId );
         
      XtFree( (char*) self );
   }
   
   /*--------------------------------------------------------------------------
   -- If the object beings destroyed is a gadget, update its parent's record!
   --------------------------------------------------------------------------*/
   if (!XtIsWidget( w ))
   {
      Widget    w_parent   = XtParent( w );
      XscObject obj_parent = _XscObjectDeriveFromWidget( w_parent );
      
      if (obj_parent)
      {
         /*------------------------------------------------------------
         -- If this is the last gadget, then it is safe to remove the
         -- pointer motion event handler
         ------------------------------------------------------------*/
         if (obj_parent->gadgetChildCount == 1)
         {
            XtRemoveEventHandler( 
	       w_parent, 
	       PointerMotionMask,
	       False,
	       _motionInObjectEH,
	       (XtPointer) obj_parent );
         }
         
         /*----------------------------------------
         -- Decrement the parent's gadget counter
         ----------------------------------------*/
         obj_parent->gadgetChildCount -= 1;
      }
   }
}


/*------------------------------------------------------------------------------
-- This function specifies the actions to take when the pointer enters an
-- object that has an associated hint
------------------------------------------------------------------------------*/
static void _enterObjectCue( XscObject self, int detail )
{
   if (detail == NotifyAncestor)
   {
      if (self->cue)
      {
         _XscCueActivate( self->cue );
      }
      else if (!XtIsWMShell( self->object ))
      {
         Widget parent = XtParent( self->object );
         
         for(;;)
         {
            XscObject parent_obj = _XscObjectDeriveFromWidget( parent );
            
            if (parent_obj && parent_obj->cue)
            {
               /*------------------------------------
               -- Thsi is a good Cue to use instead
               ------------------------------------*/
               self->cueSubstitute = parent_obj->cue;
               _XscCueActivate( self->cueSubstitute );
               break;
            }
            
            if (XtIsWMShell( parent )) break;
            
            parent = XtParent( parent );
         }
      }
   }
}


/*------------------------------------------------------------------------------
-- This function specifies the actions to take when the pointer enters an
-- object that has an associated hint
------------------------------------------------------------------------------*/
static void _enterObjectHint( XscObject self, int detail )
{
   XscDisplay display = _XscDisplayDeriveFromWidget( self->object );
   XscObject  prev_hint_object;
   
   assert( display );

   /*--------------------------------------------------------
   -- If the old hint object is still active, check if it is 
   -- flagged as compound (it better be) and keep it
   --------------------------------------------------------*/
   prev_hint_object = _XscDisplayGetActiveHint( display );
   if (prev_hint_object)
   {
      if (XscHintGetCompound( prev_hint_object->hint )) return;

      _leaveObject( prev_hint_object, True );
   }
   
   if (self->hint)
   {
      XscHint hint = self->hint;
      Boolean doit = False;

      switch( detail )
      {
      case NotifyAncestor:         doit = True;                       break;
      case NotifyInferior:         doit = True;                       break;
      case NotifyVirtual:          doit = XscHintGetCompound( hint ); break;
      case NotifyNonlinear:        doit = True;                       break;
      case NotifyNonlinearVirtual: doit = XscHintGetCompound( hint ); break;
      }

      if (doit)
      {
         _XscHintActivate( hint );
      }
   }
}



/*------------------------------------------------------------------------------
-- This function specifies the actions to take when the pointer enters an
-- object that has an associated tip
------------------------------------------------------------------------------*/
static void _enterObjectTip( XscObject self, int detail )
{
   XscDisplay display = _XscDisplayDeriveFromWidget( self->object );
   XscObject  prev_tip_object;

   assert( display );
   
   /*--------------------------------------------------------
   -- If the old tip object is still active, check if it is 
   -- flagged as compound (it better be) and keep it
   --------------------------------------------------------*/
   prev_tip_object = _XscDisplayGetActiveTip( display );
   if (prev_tip_object)
   {
      if (XscTipGetCompound( prev_tip_object->tip )) 
      {
         return;
      }
      _leaveObject( prev_tip_object, True );
   }

   if (self->tip == NULL)
   {
      /*--------------------------------------------------------------
      -- Even though this object has no tip, it may have a group id.
      -- If the group id is different than the recorded id, then
      -- cancel the group effect.
      --------------------------------------------------------------*/
      _XscDisplayCheckForTipGroupIdMatch( display, self->tipGroupId );
   }
   else
   {
      XscTip  tip  = self->tip;
      Boolean doit = False;

      switch( detail )
      {
      case NotifyAncestor:         doit = True;                     break;
      case NotifyInferior:         doit = True;                     break;
      case NotifyVirtual:          doit = XscTipGetCompound( tip ); break;
      case NotifyNonlinear:        doit = True;                     break;
      case NotifyNonlinearVirtual: doit = XscTipGetCompound( tip ); break;
      }

      /*----------------------------------------------------------------------
      -- No need to worry about the group id here since this test is 
      -- concerned about the direction of motion (inside vs. outside) that  
      -- caused this enter event to fire
      ----------------------------------------------------------------------*/
      if (doit)
      {
         _XscTipActivate( tip );
      }
   }
}


/*------------------------------------------------------------------------------
-- This function monitores when the pointer moves into or out of a widget
------------------------------------------------------------------------------*/
static void _enterOrLeaveObjectEH(
   Widget    w,
   XtPointer cd,
   XEvent*   event,
   Boolean*  cont )
{
   XscObject self = (XscObject) cd;
   
   assert( self );

   if (event->type == EnterNotify)
   {
      XCrossingEvent* enter_event = &(event->xcrossing);
      
      /*-------------------------------------------------------------
      -- Always enter the object even if we are logically entering
      -- a gadget.  This matches how the events are sent for widget
      -------------------------------------------------------------*/
      _enterObjectHint( self, enter_event->detail );
      _enterObjectTip ( self, enter_event->detail );
      
      /*-------------------------------------------------------------
      -- Don't bother looking for gadgets in non-manager widget and
      -- ignore gadgets if gadget_processing is false
      -------------------------------------------------------------*/
      if (XtIsComposite( w ) && self->gadgetProcessing)
      {
         /*------------------------------------------------
         -- See if there is a gadget at this x,y location
         ------------------------------------------------*/
         Widget gadget = _XscHelpGetGadgetChild( 
            w, enter_event->x, enter_event->y );
         
         if (gadget)
         {
            if (XtIsSensitive( gadget ))
            {
               XscObject gadget_object = _XscObjectDeriveFromWidget( gadget );
               
               /*----------------------------------------------------------
               -- Make the gadget act like it received the event directly
               ----------------------------------------------------------*/
               if (gadget_object)
               {
                  _enterObjectHint( gadget_object, NotifyAncestor );
                  _enterObjectTip ( gadget_object, NotifyAncestor );
                  
                  /*-------------------------------------------------
                  -- Record which of the object's gadgets is active
                  -------------------------------------------------*/
                  self->activeGadget = gadget_object;
               }
            }
         }
      }
   }
   else if (event->type == LeaveNotify)
   {
      XCrossingEvent* leave_event = &(event->xcrossing);
      
      /*-----------------------------------------------------------------
      -- If there is an active gadget associated with this widget, then
      -- release the gadget; then release this widget
      -----------------------------------------------------------------*/
      if (self->activeGadget)
      {
         _leaveObject( self->activeGadget, True );
         self->activeGadget = NULL;
      }

      _leaveObject( self, leave_event->detail != NotifyInferior );
   }
}


/*------------------------------------------------------------------------------
-- This function monitores when the pointer moves into or out of a widget
------------------------------------------------------------------------------*/
static void _focusChangeEH(
   Widget    w,
   XtPointer cd,
   XEvent*   event,
   Boolean*  cont )
{
   XscObject self = (XscObject) cd;
   
   XFocusChangeEvent* focus = &(event->xfocus);

   if (focus->mode != NotifyNormal) return;
   
   if (event->type == FocusIn)
   {
      /*-------------------------------------------------------------
      -- Always enter the object even if we are logically entering
      -- a gadget.  This matches how the events are sent for widget
      -------------------------------------------------------------*/
      _enterObjectCue( self, focus->detail );
   }
   else if (event->type == FocusOut)
   {
      _leaveObjectCue( self, focus->detail );
   }
}


/*------------------------------------------------------------------------------
-- This function is called whenever the pointer moves off of a screen object.
------------------------------------------------------------------------------*/
static void _leaveObject( XscObject self, Boolean drop_compound )
{
   XscDisplay display = _XscDisplayDeriveFromWidget( self->object );

   /*-----------------------------
   -- If the object has a tip...
   -----------------------------*/
   if (self->tip)
   {
      /*----------------------------------------------------------
      -- If the current tip is "compound", release it _only_ if
      -- the pointer moved to an exterior window
      ----------------------------------------------------------*/
      if (!XscTipGetCompound( self->tip ) || drop_compound)
      {
	 _XscTipPopdown( self->tip );
      }
   }
   
   /*-------------------------------
   -- If there is a hint record...
   -------------------------------*/
   if (self->hint)
   {
      /*-------------------------------------------------
      -- Verify that it is active in the display record
      -------------------------------------------------*/
      if (self == _XscDisplayGetActiveHint( display ))
      {
         /*----------------------------------------------------------
         -- If the current hint is "compound", release it _only_ if
         -- the pointer moved to an exterior window
         ----------------------------------------------------------*/
         if (!XscHintGetCompound( self->hint ) || drop_compound)
         {
            XscShell shell =_XscShellDeriveFromWidget( self->object );

            /*------------------------------------------------------------
            -- Set the timer to unmap the hint.  The hint could be
            -- unmapped right away; however, this would lead to flashing
            -- and blinking if the pointer was quickly moved from object
            -- to object (e.g., in a menu).  By delaying the unmap,
            -- it can sometimes be avoided all together.
            ------------------------------------------------------------*/
            if (shell)
            {
               _XscShellEraseHint( shell );
            }
            _XscDisplaySetActiveHint( display, NULL );
         }
      }
   }
}


static void _leaveObjectCue( XscObject self, int detail )
{
   if (detail == NotifyAncestor)
   {
      /*-------------------------------
      -- If there is a hint record...
      -------------------------------*/
      if (self->cueSubstitute)
      {
         _XscCuePopdown( self->cueSubstitute );
         self->cueSubstitute = NULL;
      }
      else if (self->cue)
      {
         _XscCuePopdown( self->cue );
      }
   }
}


/*------------------------------------------------------------------------------
-- This event handler is called when the pointer is moved within a Composite
-- widget that has one or more gadgets.  This function controls the gadget
-- processing logic.
------------------------------------------------------------------------------*/
static void _motionInObjectEH( 
   Widget    w, 
   XtPointer cd, 
   XEvent*   event, 
   Boolean*  not_used )
{
   Widget    gadget = NULL;
   XscObject self = (XscObject) cd;
   
   assert( self );

   /*----------------------------------------------------------
   -- If gadget processing is not desired, return immediately
   ----------------------------------------------------------*/
   if (!self->gadgetProcessing)
   {
      return;
   }
   
   if (event->type == MotionNotify)
   {
      if (event->xmotion.subwindow != 0) return;
 
      /*--------------------------------------
      -- See if the pointer is over a gadget
      --------------------------------------*/
      gadget = _XscHelpGetGadgetChild( w, event->xmotion.x, event->xmotion.y );

      /*-----------------------------------------
      -- If the pointer is over a gadget and...
      -----------------------------------------*/
      if (gadget)
      {
         /*-------------------------------
      	 -- If there is an active gadget
      	 -------------------------------*/
         if (self->activeGadget)
         {
            /*-------------------------------------------------------------
            -- If the new gadget is different then the active gadget,
            -- then release the active gadget; however, if the pointer is 
            -- still over the active gadget, then just forget about it
            -------------------------------------------------------------*/
            if (gadget != self->activeGadget->object)
            {
               _leaveObject( self->activeGadget, True );
               self->activeGadget = NULL;
            }
            else
            {
               gadget = NULL;
            }
         }

      	 /*------------------------------------------------------------
      	 -- If a new gadget needs to be activated, first make sure it
      	 -- is sensitive.
      	 ------------------------------------------------------------*/
         if (gadget)
         {
            if (XtIsSensitive( gadget ))
            {
               XscObject gadget_object = _XscObjectDeriveFromWidget( gadget );
               
               if (gadget_object)
               {
                  _enterObjectHint( gadget_object, NotifyAncestor );
                  _enterObjectTip ( gadget_object, NotifyAncestor );
                 self->activeGadget = gadget_object;
               }
            }
         }      
      }
      else
      {
      	 /*----------------------------------------------------------------
         -- If the pointer is not over a gadget and if there is a gadget
         -- recorded as active, then the pointer must have just moved off
         -- of it; therfore, it needs to be released and the widget 
         -- itself should be activated.
         ----------------------------------------------------------------*/
         if (self->activeGadget)
         {
            _leaveObject( self->activeGadget, True );
            self->activeGadget = NULL;

            _enterObjectHint( self, NotifyAncestor );
            _enterObjectTip ( self, NotifyAncestor );
         }
      }
   }
}


/*------------------------------------------------------------------------------
-- This function is used to retrieve tip attributes from the resource db
------------------------------------------------------------------------------*/
static void _setupCue( XscObject self, const char* default_topic )
{
   XscText text = _XscTextCreate();
   _XscTextLoadCueResources( text, self->object, default_topic );
   
   /*----------------------------------------------------------------
   -- If no text.str value was found, then, by definition, there is
   -- no cue currently associated with this object.  
   ----------------------------------------------------------------*/
   if (_XscTextStringExists( text ))
   {
      /*--------------------------------------------
      -- If there is no hint record, then make one
      --------------------------------------------*/
      if (self->cue == NULL)
      {
         self->cue = _XscCueCreate( self );
      }
      _XscCueLoadResources( self->cue, text );
   }
   else
   {
      /*-------------------------------------------------------------
      -- If there is an old cue record, destroy it since there are
      -- no cues currently defined in the resource database
      -------------------------------------------------------------*/
      if (self->cue)
      {
         _XscCueDestroy( self->cue );
         self->cue = NULL;
      }
      _XscTextDestroy( text );
   }
}



/*------------------------------------------------------------------------------
-- This function is used to retrieve hint attributes from the resource db
------------------------------------------------------------------------------*/
static void _setupHint( XscObject self, const char* default_topic )
{   
   XscText text = _XscTextCreate();
   _XscTextLoadHintResources( text, self->object, default_topic );
   
   /*----------------------------------------------------------------
   -- If no text.str value was found, then, by definition, there is
   -- no hint currently associated with this object.  
   ----------------------------------------------------------------*/
   if (_XscTextStringExists( text ))
   {
      /*--------------------------------------------
      -- If there is no hint record, then make one
      --------------------------------------------*/
      if (self->hint == NULL)
      {
         self->hint = _XscHintCreate( self );
      }
      _XscHintLoadResources( self->hint, text );
   }
   else
   {
      /*-------------------------------------------------------------
      -- If there is an old hint record, destroy it since there are
      -- no hints currently defined in the resource database
      -------------------------------------------------------------*/
      if (self->hint)
      {
         _XscHintDestroy( self->hint );
         self->hint = NULL;
      }
      _XscTextDestroy( text );
   }
}


/*------------------------------------------------------------------------------
-- This function is used to retrieve object attributes from the resource db
------------------------------------------------------------------------------*/
static void _setupObject( XscObject self )
{
   typedef struct _ObjectResources
   {
      Boolean       gadget_processing;
      int           tip_group_id;
      unsigned char tip_group_override;
      XFontStruct*  font;
   }
   ObjectResources;
   
   ObjectResources object_resources;
   XscObject       _parentObj = NULL;
   Widget          _parent    = NULL;
   
#  define OFFSET_OF( mem ) XtOffsetOf( ObjectResources, mem )
   
   static XtResource resource[] =
   {
      {
         XmNxscFont, XmCXscFont,
         XtRFontStruct, sizeof( XFontStruct* ), OFFSET_OF( font ),
         XtRString, XscHelpDefaultFont
      },{
         XmNxscGadgetProcessing, XmCXscGadgetProcessing,
         XmRBoolean, sizeof( Boolean ), OFFSET_OF( gadget_processing ),
         XtRImmediate, (XtPointer) (Boolean) True
      },{
         XmNxscTipGroupId, XmCXscTipGroupId,
         XmRXscTipGroupId, sizeof( int ), OFFSET_OF( tip_group_id ),
         XtRImmediate, (XtPointer) (int) XmXSC_TIP_GROUP_PARENT
      },{
         XmNxscTipGroupOverride, XmCXscTipGroupOverride,
         XmRXscTipGroup, sizeof( unsigned char ), OFFSET_OF(tip_group_override),
         XtRImmediate, (XtPointer) (unsigned char) XmXSC_TIP_GROUP_NULL
      }
   };
#  undef OFFSET_OF
   
   memset( (char*) &object_resources, '\0', sizeof( ObjectResources ) );
   
   XtGetApplicationResources( 
      self->object, 
      &object_resources, 
      resource,      (Cardinal) XtNumber( resource ),
      NULL,          (Cardinal) 0 );
   
   self->gadgetProcessing = object_resources.gadget_processing;
   self->font             = object_resources.font;
   
   /*---------------------------
   -- This block is deprecated
   ---------------------------*/
   switch ((int) (signed char) object_resources.tip_group_override)
   {
   case XmXSC_TIP_GROUP_SELF:
   case XmXSC_TIP_GROUP_PARENT:
      self->tipGroupId = (int) (signed char)object_resources.tip_group_override;
      break;
      
   default:
      self->tipGroupId = object_resources.tip_group_id;
      break;      
   }
   
   _parent = XtParent( self->object );
   if (_parent) 
   {
      _parentObj = _XscObjectDeriveFromWidget( _parent );
      
      if (_parentObj)
      {
      	 self->hasGroupLeader = _parentObj->hasGroupLeader;
      }
   }
   
   
   if (self->tipGroupId == XmXSC_TIP_GROUP_NULL)
   {
      if (__smartGroupDefault)
      {
	 if (_parentObj && _parentObj->hasGroupLeader)
	 {
	    self->tipGroupId = XmXSC_TIP_GROUP_PARENT;
	 }
	 else
	 {
	    self->tipGroupId = XmXSC_TIP_GROUP_SELF;
	 }
      }
      else
      {
      	 self->tipGroupId = XmXSC_TIP_GROUP_PARENT;
      }
   }
   else
   {
      /*--------------------------------------------------------------------
      -- Once this happens, by default, all children inherit group id from
      -- the parent instead of from self
      --------------------------------------------------------------------*/
      self->hasGroupLeader = True;
   }
   
   if (self->tipGroupId == XmXSC_TIP_GROUP_PARENT)
   {
      if (_parent && !XtIsWMShell( _parent ))
      {
         if (_parentObj)
         {
            self->tipGroupId = _parentObj->tipGroupId;
         }
         else
         {
            self->tipGroupId = - ((int) self->object);
         }
      }
      else
      {
         self->tipGroupId = - ((int) self->object);
      }
   }
   else if (self->tipGroupId == XmXSC_TIP_GROUP_SELF)
   {
      self->tipGroupId = - ((int) self->object);
   }
}


/*------------------------------------------------------------------------------
-- This function is used to retrieve tip attributes from the resource db
------------------------------------------------------------------------------*/
static void _setupTip( XscObject self, const char* default_topic )
{
   XscText text = _XscTextCreate();
   _XscTextLoadTipResources( text, self->object, default_topic );
   
   /*----------------------------------------------------------------
   -- If no text.str value was found, then, by definition, there is
   -- no tip currently associated with this object.  
   ----------------------------------------------------------------*/
   if (_XscTextStringExists( text ))
   {
      /*--------------------------------------------
      -- If there is no hint record, then make one
      --------------------------------------------*/
      if (self->tip == NULL)
      {
         self->tip = _XscTipCreate( self );
      }
      _XscTipLoadResources( self->tip, text );
   }
   else
   {
      /*-------------------------------------------------------------
      -- If there is an old tip record, destroy it since there are
      -- no tips currently defined in the resource database
      -------------------------------------------------------------*/
      if (self->tip)
      {
         _XscTipDestroy( self->tip );
         self->tip = NULL;
      }
      _XscTextDestroy( text );
   }
}


/*-----------------------------------------------
-- This function updates the display of a Tip
-----------------------------------------------*/
static void _updateCue( XscObject self, XscDisplay display )
{
   if (self->cue)
   {
      if (self->cue == _XscDisplayGetActiveCue( display ))
      {
         if (XscCueHasValidTopic( self->cue ))
         {
            XscScreen screen;

            _XscCueUpdate( self->cue );

            screen = _XscScreenDeriveFromWidget( self->object );
            assert( screen );

            _XscScreenRedrawCue( screen );
         }
      }
   }
}


/*-----------------------------------------------
-- This function updates the display of a Hint
-----------------------------------------------*/
static void _updateHint( XscObject self, XscDisplay display )
{
   if (self == _XscDisplayGetActiveHint( display ))
   {
      if (_XscObjectHasValidHint( self ))
      {
         XscShell shell = _XscShellDeriveFromWidget( self->object );
         
         if (shell) _XscShellUpdateHint( shell );
      }
   }
}

/*-----------------------------------------------
-- This function updates the display of a Tip
-----------------------------------------------*/
static void _updateTip( XscObject self, XscDisplay display )
{
   if (self == _XscDisplayGetActiveTip( display ))
   {
      if (_XscObjectHasValidTip( self ))
      {
         XscScreen screen;

         _XscTipUpdate( self->tip );

         screen = _XscScreenDeriveFromWidget( self->object );
         assert( screen );

         _XscScreenRedrawTip( screen );
      }
   }
}


/*==============================================================================
                             Public functions
==============================================================================*/
/*------------------------------------------------------------------------------
-- OBJECT CODE -----------------------------------------------------------------
------------------------------------------------------------------------------*/
Boolean XscHelpGetGadgetProcessing( Widget w )
{
   XscObject object = _XscObjectDeriveFromWidget( w );
   
   if (object)
   {
      return object->gadgetProcessing;
   }
   else
   {
      return False;
   }
}

void XscHelpSetGadgetProcessing( Widget w, Boolean a )
{
   XscObject object = _XscObjectDeriveFromWidget( w );
   
   if (object)
   {
      object->gadgetProcessing = a;
   }
}


void XscHelpSetCueTopic( Widget w, String a )
{
   XscObject object = _XscObjectDeriveFromWidget( w );
   
   if (object)
   {
      if (!object->cue)
      {
         _setupCue( object, a );
      }
      else
      {
         XscCueSetTopic( object->cue, a );
      }
   }
}


void XscHelpSetCueTopicDetails( 
   Widget w, String s, XmFontList fl, String flt, 
   unsigned char a, unsigned char d, unsigned char c )
{
   XscObject object = _XscObjectDeriveFromWidget( w );
   
   if (object)
   {
      XscText text;
      
      if (!object->cue)
      {
         _setupCue( object, s );
         text = _XscCueGetText( object->cue );
      }
      else
      {
         text = _XscCueGetText( object->cue );
         _XscTextSetTopic( text, XtNewString( s ) );
      }
      _XscTextSetFontList   ( text, fl  );
      _XscTextSetFontListTag( text, flt );
      _XscTextSetAlignment  ( text, a   );
      _XscTextSetDirection  ( text, d   );
      _XscTextSetConverter  ( text, c   );

      _XscTextCreateCompoundString( text, object->object );
   }
}


void XscHelpSetHintTopic( Widget w, String a )
{
   XscObject object = _XscObjectDeriveFromWidget( w );
   
   if (object)
   {
      if (!object->hint)
      {
         _setupHint( object, a );
      }
      else
      {
         XscHintSetTopic( object->hint, a );
      }
   }
}


void XscHelpSetHintTopicDetails( 
   Widget w, String s, XmFontList fl, String flt, 
   unsigned char a, unsigned char d, unsigned char c )
{
   XscObject object = _XscObjectDeriveFromWidget( w );
   
   if (object)
   {
      XscText text;
      
      if (!object->hint)
      {
         _setupHint( object, s );
         text = _XscHintGetText( object->hint );
      }
      else
      {
         text = _XscHintGetText( object->hint );
         _XscTextSetTopic( text, XtNewString( s ) );
      }
      _XscTextSetFontList   ( text, fl  );
      _XscTextSetFontListTag( text, flt );
      _XscTextSetAlignment  ( text, a   );
      _XscTextSetDirection  ( text, d   );
      _XscTextSetConverter  ( text, c   );

      _XscTextCreateCompoundString( text, object->object );
   }
}


void XscHelpSetTipTopic( Widget w, String a )
{
   XscObject object = _XscObjectDeriveFromWidget( w );
   
   if (object)
   {
      if (!object->tip)
      {
         _setupTip( object, a );
      }
      else
      {
         XscTipSetTopic( object->tip, a );
      }
   }
}


void XscHelpSetTipTopicDetails( 
   Widget w, String s, XmFontList fl, String flt, 
   unsigned char a, unsigned char d, unsigned char c )
{
   XscObject object = _XscObjectDeriveFromWidget( w );
   
   if (object)
   {
      XscText text;
      
      if (!object->tip)
      {
         _setupTip( object, s );
         text = _XscTipGetText( object->tip );
      }
      else
      {
         text = _XscTipGetText( object->tip );
         _XscTextSetTopic( text, XtNewString( s ) );
      }
      _XscTextSetFontList   ( text, fl  );
      _XscTextSetFontListTag( text, flt );
      _XscTextSetAlignment  ( text, a   );
      _XscTextSetDirection  ( text, d   );
      _XscTextSetConverter  ( text, c   );

      _XscTextCreateCompoundString( text, object->object );
   }
}












/*------------------------------------------------------------------------------
-- This function is used to force an object's tip/hint attributes to be 
-- updated from the respource database
------------------------------------------------------------------------------*/
void XscHelpDbReload( Widget obj )
{
   XscObject object = _XscObjectDeriveFromWidget( obj );
   
   /*----------------------------------------------------------
   -- If the object is known, then just update the attributes
   ----------------------------------------------------------*/
   if (object)
   {
      _setupCue ( object, NULL );
      _setupTip ( object, NULL );
      _setupHint( object, NULL );
   }
   else
   {
      /*------------------------------------------------------------------
      -- Notify the library about this "new" widget.  If it is a gadget,
      -- make sure the library knowns about its parent as well.  Install
      -- a gadget before its parent so the gadget counters stay in sync.
      ------------------------------------------------------------------*/
      _XscHelpInstallOnWidget( obj );

      if (!XtIsWidget( obj ))
      {
         Widget parent = XtParent( obj );
         
         XscObject parent_object = _XscObjectDeriveFromWidget( parent );
         
         if (!parent_object)
         {
            _XscHelpInstallOnWidget( parent );
         }
      }
      
   }
}


/*--------------------------------------------------------------
-- This function updates everything associated with the widget
--------------------------------------------------------------*/
void XscHelpUpdate( Widget obj )
{
   XscObject object = _XscObjectDeriveFromWidget( obj );
   
   if (object)
   {
      XscDisplay display = _XscDisplayDeriveFromWidget( obj );
      _updateTip ( object, display );
      _updateCue ( object, display );
      _updateHint( object, display );
   }
}
 
/*-------------------------------------------
-- This function updates the Cue if active
--------------------------------------------*/
void XscHelpCueUpdate( Widget obj )
{
   XscObject  object  = _XscObjectDeriveFromWidget ( obj );
   
   if (object)
   {
      _updateCue( object, _XscDisplayDeriveFromWidget( obj ) );
   }
}


/*-------------------------------------------
-- This function updates the Hint if active
--------------------------------------------*/
void XscHelpHintUpdate( Widget obj )
{
   XscObject  object  = _XscObjectDeriveFromWidget ( obj );
   
   if (object)
   {
      _updateHint( object, _XscDisplayDeriveFromWidget( obj ) );
   }
}

/*-------------------------------------------
-- This function updates the Tip if active
--------------------------------------------*/
void XscHelpTipUpdate( Widget obj )
{
   XscObject  object  = _XscObjectDeriveFromWidget ( obj );
   
   if (object)
   {
      _updateTip( object, _XscDisplayDeriveFromWidget( obj ) );
   }
}


/*-------------------------------------------------------------------------
-- This function get a handle to a Widget help object for later get/set
-- operations
-------------------------------------------------------------------------*/

Boolean XscHelpCueExists( Widget w )
{
   XscObject object = _XscObjectDeriveFromWidget( w );
   Boolean result = False;
   
   if (object)
   {
      result = _XscObjectHasValidCue( object );
   }
   return result;
}

Boolean XscHelpHintExists( Widget w )
{
   XscObject object = _XscObjectDeriveFromWidget( w );
   Boolean result = False;
   
   if (object)
   {
      result = _XscObjectHasValidHint( object );
   }
   return result;
}

Boolean XscHelpTipExists( Widget w )
{
   XscObject object = _XscObjectDeriveFromWidget( w );
   Boolean result = False;
   
   if (object)
   {
      result = _XscObjectHasValidTip( object );
   }
   return result;
}



/*---------------------------------------------------------------------------
-- This function is a resource converter that converts from a string to
-- a tip group id field.  It will convert numbers as well as the symbolic
-- values XmXSC_TIP_GROUP_NULL, XmXSC_TIP_GROUP_SELF, 
-- and XmXSC_TIP_GROUP_PARENT.
---------------------------------------------------------------------------*/
Boolean XscCvtStringToTipGroupId( 
   Display*   d, 
   XrmValue*  args,  	/* not used */
   Cardinal*  numArgs,  /* not used */
   XrmValue*  from, 
   XrmValue*  toVal,
   XtPointer* data ) /* not used */
{
   char*   str = (char*) from->addr;
   int     value = XmXSC_TIP_GROUP_NULL;
   Boolean result = True;
   
   if (!str) return False;
   
   /*---------------------------------------------------------
   -- If the first character is a digit, assume they all are
   ---------------------------------------------------------*/
   if (isdigit( *str ) || *str == '-' || *str == '+')
   {
      value = atoi( str );
   }
   else
   {
      char* tmp;
      char* ptr;
      
      /*---------------------------------------------------------
      -- The symbol _must_ be at least 18 charcters to be valid
      ---------------------------------------------------------*/
      if (strlen( str ) < 18) return False;
      
      tmp = XtNewString( str );
      
      /*-----------------------------------------------------------------
      -- the string being converted must be a symbol, see if we can
      -- get a match.  The rules are:
      -- 1) If it starts with Xm, then the symbol must be all uppercase
      -- 2) If not, then it can be mixed case
      -----------------------------------------------------------------*/
      if (str[0] == 'X' && str[1] == 'm')
      {
         ptr = &tmp[2];
      }
      else
      {  
      	 /*-----------------------------------------------------------------
      	 -- Convert it to upper case only for symbols not starting with Xm
	 -----------------------------------------------------------------*/
      	 for (ptr = tmp; *ptr; ptr++) *ptr = toupper( *ptr );
      	 ptr = tmp;
      }
      
      if (strcmp( ptr, "XSC_TIP_GROUP_SELF" ) == 0)
      {
      	 value = XmXSC_TIP_GROUP_SELF;
      }
      else if (strcmp( ptr, "XSC_TIP_GROUP_PARENT" ) == 0)
      {
      	 value = XmXSC_TIP_GROUP_PARENT;
      }
      else if (strcmp( ptr, "XSC_TIP_GROUP_NULL" ) == 0)
      {
      	 value = XmXSC_TIP_GROUP_NULL;
      }
      else
      {
      	 result = False;
      }
      XtFree( (void*) tmp );
   }
   
   if (result)
   {
      if (toVal->addr != NULL) 
      {
      	 if (toVal->size < sizeof( int )) 
      	 {
      	    toVal->size = sizeof( int );
      	    return False;
      	 }
      	 *(int*)(toVal->addr) = value;
      }
      else
      {
      	 static int static_val;
      	 static_val = value;
	 toVal->addr = (XtPointer)&static_val;
      }
      toVal->size = sizeof( int );
      return True;		
   }
   
   XtDisplayStringConversionWarning( d, (char*)from->addr, XmRXscTipGroupId );
   return False;
}
