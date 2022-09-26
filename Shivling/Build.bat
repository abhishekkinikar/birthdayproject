cls
del Shivling.exe
del Shivling.obj
del Shivling.res

cl.exe /c /EHsc /I C:\glew\include Shivling.cpp
rc.exe Shivling.rc
link.exe Shivling.obj Shivling.res /LIBPATH:C:\glew\lib\Release\x64 user32.lib gdi32.lib Sphere.lib /SUBSYSTEM:WINDOWS
