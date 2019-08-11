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
#include <X11/Shell.h>
#include <X11/StringDefs.h>
#include <X11/Vendor.h>

#include <Xm/DialogS.h>
#include <Xm/GadgetP.h>
#include <Xm/MenuShell.h>

#include "HooksP.h"
#include "HelpP.h"
#include "ObjectP.h"


/*==============================================================================
                                    Macros
==============================================================================*/
#if XtVersion < 11006
#  define XscX11R5 1
#endif

/*==============================================================================
                               Private data types
==============================================================================*/

/*------------------------------------------------------------------------------
-- The Help ToolKit needs to be aware of when new gadgets/widgets
-- are created on a known display.  This is _easy_ to do in X11R6 because
-- of the external agent hooks added to the Intrinsics.  However, there is
-- no "nice" way to do it in X11R5.  As a result, in pre-X11R6 (or if the
-- INFECT_COMPOSITE_CLASS macro is non-zero) the Help ToolKit actually
-- infects all of the Composite classes in order to determine when children
-- are created.  The INFECT_COMPOSITE_CLASS macro is only useful to test
-- the infection code on X11R6 systems.
--
-- Here's how the infection works.  When the Help ToolKit is installed
-- all of the WMShell widgets are infected.  This means a class extension
-- is added to the class, the normal insert_child method for the class is
-- stored in the extension, and a Help ToolKit function replaces the original
-- insert_child method.
--
-- The new insert_child method "infects" the new widget (if it is a Composite),
-- sets up the Help ToolKit attributes on the widget, and calls the original
-- insert_child method.  By infecting all created Composite widget classes,
-- the Help ToolKit guarantees that all created widgets/gadgets will
-- be seen (including non-Motif widgets.)  The only exception would be if
-- the application uses a custom shell widget.
--
-- Now this process works nicely, except for one _big_ problem.  In some cases,
-- (like the Application Shell and many Motif widgets) the normal insert_child
-- method calls a superclass insert_child method.  This is a _big_ problem
-- because the superclass insert_child method is actually the Help ToolKit
-- insert_child replacement method which calls the original insert_child
-- method for the widget which calls the superclass insert_child method....
-- In other words, and infinte loop results (until the stack is blown).
--
-- To solve this problem, the Help ToolKit restores the original
-- insert_child methods for all superclass of a widget before calling the
-- original insert_child method on a widget.  Then it reinfects the
-- superclasses when the call returns.
--
-- Bottom line is that you should try to use X11R6 :-)
------------------------------------------------------------------------------*/
#if XscX11R5

   /*--------------------------------------------------------------------------
   -- This data structure is used to maintain each Composite widget's correct
   -- insert_child method, since the Help ToolKit infects each Composite's
   -- insert_child class record.
   --------------------------------------------------------------------------*/
   typedef struct _HtkClassExtensionRec
   {
      XtPointer next_extension;
      XrmQuark  record_type;
      long      version;
      Cardinal  record_size;

      XtWidgetProc insert_child;   /* The original insert child proc */
   }
   HtkClassExtension, *HtkClassExtensionPtr;

#endif


typedef struct _HtkGadgetClassExtensionRec
{
   XtPointer next_extension;
   XrmQuark  record_type;
   long      version;
   Cardinal  record_size;

   XmWidgetDispatchProc input_dispatch;
}
HtkGadgetClassExtension, *HtkGadgetClassExtensionPtr;


/*==============================================================================
                            Private prototypes
==============================================================================*/
#if XscX11R5

   static void _infectCompositeClass( WidgetClass );
   static void _infectWidget( Widget );
   static void _insertChildHook( Widget );
   static void _insertChildSuperclassRehook( WidgetClass );
   static void _insertChildSuperclassUnhook( WidgetClass );

#else

   static void _createHook( Widget, XtPointer, XtPointer );

#endif

static void _infectGadgetClass( WidgetClass );

/*------------------------------------------------------------------
-- This function must match the prototype for XmWidgetDispatchProc
------------------------------------------------------------------*/
static void _inputDispatchGadgetHook( Widget, XEvent*, Mask );


/*==============================================================================
                           Static global variables
==============================================================================*/

/*------------------------------------------------------------------------
-- This quark is the tag used to identify the Help ToolKit Composite Class
-- extension record
------------------------------------------------------------------------*/
#if XscX11R5

   static XrmQuark _classExtensionQuark = NULLQUARK;

#endif

static XrmQuark _gadgetClassExtensionQuark = NULLQUARK;


/*==============================================================================
                               Private functions
==============================================================================*/

/*------------------------------------------------------------------------------
-- This preprocessor directive is correct.  It should _really_ be
-- testing the XtVersion constant, not the XscX11R5 constant
------------------------------------------------------------------------------*/
#if XtVersion < 11006

   /*--------------------------------------------------------------------------
   -- Create an XtGetClassExtension() compatible function that can be
   -- used in the X11R5 environment.  This function was heavily borrowed
   -- from the X11R6 code base.
   --------------------------------------------------------------------------*/
   typedef struct _XscGenericClassExtensionRec
   {
      XtPointer next_extension;
      XrmQuark  record_type;
      long      version;
      Cardinal  record_size;
   }
   XscGenericClassExtension, *XscGenericClassExtensionPtr;

   static XtPointer _XscGetClassExtension(
       WidgetClass object_class,
       Cardinal    byte_offset,
       XrmQuark	   type,
       long	   version,
       Cardinal	   record_size )
   {
       XscGenericClassExtensionPtr ext;

       ext = *(XscGenericClassExtensionPtr*)((char *)object_class+byte_offset);
       
       while (ext && (ext->record_type != type || ext->version < version ||
		      ext->record_size < record_size)) 
       {
	   ext = (XscGenericClassExtensionPtr) ext->next_extension;
       }

       return (XtPointer) ext;
   }

#else

   #define _XscGetClassExtension XtGetClassExtension

#endif


#if XscX11R5

   /*---------------------------------------------------------------------
   -- This method infects, if necessary, the specified widget with the
   -- Help ToolKit hooks.  The infection basically saves the
   -- widget classes' insert_child method and replaces it with a method
   -- that passes the infection before calling the original insert_child
   -- method. 
   ---------------------------------------------------------------------*/
   static void _infectCompositeClass( WidgetClass wc )
   {
      HtkClassExtension* ext;

      /*--------------------------------------------------------------
      -- Attempt to retrieve the Help ToolKit class extension record
      --------------------------------------------------------------*/
      ext = (HtkClassExtension*) _XscGetClassExtension(
         wc,
         XtOffsetOf( CompositeClassRec, composite_class.extension ),
         _classExtensionQuark,
         1L,
         0 );

      /*---------------------------------------------------------------------
      -- If an extension record could not be found, make one and install it
      ---------------------------------------------------------------------*/
      if (!ext)
      {
         CompositeWidgetClass cwc = (CompositeWidgetClass) wc;

         ext = XtNew( HtkClassExtension );

         ext->record_type = _classExtensionQuark;
         ext->version     = 1L;
         ext->record_size = sizeof( HtkClassExtension );

         ext->insert_child = cwc->composite_class.insert_child;
         cwc->composite_class.insert_child = _insertChildHook;

         ext->next_extension = cwc->composite_class.extension;
         cwc->composite_class.extension = (XtPointer) ext;
      }
   }


   /*---------------------------------------------------------------------
   -- This method infects, if necessary, the specified widget with the
   -- Help ToolKit hooks.  The infection basically saves the
   -- widget classes' insert_child method and replaces it with a method
   -- that passes the infection before calling the original insert_child
   -- method. 
   ---------------------------------------------------------------------*/
   static void _infectWidget( Widget obj )
   {
      /*---------------------------------------------------------------------
      -- If the new widget is a Composite, make sure it gets a Help ToolKit
      -- insert_child method.
      ---------------------------------------------------------------------*/
      if (XtIsComposite( obj ))
      {
         _infectCompositeClass( obj->core.widget_class );
      }
      else if (XmIsGadget( obj ))
      {
         _infectGadgetClass( obj->core.widget_class );
      }
   }


   /*------------------------------------------------------------------------
   -- This is a class method that replaces each Composite's insert_child
   -- method.  It infects each of it's children before calling the original
   -- widget classes' insert_child method
   ------------------------------------------------------------------------*/
   static void _insertChildHook( Widget child )
   {
      HtkClassExtension* ext;
      Widget             parent;

      parent = XtParent( child );
      
      /*------------------------------
      -- Infect the specified widget
      ------------------------------*/
      _infectWidget( child );

      /*---------------------------------------------------
      -- Install the Help ToolKit on the new widget
      ---------------------------------------------------*/
      _XscHelpInstallOnWidget( child );

      /*---------------------------------------------------------------------
      -- Attempt to retrieve the Help ToolKit class extension record of the
      -- specified widget's parent.
      ---------------------------------------------------------------------*/
      ext = (HtkClassExtension*) _XscGetClassExtension(
         parent->core.widget_class,
         XtOffsetOf( CompositeClassRec, composite_class.extension ),
         _classExtensionQuark,
         1L,
         0 );
            
      assert( ext );
      
      _insertChildSuperclassUnhook( XtSuperclass( parent ) );
      ext->insert_child( child );
      _insertChildSuperclassRehook( XtSuperclass( parent ) );
   }


  /*----------------------------------------------------------
  -- Ths function is used to restore the Composite infection
  ----------------------------------------------------------*/
  static void _insertChildSuperclassRehook( WidgetClass wc )
   {
      HtkClassExtension*   ext;
      CompositeWidgetClass cwc;
      
      if (wc != coreWidgetClass)
      {
         cwc = (CompositeWidgetClass) wc;

         /*--------------------------------------------------------------
         -- Attempt to retrieve the Help ToolKit class extension record 
         --------------------------------------------------------------*/
         ext = (HtkClassExtension*) _XscGetClassExtension(
            wc,
            XtOffsetOf( CompositeClassRec, composite_class.extension ),
            _classExtensionQuark,
            1L,
            0 );
            
         if (ext)
         {
            cwc->composite_class.insert_child = _insertChildHook;
         }
         
         /*-----------------------------
         -- Rehook the next superclass
         -----------------------------*/
        _insertChildSuperclassRehook( wc->core_class.superclass );
      }
   }


   /*-------------------------------------------------------------------
   -- Ths function is used to temporary remove the Composite infection
   -------------------------------------------------------------------*/
   static void _insertChildSuperclassUnhook( WidgetClass wc )
   {
      HtkClassExtension*    ext;
      CompositeWidgetClass cwc;
      
      if (wc != coreWidgetClass)
      {
         cwc = (CompositeWidgetClass) wc;

         /*--------------------------------------------------------------
         -- Attempt to retrieve the Help ToolKit class extension record 
         --------------------------------------------------------------*/
         ext = (HtkClassExtension*) _XscGetClassExtension(
            wc,
            XtOffsetOf( CompositeClassRec, composite_class.extension ),
            _classExtensionQuark,
            1L,
            0 );

         if (ext)
         {
            cwc->composite_class.insert_child = ext->insert_child;
         }
         
         /*-----------------------------
         -- Rehook the next superclass
         -----------------------------*/
         _insertChildSuperclassUnhook( wc->core_class.superclass );
      }
   }

#else

   /*---------------------------------------------------------------------------
   -- This function is called in X11R6+ to notify the library that a 
   -- widget/gadget was created
   ---------------------------------------------------------------------------*/
   static void _createHook( Widget obj, XtPointer cd, XtPointer cbd )
   {
      XtCreateHookData data = (XtCreateHookData) cbd;
  
      if (XmIsGadget( data->widget ))
      {
         _infectGadgetClass( data->widget->core.widget_class );
      }
      /*------------------------------------------
      -- Examine the object for hint, tips, etc.
      ------------------------------------------*/
      _XscHelpInstallOnWidget( data->widget );
   }

#endif


/*------------------------------------------------------------------------------
-- This method infects the class record of each gadget noticed.  The infection
-- allows the toolkit to trap focus events and possibly other events.
------------------------------------------------------------------------------*/
static void _infectGadgetClass( WidgetClass wc )
{
   HtkGadgetClassExtension* ext;

   /*--------------------------------------------------------------
   -- Attempt to retrieve the Help ToolKit gadget class extension record
   --------------------------------------------------------------*/
   ext = (HtkGadgetClassExtension*) _XscGetClassExtension(
      wc,
      XtOffsetOf( XmGadgetClassRec, gadget_class.extension ),
      _gadgetClassExtensionQuark,
      1L,
      0 );

   /*---------------------------------------------------------------------
   -- If an extension record could not be found, make one and install it
   ---------------------------------------------------------------------*/
   if (!ext)
   {
      XmGadgetClass gc = (XmGadgetClass) wc;

      ext = XtNew( HtkGadgetClassExtension );

      ext->record_type = _gadgetClassExtensionQuark;
      ext->version     = 1L;
      ext->record_size = sizeof( HtkGadgetClassExtension );

      ext->input_dispatch = gc->gadget_class.input_dispatch;
      gc ->gadget_class.input_dispatch = _inputDispatchGadgetHook;

      ext->next_extension = gc->gadget_class.extension;
      gc ->gadget_class.extension = (XtPointer) ext;
   }
}


static void _inputDispatchGadgetHook( Widget w, XEvent* e, Mask m )
{
   HtkGadgetClassExtension* ext;

   if (m & XmFOCUS_IN_EVENT)
   {
      XscObject object = _XscObjectDeriveFromWidget( w );
      _XscObjectGadgetFocusIn( object );
   }
   else if (m & XmFOCUS_OUT_EVENT)
   {
      XscObject object = _XscObjectDeriveFromWidget( w );
      _XscObjectGadgetFocusOut( object );
   }
#if 0
   else if (m & XmENTER_EVENT)
   {
   }
   else if (m & XmLEAVE_EVENT)
   {
   }
#endif

   /*---------------------------------------------------------------------
   -- Attempt to retrieve the Help ToolKit class extension record of the
   -- specified gadget
   ---------------------------------------------------------------------*/
   ext = (HtkGadgetClassExtension*) _XscGetClassExtension(
      w->core.widget_class,
      XtOffsetOf( XmGadgetClassRec, gadget_class.extension ),
      _gadgetClassExtensionQuark,
      1L,
      0 );

   assert( ext );

   ext->input_dispatch( w, e, m );
}



/*==============================================================================
                            Protected functions
==============================================================================*/

/*------------------------------------------------------------------------------
-- This function installs the Help ToolKit on a specific widget.
-- It is typically used to infect the session/application shells that
-- have no parents
------------------------------------------------------------------------------*/
void _XscHooksInstall( Widget shell )
{
   if (_gadgetClassExtensionQuark == NULLQUARK)
   {
      _gadgetClassExtensionQuark = XrmPermStringToQuark( "XscHelpGadget" );
   }
   
#  if XscX11R5

      /*-------------------------------------------------------------------
      -- Make sure the Help ToolKit Composite extension record identifier
      -- is properly initalized
      -------------------------------------------------------------------*/
      if (_classExtensionQuark == NULLQUARK)
      {
         _classExtensionQuark = XrmPermStringToQuark( "XscHelpComposite" );
   
         /*------------------------------------------------
         -- Initialize all the known shell widget classes
         ------------------------------------------------*/
         XtInitializeWidgetClass( applicationShellWidgetClass );
         XtInitializeWidgetClass( overrideShellWidgetClass    );
         XtInitializeWidgetClass( shellWidgetClass            );
         XtInitializeWidgetClass( topLevelShellWidgetClass    );
         XtInitializeWidgetClass( transientShellWidgetClass   );
         XtInitializeWidgetClass( vendorShellWidgetClass      );
         XtInitializeWidgetClass( wmShellWidgetClass          );

         _infectCompositeClass( applicationShellWidgetClass );
         _infectCompositeClass( overrideShellWidgetClass    );
         _infectCompositeClass( shellWidgetClass            );
         _infectCompositeClass( topLevelShellWidgetClass    );
         _infectCompositeClass( transientShellWidgetClass   );
         _infectCompositeClass( vendorShellWidgetClass      );
         _infectCompositeClass( wmShellWidgetClass          );
         _infectCompositeClass( xmDialogShellWidgetClass    );
         _infectCompositeClass( xmMenuShellWidgetClass      );
      }

#  else

      if (shell)
      {
         Widget display_hook;
         
         display_hook = XtHooksOfDisplay( XtDisplay( shell ) );
         
         XtAddCallback( display_hook, XtNcreateHook, _createHook, NULL );
      }

#  endif
}

