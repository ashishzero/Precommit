#!/bin/sh

mkdir -p bin

sourceFiles='precommit.c'

echo 'Compiling (debug)...'
compilerFlags=''
nameOfExe='pre-commit-debug'
$CC $compilerFlags $sourceFiles -o bin/$nameOfExe

echo 'Compiling (release)...'
compilerFlags='-O3'
nameOfExe='pre-commit'
$CC $compilerFlags $sourceFiles -o bin/$nameOfExe

echo 'Generating installer...'
rm -f install.sh
echo '#!/bin/sh' > install.sh
echo 'if [ ! -d "./.git/hooks" ]; then' > install.sh
echo '  echo Not a git root directory' > install.sh
echo '  exit 1' > install.sh
echo 'fi' > install.sh
echo 'echo "Installing pre-commit..."' > install.sh
echo 'echo cp -rf ""$pwd/bin/pre-commit"" ./.git/hooks/' > install.sh
echo 'echo "Installed." > install.sh

echo 'Finished.'
