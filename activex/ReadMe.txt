ActiveX 5250 Terminal emulator
------------------------------

As stated in win32/ReadMe.txt, this project started has a linux5250,
which is quite far from an ActiveX. Indeed, since the original programmers
don't use lots of global variables, contrary as most C programmers do, I've
been able to do a conversion.


SUPPORT:
---------------------
The first thing to do is to email the port author (Marc-Antoine Ruel
- maruel@pyxis.com) since there is no support on the web page for now.

The second place to go for support is our home page:
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
        is the only supported compiler for now.


BUILDING TN5250:
------------------------

VC 7.1 .NET 2003 Users
You will need to copy activex\tn5250-config.h into src\ directory.
Then, load A5250.vcproj and compile.
You will probably wants to update the resource file too.

VC 7.0 .NET 2002 Users
It is probably easy to use VC 2002 by changing the line in A5250.vcproj
where it is written Version="7.10" from "7.10" to "7.00". Maybe other
changes are needed.

VC 6 Users
VC6 is not supported, since it has outdated ATL headers and poor compiler.

And if it cry that it can't find the source control provider, removes the
lines begining with Scc in A5250.vcproj. But I should have removed them
already.


BUGS
-----------------------
There is 2 major bugs.

- There is currently BIG problems with key handling. I need to implement a
key hook to bypass IE key handling. I haven't done this yet since the 
environment where we use it don't have this problem.

- There is problem with dynamic resizing. The window won't update its size
until you click in it.

Be warned.