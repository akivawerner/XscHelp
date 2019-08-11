/*-----------------------------------------------------------------------------
--  Help ToolKit for Motif demo of set/get API
-- 
--  Copyright (c) 1997-2000, 2019 Robert S. Werner (a.k.a. Akiva Werner)  
--  ALL RIGHTS RESERVED
--
--  License: The MIT License (MIT)
-----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>

#include <X11/Xmu/Editres.h>

#include <Xm/Xm.h>
#include <Xm/ArrowB.h>
#include <Xm/ArrowBG.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/MainW.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/RowColumn.h>
#include <Xm/Scale.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/ToggleB.h>
#include <Xm/ToggleBG.h>
#include <Xm/PanedW.h>

#ifdef USE_CDE
#  include <Xsc/CdeHelp.h>
#else
#  include <Xsc/Help.h>
#endif

#include <Xsc/Hint.h>
#include <Xsc/Tip.h>

/* New Callbacks ------------------------------------------------------------*/

static Widget Colors_Widget, Edges_Widget, Flags_Widget, Font_Widget;
static Widget Offsets_Widget, Style_Widget, Timers_Widget;

typedef void          (*set_pixel_proc)        ( XtPointer, Pixel );
typedef Pixel         (*get_pixel_proc)        ( XtPointer );
typedef void          (*set_dimension_proc)    ( XtPointer, Dimension );
typedef Dimension     (*get_dimension_proc)    ( XtPointer );
typedef void          (*set_boolean_proc)      ( XtPointer, Boolean );
typedef Boolean       (*get_boolean_proc)      ( XtPointer );
typedef void          (*set_int_proc)          ( XtPointer, int );
typedef int           (*get_int_proc)          ( XtPointer );
typedef void          (*set_unsigned_long_proc)( XtPointer, unsigned long );
typedef unsigned long (*get_unsigned_long_proc)( XtPointer );

static void changeColorCB( Widget w, XtPointer cd, XtPointer cbd )
{
   XmToggleButtonCallbackStruct* data = (XmToggleButtonCallbackStruct*) cbd;

   String         resource = (String) cd;
   set_pixel_proc set_hint_func;
   get_pixel_proc get_hint_func;
   set_pixel_proc set_tip_func;
   get_pixel_proc get_tip_func;
   
   XtPointer h;
   XtPointer colors_h;
   XtPointer edges_h;
   XtPointer flags_h;
   XtPointer font_h;
   XtPointer offsets_h;
   XtPointer style_h;
   XtPointer timers_h;

   XtPointer t;
   XtPointer colors_t;
   XtPointer edges_t;
   XtPointer flags_t;
   XtPointer font_t;
   XtPointer offsets_t;
   XtPointer style_t;
   XtPointer timers_t;

   if (!Timers_Widget) return;
   
   h         = XscHintDeriveFromWidget( w );
   colors_h  = XscHintDeriveFromWidget( Colors_Widget  );
   edges_h   = XscHintDeriveFromWidget( Edges_Widget   );
   flags_h   = XscHintDeriveFromWidget( Flags_Widget   );
   font_h    = XscHintDeriveFromWidget( Font_Widget    );
   offsets_h = XscHintDeriveFromWidget( Offsets_Widget );
   style_h   = XscHintDeriveFromWidget( Style_Widget   );
   timers_h  = XscHintDeriveFromWidget( Timers_Widget  );
      
   t         = XscTipDeriveFromWidget( w );
   colors_t  = XscTipDeriveFromWidget( Colors_Widget  );
   edges_t   = XscTipDeriveFromWidget( Edges_Widget   );
   flags_t   = XscTipDeriveFromWidget( Flags_Widget   );
   font_t    = XscTipDeriveFromWidget( Font_Widget    );
   offsets_t = XscTipDeriveFromWidget( Offsets_Widget );
   style_t   = XscTipDeriveFromWidget( Style_Widget   );
   timers_t  = XscTipDeriveFromWidget( Timers_Widget  );

   if (strcmp( resource, "Background" ) == 0)
   {
      set_hint_func = (set_pixel_proc) XscHintSetBackground;
      get_hint_func = (get_pixel_proc) XscHintGetBackground;
      set_tip_func  = (set_pixel_proc) XscTipSetBackground;
      get_tip_func  = (get_pixel_proc) XscTipGetBackground;
   }
   else if (strcmp( resource, "Foreground" ) == 0)
   {
      set_hint_func = (set_pixel_proc) XscHintSetForeground;
      get_hint_func = (get_pixel_proc) XscHintGetForeground;
      set_tip_func  = (set_pixel_proc) XscTipSetForeground;
      get_tip_func  = (get_pixel_proc) XscTipGetForeground;
   }
   else if (strcmp( resource, "BorderColor" ) == 0)
   {
      set_hint_func = (set_pixel_proc) NULL;
      get_hint_func = (get_pixel_proc) NULL;
      set_tip_func  = (set_pixel_proc) XscTipSetBorderColor;
      get_tip_func  = (get_pixel_proc) XscTipGetBorderColor;
   }
   else if (strcmp( resource, "ColorBase" ) == 0)
   {
      set_hint_func = (set_pixel_proc) NULL;
      get_hint_func = (get_pixel_proc) NULL;
      set_tip_func  = (set_pixel_proc) XscTipSetColorBase;
      get_tip_func  = (get_pixel_proc) XscTipGetColorBase;
   }
   else
   {
      set_hint_func = (set_pixel_proc) NULL;
      get_hint_func = (get_pixel_proc) NULL;
      set_tip_func  = (set_pixel_proc) NULL;
      get_tip_func  = (get_pixel_proc) NULL;
   }
   
   if (h && t)
   { 
      if (data->set)
      {
         Pixel pixel;
         XtVaGetValues( w, XmNforeground, &pixel, NULL );
         if (set_hint_func) 
         {
            set_hint_func( h        , pixel );
            set_hint_func( colors_h , pixel );
            set_hint_func( edges_h  , pixel );
            set_hint_func( flags_h  , pixel );
            set_hint_func( font_h   , pixel );
            set_hint_func( offsets_h, pixel );
            set_hint_func( style_h  , pixel );
            set_hint_func( timers_h , pixel );
         }
         if (set_tip_func) 
         {
            set_tip_func ( t, pixel );
            set_tip_func( colors_t , pixel );
            set_tip_func( edges_t  , pixel );
            set_tip_func( flags_t  , pixel );
            set_tip_func( font_t   , pixel );
            set_tip_func( offsets_t, pixel );
            set_tip_func( style_t  , pixel );
            set_tip_func( timers_t , pixel );
         }
         {
            Pixel h_pixel = 0;
            Pixel t_pixel = 0;
     
            if (get_hint_func) h_pixel = get_hint_func( h );
            if (get_tip_func ) t_pixel = get_tip_func ( t );

            printf( "Pixel is %ld,%ld,%ld\n", 
               (long) pixel, (long) h_pixel, (long) t_pixel );
         }
      }
      else
      {
         Pixel pixel = XWhitePixelOfScreen( XtScreen( w ) );
         if (set_hint_func) set_hint_func( h, pixel );
         if (set_tip_func)  set_tip_func ( t, pixel );
      }
      XscHelpUpdate( w );
   }
}

static void changeDimensionCB( Widget w, XtPointer cd, XtPointer cbd )
{
   XmScaleCallbackStruct* data = (XmScaleCallbackStruct*) cbd;

   String             resource = (String) cd;
   set_dimension_proc set_hint_func;
   get_dimension_proc get_hint_func;
   set_dimension_proc set_tip_func;
   get_dimension_proc get_tip_func;
   
   XtPointer h;
   XtPointer colors_h;
   XtPointer edges_h;
   XtPointer flags_h;
   XtPointer font_h;
   XtPointer offsets_h;
   XtPointer style_h;
   XtPointer timers_h;

   XtPointer t;
   XtPointer colors_t;
   XtPointer edges_t;
   XtPointer flags_t;
   XtPointer font_t;
   XtPointer offsets_t;
   XtPointer style_t;
   XtPointer timers_t;

   if (!Timers_Widget) return;
   
   h         = XscHintDeriveFromWidget( w );
   colors_h  = XscHintDeriveFromWidget( Colors_Widget  );
   edges_h   = XscHintDeriveFromWidget( Edges_Widget   );
   flags_h   = XscHintDeriveFromWidget( Flags_Widget   );
   font_h    = XscHintDeriveFromWidget( Font_Widget    );
   offsets_h = XscHintDeriveFromWidget( Offsets_Widget );
   style_h   = XscHintDeriveFromWidget( Style_Widget   );
   timers_h  = XscHintDeriveFromWidget( Timers_Widget  );
 
   t         = XscTipDeriveFromWidget( w );
   colors_t  = XscTipDeriveFromWidget( Colors_Widget  );
   edges_t   = XscTipDeriveFromWidget( Edges_Widget   );
   flags_t   = XscTipDeriveFromWidget( Flags_Widget   );
   font_t    = XscTipDeriveFromWidget( Font_Widget    );
   offsets_t = XscTipDeriveFromWidget( Offsets_Widget );
   style_t   = XscTipDeriveFromWidget( Style_Widget   );
   timers_t  = XscTipDeriveFromWidget( Timers_Widget  );
   
   if (strcmp( resource, "BorderWidth" ) == 0)
   {
      set_hint_func = (set_dimension_proc) NULL;
      get_hint_func = (get_dimension_proc) NULL;
      set_tip_func  = (set_dimension_proc) XscTipSetBorderWidth;
      get_tip_func  = (get_dimension_proc) XscTipGetBorderWidth;
   }
   else if (strcmp( resource, "MarginBottom" ) == 0)
   {
      set_hint_func = (set_dimension_proc) NULL;
      get_hint_func = (get_dimension_proc) NULL;
      set_tip_func  = (set_dimension_proc) XscTipSetMarginBottom;
      get_tip_func  = (get_dimension_proc) XscTipGetMarginBottom;
   }
   else if (strcmp( resource, "MarginHeight" ) == 0)
   {
      set_hint_func = (set_dimension_proc) NULL;
      get_hint_func = (get_dimension_proc) NULL;
      set_tip_func  = (set_dimension_proc) XscTipSetMarginHeight;
      get_tip_func  = (get_dimension_proc) XscTipGetMarginHeight;
   }
   else if (strcmp( resource, "MarginLeft" ) == 0)
   {
      set_hint_func = (set_dimension_proc) XscHintSetMarginLeft;
      get_hint_func = (get_dimension_proc) XscHintGetMarginLeft;
      set_tip_func  = (set_dimension_proc) XscTipSetMarginLeft;
      get_tip_func  = (get_dimension_proc) XscTipGetMarginLeft;
   }
   else if (strcmp( resource, "MarginRight" ) == 0)
   {
      set_hint_func = (set_dimension_proc) XscHintSetMarginRight;
      get_hint_func = (get_dimension_proc) XscHintGetMarginRight;
      set_tip_func  = (set_dimension_proc) XscTipSetMarginRight;
      get_tip_func  = (get_dimension_proc) XscTipGetMarginRight;
   }
   else if (strcmp( resource, "MarginTop" ) == 0)
   {
      set_hint_func = (set_dimension_proc) NULL;
      get_hint_func = (get_dimension_proc) NULL;
      set_tip_func  = (set_dimension_proc) XscTipSetMarginTop;
      get_tip_func  = (get_dimension_proc) XscTipGetMarginTop;
   }
   else if (strcmp( resource, "MarginWidth" ) == 0)
   {
      set_hint_func = (set_dimension_proc) XscHintSetMarginWidth;
      get_hint_func = (get_dimension_proc) XscHintGetMarginWidth;
      set_tip_func  = (set_dimension_proc) XscTipSetMarginWidth;
      get_tip_func  = (get_dimension_proc) XscTipGetMarginWidth;
   }
   else if (strcmp( resource, "ShadowThickness" ) == 0)
   {
      set_hint_func = (set_dimension_proc) NULL;
      get_hint_func = (get_dimension_proc) NULL;
      set_tip_func  = (set_dimension_proc) XscTipSetShadowThickness;
      get_tip_func  = (get_dimension_proc) XscTipGetShadowThickness;
   }
   else
   {
      set_hint_func = (set_dimension_proc) NULL;
      get_hint_func = (get_dimension_proc) NULL;
      set_tip_func  = (set_dimension_proc) NULL;
      get_tip_func  = (get_dimension_proc) NULL;
   }

   if (h && t)
   { 
      Dimension dimension = (Dimension) data->value;

      if (set_hint_func) 
      {
         set_hint_func( h        , dimension );
         set_hint_func( colors_h , dimension );
         set_hint_func( edges_h  , dimension );
         set_hint_func( flags_h  , dimension );
         set_hint_func( font_h   , dimension );
         set_hint_func( offsets_h, dimension );
         set_hint_func( style_h  , dimension );
         set_hint_func( timers_h , dimension );
      }
      if (set_tip_func) 
      {
         set_tip_func ( t, dimension );
         set_tip_func( colors_t , dimension );
         set_tip_func( edges_t  , dimension );
         set_tip_func( flags_t  , dimension );
         set_tip_func( font_t   , dimension );
         set_tip_func( offsets_t, dimension );
         set_tip_func( style_t  , dimension );
         set_tip_func( timers_t , dimension );
      }

      {
         Dimension t_dimension, h_dimension;
         t_dimension = 0;
         h_dimension = 0;

         if (get_hint_func) h_dimension = get_hint_func( h );
         if (get_tip_func ) t_dimension = get_tip_func ( t );

         printf( "Dimension is: %ld,%ld,%ld\n", 
                 (long) dimension,(long) h_dimension, (long) t_dimension );
      }
      XscHelpUpdate( w );
   }
}

static void changeIntCB( Widget w, XtPointer cd, XtPointer cbd )
{
   XmScaleCallbackStruct* data = (XmScaleCallbackStruct*) cbd;

   String       resource = (String) cd;
   set_int_proc set_hint_func;
   get_int_proc get_hint_func;
   set_int_proc set_tip_func;
   get_int_proc get_tip_func;
   
   XtPointer h;
   XtPointer colors_h;
   XtPointer edges_h;
   XtPointer flags_h;
   XtPointer font_h;
   XtPointer offsets_h;
   XtPointer style_h;
   XtPointer timers_h;

   XtPointer t;
   XtPointer colors_t;
   XtPointer edges_t;
   XtPointer flags_t;
   XtPointer font_t;
   XtPointer offsets_t;
   XtPointer style_t;
   XtPointer timers_t;

   if (!Timers_Widget) return;
   
   h         = XscHintDeriveFromWidget( w );
   colors_h  = XscHintDeriveFromWidget( Colors_Widget  );
   edges_h   = XscHintDeriveFromWidget( Edges_Widget   );
   flags_h   = XscHintDeriveFromWidget( Flags_Widget   );
   font_h    = XscHintDeriveFromWidget( Font_Widget    );
   offsets_h = XscHintDeriveFromWidget( Offsets_Widget );
   style_h   = XscHintDeriveFromWidget( Style_Widget   );
   timers_h  = XscHintDeriveFromWidget( Timers_Widget  );
   
   t         = XscTipDeriveFromWidget( w );
   colors_t  = XscTipDeriveFromWidget( Colors_Widget  );
   edges_t   = XscTipDeriveFromWidget( Edges_Widget   );
   flags_t   = XscTipDeriveFromWidget( Flags_Widget   );
   font_t    = XscTipDeriveFromWidget( Font_Widget    );
   offsets_t = XscTipDeriveFromWidget( Offsets_Widget );
   style_t   = XscTipDeriveFromWidget( Style_Widget   );
   timers_t  = XscTipDeriveFromWidget( Timers_Widget  );
   
   if (strcmp( resource, "XOffset" ) == 0)
   {
      set_hint_func = (set_int_proc) NULL;
      get_hint_func = (get_int_proc) NULL;
      set_tip_func  = (set_int_proc) XscTipSetXOffset;
      get_tip_func  = (get_int_proc) XscTipGetXOffset;
   }
   else if (strcmp( resource, "YOffset" ) == 0)
   {
      set_hint_func = (set_int_proc) NULL;
      get_hint_func = (get_int_proc) NULL;
      set_tip_func  = (set_int_proc) XscTipSetYOffset;
      get_tip_func  = (get_int_proc) XscTipGetYOffset;
   }
   else
   {
      set_hint_func = (set_int_proc) NULL;
      get_hint_func = (get_int_proc) NULL;
      set_tip_func  = (set_int_proc) NULL;
      get_tip_func  = (get_int_proc) NULL;
   }
   
   if (h && t)
   { 
      int i = (int) data->value;

      if (set_hint_func) 
      {
         set_hint_func( h        , i );
         set_hint_func( colors_h , i );
         set_hint_func( edges_h  , i );
         set_hint_func( flags_h  , i );
         set_hint_func( font_h   , i );
         set_hint_func( offsets_h, i );
         set_hint_func( style_h  , i );
         set_hint_func( timers_h , i );
      }
      if (set_tip_func) 
      {
         set_tip_func ( t, i );
         set_tip_func( colors_t , i );
         set_tip_func( edges_t  , i );
         set_tip_func( flags_t  , i );
         set_tip_func( font_t   , i );
         set_tip_func( offsets_t, i );
         set_tip_func( style_t  , i );
         set_tip_func( timers_t , i );
      }

      {
         int t_i, h_i;
         t_i = 0;
         h_i = 0;

         if (get_hint_func) h_i = get_hint_func( h );
         if (get_tip_func ) t_i = get_tip_func ( t );

         printf( "Int is: %d,%d,%d\n", i, h_i, t_i );
      }
      XscHelpUpdate( w );
   }
}

static void changeUnsignedLongCB( Widget w, XtPointer cd, XtPointer cbd )
{
   XmScaleCallbackStruct* data = (XmScaleCallbackStruct*) cbd;

   String                 resource = (String) cd;
   set_unsigned_long_proc set_hint_func;
   get_unsigned_long_proc get_hint_func;
   set_unsigned_long_proc set_tip_func;
   get_unsigned_long_proc get_tip_func;
   
   XtPointer h;
   XtPointer colors_h;
   XtPointer edges_h;
   XtPointer flags_h;
   XtPointer font_h;
   XtPointer offsets_h;
   XtPointer style_h;
   XtPointer timers_h;

   XtPointer t;
   XtPointer colors_t;
   XtPointer edges_t;
   XtPointer flags_t;
   XtPointer font_t;
   XtPointer offsets_t;
   XtPointer style_t;
   XtPointer timers_t;

   if (!Timers_Widget) return;
   
   h         = XscHintDeriveFromWidget( w );
   colors_h  = XscHintDeriveFromWidget( Colors_Widget  );
   edges_h   = XscHintDeriveFromWidget( Edges_Widget   );
   flags_h   = XscHintDeriveFromWidget( Flags_Widget   );
   font_h    = XscHintDeriveFromWidget( Font_Widget    );
   offsets_h = XscHintDeriveFromWidget( Offsets_Widget );
   style_h   = XscHintDeriveFromWidget( Style_Widget   );
   timers_h  = XscHintDeriveFromWidget( Timers_Widget  );
 
   t         = XscTipDeriveFromWidget( w );
   colors_t  = XscTipDeriveFromWidget( Colors_Widget  );
   edges_t   = XscTipDeriveFromWidget( Edges_Widget   );
   flags_t   = XscTipDeriveFromWidget( Flags_Widget   );
   font_t    = XscTipDeriveFromWidget( Font_Widget    );
   offsets_t = XscTipDeriveFromWidget( Offsets_Widget );
   style_t   = XscTipDeriveFromWidget( Style_Widget   );
   timers_t  = XscTipDeriveFromWidget( Timers_Widget  );

   if (strcmp( resource, "PopupInterval" ) == 0)
   {
      set_hint_func = (set_unsigned_long_proc) NULL;
      get_hint_func = (get_unsigned_long_proc) NULL;
      set_tip_func  = (set_unsigned_long_proc) XscTipSetPopupInterval;
      get_tip_func  = (get_unsigned_long_proc) XscTipGetPopupInterval;
   }
   else if (strcmp( resource, "PopdownInterval" ) == 0)
   {
      set_hint_func = (set_unsigned_long_proc) NULL;
      get_hint_func = (get_unsigned_long_proc) NULL;
      set_tip_func  = (set_unsigned_long_proc) XscTipSetPopdownInterval;
      get_tip_func  = (get_unsigned_long_proc) XscTipGetPopdownInterval;
   }
   else
   {
      set_hint_func = (set_unsigned_long_proc) NULL;
      get_hint_func = (get_unsigned_long_proc) NULL;
   }
   
   if (h && t)
   { 
      unsigned long i = (unsigned long) data->value;

      if (set_hint_func) 
      {
         set_hint_func( h        , i );
         set_hint_func( colors_h , i );
         set_hint_func( edges_h  , i );
         set_hint_func( flags_h  , i );
         set_hint_func( font_h   , i );
         set_hint_func( offsets_h, i );
         set_hint_func( style_h  , i );
         set_hint_func( timers_h , i );
      }
      if (set_tip_func) 
      {
         set_tip_func ( t, i );
         set_tip_func( colors_t , i );
         set_tip_func( edges_t  , i );
         set_tip_func( flags_t  , i );
         set_tip_func( font_t   , i );
         set_tip_func( offsets_t, i );
         set_tip_func( style_t  , i );
         set_tip_func( timers_t , i );
      }

      {
         unsigned long t_i, h_i;
         t_i = 0;
         h_i = 0;

         if (get_hint_func) h_i = get_hint_func( h );
         if (get_tip_func ) t_i = get_tip_func ( t );

         printf( "ULong is: %lu,%lu,%lu\n", i, h_i, t_i );
      }
      XscHelpUpdate( w );
   }
}

static void changeBooleanCB( Widget w, XtPointer cd, XtPointer cbd )
{
   XmToggleButtonCallbackStruct* data = (XmToggleButtonCallbackStruct*) cbd;

   String           resource = (String) cd;
   set_boolean_proc set_hint_func;
   get_boolean_proc get_hint_func;
   set_boolean_proc set_tip_func;
   get_boolean_proc get_tip_func;
   
   XtPointer h;
   XtPointer colors_h;
   XtPointer edges_h;
   XtPointer flags_h;
   XtPointer font_h;
   XtPointer offsets_h;
   XtPointer style_h;
   XtPointer timers_h;

   XtPointer t;
   XtPointer colors_t;
   XtPointer edges_t;
   XtPointer flags_t;
   XtPointer font_t;
   XtPointer offsets_t;
   XtPointer style_t;
   XtPointer timers_t;

   if (!data->set)     return;
   if (!Timers_Widget) return;
   
   h         = XscHintDeriveFromWidget( w );
   colors_h  = XscHintDeriveFromWidget( Colors_Widget  );
   edges_h   = XscHintDeriveFromWidget( Edges_Widget   );
   flags_h   = XscHintDeriveFromWidget( Flags_Widget   );
   font_h    = XscHintDeriveFromWidget( Font_Widget    );
   offsets_h = XscHintDeriveFromWidget( Offsets_Widget );
   style_h   = XscHintDeriveFromWidget( Style_Widget   );
   timers_h  = XscHintDeriveFromWidget( Timers_Widget  );

   t         = XscTipDeriveFromWidget( w );
   colors_t  = XscTipDeriveFromWidget( Colors_Widget  );
   edges_t   = XscTipDeriveFromWidget( Edges_Widget   );
   flags_t   = XscTipDeriveFromWidget( Flags_Widget   );
   font_t    = XscTipDeriveFromWidget( Font_Widget    );
   offsets_t = XscTipDeriveFromWidget( Offsets_Widget );
   style_t   = XscTipDeriveFromWidget( Style_Widget   );
   timers_t  = XscTipDeriveFromWidget( Timers_Widget  );
   
   if (strcmp( resource, "Manage" ) == 0)
   {
      set_hint_func = (set_boolean_proc) XscHintSetEnabled;
      get_hint_func = (get_boolean_proc) XscHintGetEnabled;
      set_tip_func  = (set_boolean_proc) XscTipSetEnabled;
      get_tip_func  = (get_boolean_proc) XscTipGetEnabled;
   }
   else if (strcmp( resource, "MotifColorModel" ) == 0)
   {
      set_hint_func = (set_boolean_proc) XscHintSetMotifColorModel;
      get_hint_func = (get_boolean_proc) XscHintGetMotifColorModel;
      set_tip_func  = (set_boolean_proc) XscTipSetMotifColorModel;
      get_tip_func  = (get_boolean_proc) XscTipGetMotifColorModel;
   }
   else if (strcmp( resource, "InheritBackground" ) == 0)
   {
      set_hint_func = (set_boolean_proc) XscHintSetInheritBackground;
      get_hint_func = (get_boolean_proc) XscHintGetInheritBackground;
      set_tip_func  = (set_boolean_proc) NULL;
      get_tip_func  = (get_boolean_proc) NULL;
   }
   else
   {
      set_hint_func = (set_boolean_proc) NULL;
      get_hint_func = (get_boolean_proc) NULL;
      set_tip_func  = (set_boolean_proc) NULL;
      get_tip_func  = (get_boolean_proc) NULL;
   }
   
   if (h && t)
   { 
      Boolean b;
      int i;
      XtVaGetValues( w, XmNuserData, &i, NULL );
      b = (Boolean) i;

      if (set_hint_func) 
      {
         set_hint_func( h        , b );
         set_hint_func( colors_h , b );
         set_hint_func( edges_h  , b );
         set_hint_func( flags_h  , b );
         set_hint_func( font_h   , b );
         set_hint_func( offsets_h, b );
         set_hint_func( style_h  , b );
         set_hint_func( timers_h , b );
      }
      if (set_tip_func) 
      {
         set_tip_func ( t, b );
         set_tip_func( colors_t , b );
         set_tip_func( edges_t  , b );
         set_tip_func( flags_t  , b );
         set_tip_func( font_t   , b );
         set_tip_func( offsets_t, b );
         set_tip_func( style_t  , b );
         set_tip_func( timers_t , b );
      }

      {
         Dimension t_b, h_b;
         t_b = 0;
         h_b = 0;

         if (get_hint_func) h_b = get_hint_func( h );
         if (get_tip_func ) t_b = get_tip_func ( t );

         printf( "Boolean is: %ld,%ld,%ld\n", 
                 (long) b,(long) h_b, (long) t_b );
      }
      XscHelpUpdate( w );
   }
}

static void changeFontListTagCB( Widget w, XtPointer cd, XtPointer cbd )
{
   XmToggleButtonCallbackStruct* data = (XmToggleButtonCallbackStruct*) cbd;

   String resource = (String) cd;
   
   XtPointer h;
   XtPointer colors_h;
   XtPointer edges_h;
   XtPointer flags_h;
   XtPointer font_h;
   XtPointer offsets_h;
   XtPointer style_h;
   XtPointer timers_h;

   XtPointer t;
   XtPointer colors_t;
   XtPointer edges_t;
   XtPointer flags_t;
   XtPointer font_t;
   XtPointer offsets_t;
   XtPointer style_t;
   XtPointer timers_t;

   if (!Timers_Widget) return;
   
   h         = XscHintDeriveFromWidget( w );
   colors_h  = XscHintDeriveFromWidget( Colors_Widget  );
   edges_h   = XscHintDeriveFromWidget( Edges_Widget   );
   flags_h   = XscHintDeriveFromWidget( Flags_Widget   );
   font_h    = XscHintDeriveFromWidget( Font_Widget    );
   offsets_h = XscHintDeriveFromWidget( Offsets_Widget );
   style_h   = XscHintDeriveFromWidget( Style_Widget   );
   timers_h  = XscHintDeriveFromWidget( Timers_Widget  );

   t         = XscTipDeriveFromWidget( w );
   colors_t  = XscTipDeriveFromWidget( Colors_Widget  );
   edges_t   = XscTipDeriveFromWidget( Edges_Widget   );
   flags_t   = XscTipDeriveFromWidget( Flags_Widget   );
   font_t    = XscTipDeriveFromWidget( Font_Widget    );
   offsets_t = XscTipDeriveFromWidget( Offsets_Widget );
   style_t   = XscTipDeriveFromWidget( Style_Widget   );
   timers_t  = XscTipDeriveFromWidget( Timers_Widget  );
   
   if (h && t)
   { 
      if (data->set)
      {
         String tag;
         XtVaGetValues( w, XmNuserData, &tag, NULL );

         XscHintSetFontListTag( h        , tag, True );
         XscHintSetFontListTag( colors_h , tag, True );
         XscHintSetFontListTag( edges_h  , tag, True );
         XscHintSetFontListTag( flags_h  , tag, True );
         XscHintSetFontListTag( font_h   , tag, True );
         XscHintSetFontListTag( offsets_h, tag, True );
         XscHintSetFontListTag( style_h  , tag, True );
         XscHintSetFontListTag( timers_h , tag, True );

         XscTipSetFontListTag ( t, tag, True );
         XscTipSetFontListTag( colors_t , tag, True );
         XscTipSetFontListTag( edges_t  , tag, True );
         XscTipSetFontListTag( flags_t  , tag, True );
         XscTipSetFontListTag( font_t   , tag, True );
         XscTipSetFontListTag( offsets_t, tag, True );
         XscTipSetFontListTag( style_t  , tag, True );
         XscTipSetFontListTag( timers_t , tag, True );
         
         {
            String t_tag, h_tag;

            h_tag = XscHintGetFontListTag( h );
            t_tag = XscTipGetFontListTag ( t );

            printf( "Tag is: %s,%s,%s\n", 
               tag, h_tag ? h_tag : "n/a", t_tag ? t_tag : "n/a" );
         }
      }
      else
      {
         XscHintSetFontListTag( h, "normal", True );
         XscTipSetFontListTag ( t, "normal", True );
      }
      XscHelpUpdate( w );
   }
}

static void changeConverterCB( Widget w, XtPointer cd, XtPointer cbd )
{
   XmToggleButtonCallbackStruct* data = (XmToggleButtonCallbackStruct*) cbd;

   String resource = (String) cd;
   
   XtPointer h;
   XtPointer colors_h;
   XtPointer edges_h;
   XtPointer flags_h;
   XtPointer font_h;
   XtPointer offsets_h;
   XtPointer style_h;
   XtPointer timers_h;

   XtPointer t;
   XtPointer colors_t;
   XtPointer edges_t;
   XtPointer flags_t;
   XtPointer font_t;
   XtPointer offsets_t;
   XtPointer style_t;
   XtPointer timers_t;

   if (!Timers_Widget) return;
   
   h         = XscHintDeriveFromWidget( w );
   colors_h  = XscHintDeriveFromWidget( Colors_Widget  );
   edges_h   = XscHintDeriveFromWidget( Edges_Widget   );
   flags_h   = XscHintDeriveFromWidget( Flags_Widget   );
   font_h    = XscHintDeriveFromWidget( Font_Widget    );
   offsets_h = XscHintDeriveFromWidget( Offsets_Widget );
   style_h   = XscHintDeriveFromWidget( Style_Widget   );
   timers_h  = XscHintDeriveFromWidget( Timers_Widget  );

   t         = XscTipDeriveFromWidget( w );
   colors_t  = XscTipDeriveFromWidget( Colors_Widget  );
   edges_t   = XscTipDeriveFromWidget( Edges_Widget   );
   flags_t   = XscTipDeriveFromWidget( Flags_Widget   );
   font_t    = XscTipDeriveFromWidget( Font_Widget    );
   offsets_t = XscTipDeriveFromWidget( Offsets_Widget );
   style_t   = XscTipDeriveFromWidget( Style_Widget   );
   timers_t  = XscTipDeriveFromWidget( Timers_Widget  );
   
   if (h && t)
   { 
      if (data->set)
      {
         String name;
         unsigned char cvt;
         
         XtVaGetValues( w, XmNuserData, &name, NULL );
         
         if (strcmp( name, "XmXSC_STRING_CONVERTER_STANDARD" ) == 0) 
         {
            cvt = XmXSC_STRING_CONVERTER_STANDARD;
         }
         else if (strcmp( name, "XmXSC_STRING_CONVERTER_FONT_TAG"  ) == 0)
         {
            cvt = XmXSC_STRING_CONVERTER_FONT_TAG;
         }
         else
         {
            cvt = XmXSC_STRING_CONVERTER_SEGMENTED;
         }

         XscHintSetStringConverter( h        , cvt, True );
         XscHintSetStringConverter( colors_h , cvt, True );
         XscHintSetStringConverter( edges_h  , cvt, True );
         XscHintSetStringConverter( flags_h  , cvt, True );
         XscHintSetStringConverter( font_h   , cvt, True );
         XscHintSetStringConverter( offsets_h, cvt, True );
         XscHintSetStringConverter( style_h  , cvt, True );
         XscHintSetStringConverter( timers_h , cvt, True );

         XscTipSetStringConverter( t, cvt, True );
         XscTipSetStringConverter( colors_t , cvt, True );
         XscTipSetStringConverter( edges_t  , cvt, True );
         XscTipSetStringConverter( flags_t  , cvt, True );
         XscTipSetStringConverter( font_t   , cvt, True );
         XscTipSetStringConverter( offsets_t, cvt, True );
         XscTipSetStringConverter( style_t  , cvt, True );
         XscTipSetStringConverter( timers_t , cvt, True );
         
         {
            unsigned char t_cvt, h_cvt;

            h_cvt = XscHintGetStringConverter( h );
            t_cvt = XscTipGetStringConverter ( t );

            printf( "Converter is: %d,%d,%d\n", cvt, h_cvt, t_cvt );
         }
      }
      else
      {
         XscHintSetStringConverter( h, 0, True );
         XscTipSetStringConverter ( t, 0, True );
      }
      XscHelpUpdate( w );
   }
}

static void changeDirectionCB( Widget w, XtPointer cd, XtPointer cbd )
{
   XmToggleButtonCallbackStruct* data = (XmToggleButtonCallbackStruct*) cbd;

   String resource = (String) cd;
   
   XtPointer h;
   XtPointer colors_h;
   XtPointer edges_h;
   XtPointer flags_h;
   XtPointer font_h;
   XtPointer offsets_h;
   XtPointer style_h;
   XtPointer timers_h;

   XtPointer t;
   XtPointer colors_t;
   XtPointer edges_t;
   XtPointer flags_t;
   XtPointer font_t;
   XtPointer offsets_t;
   XtPointer style_t;
   XtPointer timers_t;

   if (!Timers_Widget) return;
   
   h         = XscHintDeriveFromWidget( w );
   colors_h  = XscHintDeriveFromWidget( Colors_Widget  );
   edges_h   = XscHintDeriveFromWidget( Edges_Widget   );
   flags_h   = XscHintDeriveFromWidget( Flags_Widget   );
   font_h    = XscHintDeriveFromWidget( Font_Widget    );
   offsets_h = XscHintDeriveFromWidget( Offsets_Widget );
   style_h   = XscHintDeriveFromWidget( Style_Widget   );
   timers_h  = XscHintDeriveFromWidget( Timers_Widget  );

   t         = XscTipDeriveFromWidget( w );
   colors_t  = XscTipDeriveFromWidget( Colors_Widget  );
   edges_t   = XscTipDeriveFromWidget( Edges_Widget   );
   flags_t   = XscTipDeriveFromWidget( Flags_Widget   );
   font_t    = XscTipDeriveFromWidget( Font_Widget    );
   offsets_t = XscTipDeriveFromWidget( Offsets_Widget );
   style_t   = XscTipDeriveFromWidget( Style_Widget   );
   timers_t  = XscTipDeriveFromWidget( Timers_Widget  );
   
   if (h && t)
   { 
      if (data->set)
      {
         String name;
         unsigned char dir;
         
         XtVaGetValues( w, XmNuserData, &name, NULL );
         
         if (strcmp( name, "XmSTRING_DIRECTION_L_TO_R" ) == 0) 
         {
            dir = XmSTRING_DIRECTION_L_TO_R;
         }
         else
         {
            dir = XmSTRING_DIRECTION_R_TO_L;
         }

         XscHintSetStringDirection( h        , dir, True );
         XscHintSetStringDirection( colors_h , dir, True );
         XscHintSetStringDirection( edges_h  , dir, True );
         XscHintSetStringDirection( flags_h  , dir, True );
         XscHintSetStringDirection( font_h   , dir, True );
         XscHintSetStringDirection( offsets_h, dir, True );
         XscHintSetStringDirection( style_h  , dir, True );
         XscHintSetStringDirection( timers_h , dir, True );

         XscTipSetStringDirection( t, dir, True );
         XscTipSetStringDirection( colors_t , dir, True );
         XscTipSetStringDirection( edges_t  , dir, True );
         XscTipSetStringDirection( flags_t  , dir, True );
         XscTipSetStringDirection( font_t   , dir, True );
         XscTipSetStringDirection( offsets_t, dir, True );
         XscTipSetStringDirection( style_t  , dir, True );
         XscTipSetStringDirection( timers_t , dir, True );
         
         {
            unsigned char t_dir, h_dir;

            h_dir = XscHintGetStringDirection( h );
            t_dir = XscTipGetStringDirection ( t );

            printf( "Direction is: %d,%d,%d\n", dir, h_dir, t_dir );
         }
      }
      else
      {
         XscHintSetStringDirection( h, 0, True );
         XscTipSetStringDirection ( t, 0, True );
      }
      XscHelpUpdate( w );
   }
}

static void changeShadowTypeCB( Widget w, XtPointer cd, XtPointer cbd )
{
   XmToggleButtonCallbackStruct* data = (XmToggleButtonCallbackStruct*) cbd;

   String resource = (String) cd;
   
   XtPointer t;
   XtPointer colors_t;
   XtPointer edges_t;
   XtPointer flags_t;
   XtPointer font_t;
   XtPointer offsets_t;
   XtPointer style_t;
   XtPointer timers_t;

   if (!Timers_Widget) return;
   
   t         = XscTipDeriveFromWidget( w );
   colors_t  = XscTipDeriveFromWidget( Colors_Widget  );
   edges_t   = XscTipDeriveFromWidget( Edges_Widget   );
   flags_t   = XscTipDeriveFromWidget( Flags_Widget   );
   font_t    = XscTipDeriveFromWidget( Font_Widget    );
   offsets_t = XscTipDeriveFromWidget( Offsets_Widget );
   style_t   = XscTipDeriveFromWidget( Style_Widget   );
   timers_t  = XscTipDeriveFromWidget( Timers_Widget  );
   
   if (t)
   { 
      if (data->set)
      {
         String name;
         unsigned char st;
         
         XtVaGetValues( w, XmNuserData, &name, NULL );
         
         if (strcmp( name, "XmSHADOW_IN" ) == 0) 
         {
            st = XmSHADOW_IN;
         }
         else if (strcmp( name, "XmSHADOW_OUT" ) == 0) 
         {
            st = XmSHADOW_OUT;
         }
         else if (strcmp( name, "XmSHADOW_ETCHED_IN" ) == 0) 
         {
            st = XmSHADOW_ETCHED_IN;
         }
         else
         {
            st = XmSHADOW_ETCHED_OUT;
         }

         XscTipSetShadowType( t        , st );
         XscTipSetShadowType( colors_t , st );
         XscTipSetShadowType( edges_t  , st );
         XscTipSetShadowType( flags_t  , st );
         XscTipSetShadowType( font_t   , st );
         XscTipSetShadowType( offsets_t, st );
         XscTipSetShadowType( style_t  , st );
         XscTipSetShadowType( timers_t , st );
         
         {
            unsigned char t_st, h_st;

            h_st = 0;
            t_st = XscTipGetShadowType( t );

            printf( "Shadow Type is: %d,%d,%d\n", st, h_st, t_st );
         }
      }
      else
      {
         XscTipSetShadowType( t, XmSHADOW_IN );
      }
      XscHelpUpdate( w );
   }
}

static void changePositionCB( Widget w, XtPointer cd, XtPointer cbd )
{
   XmToggleButtonCallbackStruct* data = (XmToggleButtonCallbackStruct*) cbd;

   String resource = (String) cd;
   
   XtPointer t;
   XtPointer colors_t;
   XtPointer edges_t;
   XtPointer flags_t;
   XtPointer font_t;
   XtPointer offsets_t;
   XtPointer style_t;
   XtPointer timers_t;

   if (!Timers_Widget) return;
   
   t         = XscTipDeriveFromWidget( w );
   colors_t  = XscTipDeriveFromWidget( Colors_Widget  );
   edges_t   = XscTipDeriveFromWidget( Edges_Widget   );
   flags_t   = XscTipDeriveFromWidget( Flags_Widget   );
   font_t    = XscTipDeriveFromWidget( Font_Widget    );
   offsets_t = XscTipDeriveFromWidget( Offsets_Widget );
   style_t   = XscTipDeriveFromWidget( Style_Widget   );
   timers_t  = XscTipDeriveFromWidget( Timers_Widget  );
   
   if (t)
   { 
      if (data->set)
      {
         String name;
         unsigned char pos;
         
         XtVaGetValues( w, XmNuserData, &name, NULL );
         
         if (strcmp( name, "XmXSC_TIP_POSITION_POINTER" ) == 0) 
         {
            pos = XmXSC_TIP_POSITION_POINTER;
         }
         else if (strcmp( name, "XmXSC_TIP_POSITION_TOP_LEFT" ) == 0) 
         {
            pos = XmXSC_TIP_POSITION_TOP_LEFT;
         }
         else if (strcmp( name, "XmXSC_TIP_POSITION_TOP_RIGHT" ) == 0) 
         {
            pos = XmXSC_TIP_POSITION_TOP_RIGHT;
         }
         else if (strcmp( name, "XmXSC_TIP_POSITION_BOTTOM_LEFT" ) == 0) 
         {
            pos = XmXSC_TIP_POSITION_BOTTOM_LEFT;
         }
         else
         {
            pos = XmXSC_TIP_POSITION_BOTTOM_RIGHT;
         }

         XscTipSetPosition( t        , pos );
         XscTipSetPosition( colors_t , pos );
         XscTipSetPosition( edges_t  , pos );
         XscTipSetPosition( flags_t  , pos );
         XscTipSetPosition( font_t   , pos );
         XscTipSetPosition( offsets_t, pos );
         XscTipSetPosition( style_t  , pos );
         XscTipSetPosition( timers_t , pos );
         
         {
            unsigned char t_pos, h_pos;

            h_pos = 0;
            t_pos = XscTipGetPosition( t );

            printf( "Position is: %d,%d,%d\n", pos, h_pos, t_pos );
         }
      }
      else
      {
         XscTipSetPosition( t, XmXSC_TIP_POSITION_POINTER );
      }
      XscHelpUpdate( w );
   }
}

static void changeAlignmentCB( Widget w, XtPointer cd, XtPointer cbd )
{
   XmToggleButtonCallbackStruct* data = (XmToggleButtonCallbackStruct*) cbd;

   String resource = (String) cd;
   
   XtPointer h;
   XtPointer colors_h;
   XtPointer edges_h;
   XtPointer flags_h;
   XtPointer font_h;
   XtPointer offsets_h;
   XtPointer style_h;
   XtPointer timers_h;

   XtPointer t;
   XtPointer colors_t;
   XtPointer edges_t;
   XtPointer flags_t;
   XtPointer font_t;
   XtPointer offsets_t;
   XtPointer style_t;
   XtPointer timers_t;

   if (!Timers_Widget) return;
   
   h         = XscHintDeriveFromWidget( w );
   colors_h  = XscHintDeriveFromWidget( Colors_Widget  );
   edges_h   = XscHintDeriveFromWidget( Edges_Widget   );
   flags_h   = XscHintDeriveFromWidget( Flags_Widget   );
   font_h    = XscHintDeriveFromWidget( Font_Widget    );
   offsets_h = XscHintDeriveFromWidget( Offsets_Widget );
   style_h   = XscHintDeriveFromWidget( Style_Widget   );
   timers_h  = XscHintDeriveFromWidget( Timers_Widget  );

   t         = XscTipDeriveFromWidget( w );
   colors_t  = XscTipDeriveFromWidget( Colors_Widget  );
   edges_t   = XscTipDeriveFromWidget( Edges_Widget   );
   flags_t   = XscTipDeriveFromWidget( Flags_Widget   );
   font_t    = XscTipDeriveFromWidget( Font_Widget    );
   offsets_t = XscTipDeriveFromWidget( Offsets_Widget );
   style_t   = XscTipDeriveFromWidget( Style_Widget   );
   timers_t  = XscTipDeriveFromWidget( Timers_Widget  );
   
   if (h && t)
   { 
      if (data->set)
      {
         String name;
         unsigned char align;
         
         XtVaGetValues( w, XmNuserData, &name, NULL );
         
         if (strcmp( name, "XmALIGNMENT_BEGINNING" ) == 0) 
         {
            align = XmALIGNMENT_BEGINNING;
         }
         else if (strcmp( name, "XmALIGNMENT_CENTER" ) == 0) 
         {
            align = XmALIGNMENT_CENTER;
         }
         else
         {
            align = XmALIGNMENT_END;
         }

         XscHintSetAlignment( h        , align, True );
         XscHintSetAlignment( colors_h , align, True );
         XscHintSetAlignment( edges_h  , align, True );
         XscHintSetAlignment( flags_h  , align, True );
         XscHintSetAlignment( font_h   , align, True );
         XscHintSetAlignment( offsets_h, align, True );
         XscHintSetAlignment( style_h  , align, True );
         XscHintSetAlignment( timers_h , align, True );
         
         XscTipSetAlignment( t        , align, True );
         XscTipSetAlignment( colors_t , align, True );
         XscTipSetAlignment( edges_t  , align, True );
         XscTipSetAlignment( flags_t  , align, True );
         XscTipSetAlignment( font_t   , align, True );
         XscTipSetAlignment( offsets_t, align, True );
         XscTipSetAlignment( style_t  , align, True );
         XscTipSetAlignment( timers_t , align, True );
         
         {
            unsigned char t_align, h_align;

            h_align = XscHintGetAlignment( h );
            t_align = XscTipGetAlignment ( t );

            printf( "Position is: %d,%d,%d\n", align, h_align, t_align );
         }
      }
      else
      {
         XscHintSetAlignment( h, XmALIGNMENT_CENTER, True );
         XscTipSetAlignment ( t, XmALIGNMENT_CENTER, True );
      }
      XscHelpUpdate( w );
   }
}


static void cueShellEnableCB( Widget w, XtPointer cd, XtPointer cbd )
{
   XmToggleButtonCallbackStruct* _data = (XmToggleButtonCallbackStruct*) cbd;  
   XscHelpSetCuesEnabledOnShell( w, _data->set );
}
static void cueGlobalEnableCB( Widget w, XtPointer cd, XtPointer cbd )
{
   XmToggleButtonCallbackStruct* _data = (XmToggleButtonCallbackStruct*) cbd;
   XscHelpSetCuesEnabledGlobally( _data->set );
}
static void hintShellEnableCB( Widget w, XtPointer cd, XtPointer cbd )
{
   XmToggleButtonCallbackStruct* _data = (XmToggleButtonCallbackStruct*) cbd;  
   XscHelpSetHintsEnabledOnShell( w, _data->set );
}
static void hintGlobalEnableCB( Widget w, XtPointer cd, XtPointer cbd )
{
   XmToggleButtonCallbackStruct* _data = (XmToggleButtonCallbackStruct*) cbd;
   XscHelpSetHintsEnabledGlobally( _data->set );
}
static void tipShellEnableCB( Widget w, XtPointer cd, XtPointer cbd )
{
   XmToggleButtonCallbackStruct* _data = (XmToggleButtonCallbackStruct*) cbd;  
   XscHelpSetTipsEnabledOnShell( w, _data->set );
}
static void tipGlobalEnableCB( Widget w, XtPointer cd, XtPointer cbd )
{
   XmToggleButtonCallbackStruct* _data = (XmToggleButtonCallbackStruct*) cbd;
   XscHelpSetTipsEnabledGlobally( _data->set );
}

/* Old Callbacks ------------------------------------------------------------*/

/*---------------------------------------------------------------------------
-- This structure is used to associate a resource name and a symbolic value
---------------------------------------------------------------------------*/
typedef struct _NameValueRec
{
   String name;
   String value;
}
NameValueRec;


/*-----------------------------------------------------------------------------
-- The next code block is used only if CDE is not (apparently) available.
-- If CDE is available, then the functions from CdeHelp.c are used instead.
-----------------------------------------------------------------------------*/
#ifndef USE_CDE

   /*----------------------------------------------------------------------
   -- These are the (CDE) attributes extracted from the resource database
   -- when context-sensitive help is requested
   ----------------------------------------------------------------------*/
   typedef struct _HelpDataStruct
   {
      String       text;
      Boolean      manage;
   }
   HelpDataStruct;

   /*----------------------------------------------------------
   -- This help callback simply prints out the help attibutes
   ----------------------------------------------------------*/
   void helpCB( Widget w, XtPointer cd, XtPointer cbd )
   {
      XscHelpContextCallbackStruct* help_data;
      HelpDataStruct*               help;
      char*                         text   = NULL;
      char*                         manage = NULL;

      help_data = (XscHelpContextCallbackStruct*) cbd;
      help      = (HelpDataStruct*) help_data->data;

      if (help)
      {
	 text = help->text;

	 switch( help->manage )
	 {
	 case 0:
            manage = "False";
            break;

	 case 1:
            manage = "True";
            break;

	 default:
            manage = "(huh)";
	 }
      }
      if (!text)
      {
	 text = "(null)";
      }

      printf( 
	 "\nXsc Help ToolKit callback received!\n"
	    "This is the demo replacement for the Help ToolKit CDE functions.\n"
            "Manage: %s\nTopic:  \"%s\"\n",
	 manage,
	 text );
   }

#endif


/*------------------------------------------------------------------------
-- This callback is called when a radio button is selected.  It modifies
-- the resource database to change the hint/tip behavior of the tabs
-- based on the selected radio button.
------------------------------------------------------------------------*/
void radioCB( Widget w, XtPointer cd, XtPointer cbd )
{
   XmToggleButtonCallbackStruct* data = (XmToggleButtonCallbackStruct*) cbd;
   char* value = (char*) cd;
   
   if (data->set)
   {
      char buffer[ 100 ];
      String resource;
      
      XrmDatabase db = XrmGetDatabase( XtDisplay( w ) );
      
      /*------------------------------------------------------------
      -- Get the resource name associated with the selected widget
      ------------------------------------------------------------*/
      XtVaGetValues( w, XmNuserData, &resource, NULL );
      
      /*-------------------------------
      -- Update the resource database
      -------------------------------*/
      sprintf( buffer, "Demo*switch*xscTip%s: %s", resource, value );
      XrmPutLineResource( &db, buffer );

      sprintf( buffer, "Demo*switch*xscHint%s: %s", resource, value );
      XrmPutLineResource( &db, buffer );

#if Hmmm
      if (strcmp( resource, XmNmanage ) == 0)
      {
         /*--------------------------------------------------------------
         -- Don't bother updating the context-sensitive help attributes
         -- unless it is the XmNmanage resource; its the only one used
         -- be the context-sensitive help in this demo.
         --------------------------------------------------------------*/
	 sprintf( buffer, "Demo*switch*xscContext%s: %s", resource, value );
	 XrmPutLineResource( &db, buffer );
      }
#endif
   }
}


/*--------------------------------------------------------------------
-- This callback is called when a scale is modified.  It modifies
-- the resource database to change the hint/tip behavior of the tabs
-- based on the new scale value.
--------------------------------------------------------------------*/
void scaleCB( Widget w, XtPointer cd, XtPointer cbd )
{
   XmScaleCallbackStruct* data = (XmScaleCallbackStruct*) cbd;
   char* resource = (char*) cd;
   char buffer[ 100 ];
   XrmDatabase db = XrmGetDatabase( XtDisplay( w ) );
   
   /*-------------------------------
   -- Update the resource database
   -------------------------------*/
   sprintf( buffer, "Demo*switch*xscTip%s: %d", resource, data->value );
   XrmPutLineResource( &db, buffer );
   
   sprintf( buffer, "Demo*switch*xscHint%s: %d", resource, data->value );
   XrmPutLineResource( &db, buffer );
}


/*---------------------------------------------------------------
-- This callback is called when a "tab" is selected in order to
-- manually change the displayed panel.
---------------------------------------------------------------*/
void switchCB( Widget w, XtPointer cd, XtPointer cbd )
{
   static Widget current_context = NULL;
   XmToggleButtonCallbackStruct* data = (XmToggleButtonCallbackStruct*) cbd;
   Widget new_context = (Widget) cd;
   
   if (data->set)
   {
      if (current_context != new_context)
      {
         if (current_context)
         {
            XtUnmanageChild( current_context );
            current_context = NULL;
         }
         if (new_context)
         {
            XtManageChild( new_context );
            current_context = new_context;
         }
      }
   }
}


/*-----------------------------------------------------------------
-- This function is used to generically setup a Boolean radio box
------------------------------------------------------------------*/
Widget addBoolean(
   String         name,
   String         label,
   Widget         parent,
   Widget         top_attachment,
   String         resource,
   int            initial,
   XtCallbackProc callback )
{
   Widget   frame, container, button;
   XmString cs;
   
   frame = XtVaCreateManagedWidget(
      name, xmFrameWidgetClass, parent, 
      XmNleftAttachment,    XmATTACH_FORM,
      XmNrightAttachment,   XmATTACH_FORM,
      XmNtopAttachment,     (top_attachment ? XmATTACH_WIDGET : XmATTACH_FORM),
      XmNtopWidget,         top_attachment,
      XmNleftOffset,        0,
      XmNrightOffset,       0,
      XmNtopOffset,         (top_attachment ? 5 : 0),
      NULL );
   
   container = XtVaCreateManagedWidget( 
      "container", xmRowColumnWidgetClass, frame, 
      XmNorientation,           XmHORIZONTAL,
      XmNradioBehavior,         True,
      NULL);
      
   cs = XmStringCreateLocalized( label );
   XtVaCreateManagedWidget( 
      "title", xmLabelWidgetClass, frame, 
      XmNchildType,     XmFRAME_TITLE_CHILD,
      XmNlabelString,   cs,
      NULL );
   XmStringFree( cs );
   
   
   cs = XmStringCreateLocalized( "True" );
   button = XtVaCreateManagedWidget(
      "true", xmToggleButtonWidgetClass, container,
      XmNlabelString, cs,
      XmNuserData,    1,
      NULL );
   XmStringFree( cs );
   XtAddCallback( button, XmNvalueChangedCallback, callback, resource );
      
   if (initial == 0)
   {
      XmToggleButtonSetState( button, True, True );
   }

   cs = XmStringCreateLocalized( "False" );
   button = XtVaCreateManagedWidget(
      "false", xmToggleButtonWidgetClass, container,
      XmNlabelString, cs,
      XmNuserData,    0,
      NULL );
   XmStringFree( cs );
   XtAddCallback( button, XmNvalueChangedCallback, callback, resource );
      
   if (initial == 1)
   {
      XmToggleButtonSetState( button, True, True );
   }
   return frame;
}


/*----------------------------------------------------------------
-- This function is used to generically setup a generic radio box
-----------------------------------------------------------------*/
Widget addRadioBox(
   String         name,
   String         label,
   Widget         parent,
   Widget         left_attachment,
   NameValueRec*  btn,
   Cardinal       btn_count,
   String         resource,
   int            initial,
   XtCallbackProc callback )
{
   Widget frame, container, button;
   int i;
   XmString cs;
   
   frame = XtVaCreateManagedWidget(
      name, xmFrameWidgetClass, parent, 
      XmNleftAttachment,    (left_attachment ? XmATTACH_WIDGET : XmATTACH_FORM),
      XmNtopAttachment,     XmATTACH_FORM,
      XmNbottomAttachment,  XmATTACH_FORM,
      XmNleftWidget,        left_attachment,
      XmNleftOffset,        (left_attachment ? 5 : 0),
      XmNtopOffset,         0,
      XmNbottomOffset,      0,
      NULL );
   
   container = XtVaCreateManagedWidget( 
      "container", xmRowColumnWidgetClass, frame, 
      XmNorientation,	XmVERTICAL,
      XmNradioBehavior, True,
      NULL);
      
   cs = XmStringCreateLocalized( label );
   XtVaCreateManagedWidget( 
      "title", xmLabelWidgetClass, frame, 
      XmNchildType,     XmFRAME_TITLE_CHILD,
      XmNlabelString,   cs,
      NULL );
   XmStringFree( cs );
   
   for (i = 0; i < btn_count; i++)
   {
      cs = XmStringCreateLocalized( btn[i].value );
      button = XtVaCreateManagedWidget(
         btn[i].name, xmToggleButtonWidgetClass, container,
         XmNlabelString, cs,
         XmNuserData,    btn[i].value,
         NULL );
      XmStringFree( cs );
      
      if (callback)
      {
         XtAddCallback( button,XmNvalueChangedCallback,callback,resource );
      }
      else
      {
         XtAddCallback( button,XmNvalueChangedCallback,radioCB,resource );
      }
      
      if (initial == i)
      {
         XmToggleButtonSetState( button, True, True );
      }
   }
   return frame;
}


/*------------------------------------------------------------
-- This function is used to generically setup a scale widget
------------------------------------------------------------*/
Widget addScale( 
   String name,
   String label,
   Widget parent,
   Widget top_attachment,
   int    minimum,
   int    maximum,
   String resource,
   int    value,
   XtCallbackProc callback )
{
   Widget scale;
   XmString cs;
   
   cs = XmStringCreateLocalized( label );
   scale = XtVaCreateManagedWidget(
      name, xmScaleWidgetClass, parent,
      XmNminimum,            minimum,
      XmNmaximum,            maximum,
      XmNorientation,        XmHORIZONTAL,
      XmNleftAttachment,     XmATTACH_FORM,
      XmNrightAttachment,    XmATTACH_FORM,
      XmNtopAttachment,      (top_attachment ? XmATTACH_WIDGET : XmATTACH_FORM),
      XmNtopWidget,          top_attachment,
      XmNleftOffset,         0,
      XmNrightOffset,        0,
      XmNtopOffset,          (top_attachment ? 5 : 0),
      XmNshowValue,          True,
      XmNvalue,              value,
      XmNtitleString,        cs,      
      NULL );
   XmStringFree( cs );
 
   if (!callback) callback = scaleCB;
   
   XtAddCallback( scale, XmNvalueChangedCallback, callback, resource );
   return scale;
}


/*----------------------------------------------
-- The main() function sets up the entire demo
----------------------------------------------*/
int main( argc, argv )
   int    argc;
   char** argv;
{
   XtAppContext app_context;
   Widget       toplevel;
   Widget       mainwindow, workarea, tab_manager;
   Widget       color_form, edges_form, flags_form;
   Widget       font_form, offset_form, style_form, timer_form;
   Widget       scale, hint_frame, hint;
   Widget       frame, context, button;
   Widget	manager;
   Cardinal	i;
   Arg		args[ 20 ];
   XmString     cs;

/*----------------------------------------------------
-- This block is only needed if CDE is not available
----------------------------------------------------*/
#  ifndef USE_CDE
#
#     define OFFSET_OF( mem ) XtOffsetOf( HelpDataStruct, mem )

      static XtResource resource[] =
      {
	 {
            "xscCdeTopic", "XscCdeTopic", 
            XmRString, sizeof( String ), OFFSET_OF( text ),
            XtRImmediate, NULL
	 },{
            "xscCdeManage", "XscCdeManage",
            XmRBoolean, sizeof( Boolean ), OFFSET_OF( manage ),
            XtRImmediate, (XtPointer) (Boolean) True
	 }
      };
#     undef OFFSET_OF
#
#  endif

   /*--------------------------------------------------------------
   -- Make the app-default file visible when it is in the present
   -- working directory.
   --------------------------------------------------------------*/
   putenv( "XENVIRONMENT=./Demo" );
   
   /*-----------------------------------------------------------
   -- Create the top level shell and initialize the Intrinsics
   -----------------------------------------------------------*/
   toplevel = XtAppInitialize( 
      &app_context, 
      "Demo", 
      NULL,    0,
      &argc,   argv,
      NULL, 
      NULL,    (Cardinal)0 );
   XtVaSetValues( toplevel, XmNallowShellResize, True, NULL );
   
   
   {      
      int _status = XscHelpLoadTopics( XtDisplay( toplevel ), "Topics" );
      if (_status != 0)
      {
      	 printf( "**** ERROR NO: %d\n", _status );
      }
   }
   /*-----------------------------------------------------------------------
   -- Install the Help ToolKit library, either with or without CDE support
   -----------------------------------------------------------------------*/
#  ifndef USE_CDE
#
      XscHelpContextInstall( 
         resource, 
         XtNumber( resource ), 
         sizeof( HelpDataStruct ), 
         helpCB, 
         NULL );
      XscHelpInstall( toplevel );
#
#  else
#
      XscCdeHelpInstall( toplevel, "DemoHelpVolume" );
#
#  endif

   /*------------------------------------------------------------------------
   -- Allow EditRes to work to allow easier examination and experimentation
   ------------------------------------------------------------------------*/
   XtAddEventHandler( toplevel,(EventMask)0,True,_XEditResCheckMessages,NULL );

   /*--------------------------------
   -- Create the top-level managers
   --------------------------------*/
   i = 0;
   mainwindow = XmCreateMainWindow( toplevel, "main", args, i );
   XtManageChild( mainwindow );
   
   workarea = XtVaCreateManagedWidget( 
      "workArea", xmFormWidgetClass, mainwindow, NULL );   
   
   /*---------------------------------------------------
   -- Create a container for the "hint display" widget
   ---------------------------------------------------*/
   hint_frame = XtVaCreateManagedWidget(
      "hintFrame", xmFrameWidgetClass, workarea, 
      XmNleftAttachment,        XmATTACH_FORM,
      XmNrightAttachment,       XmATTACH_FORM,
      XmNbottomAttachment,      XmATTACH_FORM,
      XmNleftOffset,            5,
      XmNrightOffset,           5,
      XmNbottomOffset,          5,
      XmNshadowType,            XmSHADOW_IN,
      XmNshadowThickness,       1,
      NULL );
   
   /*--------------------------------------------------------------------
   -- Create the hint display widget.  This can be _any_widget (must
   -- _not_ be a gadget).  A label is chosen because it can be easily
   -- sized.  By specifying a multi-lined "blank" label string, the
   -- widget will be tall enough to display multiple hint line.  Since,
   -- the label string contains only whitespace, the demo program does
   -- not need to worry about the label expose method causing problems.
   --------------------------------------------------------------------*/
   cs = XmStringCreateLtoR( " \n ", XmFONTLIST_DEFAULT_TAG );
   hint = XtVaCreateManagedWidget(
      "hint", xmLabelWidgetClass, hint_frame,
      XmNlabelString, cs,
      NULL );
   XmStringFree( cs );
   
   /*-----------------------------------------
   -- Tell the librray about the hint widget
   -----------------------------------------*/
   XscHelpHintInstall( hint );

   /*-----------------------------------------------------------------
   -- Create the "tab manager" widget, which is either the Microline
   -- folder widget or a row column widget containing toggle buttons
   -----------------------------------------------------------------*/      
   frame = XtVaCreateManagedWidget(
      "switchFrame", xmFrameWidgetClass,  workarea, 
      XmNleftAttachment,        XmATTACH_FORM,
      XmNrightAttachment,       XmATTACH_FORM,
      XmNtopAttachment,         XmATTACH_FORM,
      XmNleftOffset,            0,
      XmNrightOffset,           0,
      XmNtopOffset,             0,
      XmNshadowThickness,       1,
      XmNshadowType,            XmSHADOW_ETCHED_IN,
      NULL );
      
   tab_manager = XtVaCreateManagedWidget( 
      "switch", xmRowColumnWidgetClass, frame, 
      XmNorientation,           XmHORIZONTAL,
      XmNradioBehavior,         True,
      NULL);   
   
   context = XtVaCreateManagedWidget( 
      "workArea", xmFormWidgetClass, workarea, 
      XmNleftAttachment,        XmATTACH_FORM,
      XmNrightAttachment,       XmATTACH_FORM,
      XmNtopAttachment,         XmATTACH_WIDGET,
      XmNtopWidget,             frame,
      XmNbottomAttachment,      XmATTACH_WIDGET,
      XmNbottomWidget,          hint_frame,
      XmNleftOffset,            0,
      XmNrightOffset,           0,
      XmNtopOffset,             0,
      XmNbottomOffset,          0,
      NULL ); 

   /*------------------------
   -- Set up the color form
   ------------------------*/
   cs = XmStringCreateLocalized( "Colors" );
   
   Colors_Widget = button = XtVaCreateManagedWidget(
      "colorBtn", xmToggleButtonWidgetClass, tab_manager,
      XmNlabelString,	   cs,
      XmNindicatorOn,	   False,
      XmNshadowThickness,  1,
      NULL );

   color_form = XtVaCreateWidget( 
      "colorForm", xmFormWidgetClass, context, 
      XmNleftAttachment,        XmATTACH_FORM,
      XmNrightAttachment,       XmATTACH_FORM,
      XmNtopAttachment,         XmATTACH_FORM,
      XmNbottomAttachment,      XmATTACH_FORM,
      XmNleftOffset,            5,
      XmNrightOffset,           5,
      XmNtopOffset,             5,
      XmNbottomOffset,          5,
      NULL );
   XtAddCallback( button, XmNvalueChangedCallback, switchCB, color_form );
   XmToggleButtonSetState( button, True, True );

   XmStringFree( cs );

   /*-----------------------------------------
   -- Create each of the "color" radio boxes
   -----------------------------------------*/
   {  
      NameValueRec name_value[] = 
      {
         { "black",  "Black"  },
         { "yellow", "Yellow" },
         { "blue",   "Blue"   },
         { "cyan",   "Cyan"   },
         { "red",    "Red"    },
         { "white",  "White"  }
      };
         
      manager = addRadioBox(
        "theBackground",        "Background",
        color_form,             NULL,
        name_value,             XtNumber( name_value ),
        "Background",          1, changeColorCB );
        
      manager = addRadioBox(
        "theForeground",        "Foreground",
        color_form,             manager,
        name_value,             XtNumber( name_value ),
        "Foreground",          0, changeColorCB );

      manager = addRadioBox(
        "theBorderColor",       "Border Color",
        color_form,             manager,
        name_value,             XtNumber( name_value ),
        "BorderColor",         0, changeColorCB );

      addRadioBox(
        "theShadowColorBase",   "Shadow Color",
        color_form,             manager,
        name_value,             XtNumber( name_value ),
        "ColorBase",     1, changeColorCB );
   } 
     
   
   /*------------------------
   -- Set up the edges form
   ------------------------*/
   cs = XmStringCreateLocalized( "Edges" );

   Edges_Widget = button = XtVaCreateManagedWidget(
      "edgesBtn", xmToggleButtonWidgetClass, tab_manager,
      XmNlabelString,	   cs,
      XmNindicatorOn,	   False,
      XmNshadowThickness,  1,
      NULL );
   
   edges_form = XtVaCreateWidget( 
      "edgesForm", xmFormWidgetClass, context,
      XmNleftAttachment,        XmATTACH_FORM,
      XmNrightAttachment,       XmATTACH_FORM,
      XmNtopAttachment,         XmATTACH_FORM,
      XmNbottomAttachment,      XmATTACH_FORM,
      XmNleftOffset,            5,
      XmNrightOffset,           5,
      XmNtopOffset,             5,
      XmNbottomOffset,          5,
      NULL );
   XtAddCallback( button, XmNvalueChangedCallback, switchCB, edges_form );

   XmStringFree( cs );
   
   /*--------------------------------------------------------------------
   -- Create each of the scales that control the margins, borders, etc.
   --------------------------------------------------------------------*/
   scale = addScale( 
      "theBorderWidth",         "Border Width", 
      edges_form,               NULL,
      0,                        10, 
      "BorderWidth",           1, changeDimensionCB );
   scale = addScale( 
      "theShadowThickness",     "Shadow Thickness", 
      edges_form,               scale, 
      0,                        10, 
      "ShadowThickness",       0, changeDimensionCB );
   scale = addScale( 
      "theMarginWidth",         "Margin Width", 
      edges_form,               scale, 
      0,                        10, 
      "MarginWidth",           2, changeDimensionCB );
   scale = addScale( 
      "theMarginHeight",        "Margin Height", 
      edges_form,               scale, 
      0,                        10, 
      "MarginHeight",          1, changeDimensionCB );
   scale = addScale( 
      "theMarginLeft",          "Margin Left", 
      edges_form,               scale, 
      0,                        10, 
      "MarginLeft",            0, changeDimensionCB );
   scale = addScale( 
      "theMarginRight",         "Margin Right", 
      edges_form,               scale, 
      0,                        10, 
      "MarginRight",           0, changeDimensionCB );
   scale = addScale( 
      "theMarginTop",           "Margin Top", 
      edges_form,               scale, 
      0,                        10, 
      "MarginTop",             0, changeDimensionCB );
   scale = addScale( 
      "theMarginBottom",        "Margin Bottom", 
      edges_form,               scale, 
      0,                        10, 
      "MarginBottom",          0, changeDimensionCB );
 
   /*------------------------
   -- Set up the flags form
   ------------------------*/
   cs = XmStringCreateLocalized( "Flags" );

   Flags_Widget = button = XtVaCreateManagedWidget(
      "flagsBtn", xmToggleButtonWidgetClass, tab_manager,
      XmNlabelString,	   cs,
      XmNindicatorOn,	   False,
      XmNshadowThickness,  1,
      NULL );
   
   flags_form = XtVaCreateWidget( 
      "flagsForm", xmFormWidgetClass, context, 
      XmNleftAttachment,        XmATTACH_FORM,
      XmNrightAttachment,       XmATTACH_FORM,
      XmNtopAttachment,         XmATTACH_FORM,
      XmNbottomAttachment,      XmATTACH_FORM,
      XmNleftOffset,            5,
      XmNrightOffset,           5,
      XmNtopOffset,             5,
      XmNbottomOffset,          5,
      NULL );
   XtAddCallback( button, XmNvalueChangedCallback, switchCB, flags_form );

   XmStringFree( cs );
   
   /*-----------------------------------------
   -- Create each of the Boolean radio boxes
   -----------------------------------------*/
   manager = addBoolean(
     "theManage",    "Manage",
     flags_form,     NULL,
     "Manage",      0, changeBooleanCB );

   manager = addBoolean(
     "theColorModel",    "Motif Color Model",
     flags_form,         manager,
     "MotifColorModel", 1, changeBooleanCB );   

   manager = addBoolean(
     "theBackgroundFromParent", "Background From Parent",
     flags_form,         manager,
     "InheritBackground", 1, changeBooleanCB );   

   
   /*-----------------------
   -- Set up the font form
   -----------------------*/
    
   cs = XmStringCreateLocalized( "Font" );
   
   Font_Widget = button = XtVaCreateManagedWidget(
      "fontBtn", xmToggleButtonWidgetClass, tab_manager,
      XmNlabelString,	   cs,
      XmNindicatorOn,	   False,
      XmNshadowThickness,  1,
      NULL );

   font_form = XtVaCreateWidget( 
      "fontForm", xmFormWidgetClass, context, 
      XmNleftAttachment,        XmATTACH_FORM,
      XmNrightAttachment,       XmATTACH_FORM,
      XmNtopAttachment,         XmATTACH_FORM,
      XmNbottomAttachment,      XmATTACH_FORM,
      XmNleftOffset,            5,
      XmNrightOffset,           5,
      XmNtopOffset,             5,
      XmNbottomOffset,          5,
      NULL );
   XtAddCallback( button, XmNvalueChangedCallback, switchCB, font_form );

   XmStringFree( cs );

   /*--------------------------------------
   -- Create each of the font radio boxes
   --------------------------------------*/
   {  
      NameValueRec name_value[] = 
      {
         { "normal", "Normal" },
         { "bold"  , "Bold"   },
         { "italic", "Italic" }
      };
      
      manager = addRadioBox(
        "theTag",       "Font List Tag",
        font_form,      NULL,
        name_value,     XtNumber( name_value ),
        "FontListTag", 0, changeFontListTagCB );
   }      
   {  
      NameValueRec name_value[] = 
      {
         { "standad"  , "XmXSC_STRING_CONVERTER_STANDARD"  },
         { "fontTag"  , "XmXSC_STRING_CONVERTER_FONT_TAG"  },
         { "segmented", "XmXSC_STRING_CONVERTER_SEGMENTED" }
      };
      
      manager = addRadioBox(
        "theStrCvt",            "String Converter",
        font_form,              manager,
        name_value,             XtNumber( name_value ),
        "StringConverter",     0, changeConverterCB );
   }      
   {  
      NameValueRec name_value[] = 
      {
         { "lToR", "XmSTRING_DIRECTION_L_TO_R" },
         { "rToL", "XmSTRING_DIRECTION_R_TO_L" }
      };
      
      manager = addRadioBox(
        "theStrDir",            "String Direction",
        font_form,              manager,
        name_value,             XtNumber( name_value ),
        "StringDirection",     0, changeDirectionCB );
   }      
   
   
   /*-------------------------
   -- Set up the offset form
   -------------------------*/
   cs = XmStringCreateLocalized( "Offsets" );

   Offsets_Widget = button = XtVaCreateManagedWidget(
      "offsetBtn", xmToggleButtonWidgetClass, tab_manager,
      XmNlabelString,	   cs,
      XmNindicatorOn,	   False,
      XmNshadowThickness,  1,
      NULL );
      
   offset_form = XtVaCreateWidget( 
      "offsetForm", xmFormWidgetClass, context, 
      XmNleftAttachment,        XmATTACH_FORM,
      XmNrightAttachment,       XmATTACH_FORM,
      XmNtopAttachment,         XmATTACH_FORM,
      XmNbottomAttachment,      XmATTACH_FORM,
      XmNleftOffset,            5,
      XmNrightOffset,           5,
      XmNtopOffset,             5,
      XmNbottomOffset,          5,
      NULL );
   XtAddCallback( button, XmNvalueChangedCallback, switchCB, offset_form );

   XmStringFree( cs );

   /*------------------------------------------------------------
   -- Create the scale widgets that specify the X and Y offsets
   ------------------------------------------------------------*/
   scale = addScale( 
      "theXOffset",     "X Popup Offset", 
      offset_form,      NULL,
      -100,             100, 
      "XOffset",       0, changeIntCB );
   scale = addScale( 
      "theYOffset",     "Y Popup Offset", 
      offset_form,      scale,
      -100,             100, 
      "YOffset",       15, changeIntCB );

  
   /*------------------------
   -- Set up the style form
   ------------------------*/
   cs = XmStringCreateLocalized( "Style" );
   
   Style_Widget = button = XtVaCreateManagedWidget(
      "styleBtn", xmToggleButtonWidgetClass, tab_manager,
      XmNlabelString,	   cs,
      XmNindicatorOn,	   False,
      XmNshadowThickness,  1,
      NULL );

   style_form = XtVaCreateWidget( 
      "styleForm", xmFormWidgetClass, context,
      XmNleftAttachment,        XmATTACH_FORM,
      XmNrightAttachment,       XmATTACH_FORM,
      XmNtopAttachment,         XmATTACH_FORM,
      XmNbottomAttachment,      XmATTACH_FORM,
      XmNleftOffset,            5,
      XmNrightOffset,           5,
      XmNtopOffset,             5,
      XmNbottomOffset,          5,
      NULL );
   XtAddCallback( button, XmNvalueChangedCallback, switchCB, style_form );
   
   XmStringFree( cs );

   /*----------------------------------------
   -- Create the radio boxes for this sheet
   ----------------------------------------*/
   {  
      NameValueRec name_value[] = 
      {
         { "shadowIn"       , "XmSHADOW_IN"         },
         { "shadowOut"      , "XmSHADOW_OUT"        },
         { "shadowEtchedIn" , "XmSHADOW_ETCHED_IN"  },
         { "shadowEtchedOut", "XmSHADOW_ETCHED_OUT" }
      };
      
      manager = addRadioBox(
        "theShadowType",        "Shadow Type",
        style_form,             NULL,
        name_value,             XtNumber( name_value ),
        "ShadowType",          0, changeShadowTypeCB );
   }      
   {  
      NameValueRec name_value[] = 
      {
         { "posPointer"    , "XmXSC_TIP_POSITION_POINTER"      },
         { "posTopLeft"    , "XmXSC_TIP_POSITION_TOP_LEFT"     },
         { "posTopRight"   , "XmXSC_TIP_POSITION_TOP_RIGHT"    },
         { "posBottomLeft" , "XmXSC_TIP_POSITION_BOTTOM_LEFT"  },
         { "posBottomRight", "XmXSC_TIP_POSITION_BOTTOM_RIGHT" }
      };
      
      manager = addRadioBox(
        "thePosition",          "Popup Position",
        style_form,             manager,
        name_value,             XtNumber( name_value ),
        "Position",         0, changePositionCB );
   }      
   {  
      NameValueRec name_value[] = 
      {
         { "alignBegin" , "XmALIGNMENT_BEGINNING" },
         { "alignCenter", "XmALIGNMENT_CENTER"    },
         { "alignEnd"   , "XmALIGNMENT_END"       },
      };
      
      manager = addRadioBox(
        "theAlignment",         "Alignment",
        style_form,             manager,
        name_value,             XtNumber( name_value ),
        "Alignment",           0, changeAlignmentCB );
   }      
     
   
   /*------------------------
   -- Set up the timer form
   ------------------------*/
   cs = XmStringCreateLocalized( "Timers" );
   
   Timers_Widget = button = XtVaCreateManagedWidget(
      "timerBtn", xmToggleButtonWidgetClass, tab_manager,
      XmNlabelString,	   cs,
      XmNindicatorOn,	   False,
      XmNshadowThickness,  1,
      NULL );
      
   timer_form = XtVaCreateWidget( 
      "timerForm", xmFormWidgetClass, context,
      XmNleftAttachment,        XmATTACH_FORM,
      XmNrightAttachment,       XmATTACH_FORM,
      XmNtopAttachment,         XmATTACH_FORM,
      XmNbottomAttachment,      XmATTACH_FORM,
      XmNleftOffset,            5,
      XmNrightOffset,           5,
      XmNtopOffset,             5,
      XmNbottomOffset,          5,
      NULL );
   XtAddCallback( button, XmNvalueChangedCallback, switchCB, timer_form );

   XmStringFree( cs );
   
   /*------------------------------------------------
   -- Create the scale widgets neede for the timers
   ------------------------------------------------*/
   scale = addScale( 
      "thePopupInterval",       "Popup Interval", 
      timer_form,               NULL,
      0,                        3000,
      "PopupInterval",         1000, changeUnsignedLongCB );
   scale = addScale( 
      "thePopdownInterval",     "Popdown Interval", 
      timer_form,               scale,
      0,                        3000,
      "PopdownInterval",       0, changeUnsignedLongCB );

   /*--------------------------
   -- Set up the Enable form
   --------------------------*/
   {
      Widget _enableForm, _rc, _tb;
      
      cs = XmStringCreateLocalized( "Enable" );
      button = XtVaCreateManagedWidget(
         "enableBtn", xmToggleButtonWidgetClass, tab_manager,
         XmNlabelString,	   cs,
         XmNindicatorOn,	   False,
         XmNshadowThickness,  1,
         NULL );
      XmStringFree( cs );

      _enableForm = XtVaCreateWidget( 
         "enableForm", xmFormWidgetClass, context,
         XmNleftAttachment,        XmATTACH_FORM,
         XmNrightAttachment,       XmATTACH_FORM,
         XmNtopAttachment,         XmATTACH_FORM,
         XmNbottomAttachment,      XmATTACH_FORM,
         XmNleftOffset,            5,
         XmNrightOffset,           5,
         XmNtopOffset,             5,
         XmNbottomOffset,          5,
         NULL );
      XtAddCallback( button, XmNvalueChangedCallback, switchCB, _enableForm );

      _rc = XtVaCreateManagedWidget( 
         "enableRC", xmRowColumnWidgetClass, _enableForm,
         XmNleftAttachment,        XmATTACH_FORM,
         XmNrightAttachment,       XmATTACH_FORM,
         XmNtopAttachment,         XmATTACH_FORM,
         XmNbottomAttachment,      XmATTACH_FORM,
         XmNleftOffset,            5,
         XmNrightOffset,           5,
         XmNtopOffset,             5,
         XmNbottomOffset,          5,
         NULL );

      cs = XmStringCreateLocalized( "Enable Cues on shell" );
      _tb = XtVaCreateManagedWidget( 
         "cueShellEnableTB", xmToggleButtonWidgetClass, _rc, NULL );
      XtVaSetValues( _tb, XmNlabelString, cs, NULL );
      XmToggleButtonSetState( _tb, XscHelpAreCuesEnabledOnShell( _tb ), False );
      XtAddCallback( _tb, XmNvalueChangedCallback, cueShellEnableCB, NULL );
      XmStringFree( cs );
      
      cs = XmStringCreateLocalized( "Enable Cues globally" );
      _tb = XtVaCreateManagedWidget( 
         "cueGlobalEnableTB", xmToggleButtonWidgetClass, _rc, NULL );
      XtVaSetValues( _tb, XmNlabelString, cs, NULL );
      XmToggleButtonSetState( _tb, XscHelpAreCuesEnabledGlobally(), False );
      XtAddCallback( _tb, XmNvalueChangedCallback, cueGlobalEnableCB, NULL );
      XmStringFree( cs );

      cs = XmStringCreateLocalized( "Enable Hints on shell" );
      _tb = XtVaCreateManagedWidget( 
         "hintShellEnableTB", xmToggleButtonWidgetClass, _rc, NULL );
      XtVaSetValues( _tb, XmNlabelString, cs, NULL );
      XmToggleButtonSetState( _tb, XscHelpAreHintsEnabledOnShell( _tb ), False );
      XtAddCallback( _tb, XmNvalueChangedCallback, hintShellEnableCB, NULL );
      XmStringFree( cs );

      cs = XmStringCreateLocalized( "Enable Hints globally" );
      _tb = XtVaCreateManagedWidget( 
         "hintGlobalEnableTB", xmToggleButtonWidgetClass, _rc, NULL );
      XtVaSetValues( _tb, XmNlabelString, cs, NULL );
      XmToggleButtonSetState( _tb, XscHelpAreHintsEnabledGlobally(), False );
      XtAddCallback( _tb, XmNvalueChangedCallback, hintGlobalEnableCB, NULL );
      XmStringFree( cs );

      cs = XmStringCreateLocalized( "Enable Tips on shell" );
      _tb = XtVaCreateManagedWidget( 
         "tipShellEnableTB", xmToggleButtonWidgetClass, _rc, NULL );
      XtVaSetValues( _tb, XmNlabelString, cs, NULL );
      XmToggleButtonSetState( _tb, XscHelpAreTipsEnabledOnShell( _tb ), False );
      XtAddCallback( _tb, XmNvalueChangedCallback, tipShellEnableCB, NULL );
      XmStringFree( cs );

      cs = XmStringCreateLocalized( "Enable Tips globally" );
      _tb = XtVaCreateManagedWidget( 
         "tipGlobalEnableTB", xmToggleButtonWidgetClass, _rc, NULL );
      XtVaSetValues( _tb, XmNlabelString, cs, NULL );
      XmToggleButtonSetState( _tb, XscHelpAreTipsEnabledGlobally(), False );
      XtAddCallback( _tb, XmNvalueChangedCallback, tipGlobalEnableCB, NULL );
      XmStringFree( cs );
   }

   /*------------------------
   -- Set up the Test form
   ------------------------*/
   {
      Widget test_form, rc;
      
      cs = XmStringCreateLocalized( "Test" );
      button = XtVaCreateManagedWidget(
         "testBtn", xmToggleButtonWidgetClass, tab_manager,
         XmNlabelString,	   cs,
         XmNindicatorOn,	   False,
         XmNshadowThickness,  1,
         NULL );
      XmStringFree( cs );

      test_form = XtVaCreateWidget( 
         "testForm", xmFormWidgetClass, context,
         XmNleftAttachment,        XmATTACH_FORM,
         XmNrightAttachment,       XmATTACH_FORM,
         XmNtopAttachment,         XmATTACH_FORM,
         XmNbottomAttachment,      XmATTACH_FORM,
         XmNleftOffset,            5,
         XmNrightOffset,           5,
         XmNtopOffset,             5,
         XmNbottomOffset,          5,
         NULL );
      XtAddCallback( button, XmNvalueChangedCallback, switchCB, test_form );

      rc = XtVaCreateManagedWidget( 
         "testRC", xmRowColumnWidgetClass, test_form,
         XmNleftAttachment,        XmATTACH_FORM,
         XmNrightAttachment,       XmATTACH_FORM,
         XmNtopAttachment,         XmATTACH_FORM,
         XmNbottomAttachment,      XmATTACH_FORM,
         XmNleftOffset,            5,
         XmNrightOffset,           5,
         XmNtopOffset,             5,
         XmNbottomOffset,          5,
         NULL );

      XtVaCreateManagedWidget( 
         "labelWidget", xmLabelWidgetClass, rc, NULL );
      XtVaCreateManagedWidget( 
         "labelGadget", xmLabelGadgetClass, rc, NULL );
      XtVaCreateManagedWidget( 
         "pushButtonWidget", xmPushButtonWidgetClass, rc, NULL );
      XtVaCreateManagedWidget( 
         "pushButtonGadget", xmPushButtonGadgetClass, rc, NULL );
      XtVaCreateManagedWidget( 
         "toggleButtonWidget", xmToggleButtonWidgetClass, rc, NULL );
      XtVaCreateManagedWidget( 
         "toggleButtonGadget", xmToggleButtonGadgetClass, rc, NULL );
      XtVaCreateManagedWidget( 
         "arrowButtonWidget", xmArrowButtonWidgetClass, rc, NULL );
      XtVaCreateManagedWidget( 
         "arrowButtonGadget", xmArrowButtonGadgetClass, rc, NULL );
      XtVaCreateManagedWidget( 
         "textWidget", xmTextWidgetClass, rc, NULL );
      XtVaCreateManagedWidget( 
         "textFieldWidget", xmTextFieldWidgetClass, rc, NULL );
   }

   /*---------------------------
   -- Start processing events!
   ---------------------------*/
   XtRealizeWidget( toplevel );
   XtAppMainLoop( app_context );
   
   return 0;
}
