CLS

DEL Intro.exe
DEL Intro.obj
DEL Intro.res

cl.exe /c /EHsc /I C:\glew\include Intro.cpp

rc.exe Intro.rc

link.exe Intro.obj Intro.res /LIBPATH:C:\glew\lib\Release\x64 User32.lib Gdi32.lib /SUBSYSTEM:WINDOWS
