

cmake_minimum_required(VERSION 3.15)
project(Chat)


set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-lpthread -O2")

include_directories("../")

find_package(Protobuf REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})

aux_source_directory(.. NET_LIB_SRCS)
aux_source_directory(../log LOG_LIB_SRCS)
aux_source_directory(../base BASE_LIB_SRCS)
aux_source_directory(../Timer Timer_LIB_SRCS)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

#add_executable(testCodec
#        Codec.cpp
#        TestCodec.cpp
#        ChatMessage.pb.cc
#        DisPatcher.cpp DisPatcher.h
#        ../log/logger.cpp
#        ../base/currentThread.cpp
#        )
#target_link_libraries(testCodec ${Protobuf_LIBRARIES})

add_executable(ChatServer
        mainChatServer.cpp
        ChatServer.cpp
        DisPatcher.cpp DisPatcher.h
        Codec.cpp Codec.h
        ChatMessage.pb.cc
        ${NET_LIB_SRCS}
        ${LOG_LIB_SRCS}
        ${BASE_LIB_SRCS}
        ${Timer_LIB_SRCS}
        )
target_link_libraries(ChatServer ${Protobuf_LIBRARIES})
#
#
add_executable(ChatClient
        ChatClient.cpp
        mainChatClient.cpp
        DisPatcher.cpp DisPatcher.h
        Codec.cpp Codec.h
        ChatMessage.pb.cc
        ${NET_LIB_SRCS}
        ${LOG_LIB_SRCS}
        ${BASE_LIB_SRCS}
        ${Timer_LIB_SRCS}
        )
target_link_libraries(ChatClient ${Protobuf_LIBRARIES})




#add_executable(ChatClient
#        ChatClient.cpp
#        DisPatcher.cpp DisPatcher.h
#        Codec.cpp Codec.h
#        mainChatClient.cpp
#        )
#target_link_libraries(ChatClient ${Protobuf_LIBRARIES})






#add_executable(ChatClient
#        ChatClient.cpp
#        )