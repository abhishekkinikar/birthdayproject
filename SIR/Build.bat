cls
del PerFragment.exe
del PerFragment.obj
del PerFragment.res
del log.txt
cl.exe /c /EHsc /I C:\glew-2.1.0-win32\glew-2.1.0\include PerFragment.cpp
rc.exe PerFragment.rc
link.exe PerFragment.obj PerFragment.res /LIBPATH:C:\glew-2.1.0-win32\glew-2.1.0\lib\Release\x64 user32.lib Sphere.lib gdi32.lib
