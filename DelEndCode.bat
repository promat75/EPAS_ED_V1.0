@echo off
del .\lst\*.lst
del .\lst\*.m51
del .\hex\*.obj
del .\hex\*.__i
del .\hex\*.htm
forfiles /P .\hex /M *.hex /D -1 /C "cmd /c del .\hex\@file"
del .\Sample\EVB\*.bak
pause
