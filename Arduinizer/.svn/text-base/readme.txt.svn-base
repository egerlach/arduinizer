Arduinizer
Created by Andrew Mackie
KwartzLab
for SOONCon 2010 badge
designed by James Bastow
SourceForge ID: mackiea
=====================
This project contains:
- A compatibility for Arduino API to simplify control of some of the more 
  complex operations of the LPC1343
- An API for addressing the switches, LEDs and other accessories of the
  SOONCon 2010 badge.

The project makes use of code from the following library projects:
- CMSISv1p30_LPC13xx : for CMSIS 1.30 files relevant to LPC13xx
- LPC13xx_Lib        : for LPC13xx peripheral driver files
These two library projects must exist in the same workspace in order
for the project to successfully build.
- Install an SVN client (eg tortoise)
- Create an SVN directory
- Download the source into the directory from:
  https://arduinator.svn.sourceforge.net/svnroot/arduinator
- Include CMSISv1p30_LPC13xx and LPC13xx_Lib libraries by copying in their directories

The resulting tree should look like:
SVN
SVN\Arduinizer
SVN\LPC13xx_Lib
SVN\CMSISv1p30_LPC13xx

You can edit the "Program.c" file with code.  Although the Arduino is (mostly)
supported here (or any C file with "#include "Arduino.h"" at the top), you can
also use the GPIO API or direct register programming.  Note that any code using
GPIO or register programming will not be Arduino-compatible.

UARTS/Serial
=====================
Only 1 serial port is supported (a la Diecimila/Duemilanova, but not Mega)

String
=====================
Arduino's String class is not supported, as classes themselves are not
supported.

Comments - Please let Sourceforge user mackiea know!
You are welcome to contribute, please obtain a Sourceforge
user ID and ask a project admin for SVN access.