@echo off
if exist main.exe del main.exe

set CXX=g++

set SRCDIR=.

set INCLUDES=-I %SRCDIR%
set CXXFILES=%SRCDIR%\*.cpp %SRCDIR%\scripts\*.cpp %SRCDIR%\lodepng.c %SRCDIR%\platform\win32\*.cpp

set LIBS=-lopengl32 -lglu32 -luser32 -lgdi32 -lwinmm

set WFLAGS= -O2 -Wno-unused-parameter -Wall -Wextra


%CXX% -std=c++0x %WFLAGS% %INCLUDES% -o main.exe %CXXFILES% %LIBS%

if not exist main.exe goto end

%1 %2 %3 %4 %5

:end
