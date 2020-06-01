# abort
Macros for tracing run-time errors

ABORT, ABORT_IF, and ABORT_IF_NOT can be used to print a stack trace
leading up to the point at which the last error occured in your
program. These macros are placed inside a function or class method to
return control to the caller in the event a run-time check fails.
Customized error messages may be constructed using printf()-style
syntax. For example:

    #include <unistd.h>
     
    #include "abort/abort.h"
     
    int func1()
    {
        int fd = -1;
        
        /*
         * Some code that tries to close() a file descriptor:
         */
        int ret = ::close(fd);
        
        /*
         * This says that if close() returned with an error, then
         * return -1. The first argument is a logical expression and
         * the 2nd is the return value
         */
        ABORT_IF(ret != 0, -1);
        
        return 0;
    }
     
    bool func2()
    {
        /*
         * This says that if func1() did not return 0 (to indicate
         * success), then return false. Here we use an optional 3rd
         * argument to print a custom message
         */
        ABORT_IF(func1() != 0, false, "func1() failed :(");
        
        return true;
    }
     
    bool func3()
    {
        /*
         * This says that if func2() returns false (an error), then
         * return false. Note that we can also format our error
         * message just like printf():
         */
        ABORT_IF_NOT(func2(), false, "func2() failed. %s", "[more details]");
        
        return true;
    }
     
    int main()
    {
        ABORT_IF_NOT(func3(), -1);
        
        return 0;
    }

ABORT is like ABORT_IF and ABORT_IF_NOT except that it does not take a
logical expression but rather unconditionally returns control to the caller.
Its behavior is identical to ABORT_IF and ABORT_IF_NOT where the logical
expression is always true and false, respectively.

A fourth macro, ABORT_ON_ERRNO, reports a system call error by comparing its
return value against -1. The corresponding error message is generated by using
the value of errno:
 
    #include <unistd.h>
    #include "abort/abort.h"
     
    void run()
    {
        ABORT_ON_ERRNO(::close(-1), void());
    }
     
    int main()
    {
        run();
        return 0;
    }

Error messages are by default written to standard output. However, the message
stream object can easily be swapped out:

    #include <iostream>
    #include <sstream>
     
    #include "abort/abort.h"
     
    void run()
    {
        ABORT(void(), "Hiya buddy")
    }
     
    int main()
    {
        // Send output to a string stream:
        auto stream = std::make_shared<std::ostringstream>();
        diagnostics::set_ostream(stream);
     
        run();
        std::cout << stream->str();
     
        return 0;
    }

## Usage

cd abort/  
mkdir build && cd build  
cmake ..  
make

Note that this project imports GoogleTest as a submodule to build the unit test

## cpplint

This project imports Google's cpplint style checker as a submodule: https://github.com/cpplint/cpplint  

### To run the linter

python abort/cpplint/cpplint.py --root=. abort/include/abort/abort.h  
python abort/cpplint/cpplint.py abort/src/abort.cc abort/test/abort_ut.cc

## TODO

- To improve the accuracy of the stack frame counter, create an experimental
  branch using libunwind

## Contact

Feel free to email me with bug reports or suggestions:
jfernandez3@gatech.edu