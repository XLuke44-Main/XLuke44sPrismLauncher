This directory contains a .Net wrapper class library for the ZLib1.dll

The wrapper includes support for inflating/deflating memory buffers,
.Net streaming wrappers for the gz streams part of zlib, and wrappers
for the checksum parts of zlib..

Directory structure:
--------------------

LICENSE_1_0.txt       - License file.
readme.txt            - This file.
DotZLib.chm           - Class library documentation
DotZLib.build         - NAnt build file
DotZLib.sln           - Microsoft Visual Studio 2003 solution file

DotZLib\*.cs          - Source files for the class library


Build instructions:
-------------------

1. Using Visual Studio.Net 2003:
   Open DotZLib.sln in VS.Net and build from there. Output file (DotZLib.dll)
   will be found ./DotZLib/bin/release or ./DotZLib/bin/debug, depending on
   you are building the release or debug version of the library.

2. Using NAnt:
   Open a command prompt with access to the build environment and run nant
   in the same directory as the DotZLib.build file.
   The property debug={true|false} toggles release/debug builds (default=true).
   The target clean removes binaries.
   Output file (DotZLib.dll) will be found in either ./DotZLib/bin/release
   or ./DotZLib/bin/debug, depending on whether you are building the release
   or debug version of the library.

   Examples:
     nant
       will build a debug version of the library
     nant clean
       will remove all previously built files.


---------------------------------
Copyright (c) Henrik Ravn 2004

Use, modification and distribution are subject to the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
