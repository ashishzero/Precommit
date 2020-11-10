#!/bin/sh

mkdir -p build

if [ $1 = '-r' ]
then
	compilerFlags=-W3 -Od
	nameOfExe=precommit-debug
else
	compilerFlags=-W3 -O3
	nameOfExe=precommit
fi

echo Compiling...

sourceFiles=precommit.cpp

g++ $compilerFlags $sourceFiles -o build/$nameOfExe
