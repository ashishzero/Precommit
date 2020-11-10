#!/bin/sh

mkdir -p build

if [ "$1" = "-r" ]
then
	echo 'Compiling - Release...'
	compilerFlags='-Wno-unused-result'
	nameOfExe='pre-commit'
else
	echo 'Compiling - Debug...'
	compilerFlags='-Wno-unused-result -O3'
	nameOfExe='pre-commit-debug'
fi

sourceFiles='precommit.cpp'

g++ $compilerFlags $sourceFiles -o build/$nameOfExe

echo 'Finished.'

