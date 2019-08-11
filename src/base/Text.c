/*------------------------------------------------------------------------------
--  Help ToolKit for Motif
-- 
--  (c) Copyright 1997-1999, 2019 Robert S. Werner (a.k.a. Akiva Werner)  
--  ALL RIGHTS RESERVED
--
--  License: The MIT License (MIT)
------------------------------------------------------------------------------*/

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include <Xsc/Help.h>
#include <Xsc/StrDefs.h>

#include "TextP.h"

/*==============================================================================
                               Private data types
==============================================================================*/
/*------------------------------------------------------------------------
-- This structure is used to define the attributes of a string to render
------------------------------------------------------------------------*/
typedef struct _XscTextNameRec
{
   String    name;
   XmString  cs;
   Dimension height;
   Dimension width;
}
XscTextNameRec;

typedef struct _XscTextRec
{
   String          str;
   XmString        cs;
   Dimension       width;
   Dimension       height;
   XmFontList      fontList;
   String          fontListTag;
   unsigned char   alignment;
   unsigned char   direction;
   unsigned char   converter;
   unsigned char   showName;
   XscTextNameRec* textName;
}
XscTextRec;


/*==============================================================================
                               Private prototypes
==============================================================================*/
static void _buildFullName( Widget, char* );
static void _deriveName( XscText, Widget );

/*==============================================================================
                               Protected functions
==============================================================================*/

/*----------------------------------------------------------------------------
-- This function is used to create the data structure
------------------------------------------------------------------------------*/
XscText _XscTextCreate()
{
   XscText self = XtNew( XscTextRec );
   
   memset( (char*) self, '\0', sizeof( *self ) );
   
   self->str         = NULL;
   self->cs          = NULL;
   self->fontList    = NULL;
   self->fontListTag = NULL;
   self->textName    = NULL;
   
   return self;
}


/*------------------------------------------------------------------------------
-- This function deletes the text data structure
------------------------------------------------------------------------------*/
void _XscTextDestroy( XscText self )
{
   XtFree( self->fontListTag );
   XtFree( self->str);
   
   if (self->cs) XmStringFree( self->cs );
   
   if (self->textName)
   {
      XtFree( self->textName->name );
      if (self->textName->cs) XmStringFree( self->textName->cs );
      XtFree( (char*) self->textName );
   }
   XtFree( (char*) self );
}




/*------------------------------------------------------------------------------
-- This function creates a multi-segmented compound string using the text 
-- structure.  The resulting compound string can have portions with different 
-- fonts of directions.
------------------------------------------------------------------------------*/
void _XscTextConvertSegmented( XscText self, Widget w )
{
   String tag;
   String buffer, line_begin, line_end;
   XmString cs_text, cs_build, cs_tmp;

   tag = XmFONTLIST_DEFAULT_TAG;

   buffer = XtNewString( self->str );
   cs_build = NULL;

   for (line_end = buffer; line_end && *line_end;)
   {
      line_begin = line_end;

      line_end = strpbrk( line_begin, "@\n" );

      if (line_end)
      {
         /*--------------------------------
         -- Look for new line in the text
         --------------------------------*/
         if (*line_end == '\n')
         {
            XmString cs_sep;
            
            *line_end = '\0';
            line_end++;

            /*--------------------------------------------------
            -- Append the new line to the end of the (cs) text
            --------------------------------------------------*/
            cs_tmp  = XmStringCreate( line_begin, tag );
            cs_sep  = XmStringSeparatorCreate();
            cs_text = XmStringConcat( cs_tmp, cs_sep );
            XmStringFree( cs_tmp ); cs_tmp = NULL;
            XmStringFree( cs_sep ); cs_sep = NULL;
         }
         /*----------------------------
         -- Look for escape sequences
         ----------------------------*/
         else     /*-- (*line_end == '@') --*/
         {
            *line_end = '\0';
            line_end++;

            /*------------------------------------------------------------------
            -- Build the compound string up to the (potential) escape sequence
            ------------------------------------------------------------------*/
            cs_text = XmStringCreate( line_begin, tag );

            /*---------------------------------------------------
            -- Check for a change font escape sequence: @f[tag]
            ---------------------------------------------------*/
            if (*line_end == 'f')
            {
               line_end++;
               if (*line_end == '[')
               {
                  line_end++;
                  
                  tag = line_end;
                  for (;;)
                  {
                     if (*line_end == '\0')
                     {
                        /*----------------------------------------------
                        -- This handles the case: "...@f[tag" which is
                        -- incomplete
                        ----------------------------------------------*/
                        break;
                     }
                     else if (*line_end == ']')
                     {
                        /*----------------------------------------
                        -- This handles a complete font sequence
                        ----------------------------------------*/
                        *line_end = '\0';
                        line_end++;
                        break;
                     }
                     else
                     {
                        line_end++;
                     }
                  }
                  
                  /*--------------------------------------------------------
                  -- In the case of "...@f[]..." the tag will have the
                  -- value of "".  In this case, force the tag back to the
                  -- default value
                  --------------------------------------------------------*/
                  if (*tag == '\0')
                  {
                     tag = XmFONTLIST_DEFAULT_TAG;
                  }
               }
            }
            /*---------------------------------------------------------
            -- An @d indicated that the direction is (maybe) changing
            ---------------------------------------------------------*/
            else if (*line_end == 'd')
            {
               XmString cs_dir = NULL;
               
               line_end++;
               if (*line_end == '<')
               {
                  line_end++;
                  cs_dir = XmStringDirectionCreate( XmSTRING_DIRECTION_R_TO_L );
               }
               else if (*line_end == '>')
               {
                  line_end++;
                  cs_dir = XmStringDirectionCreate( XmSTRING_DIRECTION_L_TO_R );
               }

               if (cs_dir)
               {
                  cs_tmp = cs_text;
                  cs_text = XmStringConcat( cs_tmp, cs_dir );
                  XmStringFree( cs_dir ); cs_dir = NULL;
                  XmStringFree( cs_tmp ); cs_tmp = NULL;
               }
            }
            else if (*line_end == '@')
            {
               XmString cs_at;
               
               line_end++;
               
               cs_tmp = cs_text;
               cs_at = XmStringCreate( "@", tag );
               cs_text = XmStringConcat( cs_tmp, cs_at );
               XmStringFree( cs_at );  cs_at  = NULL;
               XmStringFree( cs_tmp ); cs_tmp = NULL;
            }
         }
      }
      else
      {
         cs_text = XmStringCreate( line_begin, tag );
      }

      if (cs_build)
      {
         cs_tmp = cs_build;
         cs_build = XmStringConcat( cs_tmp, cs_text );
         XmStringFree( cs_tmp );
         XmStringFree( cs_text );
      }
      else
      {
         cs_build = cs_text;
      }
   }
   XtFree( buffer );

   self->cs = cs_build;
}


/*------------------------------------------------------------------------------
- This converter simply creates a multi-lined compoud string with a user-
- specified font tag
------------------------------------------------------------------------------*/
void _XscTextConvertFontTag( XscText self, Widget w )
{
   String tag;
   String buffer, line_begin, line_end;
   XmString cs_text, cs_sep, cs_build, cs_tmp;

   if (self->fontListTag)
   {
      tag = self->fontListTag;
   }
   else
   {
      tag = XmFONTLIST_DEFAULT_TAG;
   }

   buffer = XtNewString( self->str );
   cs_build = NULL;

   for (line_end = buffer; line_end && *line_end;)
   {
      line_begin = line_end;
      line_end = strchr( line_begin, (int) '\n' );

      if (line_end)
      {
         *line_end = '\0';
         line_end++;

         cs_tmp  = XmStringCreate( line_begin, tag );
         cs_sep  = XmStringSeparatorCreate();
         cs_text = XmStringConcat( cs_tmp, cs_sep );
         XmStringFree( cs_tmp );
         XmStringFree( cs_sep );
      }
      else
      {
         cs_text  = XmStringCreate( line_begin, tag );
      }

      if (cs_build)
      {
         cs_tmp = cs_build;
         cs_build = XmStringConcat( cs_tmp, cs_text );
         XmStringFree( cs_tmp );
         XmStringFree( cs_text );
      }
      else
      {
         cs_build = cs_text;
      }
   }
   XtFree( buffer );

   self->cs = cs_build;
}


/*------------------------------------------------------------------------------
-- This function mimics the Motif 1.2.x converter
------------------------------------------------------------------------------*/
void _XscTextConvertStandard( XscText self, Widget w )
{
   XrmValue to, from;
   Boolean converted;

   char* buffer = XtNewString( self->str );
   
   from.size = strlen( buffer ) + 1;
   from.addr = buffer;

   to.size = sizeof( XmString );
   to.addr = (void*) &(self->cs);

   converted  = XtConvertAndStore( w, XmRString, &from, XmRXmString, &to );

   if (!converted)
   {
      self->cs = XmStringCreate( buffer, XmFONTLIST_DEFAULT_TAG );
   }
   
   XtFree( buffer );
}


/*------------------------------------------------------------------------------
-- This is a generic function used to build a compound string from
-- the text specification in the text structure
------------------------------------------------------------------------------*/
void _XscTextCreateCompoundString( XscText self, Widget w )
{
   if (self->cs)
   {
      XmStringFree( self->cs );
      self->cs = NULL;
   }
   
   assert( self->fontList );
   
   if (self->str)
   {
      switch (self->converter)
      {
      case XmXSC_STRING_CONVERTER_SEGMENTED:
         _XscTextConvertSegmented( self, w );
         break;
         
      case XmXSC_STRING_CONVERTER_FONT_TAG:
         _XscTextConvertFontTag( self, w );
         break;
         
      case XmXSC_STRING_CONVERTER_STANDARD:
      default:
         _XscTextConvertStandard( self, w );
         break;
      }
      XmStringExtent( 
         self->fontList, 
         self->cs, 
         &(self->width), 
         &(self->height) );
   }
   else
   {
      self->width  = 0;
      self->height = 0;
   }

   if (self->showName != XmXSC_SHOW_NAME_NONE)
   {
      if (self->textName == NULL)
      {
      	 self->textName = XtNew( XscTextNameRec );
	 self->textName->name = NULL;
	 self->textName->cs = NULL;
      }
      _deriveName( self, w );
      
      if (self->textName->name)
      {
      	 self->textName->cs = XmStringCreateLocalized( self->textName->name );
	 
	 XmStringExtent(
	    self->fontList,
	    self->textName->cs,
	    &(self->textName->width),
	    &(self->textName->height) );
      }
   }
   else
   {
      if (self->textName)
      {
      	 XtFree( self->textName->name );
	 if (self->textName->cs) XmStringFree( self->textName->cs );
	 self->textName = NULL;
      }
   }
   
}

static void _deriveName( XscText self, Widget w )
{
   char buffer[ 1024 ];
   
   *buffer = '\0';
   switch (self->showName)
   {
   case XmXSC_SHOW_NAME_NONE:
      break;
      
   case XmXSC_SHOW_NAME_SHELL:
      {
      	 Widget _shell = w;
	 while (!XtIsWMShell( _shell ))
	 {
	    _shell = XtParent( _shell );
	 }
	 
	 if (_shell != w)
	 {
	    strcpy( buffer, "*" );
	    strcat( buffer, XtName( _shell ) );
	 }
      }
      /* no break */
      
   case XmXSC_SHOW_NAME_SELF:
      strcat( buffer, "*" );
      strcat( buffer, XtName( w ) );
      break;
      
   case XmXSC_SHOW_NAME_ALL:
      _buildFullName( XtParent( w ), buffer );
      strcat( buffer, XtName( w ) );
      break;
   }
   
   if (*buffer)
   {
      self->textName->name = XtNewString( buffer );
   }
}


static void _buildFullName( Widget w, char* buffer )
{
   if (w)
   {
      _buildFullName( XtParent( w ), buffer );
      strcat( buffer, XtName( w ) );
      strcat( buffer, "." );
   }
   else
   {
      strcpy( buffer, "*" );
   }
}


/*------------------------------------------------------------------------------
-- These functions are used to retrieve values
------------------------------------------------------------------------------*/
unsigned char _XscTextGetAlignment  ( XscText self ){return self->alignment  ;}
unsigned char _XscTextGetConverter  ( XscText self ){return self->converter  ;}
unsigned char _XscTextGetDirection  ( XscText self ){return self->direction  ;}
XmFontList    _XscTextGetFontList   ( XscText self ){return self->fontList   ;}
const char*   _XscTextGetFontListTag( XscText self ){return self->fontListTag;}
const char*   _XscTextGetTopic      ( XscText self ){return self->str        ;}

Dimension _XscTextGetHeight( XscText self )
{
   Dimension _height = (int) (self->textName ? self->textName->height + 5 : 0);
   return self->height + _height;
}
Dimension _XscTextGetWidth( XscText self )
{
   Dimension _width = (int) (self->textName ? self->textName->width : 0);
   if (self->width > _width) _width = self->width;
   return _width;
}

const char* _XscTextGetTextName( XscText self )
{
   if (self->textName)
   {
      return self->textName->name;
   }
   return NULL;
}

/*------------------------------------------------------------------------------
-- These functions are used to set values
------------------------------------------------------------------------------*/
void _XscTextSetAlignment( XscText self,unsigned char c ){self->alignment = c;}
void _XscTextSetConverter( XscText self,unsigned char c ){self->converter = c;}
void _XscTextSetDirection( XscText self,unsigned char c ){self->direction = c;}
void _XscTextSetFontList ( XscText self,XmFontList    f ){self->fontList  = f;}

void _XscTextSetFontListTag( XscText self, String s )
{
   XtFree( self->fontListTag );
   self->fontListTag = s;
}
void _XscTextSetTopic( XscText self, String s)
{
   XtFree( self->str );
   self->str = s;
}

/*------------------------------------------------------------------------------
-- This function is used to retrieve the text values from the db
------------------------------------------------------------------------------*/
void _XscTextLoadResources(
   XscText     self, 
   Widget      w, 
   const char* def, 
   XtResource* resource, 
   Cardinal    res_count )
{
   XtFree( self->fontListTag );
   XtFree( self->str         );
   
   if (self->cs) 
   {
      XmStringFree( self->cs );
      self->cs = NULL;
   }
   
   memset( (char*) self, '\0', sizeof( *self ) );
      
   XtGetApplicationResources( w, self, resource, res_count, NULL, (Cardinal)0 );
   
   /*----------------------------------------------------------------
   -- Use the default topic string none where found in the database
   ----------------------------------------------------------------*/
   if (!self->str)
   {
      self->str = (String) def;
   }
   
   if (self->fontListTag)
   {
      self->fontListTag = XtNewString( self->fontListTag );
   }
   
   if (self->str)
   {
      /*----------------------------------------
      -- Check and see if this is a reference
      ----------------------------------------*/
      if (*self->str == '.')
      {
	 static XrmName  QUARK_NameList[ 4 ] = 
	    { NULLQUARK, NULLQUARK, NULLQUARK, NULLQUARK };
	    
	 static XrmClass QUARK_ClassList[ 4 ] = 
	    { NULLQUARK, NULLQUARK, NULLQUARK, NULLQUARK };
	 
	 static XrmQuark QUARK_String = NULLQUARK;
	 
      	 Bool              _result;
      	 XrmRepresentation _type;
	 XrmValue          _value;
	 char*             _str;
	 char*             _ptr;
	 int               _len;
	 
      	 if (QUARK_String == NULLQUARK)
	 {
	    QUARK_String = XrmPermStringToQuark( XmRString );
	    
	    QUARK_NameList [ 0 ] = XrmPermStringToQuark( "_xscHelp" );
	    QUARK_ClassList[ 0 ] = XrmPermStringToQuark( "_XscHelp" );
	    
	    QUARK_NameList [ 1 ] = XrmPermStringToQuark( "topic" );
	    QUARK_ClassList[ 1 ] = XrmPermStringToQuark( "Topic" );
	 }
      	 _str = XtNewString( (self->str) + 1 );
	 _len = strlen( _str );
	 if (_len)
	 {
	    _ptr = &_str[ _len - 1 ];
	    while (_ptr != _str)
	    {
	       if (!isspace( *_ptr ))
	       {
		  _ptr++;
		  *_ptr = '\0';
		  break;
	       }
	       _ptr--;
	    }
	 }
      	 QUARK_NameList [ 2 ] = XrmStringToQuark( _str );
	 QUARK_ClassList[ 2 ] = QUARK_NameList[ 2 ];
	 XtFree( _str );
	 
	 _result = XrmQGetResource(
	    XrmGetDatabase( XtDisplayOfObject( w ) ),
	    QUARK_NameList,
	    QUARK_ClassList,
	    &_type,
	    &_value );
	 
	 if (_result && _type == QUARK_String)
	 {
	    self->str = XtNewString( _value.addr );
	 }
	 else
	 {
	    self->str = XtNewString( self->str );
	 }
      }
      else
      {
      	 self->str = XtNewString( self->str );
      }
   }
   _XscTextCreateCompoundString( self, w );
}

void _XscTextLoadCueResources( XscText self, Widget w, const char* def )
{
#  define OFFSET_OF( mem ) XtOffsetOf( XscTextRec, mem )

   static XtResource resource[] =
   {
      {
         XmNxscCueAlignment, XmCXscCueAlignment,
         XmRAlignment, sizeof( unsigned char ), OFFSET_OF( alignment ),
         XtRImmediate, (XtPointer) (unsigned char) XmALIGNMENT_BEGINNING
      },{
         XmNxscCueFontList, XmCXscCueFontList,
         XmRFontList, sizeof( XmFontList ), OFFSET_OF( fontList ),
         XtRString, XscHelpDefaultFont
      },{
         XmNxscCueFontListTag, XmCXscCueFontListTag,
         XmRString, sizeof( String ), OFFSET_OF( fontListTag ),
         XtRImmediate, (XtPointer) NULL
      },{
         XmNxscCueShowName, XmCXscCueShowName,
         XmRXscShowName, sizeof(unsigned char), OFFSET_OF( showName ),
         XtRImmediate, (XtPointer)(unsigned char)XmXSC_SHOW_NAME_NONE
      },{
         XmNxscCueStringConverter, XmCXscCueStringConverter,
         XmRXscStringConverter, sizeof(unsigned char), OFFSET_OF( converter ),
         XtRImmediate, (XtPointer)(unsigned char)XmXSC_STRING_CONVERTER_STANDARD
      },{
         XmNxscCueStringDirection, XmCXscCueStringDirection,
         XmRStringDirection, sizeof( unsigned char ), OFFSET_OF( direction ),
         XtRImmediate, (XtPointer) (unsigned char) XmSTRING_DIRECTION_L_TO_R
      },{
         XmNxscCueTopic, XmCXscCueTopic,
         XtRString, sizeof( String ), OFFSET_OF( str ),
         XtRImmediate, NULL
      }
   };
#  undef OFFSET_OF

   _XscTextLoadResources( self, w, def, resource, XtNumber( resource ) );
}


void _XscTextLoadHintResources( XscText self, Widget w, const char* def )
{
#  define OFFSET_OF( mem ) XtOffsetOf( XscTextRec, mem )

   static XtResource resource[] =
   {
      {
         XmNxscHintAlignment, XmCXscHintAlignment,
         XmRAlignment, sizeof( unsigned char ), OFFSET_OF( alignment ),
         XtRImmediate, (XtPointer) (unsigned char) XmALIGNMENT_BEGINNING
      },{
         XmNxscHintFontList, XmCXscHintFontList,
         XmRFontList, sizeof( XmFontList ), OFFSET_OF( fontList ),
         XtRString, XscHelpDefaultFont
      },{
         XmNxscHintFontListTag, XmCXscHintFontListTag,
         XmRString, sizeof( String ), OFFSET_OF( fontListTag ),
         XtRImmediate, (XtPointer) NULL
      },{
         XmNxscHintShowName, XmCXscHintShowName,
         XmRXscShowName, sizeof(unsigned char), OFFSET_OF( showName ),
         XtRImmediate, (XtPointer)(unsigned char)XmXSC_SHOW_NAME_NONE
      },{
         XmNxscHintStringConverter, XmCXscHintStringConverter,
         XmRXscStringConverter, sizeof(unsigned char), OFFSET_OF( converter ),
         XtRImmediate, (XtPointer)(unsigned char)XmXSC_STRING_CONVERTER_STANDARD
      },{
         XmNxscHintStringDirection, XmCXscHintStringDirection,
         XmRStringDirection, sizeof( unsigned char ), OFFSET_OF( direction ),
         XtRImmediate, (XtPointer) (unsigned char) XmSTRING_DIRECTION_L_TO_R
      },{
         XmNxscHintTopic, XmCXscHintTopic,
         XtRString, sizeof( String ), OFFSET_OF( str ),
         XtRImmediate, NULL
      }
   };
#  undef OFFSET_OF

   _XscTextLoadResources( self, w, def, resource, XtNumber( resource ) );
}


void _XscTextLoadTipResources( XscText self, Widget w, const char* def )
{
#  define OFFSET_OF( mem ) XtOffsetOf( XscTextRec, mem )

   static XtResource resource[] =
   {
      {
         XmNxscTipAlignment, XmCXscTipAlignment,
         XmRAlignment, sizeof( unsigned char ), OFFSET_OF( alignment ),
         XtRImmediate, (XtPointer) (unsigned char) XmALIGNMENT_CENTER
      },{
         XmNxscTipFontList, XmCXscTipFontList,
         XmRFontList, sizeof( XmFontList ), OFFSET_OF( fontList ),
         XtRString, XscHelpDefaultFont
      },{
         XmNxscTipFontListTag, XmCXscTipFontListTag,
         XmRString, sizeof( String ), OFFSET_OF( fontListTag ),
         XtRImmediate, (XtPointer) NULL
      },{
         XmNxscTipShowName, XmCXscTipShowName,
         XmRXscShowName, sizeof(unsigned char), OFFSET_OF( showName ),
         XtRImmediate, (XtPointer)(unsigned char)XmXSC_SHOW_NAME_NONE
      },{
         XmNxscTipStringConverter, XmCXscTipStringConverter,
         XmRXscStringConverter, sizeof(unsigned char), OFFSET_OF( converter ),
         XtRImmediate, (XtPointer)(unsigned char)XmXSC_STRING_CONVERTER_STANDARD
      },{
         XmNxscTipStringDirection, XmCXscTipStringDirection,
         XmRStringDirection, sizeof( unsigned char ), OFFSET_OF( direction ),
         XtRImmediate, (XtPointer) (unsigned char) XmSTRING_DIRECTION_L_TO_R
      },{
         XmNxscTipTopic, XmCXscTipTopic,
         XtRString, sizeof( String ), OFFSET_OF( str ),
         XtRImmediate, NULL
      }
   };
#  undef OFFSET_OF

   _XscTextLoadResources( self, w, def, resource, XtNumber( resource ) );
}


/*------------------------------------------------------------------------------
-- This function draws a HtkText string
------------------------------------------------------------------------------*/
void _XscTextStringDraw( 
   XscText     self,
   Display*    x_display, 
   Window      window,
   GC          gc, 
   Position    x, 
   Position    y, 
   Dimension   width, 
   XRectangle* clip,
   Pixel       background,
   Pixel       foreground,
   Boolean     selected )
{   
   if (width == 0) width = _XscTextGetWidth( self );
 
   XSetForeground( x_display, gc, foreground );
   
   if (self->cs)
   {  
      XmStringDraw( 
	 x_display,
	 window,
	 self->fontList,
	 self->cs,
	 gc, 
	 x, 
	 y, 
	 width, 
	 self->alignment, 
	 self->direction, 
	 clip );
   }

   if (self->textName && self->textName->cs)
   {  
      if (selected)
      {
	 XSetForeground( x_display, gc, background );
	 XSetBackground( x_display, gc, foreground );
	 
	 XmStringDrawImage( 
	    x_display,
	    window,
	    self->fontList,
	    self->textName->cs,
	    gc, 
	    x, 
	    y + self->height + 5, 
	    width, 
	    self->alignment, 
	    self->direction, 
	    clip );
      }
      else
      {
	 XmStringDraw( 
	    x_display,
	    window,
	    self->fontList,
	    self->textName->cs,
	    gc, 
	    x, 
	    y + self->height + 5, 
	    width, 
	    self->alignment, 
	    self->direction, 
	    clip );
      }
   }
}


/*------------------------------------------------------------------------------
-- This function indicates if a string is present
------------------------------------------------------------------------------*/
Boolean _XscTextStringExists( XscText self )
{
   return (self && (self->cs || (self->showName != XmXSC_SHOW_NAME_NONE)));
}

Boolean _XscTextHasWidgetName( XscText self )
{
   return (self && self->textName && self->textName->cs);
}
