# Cray

## Build

Currently, the GA in the SVN repository only performs first step.  The second step should be added in.

Part 1: The standard GA build.  See the [build script](http://svn/svn/pelibs/ga/trunk/INTEG/build-new.sh) in the Cray SVN repository.

Part 2: Compile the ghost stubs for the missing routines provided by libpeigs.a

```
cd peigs
make
cp libghostpeigs.a $COMPILER_PREFIX/lib
```
Note: `COMPILER_PREFIX` is defined in [`build-new.sh`](http://svn/svn/pelibs/ga/trunk/INTEG/build-new.sh)

### Other Changes

The following SVN modifications should be made:
* remove any module associated with target CPUs, e.g. xtpe-mc12, xtpe-interlagos, craype-mc12, etc.  Currently, interlagos is loaded in the scripts.  Care should be taken to ensure that or any other default targets are not loaded.
