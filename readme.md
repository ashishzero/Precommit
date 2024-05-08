# Precommit

## What does this do?

Scans git commit contents for a specified `NocheckinKeyword`. If the `NocheckinKeyword` is found in the commit content, commit is disabled until the `NocheckinKeyword` is removed from the commit.

The default value of `NocheckinKeyword` is `nocheckin` (case insensitive).


## How to use it?
1. Build the precommit by execting the `build` script
2. Execute `install` script that is generated after building the precommit from your git repository (must be root directory)


## How to change the nocheckin keyword?

Use the following command when building the precommit:
```sh
./build.sh -DNocheckinKeyword=\"my_keyword\"
```
