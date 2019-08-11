/*------------------------------------------------------------------------------
--  Help ToolKit for Motif
-- 
--  (c) Copyright 1997-2000, 2019 Robert S. Werner (a.k.a. Akiva Werner)  
--  ALL RIGHTS RESERVED
--
--  License: The MIT License (MIT)
------------------------------------------------------------------------------*/

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <X11/IntrinsicP.h>

#include <Xm/Xm.h>
#include <Xm/AtomMgr.h>

#include <Xsc/Help.h>
#include <Xsc/StrDefs.h>

#include "DisplayP.h"
#include "ObjectP.h"
#include "ShellP.h"
#include "TextP.h"
#include "TipP.h"


/*==============================================================================
                                    Macros
==============================================================================*/
#define XscHelpDefaultTipXOffset        0
#define XscHelpDefaultTipYOffsetDynamic 0xFFFF
#define XscHelpDefaultTipYOffsetOther   0
#define XscHelpDefaultTipYOffsetPointer 15


/*==============================================================================
                               Private data types
==============================================================================*/
/*--------------------------------------------------------------------
-- This structure defines the attributes associated with a given tip 
--------------------------------------------------------------------*/
typedef struct _XscTipRec
{
   XscObject     object;
   XscText       text;
   Dimension     marginTop;         
   Dimension     marginBottom;
   Dimension     marginLeft;
   Dimension     marginRight;
   Dimension     marginHeight;
   Dimension     marginWidth;
   Pixel         background;
   Pixel         foreground;        /* Used if motif_color_model == False */
   Pixel         borderColor;
   Pixel         colorBase;         /* Used if motif_color_model == False */
   Dimension     borderWidth;
   Dimension     shadowThickness;
   unsigned char shadowType;
   unsigned char position;          /* Tip layout policy */
   Boolean       autoDbReload;
   Boolean       enabled;
   Boolean       motifColorModel;
   Boolean       compound;
   unsigned long popdownInterval;
   unsigned long popupInterval;
   unsigned long selectNameInterval;
   int           xOffset;
   int           yOffset;
   int           _rootX;         /* Used to compute the X,Y offset when */
   int           _rootY;         /* in pointer mode --------------------*/
   Boolean       _selected;
}
XscTipRec;


static Boolean _tipConvertNameSelectionCB( 
   Widget         widget, 
   Atom*          selection, 
   Atom*          target,
   Atom*          type_return, 
   XtPointer*     value_return,
   unsigned long* length_return,
   int*           format_return )
{
   static Atom _TARGETS, _TEXT, _LENGTH;
   
   Display* _display = XtDisplay( widget );
   
   if (!_TARGETS) 
   {
      _TARGETS = XmInternAtom( _display, "TARGETS", False );
      _TEXT    = XmInternAtom( _display, "TEXT"   , False );
      _LENGTH  = XmInternAtom( _display, "LENGTH" , False );
   }
   
   if (*target == _TARGETS)
   {
      Atom* _targets;
      int   _length = 0;
      
      *value_return = XtMalloc( sizeof( Atom ) * 3 );
      _targets = *(Atom**) value_return;
      
      *_targets = XA_STRING; _targets++; _length++;
      *_targets =   _TEXT  ; _targets++; _length++;
      *_targets =   _LENGTH; _targets++; _length++;
      
      *length_return = _length;
      *type_return   = XA_ATOM;
      *format_return = 32;
      
      return True;
   }
   else if (*target == _LENGTH)
   {
      long* _temp = (long*) XtMalloc( sizeof( long ) );
      
      XscDisplay _xscDisplay = _XscDisplayDeriveFromWidget( widget );
      XscObject  _xscObject  = _XscDisplayGetSelectedTip( _xscDisplay );
      XscTip     _xscTip     = _XscObjectGetTip         ( _xscObject  );
      
      const char* _text = NULL;
      if (_xscTip && _xscTip->text)
      {
      	 _text = _XscTextGetTextName( _xscTip->text );
      }
      
      if (_text)
      {
      	 *_temp = (long) strlen( _text );
      }
      else
      {
      	 *_temp = 0;
      }
      
      *value_return  = (void*) _temp;
      *type_return   = XA_INTEGER;
      *length_return = 1L;
      *format_return = 32;
      
      return True;
   }
   else if (*target == XA_STRING || *target == _TEXT)
   {
      XscDisplay _xscDisplay = _XscDisplayDeriveFromWidget( widget );
      XscObject  _xscObject  = _XscDisplayGetSelectedTip( _xscDisplay );
      XscTip     _xscTip     = _XscObjectGetTip         ( _xscObject  );
      
      const char* _text;
      if (_xscTip && _xscTip->text)
      {
      	 _text = _XscTextGetTextName( _xscTip->text );
      
	 *type_return   = XA_STRING;
	 *length_return = strlen( _text );
	 *value_return  = XtNewString( _text );
	 *format_return = 8;

	 return True;
      }
   }
   return False;
}

static void _tipLoseNameSelectionCB( Widget w, Atom* selection )
{
   XscDisplay _xscDisplay = _XscDisplayDeriveFromWidget( w );
   XscScreen  _xscScreen  = _XscScreenDeriveFromWidget ( w );
   XscObject  _xscObject  = _XscDisplayGetSelectedTip( _xscDisplay );
   XscTip     _xscTip     = _XscObjectGetTip         ( _xscObject  );
   
   _xscTip->_selected = False;
    _XscScreenRedrawTip( _xscScreen );

}


XscTip _XscTipCreate( XscObject o )
{
   XscTip self = XtNew( XscTipRec );
   
   memset( (void*) self, '\0', sizeof( *self ) );
   self->text   = NULL;
   self->object = o;

   return self;
}

void _XscTipDestroy( XscTip self )
{
   if (self->text) _XscTextDestroy( self->text );
   XtFree( (char*) self );
}


XscTip XscTipDeriveFromWidget( Widget w )
{
   XscObject o = _XscObjectDeriveFromWidget( w );
   if (!o)
   {
      return NULL;
   }
   else
   {
      return _XscObjectGetTip( o );
   }
}


void _XscTipLoadResources( XscTip self, XscText text )
{
#  define OFFSET_OF( mem ) XtOffsetOf( XscTipRec, mem )
   
   static XtResource resource[] =
   {
      {
         XmNxscTipAutoDbReload, XmCXscTipAutoDbReload,
         XmRBoolean, sizeof( Boolean ), OFFSET_OF( autoDbReload ),
         XtRImmediate, (XtPointer) (Boolean) False
      },{
         XmNxscTipBackground, XmCXscTipBackground, 
         XtRPixel, sizeof( Pixel ), OFFSET_OF( background ), 
         XtRString, "Yellow"
      },{
         XmNxscTipBorderColor, XmCXscTipBorderColor,
         XtRPixel, sizeof( Pixel ), OFFSET_OF( borderColor ), 
         XtRString, "Black"
      },{
         XmNxscTipBorderWidth, XmCXscTipBorderWidth,
         XtRDimension, sizeof( Dimension ), OFFSET_OF( borderWidth ), 
         XtRImmediate, (XtPointer) 1
      },{
         XmNxscTipColorBase, XmCXscTipColorBase,
         XmRPixel, sizeof( Pixel ), OFFSET_OF( colorBase ),
         XtRString, "Yellow"
      },{
         XmNxscTipCompound, XmCXscTipCompound,
         XmRBoolean, sizeof( Boolean ), OFFSET_OF( compound ),
         XtRImmediate, (XtPointer) (Boolean) False
      },{
         XmNxscTipEnabled, XmCXscTipEnabled,
         XmRBoolean, sizeof( Boolean ), OFFSET_OF( enabled ),
         XtRImmediate, (XtPointer) (Boolean) True
      },{
         XmNxscTipForeground, XmCXscTipForeground, 
         XtRPixel, sizeof( Pixel ), OFFSET_OF( foreground ), 
         XtRString, "Black"
      },{
         XmNxscTipMarginBottom, XmCXscTipMarginBottom,
         XmRDimension, sizeof( Dimension ), OFFSET_OF( marginBottom ),
         XtRImmediate, (XtPointer) (Dimension) 0
      },{
         XmNxscTipMarginHeight, XmCXscTipMarginHeight,
         XmRDimension, sizeof( Dimension ), OFFSET_OF( marginHeight ),
         XtRImmediate, (XtPointer) (Dimension) 1
      },{
         XmNxscTipMarginLeft, XmCXscTipMarginLeft,
         XmRDimension, sizeof( Dimension ), OFFSET_OF( marginLeft ),
         XtRImmediate, (XtPointer) (Dimension) 0
      },{
         XmNxscTipMarginRight, XmCXscTipMarginRight,
         XmRDimension, sizeof( Dimension ), OFFSET_OF( marginRight ),
         XtRImmediate, (XtPointer) (Dimension) 0
      },{
         XmNxscTipMarginTop, XmCXscTipMarginTop,
         XmRDimension, sizeof( Dimension ), OFFSET_OF( marginTop ),
         XtRImmediate, (XtPointer) (Dimension) 0
      },{
         XmNxscTipMarginWidth, XmCXscTipMarginWidth,
         XmRDimension, sizeof( Dimension ), OFFSET_OF( marginWidth ),
         XtRImmediate, (XtPointer) (Dimension) 2
      },{
         XmNxscTipMotifColorModel, XmCXscTipMotifColorModel,
         XtRBoolean, sizeof( Boolean ), OFFSET_OF( motifColorModel ),
         XtRImmediate, (XtPointer) (Boolean) TRUE
      },{
         XmNxscTipPopdownInterval, XmCXscTipPopdownInterval,
         XmRInt, sizeof( int ), OFFSET_OF( popdownInterval ),
         XtRImmediate, (XtPointer) (int) 0
      },{
         XmNxscTipPopupInterval, XmCXscTipPopupInterval,
         XmRInt, sizeof( int ), OFFSET_OF( popupInterval ),
         XtRImmediate, (XtPointer) (int) 1000
      },{
         XmNxscTipPosition, XmCXscTipPosition,
         XmRXscTipPosition, sizeof( unsigned char ), OFFSET_OF( position ),
         XtRImmediate, (XtPointer) (unsigned char) XmXSC_TIP_POSITION_POINTER
      },{
      	 XmNxscTipSelectNameInterval, XmCXscTipSelectNameInterval,
	 XmRInt, sizeof( int ), OFFSET_OF( selectNameInterval ),
	 XtRImmediate, (XtPointer) (int) -1
      },{
         XmNxscTipShadowThickness, XmCXscTipShadowThickness,
         XtRDimension, sizeof( Dimension ), OFFSET_OF( shadowThickness ), 
         XtRImmediate, (XtPointer) 0
      },{
         XmNxscTipShadowType, XmCXscTipShadowType,
         XmRShadowType, sizeof( unsigned char ), OFFSET_OF( shadowType ),
         XtRImmediate, (XtPointer) (unsigned char) XmSHADOW_OUT
      },{
         XmNxscTipXOffset, XmCXscTipXOffset,
         XmRInt, sizeof( int ), OFFSET_OF( xOffset ),
         XtRImmediate, (XtPointer) (int) XscHelpDefaultTipXOffset
      },{
         XmNxscTipYOffset, XmCXscTipYOffset,
         XmRInt, sizeof( int ), OFFSET_OF( yOffset ),
         XtRImmediate, (XtPointer) (int) XscHelpDefaultTipYOffsetDynamic
      }
   };
#  undef OFFSET_OF

   Widget w = _XscObjectGetWidget( self->object );
   
   XtGetApplicationResources( 
      w,
      self, 
      resource,   (Cardinal) XtNumber( resource ),
      NULL,       (Cardinal) 0 );
   
   if (text)
   {
      /*-------------------------------------------------------------------
      -- If there is an old string in the hint record, then get rid of it
      -------------------------------------------------------------------*/
      if (self->text)
      {
         _XscTextDestroy( self->text );
      }
      self->text = text;
   }
   else
   {
      if (self->text == NULL)
      {
         self->text = _XscTextCreate();
      }
      _XscTextLoadTipResources( self->text, w, NULL );
   }
   
   if (self->yOffset == XscHelpDefaultTipYOffsetDynamic)
   {
      if (self->position == XmXSC_TIP_POSITION_POINTER)
      {
         self->yOffset = XscHelpDefaultTipYOffsetPointer;
      }
      else
      {
         self->yOffset = XscHelpDefaultTipYOffsetOther;
      }
   }
}




void _XscTipActivate( XscTip self )
{
   XscDisplay htk_display;
   XscShell   htk_shell;
   int        tip_group_id;
   Widget     w;
   
   w = _XscObjectGetWidget( self->object );
   htk_display = _XscDisplayDeriveFromWidget( w );

   /*----------------------------------------------------
   -- This object is now the active tip on this display
   ----------------------------------------------------*/
   _XscDisplaySetActiveTip( htk_display, self->object );

   /*--------------------------------------------------------------
   -- If the tip needs to be dynamically updated, then reload its
   -- attributes from the resource database
   --------------------------------------------------------------*/
   if (self->autoDbReload)
   {
      _XscTipLoadResources( self, NULL );
   }

   tip_group_id = XscTipGetGroupId( self );
   
   htk_shell = _XscShellDeriveFromWidget( w );
   
   /*------------------------------------------------------
   -- Don't bother to go any further unless it is managed
   ------------------------------------------------------*/
   if (!self->enabled || !_XscShellIsTipDisplayable( htk_shell ))
   {
      /*------------------------------------------------------
      -- Even though the tip is not enabled, it may have a 
      -- different group id.  If so, cancel the group effect
      -------------------------------------------------------*/
      _XscDisplayCheckForTipGroupIdMatch( htk_display, tip_group_id );
   }
   else
   {
      if (!_XscTextStringExists( self->text ))
      {
         /*-----------------------------------------------------
         -- Even though the tip has no text, it may have a 
         -- different group id.  If so, cancel the group effect
         ------------------------------------------------------*/
         _XscDisplayCheckForTipGroupIdMatch( htk_display, tip_group_id );
      }
      else
      {
         if (_XscDisplayActiveTipGroupIdEquals( htk_display, tip_group_id ))
         {
            _XscTipPopup( self );
         }
         else
         {
            _XscDisplayStartTimerTipPopup( htk_display, self->object );
         }
      }
   }
}


void _XscTipUpdate( XscTip self )
{
   int           root_x, root_y, popup_x, popup_y;
   Screen*       screen;
   XscDisplay htk_display;
   XscScreen    htk_screen;
   int           text_height, text_width, popup_height, popup_width;
   Pixel         foreground_color, select_color;
   Pixel         top_shadow_color, bottom_shadow_color;
   Widget        object_widget;
   int           tip_group_id;
   int           y_offset = self->yOffset;
   
   object_widget = _XscObjectGetWidget( self->object );
   htk_screen = _XscScreenDeriveFromWidget( object_widget );
   assert( htk_screen );
   screen = XtScreenOfObject( object_widget );

   htk_display = _XscDisplayDeriveFromWidget( object_widget );
   tip_group_id = XscTipGetGroupId( self );
   
   _XscDisplaySetTipActiveGroupId( htk_display, tip_group_id );

   /*---------------------------------------------
   -- Determine the size of the tip drawing area
   ---------------------------------------------*/
   text_width  = _XscTextGetWidth ( self->text );
   text_height = _XscTextGetHeight( self->text );

   /*----------------------------------------------
   -- Adjust the size for margins and decorations
   ----------------------------------------------*/
   text_width  += self->marginWidth *2 + self->marginLeft + self->marginRight;
   text_height += self->marginHeight*2 + self->marginTop  + self->marginBottom;

   popup_height = self->borderWidth * 2 + self->shadowThickness*2 + text_height;
   popup_width  = self->borderWidth * 2 + self->shadowThickness*2 + text_width;

   /*----------------------------------------------------------
   -- Keep some compilers from thinking these values are used
   -- in some cases without being initialized.
   ----------------------------------------------------------*/
   popup_x = popup_y = 0;

   /*-----------------------------------------------------------------
   -- Based on the tip popup placement strategy, figure out the root
   -- X and Y coordinates for the popup.
   -----------------------------------------------------------------*/
   if (self->position == XmXSC_TIP_POSITION_POINTER)
   {
      if (y_offset == 0)
      {
         y_offset = XscHelpDefaultTipYOffsetPointer;
      }
      
      root_x = self->_rootX;
      root_y = self->_rootY;

      /*-----------------------------------------
      -- Add the offset to the pointer location
      -----------------------------------------*/
      popup_x = root_x + self->xOffset;
      popup_y = root_y + y_offset;

      /*---------------------------------------------------------
      -- If the Y offset is negative, the the tip's position is
      -- relative to the bottom of the tip instead of the top
      ---------------------------------------------------------*/
      if (y_offset < 0)
      {
         popup_y -= popup_height;
      }

      /*-------------------------------------------------------
      -- If the default rendering direction is RtoL, then the
      -- tip position should be relative to the right edge
      -- with the opposite offset.
      -------------------------------------------------------*/
      if (_XscTextGetDirection( self->text ) == XmSTRING_DIRECTION_R_TO_L)
      {
         popup_x -= (popup_width + 2 * self->xOffset);
      }
   }
   else
   {
      /*-----------------------------------------------------------------
      -- If the tip position is based relative to the object's geometry, 
      -- then calculate the correct position
      -----------------------------------------------------------------*/
      Position      root_pos_x, root_pos_y;
      int           w_border, w_height, w_width;
      unsigned char position;

      XtTranslateCoords( object_widget, 0, 0, &root_pos_x, &root_pos_y );
      root_x   = root_pos_x;
      root_y   = root_pos_y;
      w_border = object_widget->core.border_width;
      w_height = object_widget->core.height;
      w_width  = object_widget->core.width;

      if (_XscTextGetDirection( self->text ) == XmSTRING_DIRECTION_R_TO_L)
      {
         switch( self->position )
         {
         case XmXSC_TIP_POSITION_TOP_BEGINNING:
            position = XmXSC_TIP_POSITION_TOP_RIGHT;
            break;
         case XmXSC_TIP_POSITION_TOP_END:
            position = XmXSC_TIP_POSITION_TOP_LEFT;
            break;
         case XmXSC_TIP_POSITION_BOTTOM_BEGINNING: 
            position = XmXSC_TIP_POSITION_BOTTOM_END;
            break;
         case XmXSC_TIP_POSITION_BOTTOM_END:
            position = XmXSC_TIP_POSITION_BOTTOM_LEFT;
            break;
         default:
            position = self->position;
            break;
         }
      }
      else
      {
         switch( self->position )
         {
         case XmXSC_TIP_POSITION_TOP_BEGINNING:
            position = XmXSC_TIP_POSITION_TOP_LEFT;
            break;
         case XmXSC_TIP_POSITION_TOP_END:
            position = XmXSC_TIP_POSITION_TOP_RIGHT;
            break;
         case XmXSC_TIP_POSITION_BOTTOM_BEGINNING: 
            position = XmXSC_TIP_POSITION_BOTTOM_LEFT;
            break;
         case XmXSC_TIP_POSITION_BOTTOM_END:
            position = XmXSC_TIP_POSITION_BOTTOM_END;
            break;
         default:
            position = self->position;
            break;
         }
      }
      
      switch( position )
      {
      case XmXSC_TIP_POSITION_TOP_LEFT:
         {
            popup_x = root_x - w_border;
            popup_y = root_y - w_border - popup_height;
         }
         break;

      case XmXSC_TIP_POSITION_TOP_RIGHT:
         {
            popup_x = root_x + w_width + w_border - popup_width;
            popup_y = root_y - w_border - popup_height;
         }
         break;

      case XmXSC_TIP_POSITION_BOTTOM_LEFT:
         {
            popup_x = root_x - w_border;
            popup_y = root_y + w_height + w_border;
         }
         break;

      case XmXSC_TIP_POSITION_BOTTOM_RIGHT:
         {
            popup_x = root_x + w_width  + w_border - popup_width;
            popup_y = root_y + w_height + w_border;
         }
         break;
      }
      
      popup_y += y_offset;
      if (_XscTextGetDirection( self->text ) == XmSTRING_DIRECTION_R_TO_L)
      {
         popup_x -= self->xOffset;
      }
      else
      {
         popup_x += self->xOffset;
      }
   }

   /*--------------------------------------------------------------
   -- If part of the tip is off screen to the left or right, then
   -- force it to be completely visible
   --------------------------------------------------------------*/
   if (popup_x < 0) 
   {
      popup_x = 0;
   }
   else if (popup_x + text_width > WidthOfScreen( screen ))
   {
      popup_x = WidthOfScreen( screen ) - text_width - 1;
   }

   /*-------------------------------------------------------------
   -- If the tip is off screen to the top or bottom, then invert
   -- it's Y offset
   -------------------------------------------------------------*/
   if (self->position == XmXSC_TIP_POSITION_POINTER)
   {
      if (popup_y < 0)
      {
         popup_y = root_y - y_offset;
      }
      else if (popup_y + text_height > HeightOfScreen( screen ))
      {
         popup_y = root_y - y_offset - popup_height;
      }
   }

   {
      Widget tip_shell, tip_frame, tip_label;

      tip_shell = _XscScreenGetTipShell( htk_screen );
      tip_frame = _XscScreenGetTipFrame( htk_screen );
      tip_label = _XscScreenGetTipLabel( htk_screen );

      /*--------------------------------------------------------
      -- Assign the height and width to the tip display widget
      --------------------------------------------------------*/
      XtVaSetValues(
         tip_label,
         XtNbackground, self->background,
         XtNwidth,      (Dimension) text_width,
         XtNheight,     (Dimension) text_height,
         NULL );

      /*-----------------------------------------------------
      -- Determine the colors to use for the shadow borders
      -----------------------------------------------------*/
      if (self->motifColorModel)
      {
         XmGetColors(
            screen,
            tip_frame->core.colormap,
            self->background,
            &foreground_color,
            &top_shadow_color,
            &bottom_shadow_color,
            &select_color );
      }
      else
      {
         XmGetColors(
            screen,
            tip_frame->core.colormap,
            self->colorBase,
            &foreground_color,
            &top_shadow_color,
            &bottom_shadow_color,
            &select_color );
      }

      /*---------------------------------------
      -- Specify the shadow border attributes
      ---------------------------------------*/
      XtVaSetValues(
         tip_frame,
         XmNshadowThickness,    self->shadowThickness,
         XmNshadowType,         self->shadowType,
         XmNtopShadowColor,     top_shadow_color,
         XmNbottomShadowColor,  bottom_shadow_color,
         NULL );

      /*---------------------------------------
      -- Specify the window border attributes
      ---------------------------------------*/
      XtVaSetValues(
         tip_shell, 
         XtNx,		(Position) popup_x,
         XtNy,		(Position) popup_y,
         XtNborderWidth,self->borderWidth,
         XtNborderColor,self->borderColor,
         NULL );
   }         
}

/*------------------------------------------------------------------------------
-- This function forces a tip to pop-down 
------------------------------------------------------------------------------*/
void _XscTipPopdown( XscTip self )
{
   Widget     object_widget = _XscObjectGetWidget( self->object );
   XscDisplay xsc_display   = _XscDisplayDeriveFromWidget( object_widget );
   XscScreen  xsc_screen    = _XscScreenDeriveFromWidget ( object_widget );
   
   /*-----------------------------------------------------------------------
   -- Verify that this object is the object recorded in the display record
   -----------------------------------------------------------------------*/
   if (self->object == _XscDisplayGetActiveTip( xsc_display ))
   {
      /*------------------------------------------------------
      -- Stop all the (potential) timers and popdown the tip
      ------------------------------------------------------*/
      _XscDisplayCancelTimerTipPopup  ( xsc_display );
      _XscDisplayCancelTimerTipPopdown( xsc_display );
      _XscDisplayCancelTimerSelectName( xsc_display );
      
      _XscScreenPopdownTip( xsc_screen );
      _XscDisplaySetActiveTip( xsc_display, NULL );
      
      self->_selected = False;
   }   
}


/*------------------------------------------------------------------------------
-- This function handles the geometry managment and pop-up of a tip
------------------------------------------------------------------------------*/
void _XscTipPopup( XscTip self )
{
   Widget object_widget = _XscObjectGetWidget( self->object );
   XscDisplay htk_display = _XscDisplayDeriveFromWidget( object_widget );
   XscScreen htk_screen = _XscScreenDeriveFromWidget( object_widget );

   Bool          bool;
   Window        root_window, child_window;
   int           root_x, root_y, win_x, win_y;
   unsigned int  keys_buttons;
   Screen*       screen;
   
   assert( htk_screen );
   screen = XtScreenOfObject( object_widget );

   bool = XQueryPointer(
      XtDisplayOfObject( object_widget ),
      RootWindowOfScreen( screen ),
      &root_window,
      &child_window,
      &root_x,
      &root_y,
      &win_x,
      &win_y,
      &keys_buttons );

   if (bool)
   {
      self->_rootX = root_x;
      self->_rootY = root_y;
   }
   else
   {
      Position root_pos_x, root_pos_y;
      int      w_border, w_height, w_width;

      XtTranslateCoords( object_widget, 0, 0, &root_pos_x, &root_pos_y );
      w_border = object_widget->core.border_width;
      w_height = object_widget->core.height;
      w_width  = object_widget->core.width;
         
      self->_rootX = root_pos_x + (w_border * 2 + w_width ) / 2;
      self->_rootY = root_pos_y + (w_border * 2 + w_height) / 2;
   }
   
   if (XscTipHasValidTopic( self ))
   {
      _XscTipUpdate( self );

      /*--------------
      -- Map the tip
      --------------*/
      _XscScreenPopupTip( htk_screen );

      /*---------------------------------------------
      -- If needed, install the auto pop-down timer
      ---------------------------------------------*/
      if (self->popdownInterval)
      {
         _XscDisplayStartTimerTipPopdown( htk_display, self->object );
      }
      
      if (_XscTextHasWidgetName( self->text ))
      {
      	 _XscDisplayStartTimerSelectName( htk_display, self->object );
      }
   }
}


void _XscTipRender( XscTip self, Widget tip_widget, GC gc )
{
   if (XscTipHasValidTopic( self ))
   {
      Pixel        foreground_color, select_color;
      Pixel        top_shadow_color, bottom_shadow_color;
      Display*     display    = XtDisplay( tip_widget );

      /*---------------------
      -- Keeps the GC happy
      ---------------------*/
      XSetFont( display, gc, _XscObjectGetFont( self->object ) );

      /*---------------------------------------------
      -- Determine the foregrounf color of the text
      ---------------------------------------------*/
      if (self->motifColorModel)
      {
         XmGetColors(
            XtScreen( tip_widget ),
            tip_widget->core.parent->core.colormap,
            self->background,
            &foreground_color,
            &top_shadow_color,
            &bottom_shadow_color,
            &select_color );
      }
      else
      {
         foreground_color = self->foreground;
      }

      /*------------------
      -- Render the text
      ------------------*/
      _XscTextStringDraw( 
         self->text,
         display,
         XtWindow( tip_widget ),
         gc,
         self->marginWidth  + self->marginLeft, 
         self->marginHeight + self->marginTop,
         0,
         NULL,
	 self->background,
	 foreground_color,
	 self->_selected );
   }
}

void _XscTipSelectName( XscTip self )
{
   Widget object_widget = _XscObjectGetWidget( self->object );
   XscScreen htk_screen = _XscScreenDeriveFromWidget( object_widget );
   
   if (!XtIsWidget( object_widget ))
   {
      object_widget = XtParent( object_widget );
   }
   
   self->_selected = XtOwnSelection( 
      object_widget,
      XA_PRIMARY,
      XtLastTimestampProcessed( XtDisplay( object_widget ) ),
      _tipConvertNameSelectionCB,
      _tipLoseNameSelectionCB,
      NULL );
   
   if (self->_selected)
   {
      _XscScreenRedrawTip( htk_screen );
   }
}

/*------------------------------------------------------------------------------
-- TIP CODE --------------------------------------------------------------------
------------------------------------------------------------------------------*/


XscObject _XscTipGetObject( XscTip self ) { return self->object; }


Boolean       XscTipGetAutoDbReload   ( XscTip s ){return s->autoDbReload   ;}
Pixel         XscTipGetBackground     ( XscTip s ){return s->background     ;}
Pixel         XscTipGetBorderColor    ( XscTip s ){return s->borderColor    ;}
Dimension     XscTipGetBorderWidth    ( XscTip s ){return s->borderWidth    ;}
Pixel         XscTipGetColorBase      ( XscTip s ){return s->colorBase      ;}
Boolean       XscTipGetCompound       ( XscTip s ){return s->compound       ;}
Boolean       XscTipGetEnabled        ( XscTip s ){return s->enabled        ;}
Pixel         XscTipGetForeground     ( XscTip s ){return s->foreground     ;}
Dimension     XscTipGetMarginBottom   ( XscTip s ){return s->marginBottom   ;}
Dimension     XscTipGetMarginHeight   ( XscTip s ){return s->marginHeight   ;}
Dimension     XscTipGetMarginLeft     ( XscTip s ){return s->marginLeft     ;}
Dimension     XscTipGetMarginRight    ( XscTip s ){return s->marginRight    ;}
Dimension     XscTipGetMarginTop      ( XscTip s ){return s->marginTop      ;}
Dimension     XscTipGetMarginWidth    ( XscTip s ){return s->marginWidth    ;}
Boolean       XscTipGetMotifColorModel( XscTip s ){return s->motifColorModel;}
unsigned char XscTipGetPosition       ( XscTip s ){return s->position       ;}
Dimension     XscTipGetShadowThickness( XscTip s ){return s->shadowThickness;}
unsigned char XscTipGetShadowType     ( XscTip s ){return s->shadowType     ;}
int           XscTipGetXOffset        ( XscTip s ){return s->xOffset        ;}
int           XscTipGetYOffset        ( XscTip s ){return s->yOffset        ;}

unsigned char XscTipGetAlignment( XscTip self )
{
   return _XscTextGetAlignment( self->text ); 
}

XmFontList XscTipGetFontList( XscTip self )
{
   return _XscTextGetFontList( self->text ); 
}

String XscTipGetFontListTag( XscTip self )
{
   return XtNewString( _XscTextGetFontListTag( self->text ) ); 
}

int XscTipGetGroupId( XscTip self )
{
   return _XscObjectGetTipGroupId( _XscTipGetObject( self ) );
}

unsigned long XscTipGetPopdownInterval( XscTip self )
{
   return self->popdownInterval;
}

unsigned long XscTipGetPopupInterval( XscTip self )
{
   return self->popupInterval;
}

unsigned long XscTipGetSelectNameInterval( XscTip self )
{
   return self->selectNameInterval;
}

unsigned char XscTipGetStringConverter( XscTip self )
{
   return _XscTextGetConverter( self->text ); 
}

unsigned char XscTipGetStringDirection( XscTip self )
{
   return _XscTextGetDirection( self->text ); 
}

String XscTipGetTopic( XscTip self )
{
   return XtNewString( _XscTextGetTopic( self->text ) ); 
}

/*------------------------------------------------------------------------------
-- These functions update the various data members; they will not automatically
-- update the tip/hint if it is currently displayed
------------------------------------------------------------------------------*/

void XscTipSetAutoDbReload   ( XscTip s,Boolean       a ){s->autoDbReload   =a;}
void XscTipSetBackground     ( XscTip s,Pixel         a ){s->background     =a;}
void XscTipSetBorderColor    ( XscTip s,Pixel         a ){s->borderColor    =a;}
void XscTipSetBorderWidth    ( XscTip s,Dimension     a ){s->borderWidth    =a;}
void XscTipSetColorBase      ( XscTip s,Pixel         a ){s->colorBase      =a;}
void XscTipSetCompound       ( XscTip s,Boolean       a ){s->compound       =a;}
void XscTipSetEnabled        ( XscTip s,Boolean       a ){s->enabled        =a;}
void XscTipSetForeground     ( XscTip s,Pixel         a ){s->foreground     =a;}
void XscTipSetMarginBottom   ( XscTip s,Dimension     a ){s->marginBottom   =a;}
void XscTipSetMarginHeight   ( XscTip s,Dimension     a ){s->marginHeight   =a;}
void XscTipSetMarginLeft     ( XscTip s,Dimension     a ){s->marginLeft     =a;}
void XscTipSetMarginRight    ( XscTip s,Dimension     a ){s->marginRight    =a;}
void XscTipSetMarginTop      ( XscTip s,Dimension     a ){s->marginTop      =a;}
void XscTipSetMarginWidth    ( XscTip s,Dimension     a ){s->marginWidth    =a;}
void XscTipSetMotifColorModel( XscTip s,Boolean       a ){s->motifColorModel=a;}
void XscTipSetPosition       ( XscTip s,unsigned char a ){s->position       =a;}
void XscTipSetShadowThickness( XscTip s,Dimension     a ){s->shadowThickness=a;}
void XscTipSetShadowType     ( XscTip s,unsigned char a ){s->shadowType     =a;}
void XscTipSetXOffset        ( XscTip s,int           a ){s->xOffset        =a;}
void XscTipSetYOffset        ( XscTip s,int           a ){s->yOffset        =a;}


void XscTipSetAlignment( XscTip self, unsigned char a, Boolean u )
{
   _XscTextSetAlignment( self->text, a );                  
   if (u)                              
   {                                   
      Widget w = _XscObjectGetWidget( _XscTipGetObject( self ) );
      _XscTextCreateCompoundString( self->text, w );
   }                                   
}

void XscTipSetFontList( XscTip self, XmFontList a, Boolean u )
{
   _XscTextSetFontList( self->text, a );                  
   if (u)                              
   {                                   
      Widget w = _XscObjectGetWidget( _XscTipGetObject( self ) );
      _XscTextCreateCompoundString( self->text, w );
   }                                   
}

void XscTipSetFontListTag( XscTip self, String a, Boolean u )
{
   _XscTextSetFontListTag( self->text, XtNewString( a ) );                  
   if (u)                              
   {                                   
      Widget w = _XscObjectGetWidget( _XscTipGetObject( self ) );
      _XscTextCreateCompoundString( self->text, w );
   }                                   
}

void XscTipSetGroupId( XscTip self, int a )
{
   _XscObjectSetTipGroupId( _XscTipGetObject( self ), a );
}


void XscTipSetPopdownInterval( XscTip self, unsigned long a )
{
   self->popdownInterval = a;
}

void XscTipSetPopupInterval( XscTip self, unsigned long a )
{
   self->popupInterval = a;
}

void XscTipSetSelectNameInterval( XscTip self, unsigned long a )
{
   self->selectNameInterval = a;
}

void XscTipSetStringConverter( XscTip self, unsigned char a, Boolean u )
{
   _XscTextSetConverter( self->text, a );                  
   if (u)                              
   {                                   
      Widget w = _XscObjectGetWidget( _XscTipGetObject( self ) );
      _XscTextCreateCompoundString( self->text, w );
   }                                   
}

void XscTipSetStringDirection( XscTip self, unsigned char a, Boolean u )
{
   _XscTextSetDirection( self->text, a );                  
   if (u)                              
   {                                   
      Widget w = _XscObjectGetWidget( _XscTipGetObject( self ) );
      _XscTextCreateCompoundString( self->text, w );
   }                                   
}

void XscTipSetTopic( XscTip self, String a )
{
   Widget w = _XscObjectGetWidget( _XscTipGetObject( self ) );
   
   _XscTextSetTopic( self->text, XtNewString( a ) );
   _XscTextCreateCompoundString( self->text, w );
}



Boolean XscTipHasValidTopic( XscTip self )
{
   return _XscTextStringExists( self->text );
}


XscText _XscTipGetText( XscTip self )
{
   return self->text;
}

