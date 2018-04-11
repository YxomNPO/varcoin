# Varcoin
Similar CryptoNote protocol implementation. The next generation of real  
anonymity and application level encrypted currency.  
����CryptoNoteЭ���ʵ�֡���һ��Ӧ�ò�����ܻ��ҡ�  

## How to Build ��α���
Dependencies: GCC 4.7.3 or later, CMake 2.8.6 or later, and Boost 1.55.  
Ҫ�� GCC 4.7.3����ߣ� CMake 2.8.6����ߣ��Լ�Boost 1.55  

Download to:   
http://gcc.gnu.org/  
http://www.cmake.org/  
http://www.boost.org/  

To build, change to a directory where this file is located, and run `make`.  
The resulting executables can be found in build/release/src.  
Ҫ�������л������ļ����ڵ�Ŀ¼��������`make`��  
���ɵĿ�ִ���ļ�λ��build/release/src��  

Advanced options:  
Parallel build: run `make -j<number of threads>` instead of `make`.  
Debug build: run `make build-debug`.  
Test suite: run `make test-release` to run tests in addition to building.  
Running `make test-debug` will do the same to the debug version.  
Building with Clang: it may be possible to use Clang instead of GCC, but this may not work everywhere.  
To build, run `export CC=clang CXX=clang++` before running `make`.  
�߼�ѡ�  
���й���������`make -j <�߳���>`�滻`make`��  
���Թ���������`make build-debug`��  
�����׼������˹���֮�⣬����`make test-release`�����в��ԡ�  
����`make test-debug`���Ե��԰汾ִ����ͬ�Ĳ�����  
��Clang����������ʹ��Clang����ѡ��GCC�������ܴ������⡣  
������`make`֮ǰ������`export CC=clang CXX=clang++`��  

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


���������� https://github.com/xqymain/varcoin-wiki ���ҵ��������ļ������غ�������÷���

You can also find the usage of binary downloads and commands at https://github.com/xqymain/varcoin-wiki .  

��Ҫע����ǣ���������豸CPU��֧���ض���ָ�������ͨ�����룻����Բ�ͬ�����������Ż��������˲��ֿ�ƽ̨�ԡ�  
��������Windows�µ�����Զ����Linux��������ܣ���������Linux��ʹ�á�  
It should be noted that if your device's CPU does not support a particular set of instructions, it will not be compiled; and optimized for different processors, at the expense of some cross-platform.  
The performance of this program under Windows is far less than that of Linux. If possible, it is recommended that you use it under Linux.