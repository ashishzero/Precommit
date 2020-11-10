#!/bin/sh

mkdir -p build

if [ "$1" = "-r" ]
then
	echo 'Compiling - Release...'
	compilerFlags='-O3'
	nameOfExe='precommit'
else
	echo 'Compiling - Debug...'
	compilerFlags=''
	nameOfExe='precommit-debug'
fi

sourceFiles='precommit.cpp'

clang++ $compilerFlags $sourceFiles -o build/$nameOfExe

echo Finished.

