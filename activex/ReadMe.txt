ActiveX 5250 Terminal emulator
------------------------------
As stated in win32/ReadMe.txt, this project started has a linux5250,
which is quite far from an ActiveX. Indeed, since the original programmers
don't use lots of global variables, contrary as most C programmers do, I've
been able to do a conversion.


SUPPORT:
---------------------
The first thing to do is to email the port author (Marc-Antoine Ruel
- maruel at pyxis.com) since there is no support on the web page for now.

The second place to go for support is the linux5250 home page:
    http://tn5250.sourceforge.net

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
        
        There is another way explained later. If anyone tries it, please tell
        me! :)


NEEDED LIBRARIES:
------------------------
You need some library to work. First of all you need OpenSSL if you want
to support SSL. If you don't just comment out the #define LIB_SSL in
tn5250_config.h in /activex

Then you need glib 2. To help a bit, you only need those DLLs:
	iconv.dll, intl.dll, libglib-2.0-0.dll
and for SSL: ssleay32.dll and libeay32.dll
you may also need msvcr71.dll if it was compiled with VS.NET 2003.

To grab those files, and the needed headers of glib, look for these
files (or later versions):
	glib-2.2.2.zip and glib-dev-2.2.2.zip,
	libiconv-1.9.1.bin.woe32.zip and gettext-runtime-0.12.1.bin.woe32.zip
Sadly, OpenSSL needs to be compiled: openssl-0.9.7b.tar.gz

If it seems unclear (it is), look at tn5250/win32/ReadMe.txt which is
well done.


BUILDING TN5250:
------------------------
For your information, A5250.h, A5250_i.c, A5250_p.c and dlldata.c
are generated files.

You will need to set the right path in tn5250-config.h at the end of the 
file. You will need to change the default include path in the project also.

VC 7.1 .NET 2003 Users
You will need to copy activex\tn5250-config.h into src\ directory.
Then, load A5250.vcproj and compile.
You will probably wants to update the resource file too.

VC 7.0 .NET 2002 Users
It is probably easy to use VC 2002 by changing the line in A5250.vcproj
where it is written Version="7.10" from "7.10" to "7.00". Maybe other
changes are needed. Never tried. Tell me if it works or not.

VC 6 Users
VC6 is not supported, since it has outdated ATL headers and poor compiler.

And if it cry that it can't find the source control provider, removes the
lines begining with Scc in A5250.vcproj. But I should have removed them
already. But I don't always do.


BUILDING TN5250 without paying lots of money, about 15$US:
-----------------------
By the way, it is possible to have the C.NET compiler.
You need 3 things: Platform SDK, Windows DDK and a debugger (WinDbg).
The Platform SDK contains the MS IDL compiler (MIDL) and many required 
headers. The DDK has the .NET compiler. Sadly, Platform SDK only has the 
64 bits compiler...

You have to get the platform SDK from MS (free):
http://www.microsoft.com/msdownload/platformsdk/sdkupdate/psdk-full.htm
You have to order the XP SP1 DDK for about 15$US for shipping at:
http://www.microsoft.com/whdc/ddk/ordernetddkcd.mspx
Windbg is a not-that-bad debugger for windows (free, I actually *use* it):
http://www.microsoft.com/whdc/ddk/debugging/default.mspx

It is not possible to do a night build without these things.
Contrary to popular belief, you can compile apps with the DDK, only use 
nmake instead of build. In fact, the DDK holds all the ATL code! I didn't 
ever setup this way so I talk "en travers mon chapeau" (without having tried) 
but it is probably feasible. If anyone wants to suffer pain and agony, he 
can try :) Probably only setting some environment variables is needed. Maybe
not.
By the way, it is probably possible to not use windows if you use the GNU 
tools but I know that the MS IDL-produced files are using MS extensions to
C...


BUGS:
-----------------------
There is 2 major bugs.

- There is currently BIG problems with key handling. I need to implement a
key hook to bypass IE key handling. I haven't done this yet since the 
environment where we use it don't have this problem. In fact, only F1 doesn't
works (it bring out the help of IE...). I don't know about when it is hosted
in another application. Click on F1 instead :)

- There is problem with dynamic resizing. The window won't update its size
until you click in it.

Be warned.
