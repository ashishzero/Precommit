# Precommit

## What does this do?

Scans git commit contents for a specified `NocheckinKeyword`. If the `NocheckinKeyword` is found in the commit content, commit is disabled until the `NocheckinKeyword` is removed from the commit.

The default value of `NocheckinKeyword` is `nocheckin` (case insensitive).


## How to use it?
1. Build the precommit by execting the `build` script
2. Execute `install` script from your root git directory (install script is generated after execution of build script)


## How to change the nocheckin keyword?
1. Create a file named `keyword.h` and add the following content:

```h
#define NocheckinKeyword "my_no_checkin"
```

2. Build (rebuild) and install (reinstall) the precommit

## How to remove the precommit?
Simply delete the executable `.gits/hooks/pre-commit` from your git repository.
