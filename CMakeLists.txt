cmake_minimum_required(VERSION 2.8.11)

project(maike)



include_directories(
    /home/torbjorr/Dokument/maike/
    /home/torbjorr/Dokument/maike/

)

# Linker options
set(CMAKE_LDFLAGS "${CMAKE_LDFLAGS}  ")

# Library path
set(CMAKE_LDFLAGS "${CMAKE_LDFLAGS} -L\".\" ")

# Define the CXX sources
set ( CXX_SRCS
    directorylister-linuxgnu.cpp
    dependencygraphdefault.cpp
    errormessage.cpp
    datasinkstd-posix.cpp
    targetdirectoryloader.cpp
    strerror-posix.cpp
    maike.cpp
    fileinfo-linuxgnu.cpp
    invokerreal-linuxgnu.cpp
    spiderdefault.cpp
    targetcxxcompiler.cpp
    maike-main.cpp
    targetdirectory.cpp
    command.cpp
    exceptionhandler.cpp
    dependency.cpp
    stringformat.cpp
    targetbase.cpp
    targetcxxloader.cpp
    targetcxxtokenizer.cpp
    filein-posix.cpp
    resourceobject.cpp
)

set_source_files_properties(
    ${CXX_SRCS} PROPERTIES COMPILE_FLAGS 
    " -g -std=c++11 -Wall -Woverflow -Wconversion -Werror")

add_executable(maike ${CXX_SRCS} ${C_SRCS})

target_link_libraries(maike
    pthread
    jansson
)

