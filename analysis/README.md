# Analysis

For ROOT based analysis, do not rely on ROOT macros as they are not real code.
Instead, rely on Modern C++ and Modern CMake.
An example for how to compile C++ code using (linking against) ROOT is provided
in [root_example](root_example).


## Example ROOT program

```verbatim
cd root_example/
mkdir build
cmake ..
make
./root-example
```
