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

#include <X11/IntrinsicP.h>

#include <Xsc/Help.h>
#include <Xsc/StrDefs.h>

#include "DisplayP.h"
#include "HintP.h"
#include "ShellP.h"


/*==============================================================================
                               Private data types
==============================================================================*/
/*---------------------------------------------------------------------
-- This structure defines the attributes associated with a given hint
---------------------------------------------------------------------*/
typedef struct _XscHintRec
{
   XscObject  object;
   XscText    text;
   Dimension  marginLeft;
   Dimension  marginRight;
   Dimension  marginWidth;
   Pixel      background;
   Pixel      foreground;
   Boolean    inheritBackground; /* Use same background as parent? */
   Boolean    autoDbReload;
   Boolean    enabled;
   Boolean    motifColorModel;
   Boolean    compound;
}
XscHintRec;



XscHint _XscHintCreate( XscObject o )
{
   XscHint self = XtNew( XscHintRec );
   
   memset( (void*) self, '\0', sizeof( *self ) );
   self->text   = NULL;
   self->object = o;

   return self;
}

void _XscHintDestroy( XscHint self )
{
   if (self->text) _XscTextDestroy( self->text );
   XtFree( (char*) self );
}


XscHint XscHintDeriveFromWidget( Widget w )
{
   XscObject o = _XscObjectDeriveFromWidget( w );
   if (!o)
   {
      return NULL;
   }
   else
   {
      return _XscObjectGetHint( o );
   }
}


void _XscHintLoadResources( XscHint self, XscText text )
{
#  define OFFSET_OF( mem ) XtOffsetOf( XscHintRec, mem )
   
   static XtResource resource[] =
   {
      {
         XmNxscHintAutoDbReload, XmCXscHintAutoDbReload,
         XmRBoolean, sizeof( Boolean ), OFFSET_OF( autoDbReload ),
         XtRImmediate, (XtPointer) (Boolean) False
      },{
         XmNxscHintBackground, XmCXscHintBackground, 
         XtRPixel, sizeof( Pixel ), OFFSET_OF( background ), 
         XtRString, "Grey"
      },{
         XmNxscHintCompound, XmCXscHintCompound,
         XmRBoolean, sizeof( Boolean ), OFFSET_OF( compound ),
         XtRImmediate, (XtPointer) (Boolean) False
      },{
         XmNxscHintEnabled, XmCXscHintEnabled,
         XmRBoolean, sizeof( Boolean ), OFFSET_OF( enabled ),
         XtRImmediate, (XtPointer) (Boolean) True
      },{
         XmNxscHintForeground, XmCXscHintForeground, 
         XtRPixel, sizeof( Pixel ), OFFSET_OF( foreground ), 
         XtRString, "Black"
      },{
         XmNxscHintInheritBackground, XmCXscHintInheritBackground,
         XmRBoolean, sizeof( Boolean ), OFFSET_OF( inheritBackground ),
         XtRImmediate, (XtPointer) (Boolean) True
      },{
         XmNxscHintMarginLeft, XmCXscHintMarginLeft,
         XmRDimension, sizeof( Dimension ), OFFSET_OF( marginLeft ),
         XtRImmediate, (XtPointer) (Dimension) 0
      },{
         XmNxscHintMarginRight, XmCXscHintMarginRight,
         XmRDimension, sizeof( Dimension ), OFFSET_OF( marginRight ),
         XtRImmediate, (XtPointer) (Dimension) 0
      },{
         XmNxscHintMarginWidth, XmCXscHintMarginWidth,
         XmRDimension, sizeof( Dimension ), OFFSET_OF( marginWidth ),
         XtRImmediate, (XtPointer) (Dimension) 2
      },{
         XmNxscHintMotifColorModel, XmCXscHintMotifColorModel,
         XtRBoolean, sizeof( Boolean ), OFFSET_OF( motifColorModel ),
         XtRImmediate, (XtPointer) (Boolean) TRUE
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
      _XscTextLoadHintResources( self->text, w, NULL );
   }
}


void _XscHintActivate( XscHint self )
{
   XscShell shell;

   /*----------------------------------------------------------------
   -- Make sure that the shell associated with this object actually 
   -- has a hint display widget defined
   ----------------------------------------------------------------*/
   shell = _XscShellDeriveFromWidget( _XscObjectGetWidget( self->object ) );
   
   if (shell && _XscShellIsHintDisplayable( shell ))
   {
      Widget hint_widget = _XscShellGetHintWidget( shell );

      if (hint_widget)
      {
         XscDisplay display = _XscDisplayDeriveFromWidget( hint_widget );
         
         /*----------------------------------------------------
         -- This object is now the active hint on the display
         ----------------------------------------------------*/
         _XscDisplaySetActiveHint( display, self->object );

         /*---------------------------------------------------------------
         -- If the hint needs to be dynamically updated, then reload its
         -- attributes from the resource database
         ---------------------------------------------------------------*/
         if (self->autoDbReload)
         {
            _XscHintLoadResources( self, NULL );
         }

         /*-----------------------------------------------------------------
         -- Don't bother to go any further unless the hint is is "managed"
         -----------------------------------------------------------------*/
         if (self->enabled)
         {
            if (_XscTextStringExists( self->text ))
            {
               /*------------------------------------------------------
               -- Determine what the hint's backgrond color should be
               ------------------------------------------------------*/
               Pixel background;

               if (self->inheritBackground)
               {
                  XtVaGetValues( 
                     XtParent( hint_widget ),
                     XmNbackground,     &background,
                     NULL );
               }
               else
               {
                  background = self->background;
               }

               /*-------------------------------------------------------
               -- Store this with the shell so that it can be accessed
               -- when the hint is actually rendered.  It is needed
               -- in some cases to compute the forground color.
               -------------------------------------------------------*/
               _XscShellSetBackground( shell, background );

               /*--------------------------------------------------------
               -- Set the background color now to avoid flashing if the
               -- hint display widget is currently unmapped
               --------------------------------------------------------*/
               XtVaSetValues(
                  hint_widget,
                  XmNbackground,        background,
                  NULL );

               /*---------------------------------------------------------------
               -- If the hint display area is still mapped, then kill the 
               -- timer (used to unmap it) and clear it (generating an expose)
               ---------------------------------------------------------------*/
               _XscShellDisplayHint( shell );
            }
         }
      }
   }
}


void _XscHintRender( XscHint self, Widget hint_widget, GC gc, Pixel background )
{
   if (self && _XscTextStringExists( self->text ))
   {
      Display* x_display = XtDisplay( hint_widget );

      Pixel foreground_color, select_color;
      Pixel top_shadow_color, bottom_shadow_color;

      /*---------------------
      -- Keeps the GC happy
      ---------------------*/
      XSetFont( x_display, gc, _XscObjectGetFont( self->object ) );

      /*----------------------------------------------
      -- Determine the foreground color for the text
      ----------------------------------------------*/
      if (self->motifColorModel)
      {
         XmGetColors(
            XtScreen( hint_widget ),
            hint_widget->core.colormap,
            background,
            &foreground_color,
            &top_shadow_color,
            &bottom_shadow_color,
            &select_color );
      }
      else
      {
         foreground_color = self->foreground;
      }

      /*---------------------------------------------
      -- Render the text in the hint display widget
      ---------------------------------------------*/
      _XscTextStringDraw( 
         self->text,
         x_display,
         XtWindow( hint_widget ),
         gc,
         self->marginWidth + self->marginLeft, 
         (hint_widget->core.height - _XscTextGetHeight( self->text )) / 2,
         hint_widget->core.width - self->marginWidth * 2 -
            self->marginLeft - self->marginRight,
         NULL,
	 background,
	 foreground_color,
	 False );
   }
}



XscObject _XscHintGetObject( XscHint self ) { return self->object; }

Boolean   XscHintGetAutoDbReload( XscHint self ) { return self->autoDbReload; }
Pixel     XscHintGetBackground  ( XscHint self ) { return self->background  ; }
Boolean   XscHintGetCompound    ( XscHint self ) { return self->compound    ; }
Boolean   XscHintGetEnabled     ( XscHint self ) { return self->enabled     ; }
Pixel     XscHintGetForeground  ( XscHint self ) { return self->foreground  ; }
Dimension XscHintGetMarginLeft  ( XscHint self ) { return self->marginLeft  ; }
Dimension XscHintGetMarginRight ( XscHint self ) { return self->marginRight ; }
Dimension XscHintGetMarginWidth ( XscHint self ) { return self->marginWidth ; }

unsigned char XscHintGetAlignment( XscHint self )
{ 
   return _XscTextGetAlignment( self->text ); 
}

XmFontList XscHintGetFontList( XscHint self )
{
   return _XscTextGetFontList( self->text ); 
}

String XscHintGetFontListTag( XscHint self )
{
   return XtNewString( _XscTextGetFontListTag( self->text ) ); 
}

Boolean XscHintGetInheritBackground( XscHint self ) 
{ 
   return self->inheritBackground; 
}

Boolean XscHintGetMotifColorModel( XscHint self )
{
   return self->motifColorModel;
}

unsigned char XscHintGetStringConverter( XscHint self )
{
   return _XscTextGetConverter( self->text ); 
}

unsigned char XscHintGetStringDirection( XscHint self )
{
   return _XscTextGetDirection( self->text ); 
}

String XscHintGetTopic( XscHint self )
{
   return XtNewString( _XscTextGetTopic( self->text ) ); 
}



void XscHintSetAutoDbReload( XscHint self, Boolean   a ) {self->autoDbReload=a;}
void XscHintSetBackground  ( XscHint self, Pixel     a ) {self->background  =a;}
void XscHintSetCompound    ( XscHint self, Boolean   a ) {self->compound    =a;}
void XscHintSetEnabled     ( XscHint self, Boolean   a ) {self->enabled     =a;}
void XscHintSetForeground  ( XscHint self, Pixel     a ) {self->foreground  =a;}
void XscHintSetMarginLeft  ( XscHint self, Dimension a ) {self->marginLeft  =a;}
void XscHintSetMarginRight ( XscHint self, Dimension a ) {self->marginRight =a;}
void XscHintSetMarginWidth ( XscHint self, Dimension a ) {self->marginWidth =a;}

void XscHintSetAlignment( XscHint self, unsigned char a, Boolean u )
{
   _XscTextSetAlignment( self->text, a );                  
   if (u)                              
   {                                   
      Widget w = _XscObjectGetWidget( self->object );
      _XscTextCreateCompoundString( self->text, w );
   }                                   
}

void XscHintSetFontList( XscHint self, XmFontList a, Boolean u )
{
   _XscTextSetFontList( self->text, a );                  
   if (u)                              
   {                                   
      Widget w = _XscObjectGetWidget( self->object );
      _XscTextCreateCompoundString( self->text, w );
   }                                   
}

void XscHintSetFontListTag( XscHint self, String a, Boolean u )
{
   _XscTextSetFontListTag( self->text, XtNewString( a ) );
   if (u)
   {
      Widget w = _XscObjectGetWidget( self->object );
      _XscTextCreateCompoundString( self->text, w );
   }
}

void XscHintSetInheritBackground( XscHint self, Boolean a )
{
   self->inheritBackground = a;
}

void XscHintSetMotifColorModel( XscHint self, Boolean a )
{
   self->motifColorModel = a;
}

void XscHintSetStringConverter( XscHint self, unsigned char a, Boolean u )
{
   _XscTextSetConverter( self->text, a );                  
   if (u)                              
   {                                   
      Widget w = _XscObjectGetWidget( self->object );
      _XscTextCreateCompoundString( self->text, w );
   }                                   
}

void XscHintSetStringDirection( XscHint self, unsigned char a, Boolean u )
{
   _XscTextSetDirection( self->text, a );                  
   if (u)                              
   {
      Widget w = _XscObjectGetWidget( self->object );
      _XscTextCreateCompoundString( self->text, w );
   }                                   
}

void XscHintSetTopic( XscHint self, String a )
{
   Widget w = _XscObjectGetWidget( self->object ); 
   _XscTextSetTopic( self->text, XtNewString( a ) );
   _XscTextCreateCompoundString( self->text, w );
}



Boolean XscHintHasValidTopic( XscHint self )
{
   return _XscTextStringExists( self->text );
}


XscText _XscHintGetText( XscHint self )
{
   return self->text;
}

