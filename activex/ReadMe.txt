ActiveX 5250 Terminal emulator
------------------------------
As stated in win32/ReadMe.txt, this project started has a linux5250,
which is quite far from an ActiveX. Indeed, since the original programmers
don't use lots of global variables, contrary as most C programmers do, I've
been able to do a conversion. :)


SUPPORT:
---------------------
The first thing to do is to email the port author (Marc-Antoine Ruel
- maruel at users.sf.net) since there is no support on the web page for now.

The second place to go for support is the linux5250 home page:
    http://tn5250.sf.net

If you can't find an answer to your question listed there, you should
check the archives for our mailing list next.  The home page has a link
where you can search the archives.

If you still don't have an answer, click on the "Subscribe to the
Linux5250 mailing list" link on the home page.  Here you will be able
to sign-up for our E-mail discussion list.  All of our developers
(including myself) read this list and we will provide you with as much
assistance as we can.


SYSTEM REQUIREMENTS:
---------------------
   -- Microsoft Windows 98 (R), Windows NT 4.0(R), or higher.

   -- Internet Explorer 6 (Hasn't been tested on anything else)

   -- Since it is an ActiveX, you need the Microsoft Compiler. VS .NET 2003
        is the only supported compiler for now. That's MS technology...


NEEDED LIBRARY:
------------------------
You need one library to make it work:

OPENSSL:
You need OpenSSL if you want to support SSL. (Secure connection). It is optional.


BUILDING TN5250:
------------------------
You need two things:

Platform SDK:
You will need it to build a .CAB file so you can throw it on the Web.

OPENSSL:
- If you don't  want SSL support, just comment out in /activex/tn5250_config.h:
     #define LIB_SSL 

The project will try to find the:
- includes in                ..\..\openssl\include
- libraries (Release-DLL) in ..\..\openssl\lib
- libraries (Release) in     ..\..\openssl\lib-mt
- libraries (Debug) in       ..\..\openssl\dll

OpenSSL needs to be compiled, this is why you may want to disable
SSL support :)  Look at www.openssl.org for the needed files. You have to
compile it with the same VC compiler as TN5250. 

To compile OpenSSL:

- Download latest OpenSSL version.

- Unpack OpenSSL in a temporary folder
- be sure to have CL.EXE and NMAKE.EXE in our path by executing vcvars32.bat
- execute: perl Configure VC-WIN32
- execute: ms\do_masm
- execute: nmake -f ms\ntdll.mak
- Copy out32\ in ..\..\OpenSSL\dll (from activex directory perspective)
- Copy inc32\ in ..\..\OpenSSL\include

- Delete the temporary folder
- Unpack OpenSSL in the temporary folder (clean)
- execute: perl Configure VC-WIN32
- execute: ms\do_masm
- execute: nmake -f ms\nt.mak
- Copy out32\ in ..\..\OpenSSL\lib

- Delete the temporary folder
- Unpack OpenSSL in the temporary folder (clean)
- execute: perl Configure VC-WIN32
- execute: ms\do_masm
- Do a Find and Replace on ms\nt.mak file. Replace "/MD" to "/MT"
- execute: nmake -f ms\nt.mak
- Copy out32\ to ..\..\OpenSSL\lib-mt

The tweaks to do are in 
  Project properties, linker, general, Additional Library Directories.
You will see the call to GenCab.bat in 
  Project properties, Build Events, Post-Build Events.

If nothing works, just mail me or just remove SSL support :)


Modifying GENCAB.BAT:
Change the email adress (look in the file) and use a real certificate if
you have one. It's arguments controls which DLL gets included in the .CAB file.
It is far easier to just create an environment variable named EmailAddress
with your email address.

VC 7.1 .NET 2003 Users
Load A5250.vcproj and compile.
You will probably wants to update the resource file too.

VC 7.0 .NET 2002 Users
It is probably easy to use VC 2002 by changing the line in A5250.vcproj
where it is written Version="7.10" from "7.10" to "7.00". Maybe other
changes are needed. Never tried. Tell me if it works or not.

VC 6 Users
VC6 is not supported, since it has outdated ATL headers and poor compiler.

And if it cry that it can't find the source control provider, removes the
lines begining with Scc in A5250.vcproj. But I should have removed them
already. But I sometimes forget. :)


SUPPORTED CODE PAGES:
-----------------------
Here is a list of the currently supported code pages. More can be added on demand.

37   IBM EBCDIC US, Canada, Netherlands, Portugal, Brazil, Australia, New Zealand (default)
256  IBM EBCDIC Netherlands
273  IBM EBCDIC Austria, Germany
277  IBM EBCDIC Denmark, Norway
278  IBM EBCDIC Finland, Sweden
280  IBM EBCDIC Italy
284  IBM EBCDIC Spain, Latin America
285  IBM EBCDIC United Kingdom
290? IBM EBCDIC "Katakana Extended" Japan
297  IBM EBCDIC France
420  IBM EBCDIC Arabic
424  IBM EBCDIC Hebrew
500  IBM EBCDIC "500V1" Belgium, Canada, Switzerland
870  IBM EBCDIC "Latin-2" Eastern Europe
871  IBM EBCDIC Icelandic
875  IBM EBCDIC Greek
880  IBM EBCDIC Cyrillic
905  IBM EBCDIC Turkish - Latin3
1026 IBM EBCDIC Turkish - Latin5
