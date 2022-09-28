cls
del OGL.exe 
del OGL.obj 
del OGL.res

cl.exe /c /EHsc /I C:\glew\include OGL.cpp

rc.exe OGL.rc

link.exe OGL.obj OGL.res glew32.lib user32.lib gdi32.lib /Subsystem:windows
