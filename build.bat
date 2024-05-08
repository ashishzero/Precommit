@ECHO OFF

SET SourceFiles=precommit.c

IF NOT EXIST "bin" MKDIR "bin"

ECHO Compiling (debug)...
SET CompilerFlags=/W3 /wd4514 /wd4668 /wd4201 /EHsc /Od /ZI /MTd /DBUILD_DEBUG /std:c11 /Fo"bin\\" /Fm"bin\\" /Fd"bin\vc.pdb"
SET NameOfEXE=pre-commit-debug
cl /nologo %CompilerFlags% %* /Fe.\bin\%NameOfEXE% %SourceFiles%

ECHO Compiling (release)...
SET CompilerFlags=/W3 /wd4514 /wd4668 /wd4201 /EHsc /O2 /Ox /MT /DBUILD_RELEASE /std:c11 /Fo"bin\\" /Fm"bin\\" /Fd"bin\release\vc.pdb"
SET NameOfEXE=pre-commit
cl /nologo %CompilerFlags% %* /Fe.\bin\%NameOfEXE% %SourceFiles%

ECHO Generating installer...
if EXIST "install.bat" DEL "install.bat"
ECHO @ECHO OFF  >> "install.bat"
ECHO IF NOT EXIST ".git\\hooks" ( >> "install.bat"
ECHO 	ECHO Not a git root directory >> "install.bat"
ECHO 	EXIT /b 1 >> "install.bat"
ECHO ) >> "install.bat"
ECHO ECHO Installing pre-commit... >> "install.bat"
ECHO XCOPY "%~dp0%bin\\pre-commit.exe" ".\\.git\\hooks\\" /Y >> "install.bat"
ECHO ECHO Installed. >> "install.bat"

ECHO Finished.
