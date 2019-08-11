/*------------------------------------------------------------------------------
--  Help ToolKit for Motif/CDE
-- 
--  (c) Copyright 1997-1998, 2019 Robert S. Werner (a.k.a. Akiva Werner)  
--  ALL RIGHTS RESERVED
--
--  License: The MIT License (MIT)
------------------------------------------------------------------------------*/



#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <Xm/Xm.h>
#include <Xm/RepType.h>
#include <Dt/Dt.h>
#include <Dt/Help.h>
#include <Dt/HelpDialog.h>
#include <Dt/HelpQuickD.h>

#include <Xsc/CdeHelp.h>


/*==============================================================================
                               Private data types
==============================================================================*/

/*-------------------------------------------------------------------------
-- This record is maintained, in the X Context Manager, for (possibly)
-- each TopLevel widget in the application.  Each shell can have two
-- associated CDE help widgets: a CDE general help widget and a CDE quick 
-- help widget.  This structure maintains the help widget Ids for each
-- shell plus the Id of the widget to use as the parent when creating
-- the help widgets
-------------------------------------------------------------------------*/
typedef struct _HtkCdeStruct
{
   Widget parent;
   Widget general_help;
   Widget quick_help;
}
HtkCdeStruct;

/*----------------------------------------------------------------------
-- This structure represents the help attributes pulled out of the
-- resource database by the Help ToolKit when help is requested.
-- The topic member takes on different roles based on the value of
-- the help_type member.  However, topic must be defined (non-NULL)
-- or, by definition, no help exists for the widget.
--
--    help_type      	      	    topic
--    --------------------------    -------------
--    DtHELP_TYPE_TOPIC       	    DtNhelpVolume
--    DtHELP_TYPE_STRING      	    DtNstringData
--    DtHELP_TYPE_DYNAMIC_STRING    DtNstringData
--    DtHELP_TYPE_FILE	      	    DtNhelpFile
--    DtHELP_TYPE_MAN_PAGE    	    DtNmanPage
----------------------------------------------------------------------*/
typedef struct _HtkCdeDataStruct
{
   String         topic;
   String         help_volume;
   Dimension      columns;
   String         dialog_title;
   unsigned char  help_type;
   unsigned char  help_widget_type;
   Dimension      rows;
   String         topic_title;
}
HtkCdeDataStruct;


/*==============================================================================
                            Private prototypes
==============================================================================*/
static void          _destroyHelpCB( Widget, XtPointer, XtPointer );
static HtkCdeStruct* _getCdeHelpRecord( Widget );
static void          _helpCB( Widget, XtPointer, XtPointer );


/*==============================================================================
                           Static global variables
==============================================================================*/

/*----------------------------------------------------------------------------
-- The HtkCdeStruct data structures are managed by the X Context Manager.  
-- This is the context identifier used to look up a data structure.
----------------------------------------------------------------------------*/
static XContext _helpContextId;
static String   _helpVolume;

/*==============================================================================
                               Private functions
==============================================================================*/

/*------------------------------------------------------------------------------
-- This callback function is used to cleanup the structures when
-- a shell is destroyed
------------------------------------------------------------------------------*/
static void _destroyHelpCB( Widget w, XtPointer cd, XtPointer cbd )
{
   /*------------------------------------------------
   -- Get the structure associated with this widget
   ------------------------------------------------*/
   HtkCdeStruct* htk_cde_help = _getCdeHelpRecord( w );
   
   if (htk_cde_help)
   {
      XDeleteContext( 
         XtDisplayOfObject( w ), (XID)htk_cde_help->parent, _helpContextId );
         
      XtFree( (char*) htk_cde_help );
   }
}


/*------------------------------------------------------------------------------
-- This function is used to retrieve the CDE help strcuture associated with
-- the widgets within a "window"
------------------------------------------------------------------------------*/
static HtkCdeStruct* _getCdeHelpRecord( Widget obj )
{
   Display*      display;
   HtkCdeStruct* htk_cde_help;
   XPointer      data;
   int           not_found;
   Widget        parent;
   
   display = XtDisplayOfObject( obj );   
   
   /*-------------------------------------------------------------------------
   -- Find the nearest TopLevel Shell, which is the shell that maintains the
   -- help records for this "window".
   -------------------------------------------------------------------------*/
   parent = obj;
   while (!XtIsTopLevelShell( parent ))
   {
      parent = XtParent( parent );
   }

   /*------------------------------------
   -- Get the record (if it is defined)
   ------------------------------------*/
   not_found = XFindContext( display, (XID)parent, _helpContextId, &data );

   if (not_found)
   {
      htk_cde_help = NULL;
   }
   else
   {
      htk_cde_help = (HtkCdeStruct*) data;
   }
   return htk_cde_help;
}


/*------------------------------------------------------------------------------
-- This callback function is called after the user request context-sensitive
-- help _and_ after the Help ToolKit looks up and finds help attributes
-- for the widget (or an ancestor).  The help attributes are all available
-- to this function via the callback data (cbd) argument.
------------------------------------------------------------------------------*/
static void _helpCB( Widget obj, XtPointer cd, XtPointer cbd )
{
   Display*                      display  = XtDisplayOfObject( obj );
   XscHelpContextCallbackStruct* cb_data  = (XscHelpContextCallbackStruct*) cbd;
   HtkCdeDataStruct*             cde_data = (HtkCdeDataStruct*) cb_data->data;

   HtkCdeStruct* htk_cde_help;
   Widget        help_widget;
   XmString      cs_dialog_title;
   
   /*---------------------------------------------------------
   -- Find the nearest TopLevel Shell, the shell will be the 
   -- parent of the help dialog
   ---------------------------------------------------------*/
   Widget parent = obj;
   while (!XtIsTopLevelShell( parent ))
   {
      parent = XtParent( parent );
   }
   
   /*-------------------------------------------------------
   -- Get the screen structure associated with this widget
   -------------------------------------------------------*/
   htk_cde_help = _getCdeHelpRecord( parent );
   
   if (!htk_cde_help)
   {
      /*-------------------------------------------
      -- Since a record does not exist, make one!
      -------------------------------------------*/
      htk_cde_help = XtNew( HtkCdeStruct );

      /*----------------------
      -- Save data structure
      ----------------------*/
      XSaveContext( 
         display, (XID)parent, _helpContextId, (XPointer) htk_cde_help );

      /*----------------------------
      -- Initialize data structure
      ----------------------------*/
      htk_cde_help->parent = parent;
      
      htk_cde_help->general_help = NULL;
      htk_cde_help->quick_help   = NULL;

      /*----------------------------------------------
      -- Clean things up when the shell is destroyed
      ----------------------------------------------*/
      XtAddCallback( parent, XmNdestroyCallback, _destroyHelpCB, NULL );
   }
   
   /*------------------------------------------------------------------
   -- Examine the help attributes to determine which type of CDE help
   -- widget should be used.  If the desired type of help widget has
   -- not been created for this "window", then create one first.
   ------------------------------------------------------------------*/
   if (cde_data->help_widget_type == XmXSC_CDE_GENERAL_HELP_WIDGET)
   {
      if (!htk_cde_help->general_help)
      {
         htk_cde_help->general_help = 
            DtCreateHelpDialog( parent, "htk CDE General Help", NULL, 0 );
      }
      help_widget = htk_cde_help->general_help;
   }
   else
   {
      if (!htk_cde_help->quick_help)
      {
         htk_cde_help->quick_help = 
            DtCreateHelpQuickDialog( parent, "htk CDE Quick Help", NULL, 0 ); 
      }
      help_widget = htk_cde_help->quick_help;
   }
   
   /*--------------------------------------------------------------------------
   -- This block sets up the arguments to configure the specified help widget
   --------------------------------------------------------------------------*/
   {
      Arg      argv[ 10 ];
      Cardinal argc = 0;
      
      /*-----------------------------------------------------------------
      -- Tell the widget about it's mode (DtNhelpType) and logical size
      -----------------------------------------------------------------*/
      XtSetArg( argv[ argc ], DtNhelpType, cde_data->help_type );  argc++;
      XtSetArg( argv[ argc ], DtNcolumns , cde_data->columns   );  argc++;
      XtSetArg( argv[ argc ], DtNrows    , cde_data->rows      );  argc++;
      
      /*------------------------------------
      -- Specify a topic title, if defined
      ------------------------------------*/
      if (cde_data->topic_title)
      {
         XtSetArg( argv[ argc ], DtNtopicTitle, cde_data->topic_title ); argc++;
      }
      
      /*--------------------------------------------------
      -- Notify the widget about what it will be viewing
      --------------------------------------------------*/
      switch( cde_data->help_type )
      {
      case DtHELP_TYPE_TOPIC:
         XtSetArg( argv[ argc ], DtNlocationId, cde_data->topic       ); argc++;
         XtSetArg( argv[ argc ], DtNhelpVolume, cde_data->help_volume ); argc++;
         break;
         
      case DtHELP_TYPE_STRING:
         XtSetArg( argv[ argc ], DtNstringData, cde_data->topic ); argc++;
         break;
         
      case DtHELP_TYPE_DYNAMIC_STRING:
         XtSetArg( argv[ argc ], DtNstringData, cde_data->topic ); argc++;
         break;
         
      case DtHELP_TYPE_FILE:
         XtSetArg( argv[ argc ], DtNhelpFile, cde_data->topic ); argc++;
         break;
         
      case DtHELP_TYPE_MAN_PAGE:
         XtSetArg( argv[ argc ], DtNmanPage, cde_data->topic ); argc++;
         break;
         
      default:
         break;
      }
      
      /*--------------------------------------------------------------------
      -- If a dialog title was specified, then use it; otherwise, a dialog
      -- title needs to be manufactered
      --------------------------------------------------------------------*/
      if (cde_data->dialog_title)
      {
         cs_dialog_title = XmStringCreateLocalized( cde_data->dialog_title );
      }
      else
      {
         char  title_buffer[ 1024 ];
         char* str;
         
         switch( cde_data->help_type )
         {
         case DtHELP_TYPE_TOPIC:
            strcpy( title_buffer, cde_data->topic );
            break;

         case DtHELP_TYPE_DYNAMIC_STRING:
         case DtHELP_TYPE_FILE:
         case DtHELP_TYPE_STRING:
            if (cde_data->topic_title)
            {
               str = cde_data->topic_title;
            }
            else if (cde_data->help_type == DtHELP_TYPE_FILE)
            {
               str = cde_data->topic;
            }
            else
            {
               str = "(Topic Not Known)";
            }
            sprintf( title_buffer, "Topic: %s", str );
            break;

         case DtHELP_TYPE_MAN_PAGE:
            sprintf( title_buffer, "Manual Page: %s", cde_data->topic );
            break;

         default:
            strcpy( title_buffer, "(Topic Not Known)" );
            break;
         }
         cs_dialog_title = XmStringCreateLocalized( title_buffer );
      }
      XtSetArg( argv[ argc ], XmNdialogTitle, cs_dialog_title ); argc++;
      
      /*----------------------------
      -- Configure the help widget
      ----------------------------*/
      XtSetValues( help_widget, argv, argc );
      
      XmStringFree( cs_dialog_title );
   }
   
   /*---------------------
   -- Display the widget
   ---------------------*/
   XtManageChild( help_widget );
}


/*==============================================================================
                             Public functions
==============================================================================*/

/*-----------------------------------------------------------------------
-- This function is used to install the Help ToolKit with the CDE
-- context-sensitive help lookup.
-----------------------------------------------------------------------*/
void XscCdeHelpInstall( Widget shell, String help_volume )
{
#  define OFFSET_OF( mem ) XtOffsetOf( HtkCdeDataStruct, mem )

   static XtResource resource[] =
   {
      {
         /*----------------------------------------------------
         -- This value is used as the helpVolume, stringData, 
         -- helpFile, or manPage
         ----------------------------------------------------*/
         XmNxscCdeHelpTopic, XmCXscCdeHelpTopic, 
         XmRString, sizeof( String ), OFFSET_OF( topic ),
         XtRImmediate, NULL
      },{
         /*-------------------------------------------------------
         -- help volume _must_ be specified as the second entry.
         -- The default value is adjusted at run-time, so the
         -- correct offset into the array is assumed to be [1]
         -------------------------------------------------------*/
         XmNxscCdeHelpVolume, XmCXscCdeHelpVolume,
         XmRString, sizeof( String ), OFFSET_OF( help_volume ),
         XtRImmediate, NULL,
      },{
         XmNxscCdeHelpColumns, XmCXscCdeHelpColumns,
         XmRDimension, sizeof( Dimension ), OFFSET_OF( columns ),
         XtRImmediate, (XtPointer) (Dimension) 70
      },{
         XmNxscCdeHelpDialogTitle, XmCXscCdeHelpDialogTitle,
         XmRString, sizeof( String ), OFFSET_OF( dialog_title ),
         XtRImmediate, NULL
      },{
         XmNxscCdeHelpType, XmCXscCdeHelpType,
         DtRDtHelpType, sizeof( unsigned char ), OFFSET_OF( help_type ),
         XtRImmediate, (XtPointer) (unsigned char) DtHELP_TYPE_TOPIC
      },{
         XmNxscCdeHelpWidgetType, XmCXscCdeHelpWidgetType,
         XmRXscCdeHelpWidgetType, sizeof(unsigned char), OFFSET_OF(help_widget_type),
         XtRImmediate, (XtPointer) (unsigned char) XmXSC_CDE_GENERAL_HELP_WIDGET
      },{
         XmNxscCdeHelpRows, XmCXscCdeHelpRows,
         XmRDimension, sizeof( Dimension ), OFFSET_OF( rows ),
         XtRImmediate, (XtPointer) (Dimension) 25
      },{
         XmNxscCdeHelpTopicTitle, XmCXscCdeHelpTopicTitle,
         XmRString, sizeof( String ), OFFSET_OF( topic_title ),
         XtRImmediate, NULL
      }
   };
#  undef OFFSET_OF

   static String help_type_name[] = 
   {
      "help_type_topic", 
      "help_type_string", 
      "help_type_dynamic_string", 
      "help_type_file",
      "help_type_man_page"
   };
   static unsigned char help_type_value[] = 
   {   
      (unsigned char) DtHELP_TYPE_TOPIC,
      (unsigned char) DtHELP_TYPE_STRING,
      (unsigned char) DtHELP_TYPE_DYNAMIC_STRING,
      (unsigned char) DtHELP_TYPE_FILE,
      (unsigned char) DtHELP_TYPE_MAN_PAGE
   };

   static String help_widget_type_name[] = 
   {
      "xsc_cde_general_help_widget", 
      "xsc_cde_quick_help_widget"
   };
   static unsigned char help_widget_type_value[] = 
   {   
      (unsigned char) XmXSC_CDE_GENERAL_HELP_WIDGET,
      (unsigned char) XmXSC_CDE_QUICK_HELP_WIDGET
   };

   /*---------------------------------------------------------------------
   -- Some implementations of CDE do not define a resource converter for
   -- DtRDtHelpType
   ---------------------------------------------------------------------*/
   if (XmRepTypeGetId( DtRDtHelpType ) == XmREP_TYPE_INVALID)
   {
      XmRepTypeRegister( 
         DtRDtHelpType,
         help_type_name,
         help_type_value,
         XtNumber( help_type_value ) );
   }

   /*---------------------------------------------------------------------
   -- A resource converter for XmRHelpWidgetType _better_ not be defined
   -- since that data type is local to this library
   ---------------------------------------------------------------------*/
   if (XmRepTypeGetId( XmRXscCdeHelpWidgetType ) == XmREP_TYPE_INVALID)
   {
      XmRepTypeRegister( 
         XmRXscCdeHelpWidgetType,
         help_widget_type_name,
         help_widget_type_value,
         XtNumber( help_widget_type_value ) );
   }
   else
   {
      char* param = XmRXscCdeHelpWidgetType;
      Cardinal num_params = 1;

      XtErrorMsg(
         "repTypRegister",
         "alreadyRegistered",
         "XscCdeHelp",
         "The enumeration \"%s\" defined by the XscCdeHelp library "
            "was already registered.",
         &param, &num_params );
   }

   /*------------------------------------
   -- Determine the default volume name
   ------------------------------------*/
   XtFree( _helpVolume );
   _helpVolume = NULL;
   
   if (help_volume == NULL)
   {
      help_volume = getenv( "XSC_CDE_HELP_VOLUME_NAME" );
   }
   
   if (help_volume)
   {
      _helpVolume = XtNewString( help_volume );
   }
   resource[1].default_addr = (XtPointer) _helpVolume;
   
   /*---------------------------------
   -- Install the Help ToolKit
   ---------------------------------*/
   XscHelpContextInstall( 
      resource, 
      XtNumber( resource ), 
      sizeof( HtkCdeDataStruct ), 
      _helpCB, 
      NULL );

   XscHelpInstall( shell );
}
