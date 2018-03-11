# Varcoin
Similar CryptoNote protocol implementation. The next generation of real
anonymity and application level encrypted currency. 

## How to Build
Dependencies: GCC 4.7.3 or later, CMake 2.8.6 or later, and Boost 1.55.  

_Download to:_  
http://gcc.gnu.org/  
http://www.cmake.org/  
http://www.boost.org/  

To build, change to a directory where this file is located, and run `make`.   
The resulting executables can be found in build/release/src.  

_Advanced options:_  
Parallel build: run `make -j<number of threads>` instead of `make`.  
Debug build: run `make build-debug`.  
Test suite: run `make test-release` to run tests in addition to building.   
Running `make test-debug` will do the same to the debug version.  
Building with Clang: it may be possible to use Clang instead of GCC, but this may not work everywhere.   
To build, run `export CC=clang CXX=clang++` before running `make`.  

_On Windows:_  
Dependencies: MSVC 2013 or later, CMake 2.8.6 or later, and Boost 1.55.   
You may download them from:  
http://www.microsoft.com/  
http://www.cmake.org/  
http://www.boost.org/  

To build, change to a directory where this file is located, and run this commands:   
`mkdir build  `
`cd build  `
`cmake -G "Visual Studio 12 Win64"` ..  



Thank you for your support.