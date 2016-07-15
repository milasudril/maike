#!/bin/sh
mkdir -p __targets
set -v
g++ -c -g -fpic -std=c++11 -Wall -Woverflow -Wconversion -Werror -o __targets/./stringformat.o ./stringformat.cpp
g++ -c -g -fpic -std=c++11 -Wall -Woverflow -Wconversion -Werror -o __targets/./errormessage.o ./errormessage.cpp
g++ -c -g -fpic -std=c++11 -Wall -Woverflow -Wconversion -Werror -o __targets/./exceptionhandler.o ./exceptionhandler.cpp
g++ -c -g -fpic -std=c++11 -Wall -Woverflow -Wconversion -Werror -o __targets/./strerror.o ./strerror-posix.cpp
g++ -c -g -fpic -std=c++11 -Wall -Woverflow -Wconversion -Werror -o __targets/./pipe.o ./pipe-posix.cpp
g++ -c -g -fpic -std=c++11 -Wall -Woverflow -Wconversion -Werror -o __targets/./resourceobject.o ./resourceobject.cpp
g++ -c -g -fpic -std=c++11 -Wall -Woverflow -Wconversion -Werror -o __targets/./command.o ./command.cpp
g++ -c -g -fpic -std=c++11 -Wall -Woverflow -Wconversion -Werror -o __targets/./targetpythoninterpreter.o ./targetpythoninterpreter.cpp
g++ -c -g -fpic -std=c++11 -Wall -Woverflow -Wconversion -Werror -o __targets/./filein.o ./filein-posix.cpp
g++ -c -g -fpic -std=c++11 -Wall -Woverflow -Wconversion -Werror -o __targets/./targetpythonloader.o ./targetpythonloader.cpp
g++ -c -g -fpic -std=c++11 -Wall -Woverflow -Wconversion -Werror -o __targets/./dependency.o ./dependency.cpp
g++ -c -g -fpic -std=c++11 -Wall -Woverflow -Wconversion -Werror -o __targets/./targetbase.o ./targetbase.cpp
g++ -c -g -fpic -std=c++11 -Wall -Woverflow -Wconversion -Werror -o __targets/./targetpython.o ./targetpython.cpp
g++ -c -g -fpic -std=c++11 -Wall -Woverflow -Wconversion -Werror -o __targets/./targetpythonfactory.o ./targetpythonfactory.cpp
g++ -c -g -fpic -std=c++11 -Wall -Woverflow -Wconversion -Werror -o __targets/./targetpythonhook.o ./targetpythonhook.cpp
g++ -g -fpic -std=c++11 -Wall -Woverflow -Wconversion -Werror -shared -o __targets/./targetpython.so ./targetpythonmain.cpp __targets/./targetpythonhook.o __targets/./targetpythonfactory.o __targets/./targetpython.o __targets/./targetbase.o __targets/./dependency.o __targets/./targetpythonloader.o __targets/./filein.o __targets/./targetpythoninterpreter.o __targets/./command.o __targets/./resourceobject.o -ljansson __targets/./pipe.o __targets/./strerror.o __targets/./exceptionhandler.o __targets/./errormessage.o __targets/./stringformat.o
g++ -c -g -fpic -std=c++11 -Wall -Woverflow -Wconversion -Werror -o __targets/./writebuffer.o ./writebuffer.cpp
g++ -c -g -fpic -std=c++11 -Wall -Woverflow -Wconversion -Werror -o __targets/./options.o ./options.cpp
g++ -c -g -fpic -std=c++11 -Wall -Woverflow -Wconversion -Werror -o __targets/./datasinkstd.o ./datasinkstd-posix.cpp
g++ -c -g -fpic -std=c++11 -Wall -Woverflow -Wconversion -Werror -o __targets/./fileout.o ./fileout-posix.cpp
g++ -c -g -fpic -std=c++11 -Wall -Woverflow -Wconversion -Werror -o __targets/./versionnumber.o ./versionnumber.cpp
g++ -c -g -fpic -std=c++11 -Wall -Woverflow -Wconversion -Werror -o __targets/./sysvars.o ./sysvars-linux.cpp
g++ -c -g -fpic -std=c++11 -Wall -Woverflow -Wconversion -Werror -o __targets/./systemtargetinfo.o ./systemtargetinfo.cpp
g++ -c -g -fpic -std=c++11 -Wall -Woverflow -Wconversion -Werror -o __targets/./directorylister.o ./directorylister-linuxgnu.cpp
g++ -c -g -fpic -std=c++11 -Wall -Woverflow -Wconversion -Werror -o __targets/./fileinfo.o ./fileinfo-linuxgnu.cpp
g++ -c -g -fpic -std=c++11 -Wall -Woverflow -Wconversion -Werror -o __targets/./fileutils.o ./fileutils-linuxgnu.cpp
g++ -c -g -fpic -std=c++11 -Wall -Woverflow -Wconversion -Werror -o __targets/./targetdirectory.o ./targetdirectory.cpp
g++ -c -g -fpic -std=c++11 -Wall -Woverflow -Wconversion -Werror -o __targets/./targetdirectoryloader.o ./targetdirectoryloader.cpp
g++ -c -g -fpic -std=c++11 -Wall -Woverflow -Wconversion -Werror -o __targets/./expressionevaluator.o ./expressionevaluator.cpp
g++ -c -g -fpic -std=c++11 -Wall -Woverflow -Wconversion -Werror -o __targets/./spiderdefault.o ./spiderdefault.cpp
g++ -c -g -fpic -std=c++11 -Wall -Woverflow -Wconversion -Werror -o __targets/./dependencygraphdefault.o ./dependencygraphdefault.cpp
g++ -c -g -fpic -std=c++11 -Wall -Woverflow -Wconversion -Werror -o __targets/./parametersetdefault.o ./parametersetdefault.cpp
g++ -c -g -fpic -std=c++11 -Wall -Woverflow -Wconversion -Werror -o __targets/./targetcxxoptions.o ./targetcxxoptions.cpp
g++ -c -g -fpic -std=c++11 -Wall -Woverflow -Wconversion -Werror -o __targets/./thread.o ./thread-posix.cpp
g++ -c -g -fpic -std=c++11 -Wall -Woverflow -Wconversion -Werror -o __targets/./targetcxxpptokenizer.o ./targetcxxpptokenizer.cpp
g++ -c -g -fpic -std=c++11 -Wall -Woverflow -Wconversion -Werror -o __targets/./targetcxxcompiler.o ./targetcxxcompiler.cpp
g++ -c -g -fpic -std=c++11 -Wall -Woverflow -Wconversion -Werror -o __targets/./targetcxxloader.o ./targetcxxloader.cpp
g++ -c -g -fpic -std=c++11 -Wall -Woverflow -Wconversion -Werror -o __targets/./targetcxx.o ./targetcxx.cpp
g++ -c -g -fpic -std=c++11 -Wall -Woverflow -Wconversion -Werror -o __targets/./targetcxxfactory.o ./targetcxxfactory.cpp
g++ -c -g -fpic -std=c++11 -Wall -Woverflow -Wconversion -Werror -o __targets/./targetcxxhook.o ./targetcxxhook.cpp
g++ -c -g -fpic -std=c++11 -Wall -Woverflow -Wconversion -Werror -o __targets/./target_factorydelegatordefault.o ./target_factorydelegatordefault.cpp
g++ -c -g -fpic -std=c++11 -Wall -Woverflow -Wconversion -Werror -o __targets/./session.o ./session.cpp
python3 -- ./maikeinfo.py __targets
g++ -c -g -fpic -std=c++11 -Wall -Woverflow -Wconversion -Werror -o __targets/./maikeinfo.o __targets/maikeinfo.cpp
g++ -c -g -fpic -std=c++11 -Wall -Woverflow -Wconversion -Werror -o __targets/./maike.o ./maike.cpp
g++ -g -fpic -std=c++11 -Wall -Woverflow -Wconversion -Werror -o __targets/./maike ./maike-main.cpp __targets/./maike.o __targets/./maikeinfo.o __targets/./session.o __targets/./target_factorydelegatordefault.o __targets/./targetpythonhook.o __targets/./targetpythonfactory.o __targets/./targetpython.o __targets/./targetpythonloader.o __targets/./targetpythoninterpreter.o __targets/./targetcxxhook.o __targets/./targetcxxfactory.o __targets/./targetcxx.o __targets/./targetcxxloader.o __targets/./targetcxxcompiler.o __targets/./targetcxxpptokenizer.o __targets/./thread.o -lpthread __targets/./targetcxxoptions.o __targets/./parametersetdefault.o __targets/./command.o __targets/./pipe.o __targets/./dependencygraphdefault.o __targets/./spiderdefault.o __targets/./expressionevaluator.o __targets/./targetdirectoryloader.o __targets/./targetdirectory.o __targets/./fileutils.o __targets/./targetbase.o __targets/./dependency.o __targets/./fileinfo.o __targets/./directorylister.o __targets/./systemtargetinfo.o __targets/./sysvars.o __targets/./filein.o __targets/./versionnumber.o __targets/./resourceobject.o -ljansson __targets/./fileout.o __targets/./strerror.o __targets/./datasinkstd.o __targets/./options.o __targets/./writebuffer.o __targets/./exceptionhandler.o __targets/./errormessage.o __targets/./stringformat.o
g++ -g -fpic -std=c++11 -Wall -Woverflow -Wconversion -Werror -shared -o __targets/./targetcxx.so ./targetcxxmain.cpp __targets/./targetcxxhook.o __targets/./targetcxxfactory.o __targets/./targetcxx.o __targets/./targetbase.o __targets/./targetcxxloader.o __targets/./filein.o __targets/./targetcxxcompiler.o __targets/./targetcxxpptokenizer.o __targets/./thread.o -lpthread __targets/./fileutils.o __targets/./dependency.o __targets/./targetcxxoptions.o __targets/./parametersetdefault.o __targets/./command.o __targets/./resourceobject.o -ljansson __targets/./pipe.o __targets/./strerror.o __targets/./exceptionhandler.o __targets/./errormessage.o __targets/./stringformat.o
