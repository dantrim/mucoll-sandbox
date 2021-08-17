# Simple Plotting and Event Looping over LCTuple

## Compilation Instructions
Compile following the usual CMake steps:
```
mkdir build
cd build
cmake ..
make
```

## Simple Plotting
There are two executables: [make-histograms](src/make-histograms.cpp) and [make-histograms-loop](src/make-histograms-loop.cpp).
The former specifies histograms using a simple C++ struct `HistoConfig` and an `std::map` to declare which
variables can be plotted. The latter performs an event loop over the events contained
in the LCTuple ROOT file and fills histograms at each event.

After [compiling](#compilation-instructions), the executables `make-histograms` and `make-histograms-loop` will be available.
Providing the `-h` command line option will print out the usage,

```
$ ./make-histograms -h
------------------------------------------------
 Draw simple histograms of LCTuple file

 Usage: ./make-histograms -i <file> [OPTIONS]

  -i|--input         Input LCTuple ROOT file [REQUIRED]
  -t|--tree          Name of TTree [OPTIONAL, default: MyLCTuple]
  -c|--cut           Cut selection [OPTIONAL, default: none]
  -v|--var           Specify a specific variable to plot [OPTIONAL, default: all available]
  -l|--logy          Use log scale for y-axes [OPTIONAL, default: false]
  -p|--print-config  Print the variables and cuts available for plotting and exit
  -h|--help          Print this help message and exit

------------------------------------------------
```

Plots are saved to the directory in which the executables are called.

