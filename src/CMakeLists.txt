set(srcs
    epoll.cpp  
    ThreadPoll.cpp  
    eventloop.cpp
    HandleRequest.cpp
    Log.cpp
    Timer.cpp
)


include_directories(${PROJECT_SOURCE_DIR}/Inc)

add_library(libserver ${srcs})

