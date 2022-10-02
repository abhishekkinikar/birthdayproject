cls
del BinaryTree.exe
del BinaryTree.obj
del BinaryTree.res

cl.exe /c /EHsc /I C:\glew\include BinaryTree.cpp
rc.exe BinaryTree.rc
link.exe BinaryTree.obj BinaryTree.res /LIBPATH:C:\glew\lib\Release\x64 user32.lib gdi32.lib Sphere.lib /SUBSYSTEM:WINDOWS
