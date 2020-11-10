#!/bin/sh

mkdir -p build

if [ $1 = '-r' ]
then
	compilerFlags=-W3 -Od
	nameOfExe=pre-commit-debug
else
	compilerFlags=-W3 -O3
	nameOfExe=pre-commit
fi

echo Compiling...

sourceFiles=precommit.cpp

clang++ $compilerFlags $sourceFiles -o build/$nameOfExe
