# Varcoin 

[![Build Status](https://travis-ci.org/xqymain/varcoin.svg?branch=master)](https://travis-ci.org/xqymain/varcoin) 

Similar CryptoNote protocol implementation. The next generation of real 
anonymity and application level encrypted currency. 
类似CryptoNote协议的实现。下一代应用层面加密货币。 

## How to Build 如何编译
Dependencies: GCC 4.7.3 or later, CMake 2.8.6 or later, and Boost 1.55.  
要求： GCC 4.7.3或更高， CMake 2.8.6或更高，以及Boost 1.55  

Download to:   
http://gcc.gnu.org/  
http://www.cmake.org/  
http://www.boost.org/  

To build, change to a directory where this file is located, and run `make`.  
The resulting executables can be found in build/release/src.  
要构建，切换到该文件所在的目录，并运行`make`。  
生成的可执行文件位于build/release/src。  

Advanced options:  
Parallel build: run `make -j<number of threads>` instead of `make`.  
Debug build: run `make build-debug`.  
Test suite: run `make test-release` to run tests in addition to building.  
Running `make test-debug` will do the same to the debug version.  
Building with Clang: it may be possible to use Clang instead of GCC, but this may not work everywhere.  
To build, run `export CC=clang CXX=clang++` before running `make`.  
高级选项：  
并行构建：运行`make -j <线程数>`替换`make`。  
调试构建：运行`make build-debug`。  
测试套件：除了构建之外，运行`make test-release`来运行测试。  
运行`make test-debug`将对调试版本执行相同的操作。  
用Clang构建：可以使用Clang，不选择GCC，但可能存在问题。  
在运行`make`之前，运行`export CC=clang CXX=clang++`。  

On Windows:  
Dependencies: MSVC 2013 or later, CMake 2.8.6 or later, and Boost 1.55.  
You may download them from:  
http://www.microsoft.com/  
http://www.cmake.org/  
http://www.boost.org/  

To build, change to a directory where this file is located, and run this commands:  
mkdir build  
cd build  
cmake -G "Visual Studio 12 Win64" ..  


您还可以在 https://github.com/xqymain/varcoin-wiki 里找到二进制文件的下载和命令的用法。

You can also find the usage of binary downloads and commands at https://github.com/xqymain/varcoin-wiki .  

需要注意的是，如果您的设备CPU不支持特定的指令集，不会通过编译；并针对不同处理器进行优化，牺牲了部分跨平台性。  
本程序在Windows下的性能远不及Linux，如果可能，建议您在Linux下使用。  
It should be noted that if your device's CPU does not support a particular set of instructions, it will not be compiled; and optimized for different processors, at the expense of some cross-platform.  
The performance of this program under Windows is far less than that of Linux. If possible, it is recommended that you use it under Linux.
