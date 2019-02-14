cmake_minimum_required (VERSION 3.1)

project (WebServer)

#分别设定可执行文件和库文件的生成目录，这是用了两种方法写目录
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ./build)
#set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build/lib)

#设定编译的类型，可以为GBD调试生成调试信息
set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -O0 -Wall -g -ggdb -std=c++11 -pthread")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -g")

set(sourcefile
    main.cpp
)

set(CPP_FLAGS 
    -g
    -Wall
    -std=c++11
)

#string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CPP_FLAGS}")

add_executable(webserver ${sourcefile})
target_link_libraries(webserver libserver)

add_subdirectory(src)
