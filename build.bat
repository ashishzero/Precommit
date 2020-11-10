@ECHO OFF

IF NOT EXIST "build" MKDIR "build"
IF NOT EXIST "bin" MKDIR "bin"
IF NOT EXIST "bin\debug" MKDIR "bin\debug"
IF NOT EXIST "bin\release" MKDIR "bin\release"

IF "%1" == "-r" GOTO release
REM Defaults to debug mode if no argument is passed
IF "%1" == "-d" GOTO debug

REM Compiler flags for debug
:debug
SET CompilerFlags=/W3 /wd4514 /wd4668 /wd4201 /EHsc /Od /ZI /MTd /DBUILD_DEBUG /std:c++17 /Fo"bin\debug\\" /Fm"bin\debug\\" /Fd"bin\debug\vc.pdb"
SET NameOfEXE=precommit-debug
GOTO compile

:release
SET CompilerFlags=/W3 /wd4514 /wd4668 /wd4201 /EHsc /O2 /Ox /MT /DBUILD_RELEASE /std:c++17 /Fo"bin\release\\" /Fm"bin\release\\" /Fd"bin\release\vc.pdb"
SET NameOfEXE=precommit
GOTO compile

:compile
ECHO "Compiling..."

SET SourceFiles=precommit.cpp

rem PUSHD 
cl  %CompilerFlags% /Fe.\build\%NameOfEXE% %SourceFiles%
rem POPD

ECHO "Build finished."
