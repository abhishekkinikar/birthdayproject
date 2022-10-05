cls
del BirthdayDemo.exe
del BirthdayDemo.obj
del BirthdayDemo.res
del log.txt
cl.exe /c /EHsc /I C:\glew-2.1.0-win32\glew-2.1.0\include BirthdayDemo.cpp
rc.exe BirthdayDemo.rc
link.exe BirthdayDemo.obj BirthdayDemo.res /LIBPATH:C:\glew-2.1.0-win32\glew-2.1.0\lib\Release\x64 user32.lib Sphere.lib gdi32.lib
