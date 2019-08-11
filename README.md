# XscHelp
Help ToolKit for OSF/Motif and the X-Window System

## Help ToolKit Overview

The Help ToolKit for Motif allows developers to easily add and modify various 
types of on-line context-sensitive help to Motif applications.  Using a small 
set of functions to install the library, every widget and gadget in a Motif 
application seemingly inherits new resources allowing help to be configured 
and changed via X resource files.

The ToolKit supports three core help types: **Tips**, **Cues**, and **Hints**.  All of 
these help types can be assigned to any widget and any Motif-based gadget.

**Tips** are brief notes that appear in small pop-up windows over or near a 
screen object when the mouse cursor briefly rests over the object. Tips 
are commonly seen with icon buttons and where originally popularized in 
Windows 95 applications.

**Cues** are displayed in small pop-up windows that normally float above the 
shell widget that contains the keyboard focus. The content of the Cue is 
based on the widget or gadget that currently has the keyboard focus. 

**Hints** are textual messages that appear in a fixed location on a window when 
the mouse cursor moves over a given widget or gadget contained in that window.

In addition, the ToolKit provides an API that allows a developer to plug-in 
virtually any on-line help system, such as the Help system provided through 
CDE.  In fact, a sample CDE Help system plug-in is provided with the 
registered version of the ToolKit. The Help ToolKit also provides a rich 
API allowing help to be controlled and manipulated programmatically.

## History

The Help Toolkit was written in the late 1990s.  It was originally licensed as
shareware controlled by my company Software Components, Inc. -- that's why it is
called **Xsc**Help. The company is now defunct and the software has been dormant for well over a decade.  I recently noticed it on an old backup, so I am now making
it available to the general community for whatever purpose under the MIT license.

There were a couple of articles published about XscHelp in The X Advisor which
was an early webzine for X Window System development.  I do not have copies and,
amazingly, all the links are dead.  I would be happy if they were rediscovered
somewhere.
- "Providing User Feedback with Quick Help -- Part 1: Quick Help Described," *The X Advisor*, Feburary 1996
- "Providing User Feedback with Quick Help -- Part 2: Quick Help Implementation Details," *The X Advisor*, March 1996

## Caveats

I have not built this software in over a decade and I do not currently have
an environment to do so.  

I have a User's Guide in postscript and PDF which are now also licensed under 
the MIT License regardless of what the printed document states.

## License

Copywrite (c) Robert S. Werner (a.k.a Akiva Werner).  All rights reserved.

Licensed under the [MIT license](LICENSE).
