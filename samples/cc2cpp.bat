@echo off
rem
rem Renaming all .cc files to .cpp for building the samples from the MSVC IDE.
rem

if exist .cpps exit
echo. > .cpps

ren dyna\*.cc *.cpp
ren fddi\*.cc *.cpp
ren fifo1\*.cc *.cpp
ren fifo2\*.cc *.cpp
ren hcube\*.cc *.cpp
ren hist\*.cc *.cpp
ren nim\*.cc *.cpp
ren pvmex\*.cc *.cpp
ren token\*.cc *.cpp
