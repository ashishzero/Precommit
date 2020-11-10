# Precommit

## What does this do?
- Scans the new changes in the repository for a `KEY` and fails the commit if the `KEY` is present
- If the `KEY` is in quotes, it passes the scan
- The deault key is `nocheckin`

## How to use it?
Either use the prebuild binaries present in `build` directory or build it using the scripts present in the repo (build.bat for windows, build.sh or buildgcc.sh for linux). Then place the `pre-commit` executable file in `.git/hooks` directory of your repository where you want to preform the scan

## How to change the key?
In the source `precommit.cpp`, line 55:
```cpp
const char KEY[] = "nocheckin";
```
Change the `"nocheckin"` to the value you want and build the project.
