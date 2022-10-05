cls

del OGL.exe

del OGL.obj

del OGL.res 

cl.exe /c /EHsc /I C:\glew\include 3DShapes.cpp

rc.exe OGL.rc

link.exe 3DShapes.obj OGL.res /LIBPATH:C:\glew\lib\Release\x64 user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

