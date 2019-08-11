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

#include <Xsc/Help.h>
#include <Xsc/StrDefs.h>

#include "CueP.h"
#include "DisplayP.h"
#include "ObjectP.h"
#include "ScreenP.h"
#include "ShellP.h"
#include "TextP.h"


/*==============================================================================
                                    Macros
==============================================================================*/
#define XscHelpDefaultCueXOffset        0
#define XscHelpDefaultCueYOffsetDynamic 0xFFFF
#define XscHelpDefaultCueYOffsetOther   0
#define XscHelpDefaultCueYOffsetShell   (-10)


/*==============================================================================
                               Private data types
==============================================================================*/
/*--------------------------------------------------------------------
-- This structure defines the attributes associated with a given cue 
--------------------------------------------------------------------*/
typedef struct _XscCueRec
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
   unsigned char position;          /* Cue layout policy */
   Boolean       autoDbReload;
   Boolean       enabled;
   Boolean       motifColorModel;
   int           xOffset;
   int           yOffset;
}
XscCueRec;


XscCue _XscCueCreate( XscObject o )
{
   XscCue self = XtNew( XscCueRec );
   
   memset( (void*) self, '\0', sizeof( *self ) );
   self->text   = NULL;
   self->object = o;

   return self;
}

void _XscCueDestroy( XscCue self )
{
   if (self->text) _XscTextDestroy( self->text );
   XtFree( (char*) self );
}


XscCue XscCueDeriveFromWidget( Widget w )
{
   XscObject o = _XscObjectDeriveFromWidget( w );
   if (!o)
   {
      return NULL;
   }
   else
   {
      return _XscObjectGetCue( o );
   }
}


void _XscCueLoadResources( XscCue self, XscText text )
{
#  define OFFSET_OF( mem ) XtOffsetOf( XscCueRec, mem )
   
   static XtResource resource[] =
   {
      {
         XmNxscCueAutoDbReload, XmCXscCueAutoDbReload,
         XmRBoolean, sizeof( Boolean ), OFFSET_OF( autoDbReload ),
         XtRImmediate, (XtPointer) (Boolean) False
      },{
         XmNxscCueBackground, XmCXscCueBackground, 
         XtRPixel, sizeof( Pixel ), OFFSET_OF( background ), 
         XtRString, "Gold"
      },{
         XmNxscCueBorderColor, XmCXscCueBorderColor,
         XtRPixel, sizeof( Pixel ), OFFSET_OF( borderColor ), 
         XtRString, "Black"
      },{
         XmNxscCueBorderWidth, XmCXscCueBorderWidth,
         XtRDimension, sizeof( Dimension ), OFFSET_OF( borderWidth ), 
         XtRImmediate, (XtPointer) 1
      },{
         XmNxscCueColorBase, XmCXscCueColorBase,
         XmRPixel, sizeof( Pixel ), OFFSET_OF( colorBase ),
         XtRString, "Gold"
      },{
         XmNxscCueEnabled, XmCXscCueEnabled,
         XmRBoolean, sizeof( Boolean ), OFFSET_OF( enabled ),
         XtRImmediate, (XtPointer) (Boolean) True
      },{
         XmNxscCueForeground, XmCXscCueForeground, 
         XtRPixel, sizeof( Pixel ), OFFSET_OF( foreground ), 
         XtRString, "Black"
      },{
         XmNxscCueMarginBottom, XmCXscCueMarginBottom,
         XmRDimension, sizeof( Dimension ), OFFSET_OF( marginBottom ),
         XtRImmediate, (XtPointer) (Dimension) 0
      },{
         XmNxscCueMarginHeight, XmCXscCueMarginHeight,
         XmRDimension, sizeof( Dimension ), OFFSET_OF( marginHeight ),
         XtRImmediate, (XtPointer) (Dimension) 1
      },{
         XmNxscCueMarginLeft, XmCXscCueMarginLeft,
         XmRDimension, sizeof( Dimension ), OFFSET_OF( marginLeft ),
         XtRImmediate, (XtPointer) (Dimension) 0
      },{
         XmNxscCueMarginRight, XmCXscCueMarginRight,
         XmRDimension, sizeof( Dimension ), OFFSET_OF( marginRight ),
         XtRImmediate, (XtPointer) (Dimension) 0
      },{
         XmNxscCueMarginTop, XmCXscCueMarginTop,
         XmRDimension, sizeof( Dimension ), OFFSET_OF( marginTop ),
         XtRImmediate, (XtPointer) (Dimension) 0
      },{
         XmNxscCueMarginWidth, XmCXscCueMarginWidth,
         XmRDimension, sizeof( Dimension ), OFFSET_OF( marginWidth ),
         XtRImmediate, (XtPointer) (Dimension) 2
      },{
         XmNxscCueMotifColorModel, XmCXscCueMotifColorModel,
         XtRBoolean, sizeof( Boolean ), OFFSET_OF( motifColorModel ),
         XtRImmediate, (XtPointer) (Boolean) FALSE
      },{
         XmNxscCuePosition, XmCXscCuePosition,
         XmRXscCuePosition, sizeof( unsigned char ), OFFSET_OF( position ),
         XtRImmediate, (XtPointer) (unsigned char) XmXSC_CUE_POSITION_SHELL
      },{
         XmNxscCueShadowThickness, XmCXscCueShadowThickness,
         XtRDimension, sizeof( Dimension ), OFFSET_OF( shadowThickness ), 
         XtRImmediate, (XtPointer) 0
      },{
         XmNxscCueShadowType, XmCXscCueShadowType,
         XmRShadowType, sizeof( unsigned char ), OFFSET_OF( shadowType ),
         XtRImmediate, (XtPointer) (unsigned char) XmSHADOW_OUT
      },{
         XmNxscCueXOffset, XmCXscCueXOffset,
         XmRInt, sizeof( int ), OFFSET_OF( xOffset ),
         XtRImmediate, (XtPointer) (int) XscHelpDefaultCueXOffset
      },{
         XmNxscCueYOffset, XmCXscCueYOffset,
         XmRInt, sizeof( int ), OFFSET_OF( yOffset ),
         XtRImmediate, (XtPointer) (int) XscHelpDefaultCueYOffsetDynamic
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
      _XscTextLoadCueResources( self->text, w, NULL );
   }
   
   if (self->yOffset == XscHelpDefaultCueYOffsetDynamic)
   {
      if (self->position == XmXSC_CUE_POSITION_SHELL)
      {
         self->yOffset = XscHelpDefaultCueYOffsetShell;
      }
      else
      {
         self->yOffset = XscHelpDefaultCueYOffsetOther;
      }
   }
}




void _XscCueActivate( XscCue self )
{
   XscDisplay hpt_display;
   XscShell   htk_shell;
   Widget     w;
   
   w = _XscObjectGetWidget( self->object );
   hpt_display = _XscDisplayDeriveFromWidget( w );
   htk_shell   = _XscShellDeriveFromWidget( w );
   
   if (_XscShellIsCueDisplayable( htk_shell ))
   {
      /*----------------------------------------------------
      -- This object is now the active cue on this display
      ----------------------------------------------------*/
      _XscDisplaySetActiveCue( hpt_display, self );

      /*--------------------------------------------------------------
      -- If the cue needs to be dynamically updated, then reload its
      -- attributes from the resource database
      --------------------------------------------------------------*/
      if (self->autoDbReload)
      {
	 _XscCueLoadResources( self, NULL );
      }

      /*------------------------------------------------------
      -- Don't bother to go any further unless it is managed
      ------------------------------------------------------*/
      if (self->enabled)
      {
	 if (_XscTextStringExists( self->text ))
	 {
            _XscCuePopup( self );
	 }
      }
   }
}


void _XscCueUpdate( XscCue self )
{
   int        root_x, root_y, popup_x, popup_y;
   Screen*    screen;
   XscDisplay hpt_display;
   XscScreen  hpt_screen;
   int        text_height, text_width, popup_height, popup_width;
   Pixel      foreground_color, select_color;
   Pixel      top_shadow_color, bottom_shadow_color;
   Widget     object_widget;
   
   object_widget = _XscObjectGetWidget( self->object );
   hpt_screen = _XscScreenDeriveFromWidget( object_widget );
   assert( hpt_screen );
   screen = XtScreenOfObject( object_widget );

   hpt_display = _XscDisplayDeriveFromWidget( object_widget );

   /*---------------------------------------------
   -- Determine the size of the cue drawing area
   ---------------------------------------------*/
   text_width  = _XscTextGetWidth ( self->text );
   text_height = _XscTextGetHeight( self->text );

   /*----------------------------------------------
   -- Adjust the size for margins and decorations
   ----------------------------------------------*/
   text_width  += self->marginWidth *2 + self->marginLeft + self->marginRight;
   text_height += self->marginHeight*2 + self->marginTop  + self->marginBottom;

   popup_height = self->borderWidth*2 + self->shadowThickness*2 + text_height;
   popup_width  = self->borderWidth*2 + self->shadowThickness*2 + text_width;

   /*----------------------------------------------------------
   -- Keep some compilers from thinking these values are used
   -- in some cases without being initialized.
   ----------------------------------------------------------*/
   popup_x = popup_y = 0;

   if (self->position == XmXSC_CUE_POSITION_SHELL)
   {
      /*------------------------
      -- Find the shell widget
      ------------------------*/
      Widget       shell   = object_widget;
      Display*     display = XtDisplayOfObject( shell );
      Window       frame, parent, root;
      Window*      children;
      int          frame_x, frame_y;
      int          parent_x, parent_y;
      unsigned int frame_width , frame_height , frame_border;
      unsigned int parent_width, parent_height, parent_border, parent_depth;
      unsigned int num_children;
      Screen*      screen;
      
      while (!XtIsWMShell( shell )) shell = XtParent( shell );
      
      assert( XtIsRealized( shell ) );
      
      screen = XtScreen( shell );
      frame  = XtWindow( shell );
      frame_x      = shell->core.x;
      frame_y      = shell->core.y;
      frame_width  = shell->core.width;
      frame_height = shell->core.height;
      frame_border = shell->core.border_width;
      
      for(;;)
      {
         XQueryTree( display, frame, &root, &parent, &children, &num_children);
         if (children) XFree( (void*) children );
         
         if (parent == root) break;
         
         XGetGeometry( 
            display, 
            parent, 
            &root, 
            &parent_x, 
            &parent_y,
            &parent_width,
            &parent_height,
            &parent_border,
            &parent_depth );

         if (parent_width == WidthOfScreen( screen ))
         {
            if (parent_height == HeightOfScreen( screen ))
            {
               break;
            }
         }
         frame        = parent;
         frame_x      = parent_x;
         frame_y      = parent_y;
         frame_width  = parent_width;
         frame_height = parent_height;
         frame_border = parent_border;
      }

      popup_y = frame_y - frame_border - popup_height + self->yOffset;

      if (_XscTextGetDirection( self->text ) == XmSTRING_DIRECTION_R_TO_L)
      {
         popup_x = frame_x + frame_width + 2 * frame_border - popup_width;
         popup_x -= self->xOffset;
      }
      else
      {
         popup_x = frame_x - frame_border + self->xOffset;
      }
   }
   else
   {
      /*-----------------------------------------------------------------
      -- If the cue position is based relative to the object's geometry, 
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
         case XmXSC_CUE_POSITION_TOP_BEGINNING:
            position = XmXSC_CUE_POSITION_TOP_RIGHT;
            break;
         case XmXSC_CUE_POSITION_TOP_END:
            position = XmXSC_CUE_POSITION_TOP_LEFT;
            break;
         case XmXSC_CUE_POSITION_BOTTOM_BEGINNING: 
            position = XmXSC_CUE_POSITION_BOTTOM_END;
            break;
         case XmXSC_CUE_POSITION_BOTTOM_END:
            position = XmXSC_CUE_POSITION_BOTTOM_LEFT;
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
         case XmXSC_CUE_POSITION_TOP_BEGINNING:
            position = XmXSC_CUE_POSITION_TOP_LEFT;
            break;
         case XmXSC_CUE_POSITION_TOP_END:
            position = XmXSC_CUE_POSITION_TOP_RIGHT;
            break;
         case XmXSC_CUE_POSITION_BOTTOM_BEGINNING: 
            position = XmXSC_CUE_POSITION_BOTTOM_LEFT;
            break;
         case XmXSC_CUE_POSITION_BOTTOM_END:
            position = XmXSC_CUE_POSITION_BOTTOM_END;
            break;
         default:
            position = self->position;
            break;
         }
      }
      
      switch( position )
      {
      case XmXSC_CUE_POSITION_TOP_LEFT:
         {
            popup_x = root_x - w_border;
            popup_y = root_y - w_border - popup_height;
         }
         break;

      case XmXSC_CUE_POSITION_TOP_RIGHT:
         {
            popup_x = root_x + w_width + w_border - popup_width;
            popup_y = root_y - w_border - popup_height;
         }
         break;

      case XmXSC_CUE_POSITION_BOTTOM_LEFT:
         {
            popup_x = root_x - w_border;
            popup_y = root_y + w_height + w_border;
         }
         break;

      case XmXSC_CUE_POSITION_BOTTOM_RIGHT:
         {
            popup_x = root_x + w_width  + w_border - popup_width;
            popup_y = root_y + w_height + w_border;
         }
         break;
      }
      
      popup_y += self->yOffset;
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
   -- If part of the cue is off screen to the left or right, then
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

   {
      Widget cue_shell, cue_frame, cue_label;

      cue_shell = _XscScreenGetCueShell( hpt_screen );
      cue_frame = _XscScreenGetCueFrame( hpt_screen );
      cue_label = _XscScreenGetCueLabel( hpt_screen );

      /*---------------------------------------
      -- Specify the window border attributes
      ---------------------------------------*/
      XtVaSetValues(
         cue_shell, 
         XtNx,		(Position) popup_x,
         XtNy,		(Position) popup_y,
         XtNborderWidth,self->borderWidth,
         XtNborderColor,self->borderColor,
         NULL );

      /*-----------------------------------------------------
      -- Determine the colors to use for the shadow borders
      -----------------------------------------------------*/
      if (self->motifColorModel)
      {
         XmGetColors(
            screen,
            cue_frame->core.colormap,
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
            cue_frame->core.colormap,
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
         cue_frame,
         XmNshadowThickness,    self->shadowThickness,
         XmNshadowType,         self->shadowType,
         XmNtopShadowColor,     top_shadow_color,
         XmNbottomShadowColor,  bottom_shadow_color,
         NULL );

      /*--------------------------------------------------------
      -- Assign the height and width to the cue display widget
      --------------------------------------------------------*/
      XtVaSetValues(
         cue_label,
         XtNbackground, self->background,
         XtNwidth,      (Dimension) text_width,
         XtNheight,     (Dimension) text_height,
         NULL );
   }         
}

/*------------------------------------------------------------------------------
-- This function forces a cue to pop-down 
------------------------------------------------------------------------------*/
void _XscCuePopdown( XscCue self )
{
   Widget object_widget = _XscObjectGetWidget( self->object );
   XscDisplay hpt_display = _XscDisplayDeriveFromWidget( object_widget );
   
   /*-----------------------------------------------------------------------
   -- Verify that this object is the object recorded in the display record
   -----------------------------------------------------------------------*/
   if (self == _XscDisplayGetActiveCue( hpt_display ))
   {
      XscScreen hpt_screen = _XscScreenDeriveFromWidget( object_widget );
      
      /*------------------------------------------------------
      -- Stop all the (potential) timers and popdown the cue
      ------------------------------------------------------*/
      _XscScreenPopdownCue( hpt_screen );
      _XscDisplaySetActiveCue( hpt_display, NULL );
   }   
}


/*------------------------------------------------------------------------------
-- This function handles the geometry managment and pop-up of a tip
------------------------------------------------------------------------------*/
void _XscCuePopup( XscCue self )
{
   Widget object_widget = _XscObjectGetWidget( self->object );
   XscScreen hpt_screen = _XscScreenDeriveFromWidget( object_widget );

   Screen*       screen;
   
   assert( hpt_screen );
   screen = XtScreenOfObject( object_widget );

   if (XscCueHasValidTopic( self ))
   {
      _XscCueUpdate( self );

      /*--------------
      -- Map the tip
      --------------*/
      _XscScreenPopupCue( hpt_screen );
   }
}


void _XscCueRender( XscCue self, Widget cue_widget, GC gc )
{
   if (XscCueHasValidTopic( self ))
   {
      Pixel        foreground_color, select_color;
      Pixel        top_shadow_color, bottom_shadow_color;
      Display*     display    = XtDisplay( cue_widget );

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
            XtScreen( cue_widget ),
            cue_widget->core.parent->core.colormap,
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
         XtWindow( cue_widget ),
         gc,
         self->marginWidth  + self->marginLeft, 
         self->marginHeight + self->marginTop,
         0,
         NULL,
	 self->background,
	 foreground_color,
	 False );
   }
}

/*------------------------------------------------------------------------------
-- CUE CODE --------------------------------------------------------------------
------------------------------------------------------------------------------*/


XscObject _XscCueGetObject( XscCue self ) { return self->object; }


Boolean       XscCueGetAutoDbReload   ( XscCue s ){return s->autoDbReload   ;}
Pixel         XscCueGetBackground     ( XscCue s ){return s->background     ;}
Pixel         XscCueGetBorderColor    ( XscCue s ){return s->borderColor    ;}
Dimension     XscCueGetBorderWidth    ( XscCue s ){return s->borderWidth    ;}
Pixel         XscCueGetColorBase      ( XscCue s ){return s->colorBase      ;}
Boolean       XscCueGetEnabled        ( XscCue s ){return s->enabled        ;}
Pixel         XscCueGetForeground     ( XscCue s ){return s->foreground     ;}
Dimension     XscCueGetMarginBottom   ( XscCue s ){return s->marginBottom   ;}
Dimension     XscCueGetMarginHeight   ( XscCue s ){return s->marginHeight   ;}
Dimension     XscCueGetMarginLeft     ( XscCue s ){return s->marginLeft     ;}
Dimension     XscCueGetMarginRight    ( XscCue s ){return s->marginRight    ;}
Dimension     XscCueGetMarginTop      ( XscCue s ){return s->marginTop      ;}
Dimension     XscCueGetMarginWidth    ( XscCue s ){return s->marginWidth    ;}
Boolean       XscCueGetMotifColorModel( XscCue s ){return s->motifColorModel;}
unsigned char XscCueGetPosition       ( XscCue s ){return s->position       ;}
Dimension     XscCueGetShadowThickness( XscCue s ){return s->shadowThickness;}
unsigned char XscCueGetShadowType     ( XscCue s ){return s->shadowType     ;}
int           XscCueGetXOffset        ( XscCue s ){return s->xOffset        ;}
int           XscCueGetYOffset        ( XscCue s ){return s->yOffset        ;}

unsigned char XscCueGetAlignment( XscCue self )
{
   return _XscTextGetAlignment( self->text ); 
}

XmFontList XscCueGetFontList( XscCue self )
{
   return _XscTextGetFontList( self->text ); 
}

String XscCueGetFontListTag( XscCue self )
{
   return XtNewString( _XscTextGetFontListTag( self->text ) ); 
}

unsigned char XscCueGetStringConverter( XscCue self )
{
   return _XscTextGetConverter( self->text ); 
}

unsigned char XscCueGetStringDirection( XscCue self )
{
   return _XscTextGetDirection( self->text ); 
}

String XscCueGetTopic( XscCue self )
{
   return XtNewString( _XscTextGetTopic( self->text ) ); 
}

/*------------------------------------------------------------------------------
-- These functions update the various data members; they will not automatically
-- update the cue if it is currently displayed
------------------------------------------------------------------------------*/

void XscCueSetAutoDbReload   ( XscCue s,Boolean       a ){s->autoDbReload   =a;}
void XscCueSetBackground     ( XscCue s,Pixel         a ){s->background     =a;}
void XscCueSetBorderColor    ( XscCue s,Pixel         a ){s->borderColor    =a;}
void XscCueSetBorderWidth    ( XscCue s,Dimension     a ){s->borderWidth    =a;}
void XscCueSetColorBase      ( XscCue s,Pixel         a ){s->colorBase      =a;}
void XscCueSetEnabled        ( XscCue s,Boolean       a ){s->enabled        =a;}
void XscCueSetForeground     ( XscCue s,Pixel         a ){s->foreground     =a;}
void XscCueSetMarginBottom   ( XscCue s,Dimension     a ){s->marginBottom   =a;}
void XscCueSetMarginHeight   ( XscCue s,Dimension     a ){s->marginHeight   =a;}
void XscCueSetMarginLeft     ( XscCue s,Dimension     a ){s->marginLeft     =a;}
void XscCueSetMarginRight    ( XscCue s,Dimension     a ){s->marginRight    =a;}
void XscCueSetMarginTop      ( XscCue s,Dimension     a ){s->marginTop      =a;}
void XscCueSetMarginWidth    ( XscCue s,Dimension     a ){s->marginWidth    =a;}
void XscCueSetMotifColorModel( XscCue s,Boolean       a ){s->motifColorModel=a;}
void XscCueSetPosition       ( XscCue s,unsigned char a ){s->position       =a;}
void XscCueSetShadowThickness( XscCue s,Dimension     a ){s->shadowThickness=a;}
void XscCueSetShadowType     ( XscCue s,unsigned char a ){s->shadowType     =a;}
void XscCueSetXOffset        ( XscCue s,int           a ){s->xOffset        =a;}
void XscCueSetYOffset        ( XscCue s,int           a ){s->yOffset        =a;}


void XscCueSetAlignment( XscCue self, unsigned char a, Boolean u )
{
   _XscTextSetAlignment( self->text, a );                  
   if (u)                              
   {                                   
      Widget w = _XscObjectGetWidget( _XscCueGetObject( self ) );
      _XscTextCreateCompoundString( self->text, w );
   }                                   
}

void XscCueSetFontList( XscCue self, XmFontList a, Boolean u )
{
   _XscTextSetFontList( self->text, a );                  
   if (u)                              
   {                                   
      Widget w = _XscObjectGetWidget( _XscCueGetObject( self ) );
      _XscTextCreateCompoundString( self->text, w );
   }                                   
}

void XscCueSetFontListTag( XscCue self, String a, Boolean u )
{
   _XscTextSetFontListTag( self->text, XtNewString( a ) );                  
   if (u)                              
   {                                   
      Widget w = _XscObjectGetWidget( _XscCueGetObject( self ) );
      _XscTextCreateCompoundString( self->text, w );
   }                                   
}

void XscCueSetStringConverter( XscCue self, unsigned char a, Boolean u )
{
   _XscTextSetConverter( self->text, a );                  
   if (u)                              
   {                                   
      Widget w = _XscObjectGetWidget( _XscCueGetObject( self ) );
      _XscTextCreateCompoundString( self->text, w );
   }                                   
}

void XscCueSetStringDirection( XscCue self, unsigned char a, Boolean u )
{
   _XscTextSetDirection( self->text, a );                  
   if (u)                              
   {                                   
      Widget w = _XscObjectGetWidget( _XscCueGetObject( self ) );
      _XscTextCreateCompoundString( self->text, w );
   }                                   
}

void XscCueSetTopic( XscCue self, String a )
{
   Widget w = _XscObjectGetWidget( _XscCueGetObject( self ) );
   
   _XscTextSetTopic( self->text, XtNewString( a ) );
   _XscTextCreateCompoundString( self->text, w );
}



Boolean XscCueHasValidTopic( XscCue self )
{
   return _XscTextStringExists( self->text );
}


XscText _XscCueGetText( XscCue self )
{
   return self->text;
}

